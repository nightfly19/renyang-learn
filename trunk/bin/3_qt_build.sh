#!/bin/bash

echo "=================================================================================="
echo "if you don't want to create .pro file, you can add any parameter after instruction"
echo "=================================================================================="

if [ "$1" == "" ]; then
	qmake-qt3 -project
fi

qmake-qt3;make
