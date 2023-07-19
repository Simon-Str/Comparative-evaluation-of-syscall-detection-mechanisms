#include <iostream>
#include "AuparseFunctions.h"

bool getRecord(auparse_state_t *au, const std::string &name) {
    bool result = false;
    // Reset Record so all Records can be found
    auparse_first_record(au);

    // Get Number of Records in event
    unsigned int numberOfRecords = auparse_get_num_records(au);
    if (numberOfRecords > 0) {
        for (int i = 0; i < numberOfRecords; ++i) {
            // Get Type Name and Compare to given Name
            std::string type = auparse_get_type_name(au);
            if (type == name) {
                result = true;
                // The inner pointer is set to the desired record in au.
                break;
            }
            auparse_next_record(au);
        }
    }
    return result;
}

std::string getRecordField(auparse_state_t *au, const std::string &name) {
    std::string result = "";
    // returns NULL if no field matches
    // calling this function once failed -> could not get all values
    if (auparse_find_field(au, name.c_str()) != nullptr){
        result = auparse_find_field(au, name.c_str());
        // Reset au -> otherwise other entries can't be found
        auparse_first_field(au);
    }
    return result;
}


std::string getPaths(auparse_state_t *au) {
    std::string result = "";
    // Reset Record so all Records can be found
    auparse_first_record(au);
    // Get Number of Records in event
    unsigned int numberOfRecords = auparse_get_num_records(au);
    if (numberOfRecords > 0) {
        // Counter to get number of Paths
        int counter = 0;
        std::string tmp;
        // Iterate trough Records and check for Path-Entries
        for (int i = 0; i < numberOfRecords; ++i) {
            std::string type = auparse_get_type_name(au);
            if (type == "PATH") {
                // Get Filename with Field "name"
                tmp = getRecordField(au, "name");
                // if found field, save path to string
                if (!tmp.empty()) {
                    // Add "&" if multiple paths
                    if (counter != 0) {
                        result += " & ";
                    }
                    result += tmp;
                    // increment counter
                    counter++;
                }
            }
            auparse_next_record(au);
        }
    }
    return result;
}


bool getEventInformations(auparse_state_t *au, auditEntry *e, std::vector<std::string> *allSyscall) {
    bool result = false;

    // Set au to first record of event
    auparse_first_record(au);

    // Check if it is a Syscall and set pointer
    if (getRecord(au, "SYSCALL")) {
        // get Syscall number
        e->syscall = getRecordField(au, "syscall");
        // Check if systemcall is relevant
        if (find(allSyscall->begin(), allSyscall->end(), e->syscall) != allSyscall->end()) {
            //is relevant -> return true
            result = true;

            // Save PID, PPID and Exit an Exe-Name value from SYSCALL
            e->pid = getRecordField(au, "pid");
            e->ppid = getRecordField(au, "ppid");
            e->exitValue = getRecordField(au, "exit");
            e->exe = getRecordField(au, "exe");
            // Get File Name(s)
            e->fileName = getPaths(au);

            // Check for Open-Systemcalls and flags
            if (e->syscall == "257") {
                getRecord(au, "SYSCALL");
                e->flag = getRecordField(au, "a2");
            } else if (e->syscall == "2") {
                getRecord(au, "SYSCALL");
                e->flag = getRecordField(au, "a1");
            }
            // Check AT_EMPTY_PATH Flag for Newfstatat-Syscall
            else if (e->syscall == "262") {
                getRecord(au, "SYSCALL");
                try {
                    int flag = stoi(getRecordField(au, "a3"), 0, 16);
                    // Check if Flag is set
                    if ((flag | AT_EMPTY_PATH) == flag) {
                        // file access already detected -> this Event can be ignored
                        result = false;
                    }
                }
                catch (std::exception &exp) {
                    // If Flag could not be converted,
                    // check if path is empty
                    if (e->fileName.empty()) {
                        result = false;
                    }
                }
            }
        }
    }
    return result;
}