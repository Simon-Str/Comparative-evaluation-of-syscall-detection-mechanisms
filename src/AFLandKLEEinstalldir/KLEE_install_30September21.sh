#!/bin/bash 


set -e
set -u
set -o pipefail
# Set parameters to let script Fail gracefully https://gist.github.com/csantanapr/bfd94b5f6b155c979ea0ce4fb3f10165

DIR="$(cd "$(dirname "$0")" && pwd)"

show_help() {
  local name="$1"
  local installpath="$DIR"
  echo "$name [--install-KLEE] Install KLEE with all Dependencies from Source to $installpath. To change the Installation Directory, please move the Install-script to the desired Location and execute it there" 

  return 0
}

USAGE="
INSTALL KLEE
"
cd $DIR
echo -e "(+) Check if Install Base Directory already exists"
if [ -d "KLEE1" ]; then 
  if [ -L "KLEE1" ]; then
    # It is a symlink!
    # Symbolic link specific commands go here.
    echo -e "(+) Base Directory already exists and is a symlink"
    exit 0
  else
    # It's a directory!
    # Directory command goes here
    echo -e "(+) Base Directory already exists and will be deleted"
    rmdir "KLEE1"
    exit 0
  fi
fi

echo -e "(+) Create Install Base Directory"
mkdir KLEE1
echo -e "(+) Change Current Working Dir to Install Base Directory"
cd KLEE1

echo -e "(+) Install Python2-dev"
sudo apt install -yy python2-dev
echo -e "(+) Install Python3-dev"
sudo apt install -yy python3-dev

echo -e "(+) Install Python3-tabulate"
sudo apt install -yy python3-tabulate
# Error
#echo -e "(+) Install Python2-tabulate"
#sudo apt install -yy python-tabulate
# Error
echo -e "(+) Install Python3-pip"
sudo apt install -yy python3-pip
echo -e "(+) Install PIP3 Tabulate"
sudo pip3 install tabulate
echo -e "(+) Install Python2-pip"
# Taken out 09.2021
#sudo apt install -yy python-pip
# Added Multilibs 2021 
echo -e "(+) Install gcc-multilib"
sudo apt install -yy gcc-multilib
echo -e "(+) Install g++-multilib"
sudo apt install -yy g++-multilib
echo -e "(+) Install Build-essentials Package"
sudo apt install -yy build-essential
echo -e "(+) Install Build-essentials Package"
sudo apt install -yy curl 
echo -e "(+) Install libcap-dev"
sudo apt install -yy libcap-dev
echo -e "(+) Install git"
sudo apt install -yy git
echo -e "(+) Install cmake"
sudo apt install -yy cmake
echo -e "(+) Install libncurses5-dev"
sudo apt install -yy libncurses5-dev
echo -e "(+) Install unzip"
sudo apt install -yy unzip
echo -e "(+) Install libtcmalloc-minimal4"
sudo apt install -yy libtcmalloc-minimal4
echo -e "(+) Install libgoogle-perftools-dev"
sudo apt install -yy libgoogle-perftools-dev
echo -e "(+) Install libsqlite3-dev Package"
sudo apt install -yy libsqlite3-dev
echo -e "(+) Install doxygen Package"
sudo apt install -yy doxygen
echo -e "(+) Install clang-9 (wird seit 3. März 2020 unterstützt (LLVM Version 9)"
sudo apt install -yy clang-9
echo -e "(+) Install clang-tools-9 (wird seit 3. März 2020 unterstützt (LLVM Version 9)"
sudo apt install -yy clang-tools-9
echo -e "(+) Install clang"
sudo apt install -yy clang
echo -e "(+) Install llvm-9 Added 2021 "
sudo apt install -yy llvm-9
echo -e "(+) Install llvm-9-dev (wird seit 3. März 2020 unterstützt (LLVM Version 9)"
sudo apt install -yy llvm-9-dev
echo -e "(+) Install llvm-9-tools (wird seit 3. März 2020 unterstützt (LLVM Version 9)"
sudo apt install -yy llvm-9-tools
echo -e "(+) Install llvm-9-runtime (wird seit 3. März 2020 unterstützt (LLVM Version 9)"
sudo apt install -yy llvm-9-runtime

# Changed 20-05-2021
#echo -e "(+) Install llvm and llvm-dev"
#sudo apt install -yy llvm llvm-dev
# End Change 20-05-2021
#sudo apt install -yy llvm-6.0-runtime

# Build with Z3 as CSP
echo -e "(+) Clone Z3 Solver in Directory z3"
git clone https://github.com/Z3Prover/z3.git
echo -e "(+) Change Current Working Directory to z3"
cd z3
echo -e "(+) Build Z3 with python Skript using clang++ and clang"
CXX=clang++-9 CC=clang-9 python3 scripts/mk_make.py
echo -e "(+) Change Current Working Directory to build Directory which was Created with Installation Skript"
cd build
echo -e "(+) make z3 witch -j(nproc) Threads "
make -j$(nproc)
echo -e "(+) Install z3"
sudo make install
echo -e "(+) Change Current Working Directory to KLEE1"
cd ..

