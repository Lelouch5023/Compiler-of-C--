%locations
%error-verbose
%{
#include "Node.h"
extern int yylineno;


int display(Pnode T,int indent);
int num=0;
char type[32];
char rtype[32];//记录返回值类型
char tmp[32];
Pfunc tmp_func;
Pstruc tmp_struc;
%}

%union{
	struct Node* pnode;
};
/*declare tokens*/
%token  <pnode> INT FLOAT
%token <pnode> TYPE STRUCT RETURN IF ELSE WHILE ID SPACE SEMI COMMA ASSIGNOP RELOP PLUS
MINUS STAR DIV AND OR DOT NOT LP RP LB RB LC RC
%type  <pnode> Program ExtDefList ExtDef ExtDecList Specifire StructSpecifire 
OptTag  Tag VarDec  FunDec VarList ParamDec Compst StmtList Stmt DefList Def DecList Dec Exp Args
/*priority*/
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT 
%left LP RP LB RB DOT
%%
Program:ExtDefList {$$=new_N("Program",$1);$$->lineno=@1.first_line;/*display($$,0);*/}

    ;
ExtDefList:ExtDef ExtDefList {$$=new_N("ExtDefList",$1);$$->lineno=@1.first_line;$1->r=$2;}
	| {$$=new_c("",0,0,"");}
	;
