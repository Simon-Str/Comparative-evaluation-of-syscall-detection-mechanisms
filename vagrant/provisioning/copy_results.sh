#!/bin/bash
BASEDIR=$(pwd)
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo "Working directory: $BASEDIR"
if [ $# -eq 0 ]
  then echo -e "${RED}Provide a binary name${NC}"
  exit 1
fi
if [ $# -eq 1 ] ; then
  echo "Binary: $1"
  BINARY="$1"
fi
cp ./outputs/* /home/vagrant/testbench/results/$BINARY/
cp ./stats/* /home/vagrant/testbench/results/$BINARY/
cp ./outputs/* /home/vagrant/testbench/results/$BINARY/