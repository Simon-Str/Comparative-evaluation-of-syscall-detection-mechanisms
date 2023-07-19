//
// Provides Functions for setting rules to Auditd
//

#ifndef PROJEKTARBEIT_AUDITCTLFUNCTIONS_H
#define PROJEKTARBEIT_AUDITCTLFUNCTIONS_H

#include <string>
#include <libaudit.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/utsname.h>

#include "AuditInformations.h"

std::string checkOpenMode(std::string flag);
void printFileAccess(auditEntry *e, std::ofstream &ofstream);
void addAuditRule(int fd, std::string &syscall, std::string &arch, std::vector<audit_rule_data> &rules);

#endif //PROJEKTARBEIT_AUDITCTLFUNCTIONS_H
