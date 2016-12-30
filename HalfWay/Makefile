syntax: pp.l qq.y syntree.c hashtable.c typesys.c head.h
	flex pp.l
	bison -d qq.y
	gcc -o syntax hashtable.c typesys.c  syntree.c qq.tab.c lex.yy.c -ll
	#rm qq.tab.*
	#rm lex.yy.c







