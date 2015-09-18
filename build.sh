#!/usr/bin/env bash
# Author: Soonho Kong (soonhok@cs.cmu.edu)

OS=`uname`
echo OS:  $OS

########################################################################
# Find C++11 Compiler and C Compiler
########################################################################
for CXX in ccache-g++ g++-4.8 g++-4.9 ccache-clang++ clang++-3.5 clang++-3.4 clang++-3.3
do
    CXX_PATHNAME=`which $CXX`
    if [ -e "${CXX_PATHNAME}" ]; then
        echo CXX: ${CXX} found at ${CXX_PATHNAME}...
        break;
    fi
done
CXX=clang++
CLANG=`which $CXX`
if [ -e "${CLANG}" ] && [ "3.3" = "$(echo -e $(clang --version | head -n1 | awk '{print $3}')'\n'3.3 | sort -V | head -n1)" ]; then
    echo CXX: compatible clang found at ${CLANG}...
    CXX_PATHNAME=${CLANG}
fi
if [ ! -e "${CXX_PATHNAME}" ]; then
    cat <<EOF
It seems that C++11-compatible compilers are not installed on your system.
Please install either g++ 4.8 (or newer) or clang++ 3.3 (or newer).
EOF
    exit 1
fi
for CC in gcc-4.8 gcc-4.9 clang-3.5 clang-3.4 clang-3.3 clang
do
    CC_PATHNAME=`which $CC`
    if [ -e "$CC_PATHNAME" ]; then
        echo CC:  $CC found at ${CC_PATHNAME}...
        break;
    fi
done

########################################################################
# Find oasis, opam, and ocaml
#######################################################################
for OCAMLTOOL in oasis opam ocamlc ocaml
do
    PATHNAME=`which $OCAMLTOOL`
    if [ ! -e "${PATHNAME}" ]; then
        echo Ocaml Tool: ${OCAMLTOOL} is not found. Please install ${OCAMLTOOL}.
        exit 1
    fi
done

########################################################################
# Build Solver (C++)
########################################################################
if [ ! -d build ]; then
    mkdir build
fi
cd build
cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_C_COMPILER=$CC -DCMAKE_BUILD_TYPE=RELEASE ../src
make -j 2
cd ../

########################################################################
# Build Tools (Ocaml)
########################################################################
cd tools
make
cd ..
