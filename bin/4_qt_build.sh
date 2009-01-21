#!/bin/bash

# 若不要產生.pro檔,則在./delete 後加隨便一個參數

echo "=================================================================================="
echo "if you don't want to create .pro file, you can add any parameter after instruction"
echo "=================================================================================="

if [ "$1" == "" ]; then
	qmake -project
fi

qmake;make
