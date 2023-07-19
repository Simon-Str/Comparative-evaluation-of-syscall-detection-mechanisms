project(ProjektarbeitHieberLena)

cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE true)
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/CMakeModules)

FIND_PACKAGE(Libaudit REQUIRED)
FIND_PACKAGE(Auparse REQUIRED)

INCLUDE_DIRECTORIES(${LIBAUDIT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${AUPARSE_INCLUDE_DIR})
LINK_DIRECTORIES(/usr/local/lib)

add_executable(LogEvaluation LogEvaluation.cpp AuditInformations.h
        AuparseFunctions.h AuparseFunctions.cpp AuditctlFunctions.h AuditctlFunctions.cpp)
TARGET_LINK_LIBRARIES(LogEvaluation ${LIBAUDIT_LIBRARY} ${AUPARSE_LIBRARY})

add_executable(PluginEvaluation PluginEvaluation.cpp AuditInformations.h AuditctlFunctions.h AuditctlFunctions.cpp)
TARGET_LINK_LIBRARIES(PluginEvaluation ${LIBAUDIT_LIBRARY})

add_executable(AudispPlugin AudispPlugin.cpp AuparseFunctions.h AuparseFunctions.cpp AuditInformations.h)
TARGET_LINK_LIBRARIES(AudispPlugin ${AUPARSE_LIBRARY})
