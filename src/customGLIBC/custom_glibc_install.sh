#!/bin/bash 

echo -e "Installing Custom GLIBC for fuzzer approach..."

echo -e "Updating Dependency Bison" 
apt-get install -y  bison  

git clone git://sourceware.org/git/glibc.git
cd glibc 
git checkout glibc-2.35
cp ../libc-start.c ./csu/ 
mkdir build 
cd build 

export glibc_install="$(pwd)/install"

echo -e "Installing Custom GLIBC to $glibc_install"

../configure --prefix "$glibc_install" 
make -j `nproc`
make install -j `nproc`
