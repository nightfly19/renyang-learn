# 使用的場合是當以下的指令常常出現的話,就可以把它合併成一個指令
# 因此當要執行run-yacc時,會同時執行
# echo "Hello World"
# 與
# mv foo.y $@
# 兩個指令
define run-yacc
touch foo.y
mv foo.y $@
endef

foo.c:
	$(run-yacc)

distclean:
	rm -fv foo.c foo.y
