#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <libaudit.h>
#include <sys/wait.h>
#include <auparse.h>
#include <vector>
#include <cerrno>
#include <list>

#include "AuparseFunctions.h"
#include "AuditctlFunctions.h"

using namespace std;

// Vector to store rules
vector<audit_rule_data> rules;
bool terminationSignal = false;

// Handle SigInt
void signalHandler(int sig) {
    terminationSignal = true;

}

int main(int argc, char **argv) {
    // Must be run as sudo
    if (getuid() != 0) {
        perror("Not executed as sudo");
        return 1;
    }
    if (argc < 5) {
        perror("Not enough Arguments: file, uid, gid, path to monitored programm, args for monitored program");
        return 1;
    }

    // Get File to save output
    ofstream writeOutputToFile;
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

    // Get Timestamp
    time_t time_begin = time(0);

    // initialize Pipe: pipe_fd[0] = read, [1] = write
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Create Pipe Failed");
        return 1;
    }
    // Fork
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
                return 1;
            }
        }
        if (uid_toBeMonitored != uid && uid == 0) {
            // Change UID
            if (setuid(uid_toBeMonitored) != 0) {
                perror("SetUID Failed");
                return 1;
            }
        }
        // Parent Process must set the Auditrule
        char pipe_in_buffer[2];
        close(pipe_fd[1]);
        read(pipe_fd[0], pipe_in_buffer, 2);

        // Run toBeMonitored
        //execv(args[0], &args.front());
        execv(argv[4], &argv[4]);

        // Error Case
        perror("Execl failed, Program could not be run");
        return 1;
    }
    // Case Parent
    // Initialize Signalhandler for SigInt, SigHub and SigTerm
    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    //2. Set Auditfilter
    int fd = audit_open();
    if (fd < 0) {
        perror("Could not open audit");
        return 1;
    }
    // Watch Systemcalls
    // Set rule for each syscall
    for (auto syscall : relevantSystemcalls) {
        addAuditRule(fd, syscall.name, arch, rules);
    }
    audit_close(fd);

    // Close Pipe, child can be executed
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // Wait for child exit
    if (errno != ECHILD && errno != EINTR) {
        int status;
        while (waitpid(pid, &status, 0) == -1);
    }

    // Remove audit Rule
    fd = audit_open();

    for (audit_rule_data rule: rules) {
        audit_delete_rule_data(fd, &rule, AUDIT_FILTER_EXIT, AUDIT_ALWAYS);
    }
    audit_close(fd);
    rules.clear();

    // Setup Syscall List for Evaluation
    vector<string> syscallList;
    for (auto syscall: relevantSystemcalls) {
        syscallList.push_back(to_string(syscall.number));
    }
    // Setup relevantPid list for Child-Logic
    list<string> relevantPid;
    // Save child pid
    relevantPid.push_back(to_string(pid));

    // Evaluate the Audit-Log
    // Initialize au to audit logs
    auparse_state_t *au;
    au = auparse_init(AUSOURCE_LOGS, nullptr);

    // au initialization failed
    if (au == nullptr) {
        perror("Auparse could not be initialized");
        return 1;
    }

    // Set termination bool for leaving the loop
    bool allRelevantEventsAnalysed = false;

    // Stop Program in Case of signal
    if (terminationSignal) {
        perror(" Signal Interrupt");
        return 1;
    }

    // Set au to first Record
    auparse_first_record(au);
    // Check each Event
    do {
        // Ignore old Events and ignore non-Syscall Events
        if (auparse_get_time(au) >= time_begin) {
            // Initialize auditEntry for storage
            auto e = auditEntry();
            // Get Audit Values from Systemcall, if relevant
            if (getEventInformations(au, &e, &syscallList)) {
                // Check if it is a special Systemcall
                // Only accept known pids
                if (find(relevantPid.begin(), relevantPid.end(), e.pid) != relevantPid.end()) {
                    // Check if message is an exit Systemcall from to the monitored program
                    if ((e.syscall == "60" || e.syscall == "231") && relevantPid.front() == e.pid) {
                        allRelevantEventsAnalysed = true;
                    }
                        // Check if message is a fork Systemcall
                    else if (e.syscall == "56" || e.syscall == "57" || e.syscall == "58") {
                        // Child Pid is in exit value
                        relevantPid.push_back(e.exitValue);
                    } else {
                        // File Access Detected, printing
                        printFileAccess(&e, writeOutputToFile);
                    }
                }
            }
        }
    } while (auparse_next_event(au) > 0 && !allRelevantEventsAnalysed && !terminationSignal);

    auparse_destroy(au);
    // Close File
    if (writeOutputToFile.is_open()){
        writeOutputToFile.close();
    }


    return 0;
}



