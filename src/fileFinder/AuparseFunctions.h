//
// Provides functions for Auparse-Audit-Evaluation
//

#ifndef PROJEKTARBEIT_AUPARSEFUNCTIONS_H
#define PROJEKTARBEIT_AUPARSEFUNCTIONS_H

#include <auparse.h>
#include <string>
#include <algorithm>
#include <fcntl.h>
#include "AuditInformations.h"

bool getRecord(auparse_state_t *au, const std::string &name);
std::string getRecordField(auparse_state_t *au, const std::string &name);
std::string getPaths(auparse_state_t *au);
bool getEventInformations(auparse_state_t *au, auditEntry *e, std::vector<std::string> *allSyscall);

#endif //PROJEKTARBEIT_AUPARSEFUNCTIONS_H
