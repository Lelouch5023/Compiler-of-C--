#######

.PHONY: hcc run clean

USERPROGRAM := testcase/3_3

hcc: pp.l qq.y syntree.c dfvisit.c hashtable.c typesys.c intercode.c targetcode.c head.h 
	flex pp.l
	bison -d qq.y
	gcc -o hcc targetcode.c intercode.c hashtable.c typesys.c  syntree.c dfvisit.c qq.tab.c lex.yy.c -ll 
	@mkdir -p bin
	@mv hcc bin/
	@rm qq.tab.*
	@rm lex.yy.c

clean:
	-rm -rf qq.tab.* 2> /dev/null
	-rm -rf lex.yy.c 2> /dev/null
	-rm -rf bin/ 2> /dev/null
	-rm -rf *.s 2> /dev/null
	-rm -rf IC.txt 2> /dev/null


run: hcc
	@mkdir -p obj
	./bin/hcc $(USERPROGRAM).c
	@echo "-----------------------Original Sourcefile----------------------"
	@cat $(USERPROGRAM).c
	@echo "-----------------------Assembly Mips Codes----------------------"
	@cat mips.s
	@mkdir -p obj obj/testcase
	@cp mips.s obj/$(USERPROGRAM).s
	@cp IC.txt obj/$(USERPROGRAM)_intercode.txt
	@echo "-----------------------Now     we     run!----------------------"
	spim -f mips.s
	
