#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <csignal>
#include <fcntl.h>
#include <unistd.h>

#include "AuparseFunctions.h"

using namespace std;

// Function Declaration
void signalHandler(int sig);
void connectionLost(int sig);

void closeConnection();

static void handle_event(auparse_state_t *au,
                         auparse_cb_event_t cb_event_type, void *user_data);

// Global Variables Declaration
// Socket Communication
int socketFd = -1;
int connFd = -1;
// Auparse Parser
auparse_state_t *au = nullptr;
// List with relevant Systcalls
vector<string> syscallList;

// Savety bools, to end current connection or exit plugin
bool connectionEstablished = false;
bool terminateSignal = false;

int main(int argc, char *argv[]) {
    // Initialize Signalhandler for SigHub and SigTerm
    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);
    // Ignore SigPipe -> when Client closes Connection
    signal(SIGPIPE, connectionLost);

    // Get relevant Systemcalls
    for (const auto &syscall: relevantSystemcalls) {
        syscallList.push_back(to_string(syscall.number));
    }
    // create Socket to communicate with Main Programm
    socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketFd == -1) {
        perror("Could not initialize Socket");
        return 1;
    }
    // Socket must be Non-Bocking, to avoid blocked Plugin at auditd stop
    fcntl(socketFd, F_SETFL, O_NONBLOCK);

    // Unlink old Socket and connect to new
    unlink("/tmp/AuditP");
    struct sockaddr socketAddress = {AF_UNIX, "/tmp/AuditP"};
    if (bind(socketFd, &socketAddress, sizeof(socketAddress))) {
        perror("Could not bind the Socket");
        return 1;
    }

    if (listen(socketFd, 2) < 0) {
        perror("Could not listen");
        return 1;
    }

    while (!terminateSignal) {
        // Accept Socket connections
        connFd = accept(socketFd, nullptr, nullptr);
        if (connFd != -1) {
            connectionEstablished = true;
            // Initialize the auparse library
            au = auparse_init(AUSOURCE_FEED, nullptr);

            if (au != nullptr) {
                // Set Callback-Function
                auparse_add_callback(au, handle_event, nullptr, nullptr);

                // Main Loop, get all Events
                while (connectionEstablished && !terminateSignal) {
                    // Read all lines from Stdin
                    string line;
                    while (connectionEstablished && !terminateSignal && getline(cin, line)) {
                        line += "\n";
                        auparse_feed(au, line.c_str(), line.size());
                    }
                    // Flush the events to analyze them
                    auparse_flush_feed(au);
                }
                // Clean up auparse
                auparse_destroy(au);
                au = nullptr;
            } else {
                // Terminate, so the Plugin will be restarted by auditspd
                perror("Could not Initialize Auparse");
                terminateSignal = true;
            }
        }
    }

    // Close Socket
    closeConnection();
    close(socketFd);

    // Necessary, to avoid too fast termination for auditd
    sleep(2);

    return 0;
}

// Handle Signals
void signalHandler(int sig) {
    // if this bool is set, the Programm should be cleaned up and exited
    terminateSignal = true;
}
void connectionLost(int sig) {
    // Stop Connection
    closeConnection();
}

void closeConnection() {
    connectionEstablished = false;
    if (connFd != -1) {
        close(connFd);
        connFd = -1;
    }
}

static void handle_event(auparse_state_t *au,
                         auparse_cb_event_t cb_event_type, void *user_data) {
    if (cb_event_type != AUPARSE_CB_EVENT_READY)
        return;
    // Initialize auditEntry for storage
    auto e = auditEntry();
    // Get Audit Values from Systemcall, if relevant
    if (getEventInformations(au, &e, &syscallList)) {
        // Prepare String for sending to main Program
        string message =
                e.syscall + "," + e.pid + "," + e.ppid + "," + e.exitValue + "," + e.fileName +
                "," + e.exe + "," + e.flag + ";";

        // Send Information over Socket
        long numberSend = send(connFd, message.c_str(), message.size(), 0);
        //  check if connection Problems occurred
        if (numberSend <= 0) {
            closeConnection();
        }
    }
}
