#!/bin/bash
BASEDIR=$(pwd)
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

rm $BASEDIR/stats/* && echo "Deleted: $BASEDIR/stats"
rm $BASEDIR/outputs/* && echo "Deleted: $BASEDIR/outputs"
rm $BASEDIR/callgraphs/ls*; rm $BASEDIR/callgraphs/file ; rm $BASEDIR/callgraphs/diff
exit 0