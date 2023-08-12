#!/bin/bash
BASEDIR=$(pwd)
RED='\033[0;31m'
CYAN='\033[0;36m' 
NC='\033[0m' # No Color

echo "Working directory: $BASEDIR"
echo -e "${CYAN}Keep in mind that you need to modifiy the app.to.properties.json${NC}"
# e.g. ls.0.5.precodegen.bc
if [ $# -eq 0 ]
  then echo -e "${RED}Provide a binary name and optional the bitcode extension like '.0.5.precodegen.bc'${NC}"
  exit 1
fi
if [ $# -eq 1 ] ; then
  echo "Binary: $1"
  BC_NAME="$1.bc"
  BINARY="$1"
fi
if [ $# -eq 2 ]; then
  echo "Extension: $2"
  BC_NAME="$1$2"
  BINARY="$1"
  echo "Combined: $BC_NAME"
fi
# Main callgraph with vanilla SVF
export SVF_HOME=/home/vagrant/temporal-specialization/SVF-SVF-1.7/; export PATH=$SVF_HOME/Release-build/bin:$PATH
wpa -ander -dump-callgraph $BASEDIR/bitcodes/$BC_NAME
python3 $BASEDIR/convertSvfCfgToHumanReadable.py callgraph_final.dot > $BASEDIR/callgraphs/$BINARY.svf1-7.cfg
export SVF_HOME=/home/vagrant/temporal-specialization/SVF/; export PATH=$SVF_HOME/Release-build/bin:$PATH
wpa -print-fp -ander -dump-callgraph $BASEDIR/bitcodes/$BC_NAME
python3 $BASEDIR/convertSvfCfgToHumanReadable.py callgraph_final.dot > $BASEDIR/callgraphs/$BINARY.svf.type.cfg
spa -condition-cfg $BASEDIR/bitcodes/$BC_NAME 2>&1 | tee $BASEDIR/callgraphs/$BINARY.svf.conditional.direct.calls.cfg
spa -simple $BASEDIR/bitcodes/$BC_NAME 2>&1 | tee $BASEDIR/callgraphs/$BINARY.svf.function.pointer.allocations.wglobal.cfg
python3 $BASEDIR/python-utils/graphCleaner.py --fpanalysis --funcname main --output $BASEDIR/callgraphs/$BINARY.svf.new.type.fp.wglobal.cfg --directgraphfile $BASEDIR/callgraphs/$BINARY.svf.conditional.direct.calls.cfg --funcpointerfile $BASEDIR/callgraphs/$BINARY.svf.function.pointer.allocations.wglobal.cfg -c $BASEDIR/callgraphs/$BINARY.svf.type.cfg
mkdir -p $BASEDIR/outputs; mkdir -p $BASEDIR/stats
python3 $BASEDIR/change_appToProperties.py -i $BASEDIR/app.to.properties.json -b $BINARY
python3 $BASEDIR/createSyscallStats.py -c $BASEDIR/callgraphs/glibc.callgraph --apptopropertymap $BASEDIR/app.to.properties.json --binpath $BASEDIR/binaries/ --outputpath $BASEDIR/outputs/ --apptolibmap $BASEDIR/app.to.lib.map.json --sensitivesyscalls $BASEDIR/sensitive.syscalls --sensitivestatspath $BASEDIR/stats/sensitive.stats --syscallreductionpath $BASEDIR/stats/syscallreduction.stats --libdebloating --othercfgpath $BASEDIR/otherCfgs/ --cfgpath $BASEDIR/callgraphs
exit 0