ExtDef:Specifire ExtDecList SEMI    
	{
		$$=new_N("ExtDef",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
	}    
	|Specifire SEMI	
	{
		$$=new_N("ExtDef",$1);$$->lineno=@1.first_line;$1->r=$2;
	}
	|Specifire FunDec SEMI
	{
		$$=new_N("ExtDef",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(isFuncDef($2->content,&tmp_func)){
			printf("Error at Line %d:Redefined func \"%s\"\n",@1.first_line,$2->content );
		}
		else{
			if(tmp_func==NULL)
				add_funcSym($1->type,$2->content,"",@1.first_line,1);//函数声明时无法获取返回值,1表明是声明
			else
			{
				Pvar var=tmp_func->VarList;
					while(var&&VarList){
						if(strcmp(var->name,VarList->name)||strcmp(var->type,VarList->type)){
							printf("Error type 19 at line %d:inconsistent declaration of function \"%s\"\n",@1.first_line,$2->content );
							break;
						}
						VarList=VarList->next;
						var=var->next;
					}
			}
		}

		VarList=NULL;
		pnum=0;
	}
	|Specifire FunDec Compst	
	{
		$$=new_N("ExtDef",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(($1->type)&&($3->type)){

			if(strcmp($1->type,$3->type)&&!strcmp($3->content,"return")){//从compst中获取返回值类型
				printf("Error type 8 at %d: Type mismatched for return\n",@1.first_line );
			}
			if(isFuncDef($2->content,&tmp_func))
				printf("Error type 4 at Line %d: Redefined function \"%s\"\n",@1.first_line,$2->content );
			else
			{
				if(tmp_func==NULL){//未声明，未定义
					add_funcSym($1->type,$2->content,$3->type,@1.first_line,2);//倒数第二个是函数的返回值
				}
				else if(tmp_func->isDef==1){//已经声明了
					Pvar var=tmp_func->VarList;
					while(var&&VarList){
						if(strcmp(var->name,VarList->name)||strcmp(var->type,VarList->type)){
							printf("Error type 19 at line %d:inconsistent declaration of function \"%s\"\n",@1.first_line,$2->content );
							break;
						}
						VarList=VarList->next;
						var=var->next;
					}
					tmp_func->rtype=$3->type;
				}

			}//倒数第二个是函数的返回值,2表示是定义
		}
		
		else{
			if(isFuncDef($2->content,&tmp_func))
				printf("Error type 4 at Line %d: Redefined function \"%s\"\n",@1.first_line,$2->content );
			else{
				if(tmp_func){
					if(tmp_func->isDef==1){
						Pvar var=tmp_func->VarList;
						
						while(var&&VarList){

							if(strcmp(var->name,VarList->name)||strcmp(var->type,VarList->type)){
								printf("Error type 19 at line %d:inconsistent declaration of function \"%s\"\n",@1.first_line,$2->content );
								break;
							}

							VarList=VarList->next;
							var=var->next;
						}
						tmp_func->rtype=$3->type;
					}

				}
				else
					add_funcSym($1->type,$2->content,"",@1.first_line,2);//倒数第二个是函数的返回值
			}
		}
		pnum=0;//将参数个数重新清零
		VarList=NULL;
	}
	;
ExtDecList:VarDec
		   {
				$$=new_N("ExtDecList",$1);$$->lineno=@1.first_line;
				if(!strcmp($1->type,"array")){
					if(isArrDef($1->content,tmp)){
						printf("Error type 3 at line %d: Redefined array \" %s\"\n",@1.first_line,$1->content );
					}
					else
						add_arraySym( $1->content,type,$1->i);
				}
				else{
					if(isVarDef($1->content,tmp)){
					printf("Error type 3 at line %d: Redefined variable \" %s\"\n",@1.first_line,$1->content );
					}
					else
						add_varSym( $1->content,type);
				}
						
			}
	|VarDec COMMA ExtDecList 
	 {
	 	$$=new_N("ExtDecList",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(!strcmp($1->type,"array")){
			if(isArrDef($1->content,tmp)){
			printf("Error type 3 at line %d: Redefined array \" %s\"\n",@1.first_line,$1->content );
			}
			else
				add_arraySym( $1->content,type,$1->i);
		}
		else{
			if(isVarDef($1->content,tmp)){
			printf("Error type 3 at line %d: Redefined variable \" %s\"\n",@1.first_line,$1->content );
			}
			else
				add_varSym( $1->content,type);
		}
	}
	;
/*Specifire*/
Specifire:TYPE 	{
					$1->type=$1->content;

					strcpy(type,$1->type);
					$$=new_N("Specifire",$1);$$->lineno=@1.first_line;
				}
	|StructSpecifire
	{
		$$=new_N("Specifire",$1);$$->lineno=@1.first_line;
	}
	;
StructSpecifire:STRUCT OptTag LC DefList RC 
				{

					$$=new_N("StructSpecifire",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;$4->r=$5;
					strcpy(tmp,"struct ");
					strcat(tmp,$2->content);
					if(isStrucDef(tmp,&tmp_struc)){
						printf("Error type 16 at line %d:Duplicated name \"%s\"\n",@1.first_line,$2->content );
					}
					else{
						add_strucSym(tmp,"struct");
					}
					pnum=0;
					VarList=NULL;
					inStruc=0;
				}
	|STRUCT Tag 
	{
		strcpy(tmp,"struct ");
		strcat(tmp,$2->content);
		if(!isStrucDef(tmp,&tmp_struc))
			printf("Error 17 at line %d:undefined structure \"%s\"\n", @1.first_line,$2->content);
		else{
			strcpy(type,"struct ");
			strcat(type,$2->content);
			$1->type=(char *)malloc(sizeof(34));
			strcpy($1->type,type);
		}
		$$=new_N("StructSpecifire",$1);$$->lineno=@1.first_line;$1->r=$2;
		inStruc=0;
	}
	;
OptTag:ID {$$=new_N("OptTag",$1);$$->lineno=@1.first_line;}
	|{$$=new_c("",0,0,"");}
	;
Tag:ID {$$=new_N("Tag",$1);$$->lineno=@1.first_line;}
	;
/*Declarators*/
VarDec:ID {
			$1->type=(char *)malloc(strlen(type)+1);
			strcpy($1->type,type);
			$$=new_N("VarDec",$1);$$->lineno=@1.first_line;
		  }
	| ID LB INT RB 
	{
		$$=new_N("VarDec",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;
		$$->type= (char *)malloc(30);
		$$->content=(char *)malloc(30);
		strcpy($$->content,$1->content);
		strcpy($$->type,"array");
		$$->i=$3->i;
	}
	;
FunDec:ID LP VarList RP 
		{
			$1->type=(char *)malloc(strlen(type)+1);
			strcpy($1->type,type);
			/*while(VarList){
				printf("%s %s %d\n",VarList->name,VarList->type ,pnum);
				VarList=VarList->next;
			}*/
			$$=new_N("FunDec",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;
			
		}
	|ID LP RP 
	{
		$1->type=(char *)malloc(strlen(type)+1);
		strcpy($1->type,type);
		/*add_funcSym($1->content,type,1);*/
		$$=new_N("FunDec",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
	}
	;
VarList:ParamDec COMMA VarList 
	{
		$$=new_N("VarList",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		add_param($1->content,$1->type);
	}
	|ParamDec 
	{
		$$=new_N("VarList",$1);$$->lineno=@1.first_line;
		add_param($1->content,$1->type);
	}
	;
ParamDec:Specifire VarDec 
	{
		$$=new_N("ParamDec",$1);$$->lineno=@1.first_line;$1->r=$2;
		$$->content=$2->content;//获取参数的名字，由于默认是$1,所以这里要修改一下
	}
    ;

/*Statement*/
Compst:LC DefList StmtList RC 
	{
		$$=new_N("Compst",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;
		$$->type=$3->type;
		$$->content=$3->content;
	}
	;
StmtList:Stmt StmtList{$$=new_N("StmtList",$1);$$->lineno=@1.first_line;$1->r=$2;}
	| {$$=new_c("",0,0,"");}
	;
Stmt:Exp SEMI 
	{
		$$=new_N("Stmt",$1);$$->lineno=@1.first_line;$1->r=$2;
	}
	|Compst {$$=new_N("Stmt",$1);$$->lineno=@1.first_line;}
	|RETURN Exp SEMI 
	{
		$$=new_N("Stmt",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		/*printf("return %s\n",$2->type );*/
		$$->type=$2->type;
		$$->content=(char *)malloc(sizeof(8));
		strcpy($$->content,"return");
	}
	|IF LP Exp RP Stmt {$$=new_N("Stmt",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;$4->r=$5;}
	|IF LP Exp RP Stmt ELSE Stmt {$$=new_N("Stmt",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;$4->r=$5;$5->r=$6;$6->r=$7;}
	|WHILE LP Exp RP Stmt {$$=new_N("Stmt",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;$4->r=$5;}
	;
/*Local Definitions*/
DefList:Def DefList
	{
		$$=new_N("DefList",$1);$$->lineno=@1.first_line;$1->r=$2;
	}
	| {$$=new_c("",0,0,"");}
	;
Def:Specifire DecList SEMI 
	{

		$$=new_N("Def",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
	}
|Specifire error DecList SEMI {}
;
DecList:Dec 
	{

		$$=new_N("DecList",$1);$$->lineno=@1.first_line;
		Pvar var=VarList;
		int find=0;
		while(var){
			if(!strcmp(var->name,$1->content))
			{
				find=1;
				break;
			}
			var=var->next;
		}
		if(!find){
			if(inStruc)
				add_param($1->content,type);
		}
			
		else
			printf("Error type 15 at Line %d：Redefined field \"%s\"\n",@1.first_line,$1->content );
		find=0;
	}
	|Dec COMMA DecList 
	{
		$$=new_N("DecList",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		Pvar var=VarList;
		int find=0;
		while(var){
			if(!strcmp(var->name,$1->content))
			{
				find=1;
				break;
			}
			var=var->next;
		}
		if(!find)	
		{
			if(inStruc)
				add_param($1->content,type);
		}
		else
			printf("Error type 15 at Line %d：Redefined field \"%s\"\n",@1.first_line,$1->content );
		find=0;
	}
	;
Dec:VarDec 
	{
		$$=new_N("Dec",$1);$$->lineno=@1.first_line;
		if(!inStruc)//如果不是在定义结构体组织
		{
			if($1->type){
			if(!strcmp($1->type,"array")){

				if(isArrDef($1->content,tmp)){
					printf("Error type 3 at line %d: Redefined array \" %s\"\n",@1.first_line,$1->content );
				}
				else
					{add_arraySym( $1->content,type,$1->i);}
			}
			else{
				if(isVarDef($1->content,tmp)){
				printf("Error type 3 at line %d: Redefined variable \" %s\"\n",@1.first_line,$1->content );
				}
				else
					add_varSym( $1->content,type);
			}
			}
			
		}

	}
	|VarDec ASSIGNOP Exp 
	{
		$$=new_N("Dec",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(!inStruc)
		{
			if(!strcmp($1->type,"array")){
				if(isArrDef($1->content,tmp)){
					printf("Error type 3 at line %d: Redefined array \" %s\"\n",@1.first_line,$1->content );
				}
				else
					add_arraySym( $1->content,type,$1->i);
			}
			else{
				if(isVarDef($1->content,tmp)){
				printf("Error type 3 at line %d: Redefined variable \" %s\"\n",@1.first_line,$1->content );
				}
				else
					add_varSym( $1->content,type);
			}
		}
	}
	;
/*Expressions*/
Exp:Exp ASSIGNOP Exp
	{
		$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(($1->type)&&($3->type)){
			if(!strcmp($1->content,"##"))
				printf("Error type 6 at Line %d:The left-hand side of an assignment must be a variable\n",$$->lineno );
			if(strstr($1->type,"struct")&&strstr($3->type,"struct"))
			{
				if(!isStrucEqual($1->type,$3->type)){
					printf("Error type 5 at Line %d:type mismatched for assignment\n",$$->lineno);
				}
			}
			else if(strcmp($1->type,$3->type)){
				printf("Error type 5 at Line %d:type mismatched for assignment\n",$$->lineno);
			}
			else{
				if(!strcmp($1->type,"int"))
				{
					$1->i=$2->i;
				}
				else{
					$1->f=$2->f;
				}
			}
		}//未定义的错误也可以在这下面加一个else写，不过没必要
	}
    |Exp AND Exp{$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;}
    |Exp OR Exp{$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;}
	|Exp RELOP Exp{$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;}
	|Exp PLUS Exp
	{
		$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(strcmp($1->type,$3->type))
			printf("Error type 7 at line %d : Type mismatched for operands\n", @1.first_line);
	}
	|Exp MINUS Exp
	{
		$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(strcmp($1->type,$3->type))
			printf("Error type 7 at line %d : Type mismatched for operands\n", @1.first_line);
	}
	|Exp STAR Exp
	{
		$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(strcmp($1->type,$3->type))
			printf("Error type 7 at line %d : Type mismatched for operands\n", @1.first_line);
	}
	|Exp DIV Exp
	{
		$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
		if(strcmp($1->type,$3->type))
			printf("Error type 7 at line %d : Type mismatched for operands\n", @1.first_line);
	}
    |LP Exp RP{$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;}
    |MINUS Exp {$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;}
    |PLUS Exp {$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;}
    |NOT Exp {$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;}
    |ID LP Args RP 
    {
    	if(isFuncDef($1->content,&tmp_func)){
    		if(!isArg_parm(tmp_func))//进行arg和parm的比较
    		{
    			printf("Error type 9 at Line %d : function \"func(",@1.first_line );
    			Pvar var=tmp_func->VarList;
    			while(var){
    				printf("%s",var->type );
    				var=var->next;
    				if(var==NULL)
    					printf(")");
    				else
    					printf(",");
    			}
    			printf("\" is not applicable for arguments \"(");
    			for (int i = 0; i < arg_num; ++i)
    			{
    				printf("%s",arg_type[i] );
    				if(i==arg_num-1)
    					printf(")\".");
    				else
    					printf(",");
    			}
    		}
    	}
    	else if(isVarDef($1->content,tmp)||isArrDef($1->content,tmp))
    	{
    		printf("Error type 11 at Line %d :\"%s\" is not a function\n",@1.first_line,$1->content );
    	}
    	else{
    		printf("Error type 2 at Line %d : undefined function \" %s\"\n",@1.first_line,$1->content );
    	}
    	$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;
    	arg_num=0;
    }
    |ID LP RP {$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;}
    |Exp LB Exp RB 
    {
    	if(!isArrDef($1->content,tmp)){
    		printf("Error type 10 at Line %d :\"%s\" is not an array",@1.first_line,$1->content );
    	}
    	else if(strcmp($3->type,"int")){
    		if(strcmp($3->content,"##"))
    			printf("Error type 12 at Line %d: \"%s\" is not an integer\n",@1.first_line,$3->content );
    		else
    			printf("Error type 12 at Line %d: \"%f\" is not an integer\n",@1.first_line,$3->f );
    	}
    	$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;$3->r=$4;
    }
    |error LB Exp RB {}
    |Exp DOT ID 
    {

    	$$=new_N("Exp",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
    	if( !isVarDef($1->content,tmp) | !isArrDef($1->content,tmp)){
    		if(strstr(tmp,"struct")==NULL)
    			printf("Error type 13 at line %d : Illegal use of \".\"\n",@1.first_line );
    		else if(!isFieldDef(tmp,$3->content))//根据变量1的类型去找对应的结构体定义，然后再看结构体定义中是否有对应的定义
    			printf("Error type 14 at line %d : Non-existent field \"%s\"\n",@1.first_line,$3->content );
    	}
    }
    |ID 
    {

    	if(isVarDef($1->content,tmp)|isParamDef($1->content,tmp)|isArrDef($1->content,tmp)){
    		$1->type=(char *)malloc(strlen(tmp)+1);
    		strcpy($1->type,tmp);
    	}
    	else
    	{	
    		printf("Error type 1 at Line %d:undefined variable \"%s\"\n",@1.first_line,$1->content );
		}
    	$$=new_N("Exp",$1);$$->lineno=@1.first_line;
    }
    |INT
    {
    	$$=new_N("Exp",$1);$$->lineno=@1.first_line;
    	$$->i=$1->i;
    	$$->content=(char*)malloc(3);
    	strcpy($$->content,"##");
    	
    }
    |FLOAT
    {
    	$$=new_N("Exp",$1);$$->lineno=@1.first_line;
    	$$->f=$1->f;
    	$$->content=(char*)malloc(3);
    	strcpy($$->content,"##");//用这样的语义值来表示自己是一个常数
    	
    }
;
Args:Exp COMMA Args 
	{
		add_arg($1->type);
		$$=new_N("Args",$1);$$->lineno=@1.first_line;$1->r=$2;$2->r=$3;
	}
    |Exp 
    {
		add_arg($1->type);
    	$$=new_N("Args",$1);$$->lineno=@1.first_line;
    }
        ;
%%

int display(Pnode T,int indent){
	if(T){
		switch(T->kind){
			case ID: printf("%*cID:%s\n",indent,' ',T->content);break;
			case INT:printf("%*cINT:%d\n",indent,' ',T->i );break;
			case FLOAT:printf("%*cFLOAT:%f\n",indent,' ',T->f );break;
			case TYPE:printf("%*cTYPE:%s\n",indent,' ',T->content);break;
			case SEMI:printf("%*cSEMI\n",indent,' ');break;
			case COMMA:printf("%*cCOMMA\n",indent,' ');break;
			case ASSIGNOP:printf("%*cASSIGNOP\n",indent,' ');break;
			case RELOP:printf("%*cRELOP:%s\n",indent,' ', T->content);break;
			case PLUS:printf("%*cPLUS\n",indent,' ');break;
			case MINUS:printf("%*cMINUS\n",indent,' ');break;
			case DIV:printf("%*cDIV\n",indent,' ');break;
			case AND:printf("%*cAND\n",indent,' ');break;
			case OR:printf("%*cOR\n",indent,' ');break;
			case DOT:printf("%*cDOT\n",indent,' ');break;
			case NOT:printf("%*cNOT\n",indent,' ');break;
			case LP:printf("%*cLP\n",indent,' ');break;
			case RP:printf("%*cRP\n",indent,' ');break;
			case LB:printf("%*cLB\n",indent,' ');break;
			case RB:printf("%*cRB\n",indent,' ');break;
			case LC:printf("%*cLC\n",indent,' ');break;
			case RC:printf("%*cRC\n",indent,' ');break;
			case IF:printf("%*cIF\n",indent,' ');break;
			case ELSE:printf("%*cELSE\n",indent,' ');break;
			case STRUCT:printf("%*cSTRUCT\n",indent,' ');break;
			case WHILE:printf("%*cWHILE\n",indent,' ');break;
			case RETURN:printf("%*cRETURN\n",indent,' ');break;

			case -1:{
				printf("%*c%s(%d)\n",indent,' ',T->name,T->lineno );
			}
		}
		display(T->l,indent+3);
		display(T->r,indent);	
	}
}


	
yyerror(char *msg){
   printf("Error type B at %d :%s\n",yylineno,msg);
 }