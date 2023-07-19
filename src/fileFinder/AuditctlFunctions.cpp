#include "AuditctlFunctions.h"

std::string checkOpenMode(std::string flag) {
    std::string result = "?";
    int mode = -1;
    // Flag is stored Hexadecimal, get int mode
    try {
        mode = stoi(flag, 0, 16);
    } catch (std::exception &e) {
        // Error case: return ?
        return result;
    }
    // Check which mode it ist
    if ((mode & O_ACCMODE) == O_RDONLY) {
        result = "r";
    } else if ((mode & O_ACCMODE) == O_WRONLY) {
        result = "w";
    } else if ((mode & O_ACCMODE) == O_RDWR) {
        result = "rw";
    }

    return result;
}

void printFileAccess(auditEntry *e, std::ofstream &ofstream) {
    std::string modus, syscall, pid;
    // Check if it is open Systemcalls, if yes -> evaluate flag
    if (e->syscall == "257") {
        syscall = "\"openat\"";
        modus = "\"" + checkOpenMode(e->flag) + "\"";
    } else if (e->syscall == "2") {
        syscall = "\"open\"";
        modus = "\"" + checkOpenMode(e->flag) + "\"";
    } else {
        // Search for Syscall data in relevantSystemcalls
        for (const auto &systemcall: relevantSystemcalls) {
            if (std::to_string(systemcall.number) == e->syscall && systemcall.mode != "-") {
                modus = "\"" + systemcall.mode + "\"";
                syscall = "\"" + systemcall.name + "\"";
                break;
            }
        }
    }
    // Print and Write to File
    if (!e->fileName.empty() && !modus.empty()) {
        // Setup message
        std::stringstream stringstream;
        stringstream << "PID: " << std::setw(6) << std::left << e->pid
                  << " Modus: " << std::setw(4) << std::left << modus
                  << " Systemcall: " << std::setw(12) << std::left << syscall
                  << " Datei: " << e->fileName
                  << ", Exe: " << e->exe
                  << std::endl;
        std::string message = stringstream.str();
        // Print
        std::cout << message;
        // Save to File
        if (ofstream.is_open()) {
            ofstream << message;
        }
    }
}

void addAuditRule(int fd, std::string &syscall, std::string &arch, std::vector<audit_rule_data> &rules) {
    // create rule for syscall
    struct audit_rule_data *rule = new audit_rule_data();
    audit_rule_syscallbyname_data(rule, syscall.c_str());
    audit_rule_fieldpair_data(&rule, arch.c_str(), AUDIT_FILTER_EXIT);

    // Set rule to Auditfilter
    audit_add_rule_data(fd, rule, AUDIT_FILTER_EXIT, AUDIT_ALWAYS);

    // Save rule to vector
    rules.push_back(*rule);
}
