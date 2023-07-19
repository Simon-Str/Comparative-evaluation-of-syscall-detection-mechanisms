//
// Provides necessary structs and relevant Systemcalls
//

#ifndef PROGRAMM_AUDITEVALUATIONMETHODS_H
#define PROGRAMM_AUDITEVALUATIONMETHODS_H

#include <vector>

// Definition für einen Audit Eintrag
struct auditEntry {
    std::string syscall;
    std::string pid;
    std::string ppid;
    std::string exitValue;
    std::string fileName;
    std::string flag;
    std::string exe;
};

struct relevantSystemcall {
    int number;
    std::string name;
    std::string mode;
};

static const std::vector<relevantSystemcall> relevantSystemcalls = {
        {2,   "open",         "d"},
        {4,   "stat",         "r"},
        {6,   "lstat",        "r"},
        {21,  "access",       "r"},
        {56,  "clone",        "-"},
        {57,  "fork",         "-"},
        {58,  "vfork",        "-"},
        {59,  "execve",       "x"},
        {60,  "exit",         "-"},
        {76,  "truncate",     "w"},
        {82,  "rename",       "w"},
        {85,  "creat",        "w"},
        {86,  "link",         "w"},
        {87,  "unlink",       "w"},
        {88,  "symlink",      "w"},
        {89,  "readlink",     "r"},
        {90,  "chmod",        "w"},
        {92,  "chown",        "w"},
        {94,  "lchown",       "w"},
        {132, "utime",        "w"},
        {133, "mknod",        "w"},
        {188, "setxattr",     "w"},
        {189, "lsetxattr",    "w"},
        {191, "getxattr",     "r"},
        {192, "lgetxattr",    "r"},
        {194, "listxattr",    "r"},
        {195, "llistxattr",   "r"},
        {197, "removexattr",  "w"},
        {198, "lremovexattr", "w"},
        {231, "exit_group",   "-"},
        {235, "utimes",       "w"},
        {257, "openat",       "d"},
        {259, "mknodat",      "w"},
        {260, "fchownat",     "w"},
        {262, "newfstatat",   "r"},
        {263, "unlinkat",     "w"},
        {264, "renameat",     "w"},
        {265, "linkat",       "w"},
        {266, "symlinkat",    "w"},
        {267, "readlinkat",   "r"},
        {268, "fchmodat",     "w"},
        {269, "faccessat",    "r"},
        {316, "renameat2",    "w"},
        {322, "execveat",     "x"}
};

#endif //PROGRAMM_AUDITEVALUATIONMETHODS_H
