#!/bin/bash
BASEDIR=$(pwd)
RED='\033[0;31m'
CYAN='\033[0;36m' 
NC='\033[0m' # No Color

echo "Working directory: $BASEDIR"
echo -e "${CYAN}Keep in mind that you need to modifiy the app.to.lib.map.json${NC}"
# e.g. ls.0.5.precodegen.bc
if [ $# -eq 0 ]
  then echo -e "${RED}Provide a binary name and optinal the bitcode extension like '.0.5.precodegen.bc'${NC}"
  exit 1
fi
if [ $# -eq 1 ]
  echo "Binary: $1"
  then BC_NAME="$1"
fi
if [ $# -eq 2 ]
  echo "Extension: $2"
  then BC_NAME="$1$2"
  echo "Combined: $BC_NAME"
fi
wpa -print-fp -ander -dump-callgraph $BASEDIR/bitcodes/$BC_NAME
python3 $BASEDIR/convertSvfCfgToHumanReadable.py callgraph_final.dot > $BASEDIR/callgraphs/$1.svf.type.cfg
spa -condition-cfg $BASEDIR/bitcodes/$BC_NAME 2>&1 | tee $BASEDIR/callgraphs/$1.svf.conditional.direct.calls.cfg
spa -simple $BASEDIR/bitcodes/$BC_NAME 2>&1 | tee $BASEDIR/callgraphs/$1.svf.function.pointer.allocations.wglobal.cfg
python3 $BASEDIR/python-utils/graphCleaner.py --fpanalysis --funcname main --output $BASEDIR/callgraphs/$1.svf.new.type.fp.wglobal.cfg --directgraphfile $BASEDIR/callgraphs/$1.svf.conditional.direct.calls.cfg --funcpointerfile $BASEDIR/callgraphs/ls.svf.function.pointer.allocations.wglobal.cfg -c $BASEDIR/callgraphs/ls.svf.type.cfg
mkdir -p outputs; mkdir -p stats
python3 $BASEDIR/createSyscallStats.py -c $BASEDIR/callgraphs/glibc.callgraph --apptopropertymap $BASEDIR/app.to.properties.json --binpath $BASEDIR/binaries/ --outputpath $BASEDIR/outputs/ --apptolibmap $BASEDIR/app.to.lib.map.json --sensitivesyscalls $BASEDIR/sensitive.syscalls --sensitivestatspath $BASEDIR/stats/sensitive.stats --syscallreductionpath $BASEDIR/stats/syscallreduction.stats --libdebloating --othercfgpath $BASEDIR/otherCfgs/ --cfgpath $BASEDIR/callgraphs
exit 0