cd ..
echo -e "(+) Clone uclibc to klee-uclibc"
git clone https://github.com/klee/klee-uclibc.git
echo -e "(+) Hack for Issue 30 __isoc99scanf.c (Copy scanf.c and rename)"
cp $DIR/KLEE1/klee-uclibc/libc/stdio/scanf.c $DIR/KLEE1/klee-uclibc/libc/stdio/__isoc99_scanf.c
echo -e "(+) Change of MakefileIN to inlcude __isoc (1)"
(sed '71 a \ \ \ \ \ \ \ \ __isoc99_scanf.c \\' $DIR/KLEE1/klee-uclibc/libc/stdio/Makefile.in) 2>&1 | tee $DIR/KLEE1/klee-uclibc/libc/stdio/Makefile1.in
echo -e "(+) Change of MakefileIN to inlcude __isoc (2)"
rm $DIR/KLEE1/klee-uclibc/libc/stdio/Makefile.in
echo -e "(+) Change of MakefileIN to inlcude __isoc (3)"
mv $DIR/KLEE1/klee-uclibc/libc/stdio/Makefile1.in $DIR/KLEE1/klee-uclibc/libc/stdio/Makefile.in
echo -e "(+) Change Current Working Directory to klee-uclibc"
cd klee-uclibc
echo -e "(+) Set CXX to Clang++9"
CXX=clang++-9 
export CXX
echo -e "(+) Set CC to Clang 9"
CC=clang-9
export CC
echo -e "(+) configure with --make-llvm-lib"
#./configure --make-llvm-lib
# Test 20.05.2021
./configure --make-llvm-lib --with-llvm-config /bin/llvm-config-9
echo -e "(+) Make uclibc"
make -j2
echo -e "(+) Change Current Working Directory to KLEE1"
cd ..


echo -e "(+) Install wllvm with pip3"
sudo pip3 install wllvm
echo -e "(+) Download googletests"
curl -OL https://github.com/google/googletest/archive/release-1.7.0.zip
echo -e "(+) unzip googletests"
unzip release-1.7.0.zip
echo -e "(+) Install lit with pip3"
sudo pip3 install lit



echo -e "(+) Clone klee Sourcefiles from github in klee Directory"
git clone https://github.com/klee/klee.git
echo -e "(+) Change Current Working Directory to klee"
cd klee
echo -e "(+) CREATE libc+ Directory for libc++ Support for klee"
mkdir libc+
echo -e "(+) Set Building Variables for libc++ Support and execute libcxx build Script with memory Sanitizer"
LLVM_VERSION=9 SANITIZER_BUILD=memory ENABLE_OPTIMIZED=1 ENABLE_DEBUG=0 DISABLE_ASSERTIONS=1 REQUIRES_RTTI=1 BASE=$DIR/KLEE1/klee/libc+/ ./scripts/build/build.sh libcxx
echo -e "(+) Create Build Directory: build"
mkdir build
echo -e "(+) Change Current Working Directory to build"
cd build
# Added Fix libc++install-9 --> libc++-install-90 in 2 Places
# Added KLEE’s exception handling support for C++ ; See https://klee.github.io/build-llvm9/  $DIR/KLEE1/klee/libc+/libc++-install-90/lib/ $DIR/KLEE1/klee/libc+/llvm-90/libcxxabi
echo -e "(+) Configure Build of KLEE with Z3, uclibc and libc++ Support"
cmake -DENABLE_SOLVER_STP=OFF -DENABLE_POSIX_RUNTIME=ON -DENABLE_KLEE_UCLIBC=ON -DKLEE_UCLIBC_PATH=$DIR/KLEE1/klee-uclibc -DENABLE_KLEE_LIBCXX=ON -DKLEE_LIBCXX_DIR=$DIR/KLEE1/klee/libc+/libc++-install-90/ -DKLEE_LIBCXX_INCLUDE_DIR=$DIR/KLEE1/klee/libc+/libc++-install-90/include/c++/v1/ -DENABLE_KLEE_EH_CXX=ON -DKLEE_LIBCXXABI_SRC_DIR=$DIR/KLEE1/klee/libc+/llvm-90/libcxxabi/ -DENABLE_UNIT_TESTS=ON -DGTEST_SRC_DIR=$DIR/KLEE1/googletest-release-1.7.0/ -DLLVM_CONFIG_BINARY=/bin/llvm-config-9 -DLLVMCC=/bin/clang-9 -DLLVMCXX=/bin/clang++-9 $DIR/KLEE1/klee/
echo -e "(+) Execute make"
make
echo -e "(+) Install KLEE"
sudo make install
echo -e "(+) Run Checks on KLEE Build"
make check
echo -e "(+) Set ulimit -n 99999 (This isn't permanent and needs to be changed in the System Configuration)"
ulimit -n 999999
