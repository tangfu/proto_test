#!/bin/bash

killall -9 proto_test

BASE_PATH=$(dirname `which $0`)
cd ${BASE_PATH}

sleep 1
../build/src/proto_test -conf=proto_test.conf -logbufsecs=0 -v=1

