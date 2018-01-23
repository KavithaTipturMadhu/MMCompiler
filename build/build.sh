#!/bin/bash
#file=./Makefile
#
#if [-f "$file"]
#then
#	make -j4 CXXFLAGS="-I $PWD/../ -L $PWD/../lpsolve"
#else
# ../configure --prefix=$PWD/../
	make -j4 CXXFLAGS="-std=c++11 -I $PWD/../ -L $PWD/../lpsolve -frtti"
#fi
