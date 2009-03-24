#!/bin/bash

# 在執行程式時,因為系統會跑去預設的地方找shared library(libmylib.so.1)
# 有幾個方式可以處理.
# a.把libmylib.so.1安裝到系統的library目錄,如/usr/lib下
# b.設定/etc/ld.so.conf,加入一個新的library搜尋目錄,並執行ldconfig更新快取
# c.設定LD_LIBRARY_PATH環境變數來搜尋library:LD_LIBRARY_PATH=.

# 請透過以下指令執行此檔案
# $ source readme.sh

export LD_LIBRARY_PATH=.
