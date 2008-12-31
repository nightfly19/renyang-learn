#!/bin/sh

#記錄目前的目錄
PRESENT_DIRECTION=`pwd`

#開始分析這一行一共有幾欄
father_direction=`echo $PRESENT_DIRECTION | awk 'BEGIN {FS="/"} {print $NF}'`

#開始刪除資料
make clean;rm -fv $father_direction $father_direction".pro" Makefile
