#!/bin/bash 

ROOT=`pwd`
if [ ! -d ../../build/src ];then
		echo "系统还没有编译或者CMAKE目录已经被清空"
		exit 1
fi
cd ../../build/src/CMakeFiles/
lcov -b ./ -d qplus_user.dir -c -o $ROOT/output.info
genhtml $ROOT/output.info -o $ROOT/html

