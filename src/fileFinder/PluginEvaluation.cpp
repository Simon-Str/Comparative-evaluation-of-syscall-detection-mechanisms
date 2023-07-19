#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <libaudit.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/socket.h>
#include <vector>
#include <list>
#include <algorithm>

#include "AuditctlFunctions.h"

using namespace std;

// Declaration Functions
void signalHandler(int sig);

bool splitString(std::string *source, std::string *target, char delimiter);

void evaluateAuditEvent(std::string *eventString, bool *childExited);

// Declaration Global Variables
vector<audit_rule_data> rules;
list<string> relevantPid;
ofstream writeOutputToFile;
bool terminationSignal = false;

int main(int argc, char **argv) {
    // Must be run as sudo
    if (getuid() != 0) {
        perror("Not executed as sudo");
        exit(1);
    }
    if (argc < 5) {
        perror("Not enough Arguments: file, uid, gid, path to monitored programm, args for monitored program");
        return 1;
    }

    // Get File to save output
    string openFileName = argv[1];
    if (openFileName != "-"){
        writeOutputToFile.open(argv[1]);
        if (! writeOutputToFile){
            perror("Could not open file");
            return 1;
        }
    }

    // Get UID and GID
    int uid_toBeMonitored = 0, gid_toBeMonitored = 0;
    try {
        uid_toBeMonitored = stoi(argv[2]);
        gid_toBeMonitored = stoi(argv[3]);
    } catch (exception &e) {
        perror("Error while parsing UID or GID");
        return 1;
    }
    // Get System-Arch for Auditrules
    struct utsname uts_buffer{};
    if (uname(&uts_buffer) < 0) {
        perror("Could not get Uname");
        return 1;
    }
    string arch = "arch=" + (string) uts_buffer.machine;

    // Fork
    // initialize Pipe and correct answer pipe_fd[0] = read, [1] = write
    int pipe_fd[2];
    char pipe_okay[1];
    pipe_okay[0] = 1;
    if (pipe(pipe_fd) == -1) {
        perror("Create Pipe Failed");
        return 1;
    }

    pid_t pid = fork();
    // Case Error
    if (pid == -1) {
        perror("Fork failed");
        return 1;
    }
        // Case Child
    else if (pid == 0) {
        // Change UID and GID
        int uid = getuid();
        int gid = getgid();

        if (gid_toBeMonitored != gid && gid == 0) {
            // Change UID
            if (setgid(gid_toBeMonitored) != 0) {
                perror("SetGID Failed");
                exit(1);
            }
        }
        if (uid_toBeMonitored != uid && uid == 0) {
            // Change UID
            if (setuid(uid_toBeMonitored) != 0) {
                perror("SetUID Failed");
                exit(1);
            }
        }
        // Wait for answer, because Parent must set the Auditrule
        // Close writing end of pipe, read okay and close read end
        close(pipe_fd[1]);
        char pipe_answer[1];
        read(pipe_fd[0], pipe_answer, 1);

        // Run toBeMonitored
        // warning: null argument where non-null required (argument 2) [-Wnonnull]
        if (pipe_answer[0] == pipe_okay[0]) {
            execv(argv[4], &argv[4]);
            // Error Case
            perror("Execl failed");
        }
        return 1;
    }
    // Case Parent
    relevantPid.push_back(to_string(pid));

    // Initialize Signalhandler for SigInt, SigHub and SigTerm
    signal(SIGINT, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);

    // Check for Plugin
    int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr socketAddress = {AF_UNIX, "/tmp/AuditP"};
    if (connect(socketFd, &socketAddress, sizeof(socketAddress))) {
        perror("Could not connect to the Socket");

        // Set answer to child, so toBeMonitored doesn't run
        pipe_okay[0] = 0;
        // Set Termination-Bit to avoid rule-Setting, etc
        terminationSignal = true;
    }
    bool childExited = false;
    bool auditFilterSet = false;
    // Set Auditfilter
    int fd = audit_open();

    // Watch Systemcalls
    // Set rule for each syscall
    if (!terminationSignal) {
        auditFilterSet = true;
        for (auto syscall: relevantSystemcalls) {
            addAuditRule(fd, syscall.name, arch, rules);
        }
        audit_close(fd);
    }
    // Send answer to child
    write(pipe_fd[1], pipe_okay, 1);
    // Close Pipe, child can be executed
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // Initialize strings and buffer for socket communication
    string singleEvent, received;
    char buffer[1024];
    while (!childExited && !terminationSignal) {
        // Read Message from Plugin
        long numberRead = read(socketFd, buffer, 1023);
        // Check if read was successful
        if (numberRead != -1) {
            buffer[numberRead] = '\0';
            received += buffer;
            // Split message in single Audit-Events
            while (splitString(&received, &singleEvent, ';')) {
                evaluateAuditEvent(&singleEvent, &childExited);
            }
        } else {
            // Close Connection in Error-Case
            terminationSignal = true;
        }
    }
    // Close Connection to Plugin
    close(socketFd);
    // Close File
    if (writeOutputToFile.is_open()){
        writeOutputToFile.close();
    }
    // Remove audit Rule
    if (auditFilterSet) {
        fd = audit_open();
        for (audit_rule_data rule: rules) {
            audit_delete_rule_data(fd, &rule, AUDIT_FILTER_EXIT, AUDIT_ALWAYS);
        }
        audit_close(fd);
        rules.clear();
    }

    return 0;
}


// Handle Terminations-Signals
void signalHandler(int sig) {
    terminationSignal = true;
}

bool splitString(std::string *source, std::string *target, char delimiter) {

    if (source->empty()) {
        return false;
    }
    // Find firs occurrence
    size_t position = source->find(delimiter);

    // No delimiter found
    if (position == std::string::npos) {
        *target = *source;
        return false;
    }

    // Split first occurrence from source and save in target
    *target = source->substr(0, position);
    *source = source->substr(position + 1, source->length());

    return true;
}

void evaluateAuditEvent(string *eventString, bool *childExited) {
    // Save Message to auditEntry
    auditEntry e;
    // Systemcall
    splitString(eventString, &e.syscall, ',');
    // Systemcall Name
    //splitString(eventString, &e.syscallName, ',');
    // PID
    splitString(eventString, &e.pid, ',');
    // PPID
    splitString(eventString, &e.ppid, ',');
    // ExitValue
    splitString(eventString, &e.exitValue, ',');
    // File Name
    splitString(eventString, &e.fileName, ',');
    // exe
    splitString(eventString, &e.exe, ',');
    // Flag / Mode
    splitString(eventString, &e.flag, ',');

    // Check if PID is known
    if (find(relevantPid.begin(), relevantPid.end(), e.pid) != relevantPid.end()) {
        // Check if eventString is an exit Systemcall from to the monitored program
        if ((e.syscall == "60" || e.syscall == "231") && relevantPid.front() == e.pid) {
            *childExited = true;
        }
            // Check if eventString is a fork Systemcall
        else if (e.syscall == "56" || e.syscall == "57" || e.syscall == "58") {
            // GrandchildPid is in exit value
            relevantPid.push_back(e.exitValue);
        } else {
            // Print file access
            printFileAccess(&e, writeOutputToFile);
        }
    }
}


