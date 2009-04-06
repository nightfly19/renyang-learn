# 列出所有的子目錄
SUBDIRS	= ../../lib client/version01 server/01_iterative_server server/02_concurrent_server

# 列出所有的make後面可接的指令
INSTR	= everything all clean distclean

.PHONY: $(INSTR)

# 透過shell的指令到每一個子目錄尋找Makefile並執行其makefile
$(INSTR): $(SUBDIRS)
	for i in $(SUBDIRS); do (cd $$i && make $@);done
