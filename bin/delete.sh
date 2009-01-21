#!/bin/bash

echo "=================================================================================="
echo "if you don't want to delete .pro file, you can add any parameter after instruction"
echo "=================================================================================="

#記錄目前的目錄
PRESENT_DIRECTION=`pwd`

#開始分析這一行一共有幾欄
father_direction=`echo $PRESENT_DIRECTION | awk 'BEGIN {FS="/"} {print $NF}'`

#開始刪除資料

make clean;rm -fv $father_direction Makefile

# 詢問是否要刪除.pro檔,若指令後面沒有加參數,則表示預設刪除.pro
if [ "$1" == "" ]; then
	rm -fv $father_direction".pro"
fi

