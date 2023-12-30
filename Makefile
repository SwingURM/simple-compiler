VMCODE_OUTPUT = vmcode.txt
GRAMMAR_ANALYSIS_OUTPUT = grammar_analysis.txt
PLCODE = simple.pl

grammar: mybison.y test.l util.c codegen.c codegen.h util.h symbol.c symbol.h code.h
	bison -d mybison.y -Wcex --graph
	dot -Tpdf mybison.gv -o mybison.pdf
	flex test.l
	gcc symbol.c codegen.c util.c mybison.tab.c lex.yy.c -o grammar

run: grammar Test1.pl interpreter
	./grammar < ${PLCODE} > ${VMCODE_OUTPUT} 2> ${GRAMMAR_ANALYSIS_OUTPUT}
	./interpreter ${VMCODE_OUTPUT}

interpreter: interpreter.c code.h
	gcc interpreter.c -o interpreter

clean:
	rm mybison.tab.c mybison.tab.h lex.yy.c grammar interpreter mybison.output mybison.gv mybison.pdf ${VMCODE_OUTPUT} ${GRAMMAR_ANALYSIS_OUTPUT}

.PHONY=clean run
