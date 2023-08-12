#!/bin/bash
BASEDIR=$(pwd)
RED='\033[0;31m'
CYAN='\033[0;36m' 
NC='\033[0m' # No Color

echo "Working directory: $BASEDIR"
echo -e "${CYAN}Keep in mind that you need to modifiy the app.to.properties.json${NC}"
# e.g. ls.0.5.precodegen.bc
if [ $# -eq 0 ]
  then echo -e "${RED}Provide a binary name${NC}"
  exit 1
fi
if [ $# -eq 1 ] ; then
  echo "Binary: $1"
  BINARY="$1"
fi
if [ $# -ge 2 ]; then
  echo -e "${RED}Provide just 1 argument${NC}"
  exit 1
fi

          