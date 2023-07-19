#!/bin/bash 

set -e
set -u
set -o pipefail
# Set parameters to let script Fail gracefully https://gist.github.com/csantanapr/bfd94b5f6b155c979ea0ce4fb3f10165

DIR="$(cd "$(dirname "$0")" && pwd)"

echo -e "[+] [8] INSTALL ninja Build System"
sudo apt install -yy ninja-build

echo -e "[+] [8] INSTALL ninja Build System"
sudo apt install -yy ninja-build

echo -e "[+] [8] Update System"
sudo apt-get update && sudo apt-get upgrade -y
echo -e "[+] [8] INSTALL Clang 12"
sudo apt-get install -y clang-12 clang-tools-12 libc++1-12 libc++-12-dev \
    libc++abi1-12 libc++abi-12-dev libclang1-12 libclang-12-dev \
    libclang-common-12-dev libclang-cpp12 libclang-cpp12-dev liblld-12 \
    liblld-12-dev liblldb-12 liblldb-12-dev libllvm12 libomp-12-dev \
    libomp5-12 lld-12 lldb-12 llvm-12 llvm-12-dev llvm-12-runtime llvm-12-tools

echo -e "[+] [8] set Variables for Paths in Ubuntu 21"

export LLVM_CONFIG="/usr/bin/llvm-config-12"
echo -e "[+] [8] Set LLVM_CONFIG to /usr/bin/llvm-config-12 "

export LD_LIBRARY_PATH="$(llvm-config-12 --libdir)${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

#git clone https://github.com/vanhauser-thc/AFLplusplus.git
# Changed due to new repository name 

echo -e "(+) Check if Install Base Directory already exists"
if [ -d "AFLplusplus" ]; then 
  if [ -L "AFLplusplus" ]; then
    # It is a symlink!
    # Symbolic link specific commands go here.
    echo -e "(+) Base Directory already exists and is a symlink"
    exit 0
  else
    # It's a directory!
    # Directory command goes here
    echo -e "(+) Base Directory already exists, so we will use it"
    exit 0
  fi
else
  echo -e "[+] clone AFLplusplus into the current directory"
  git clone https://github.com/AFLplusplus/AFLplusplus
fi

# Maybe later compare to https://github.com/AFLplusplus/LibAFL 
#echo -e "[+] INSTALLING AFLplusplus into the current directory"
#git clone https://github.com/AFLplusplus/AFLplusplus
cd AFLplusplus
echo -e "[+] INSTALL REQUIRED DEPENDENCIES"
# Changed August 2021
#echo -e "[+] [0] INSTALL clang-6 und clang-tools-6.0"
#sudo apt install -yy clang-6.0
#sudo apt install -yy clang-tools-6.0
# Changed Sep 2021
#echo -e "[+] [0] INSTALL clang-12 und clang-tools-12.0"
#sudo apt install -yy clang
#sudo apt install -yy clang-12
echo -e "[+] [1] INSTALL LIBTOOL-BIN"
sudo apt install -yy libtool-bin
echo -e " "
echo -e "[+] [2] INSTALL AUTOMAKE"
sudo apt install -yy automake
echo -e " "
echo -e "[+] [3] INSTALL PYTHON SETUPTOOLS"
echo -e " "
sudo apt install python3-setuptools
echo -e " "
#echo -e "[+] [x] INSTALL librust-glib+v2-58-dev"
#sudo apt install -yy librust-gilb+v2-58-dev
#sudo apt install -yy python-gi
#sudo apt install -yy python3-gi
echo -e "[+] [4] INSTALL LIBGLIB2.0-DEV"
sudo apt install -yy libglib2.0-dev
sudo apt install -yy libglib2.0-dev-bin
echo -e " "
#sudo apt install -yy libglibmm-2.4-dev
#sudo apt install -yy libmbim-glib-dev

#sudo apt install -yy libpackagekit-glib2-dev
#sudo apt install -yy linux-libc-dev
echo -e "[+] [5] INSTALL LIBPIXMAN-1-DEV"
sudo apt install -yy libpixman-1-dev
echo -e " "
echo -e "[+] [6] INSTALL BISON"
sudo apt install -yy bison
echo -e "[+] [7] INSTALL g++ MULTILIB"
sudo apt install -yy g++-multilib
echo -e "[+] [8] INSTALL gcc-x-plugin-dev"
#sudo apt install -yy gcc-9-plugin-dev
sudo apt install -yy gcc-11 
sudo apt install -yy gcc-11-plugin-dev
sudo apt install -yy gcc-11-multilib
sudo apt install -yy g++-11
sudo apt install -yy g++-11-multilib
echo -e "[+] [8] INSTALL cmocka for Tests later"
sudo apt install -yy libcmocka0 libcmocka-dev libkeyutils1 libkeyutils-dev fuse3 capstone-tool

