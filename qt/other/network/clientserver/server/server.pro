# 建立一個會產生執行檔的Makefile
TEMPLATE	= app
# 建立的執行檔名稱
TARGET		= server

# qt - 表示是一個Qt應用程序, 並且Qt函式庫會被連接
# warn_on - 表示儘可能多的警告訊息
# release - 表示應用程序將以release模式編譯
CONFIG		+= qt warn_on release

# header file list
HEADERS		=
# .cpp file list
SOURCES		= server.cpp
