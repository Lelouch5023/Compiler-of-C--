%{
/*			lex file			*/
#include "head.h"
#include "qq.tab.h"
#define YYSTYPE pnode
int num_lines = 0;
int num_chars = 0;
extern pht SymbolTable;

%}

blank [ \t\b\r\f]
digit [0-9]
letter [a-zA-Z]
number {digit}+
INT {number}
FLOAT {number}"."{number}
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP (">"|"<"|">="|"<="|"=="|"!=")
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE ("int"|"float")
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT ("struct"|"class")
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
ID ("_"|{letter})({digit}|{letter}|"_")*



%option noyywrap
%option yylineno

%%
"#\n" {	/* Lex Code Here!  */
	return EOP;		// End of Program: defined to mark a entire program
	//Now it is discarded!
}

"//" {
	char c = input();
	while (c != '\n')
		c = input();
}
"/*" {
	int _out = 0;
	char c = input();

	while (_out == 0)
	{
		while (c != '*')
		{
			c = input();
		}
		if ((c = input())=='/')
		{
			_out = 1;
		}
	}//end while
}

{INT} {
	yylval = makenode("INT", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	/*parsing*/
	yylval->typesys->kind = BASIC;
	yylval->typesys->u.basic = INT_TYPE;
	return INT;
}
{FLOAT} {
	yylval = makenode("FLOAT", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	/*parsing*/
	yylval->typesys->kind = FLOAT;
	yylval->typesys->u.basic = FLOAT_TYPE;
	return FLOAT;
}
{SEMI} {
	yylval = makenode("SEMI", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return SEMI;
}
{COMMA} {
	yylval = makenode("COMMA", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return COMMA;
}
{ASSIGNOP} {
	yylval = makenode("ASSIGNOP", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return ASSIGNOP;
}
{RELOP} {
	yylval = makenode("RELOP", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return RELOP;
}
{PLUS} {
	yylval = makenode("PLUS", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return PLUS;
}
{MINUS} {
	yylval = makenode("MINUS", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return MINUS;	
}
{STAR} {
	yylval = makenode("STAR", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return STAR;	
}
{DIV} {
	yylval = makenode("DIV", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return DIV;
}
{AND} {
	yylval = makenode("AND", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return AND;
}
{OR} {
	yylval = makenode("OR", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext); 
	return OR;
}
{DOT} {
	yylval = makenode("DOT", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return DOT;
}
{NOT} {
	yylval = makenode("NOT", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return NOT; 
}
{TYPE} {
	yylval = makenode("TYPE", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	/*parsing*/	
	yylval->typesys->kind = BASIC;
	if (strcmp(yylval->data, "int")==0)
	{
		yylval->typesys->u.basic = INT_TYPE;
	}
	else
	{
		yylval->typesys->u.basic = FLOAT_TYPE;
	}
	return TYPE;
}
{LP} {
	yylval = makenode("LP", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return LP; 
}
{RP} {
	yylval = makenode("RP", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	return RP;
}
{LB} {
	yylval = makenode("LB", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	/*parsing*/
	yylval->typesys->kind = ARRAY;
	return LB; 
}
{RB} {
	yylval = makenode("RB", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);
	/*parsing*/
	yylval->typesys->kind = ARRAY;
	return RB;	
}
{LC} {
	yylval = makenode("LC", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);return LC;	
}
{RC} {
	yylval = makenode("RC", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);return RC;
}
{STRUCT} {
	yylval = makenode("STRUCT", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);return STRUCT; 
}
{RETURN} {
	yylval = makenode("RETURN", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);return RETURN; 
}
{IF} {
	yylval = makenode("IF", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);return IF;
}
{ELSE} {
	yylval = makenode("ELSE", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext); return ELSE;
}
{WHILE} {
	yylval = makenode("WHILE", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);return WHILE;
}
{blank} {
	/* do nothing */
}
{ID} {
	yylval = makenode("ID", 0);
	yylval->type = 1;
	strcpy(yylval->data, yytext);return ID;
}
"\n" {
	/**/
}
. {
	printf("Mysterious character %c\n", *yytext);
}

%%