#echo -e "[+] [9] MAKE SOURCE ONLY"
# Adjust Skript
sudo apt-get install -yy build-essential python3-dev automake git flex bison libglib2.0-dev libpixman-1-dev python3-setuptools

# Changed Sep 2021
# try to install llvm 11 and install the distro default if that fails
#echo -e "[+] [8] INSTALL LLD-11"
#sudo apt-get install -yy lld-11
#echo -e "[+] [8] INSTALL LLD-12"
#sudo apt-get install -yy llvm-12
#echo -e "[+] [8] INSTALL LLVM-12-DEV"
#sudo apt-get install -yy llvm-12-dev
#echo -e "[+] [8] INSTALL Clang-12"
#sudo apt-get install -yy llvm-12-linker-tools llvm-runtime llvm-dev llvm-12-runtime llvm-12-tools




sudo apt-get install -yy gcc-$(gcc --version|head -n1|sed 's/.* //'|sed 's/\..*//')-plugin-dev libstdc++-$(gcc --version|head -n1|sed 's/.* //'|sed 's/\..*//')-dev
echo -e "[+] [9] MAKE DISTRIB"
make distrib
#sudo make install
# Adjust Skript End
#sudo make source-only
#echo -e "[+] [10] MAKE TESTS"
#sudo make tests
echo -e "[+] [11] MAKE INSTALL"
sudo make install

echo -e "[+] [14] Install gcovr"
sudo apt install -yy gcovr

echo -e "[+] [12] Install lcov"
sudo apt install -yy lcov
echo -e "[+] [13] Install python-subprocess32 via pip3"
#sudo apt install -yy python-subprocess32
sudo pip3 install subprocess32
echo -e "[+] [13] Install genhtml via pip3"
pip3 install genhtml
#echo -e "[+] [15] Get afl-cov Version VANHAUSER-THC"
#git clone https://github.com/vanhauser-thc/afl-cov.git
#cd afl-cov
#echo -e "[+] [15] configure afl-cov for CLANG"
#./afl-cov-build.sh -c
#echo -e "[+] [15] install afl-cov for CLANG"
#sudo make install
#echo -e "[+] [15] Get afl-cov Version 0.6.1-2all deb x64"
#curl -O http://archive.ubuntu.com/ubuntu/pool/universe/a/afl-cov/afl-cov_0.6.1-2_all.deb
#echo -e "[+] [16] dpkg install afl-cov"
#sudo dpkg -i afl-cov_0.6.1-2_all.deb

echo -e "[+] [17] USAGE of afl-cov: Compile Source with:"
echo -e "[Optional] export AFL_USE_ASAN=1 and/or export AFL_USE_UBSAN=1"
echo -e "[required] make CC=afl-gcc CXX=afl-gcc++ LD=afl-gcc CFLAGS='-g -O1 -fprofile-arcs -ftest-coverage' CXXFLAGS='g -O1 -fprofile-arcs -ftest-coverage' LDFLAGS='-fprofile-arcs"
echo -e "[Command Example for afl-cov]: "
echo -e " ./afl-cov -d /home/j/Tutorial/libxml2/fuzz/out --coverage-cmd \"cat AFL_FILE | /home/j/Tutorial3ggov/libxml2/xmllint\" --code-dir /home/j/Tutorial3ggov/libxml2/ --overwrite --coverage-at-exit --enable-branch-coverage"
echo -e " "

echo -e " (select afl output dir) -d '/directory_with_afl_Fuzzing_output'"
echo -e " (optional of live coverage, paralell to afl) --live" 
echo -e " (select binary which was or is executed from afl) --coverage-cmd \"cat AFL_FILE | /directory_with_compiled_source_with_coverage_support/executed_binary\" "
echo -e " (select path to code directoy (with coverage) --code-dir /directory_with_compiled_source_with_coverage/ "
echo -e "--overwrite --coverage-at-exit --enable-branch-coverage"
echo -e " "
echo -e " Hint1: -d Parameter shouldn't end with '/'"
echo -e " Hint2: if afl-cov results in Error check if ggov was installed and if gcc, gcov and lcov are on the same version "
echo -e " "
echo -e " Honk! "





