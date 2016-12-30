#include "head.h"
extern pht SymbolTable;

#ifndef HEAD_H
/*
 *  新表，newHT
 *	插入，insertHT
 *	查找，getHT
 *	删除，delHT
 */
typedef struct __term
{
	/*	data field	*/
	char name[DATALEN];
	Type typesys;
	int scope;
	unsigned addr;
	int var_no;
	/*	ptr field	*/
	struct __term * next;
}term, *pterm;

typedef struct __ht
{
	int N;	//no. of aries
	int total;	//total num of terms
	int* Num;	//num of terms from each branch 
	pterm* a;	//then a[k] is the address of the term
}ht, *pht;

int hashCMP(pterm A, pterm B);
pht newHT(int N);
unsigned hash(char* name, int N);

int insertHT(pht H, char* name, Type typesys);
int getHT(pht H, char* name, Type typesys, int* index, int* forward);
int delHT(pht H, char* name, Type typesys);
Type getTypeByName(pht H, char* name);

#endif

/*****************************************************************************************************************************************************************
 */


int hashCMP(pterm A, pterm B)
{
	return strcmp(A->name, B->name) && typeCMP(A->typesys, B->typesys);
}

pht newHT(int N)
{
	pht H = (pht)calloc(1, sizeof(ht));
	H->N = N;
	H->Num = (int *)calloc(N, sizeof(int));
	H->total = 0;
	H->a = (pterm *)calloc(N, sizeof(pterm));
	return H;
}


unsigned hash(char* name, int N)
{
	if (name==NULL)
		return 0;

	unsigned val = strlen(name);
	
	while (*name)
	{
		val += *name-'0';
		name ++;
	}
	val = (val) % N;
	return val;
}

int insertHT(pht H, char* name, Type typesys)
{
	pterm T = (pterm) calloc(1, sizeof(term));
	if (name!=NULL)
		strcpy(T->name, name);
	else
		strcpy(T->name, "");
	//T->scope = scope;
	T->typesys = typesys;
	int v = hash(name, H->N);		/* call hash */
	pterm p = H->a[v];
	H->a[v] = T;
	T->next = p;
	H->Num[v] ++;
	H->total ++;
	return v;
}

int getHT(pht H, char* name, Type typesys, int* index, int* forward)		// Output: index, forward
{
	int find = 0;
	*forward = 0;
	int v = hash(name, H->N);		/* call hash */
	pterm ptr = H->a[v];
	while (ptr!=NULL)
	{
		if (strcmp(name, ptr->name)==0 && typeCMP(typesys, ptr->typesys)==true)
		{
			find = 1;
			break;
		}
		ptr = ptr->next;
		++(*forward);
	}
	if (find)
		*index = v;
	return find;
}

pterm findTermByName(pht H, char* name)
{
	int v = hash(name, H->N);		/* call hash */
	pterm ptr = H->a[v];
	while (ptr!=NULL)
	{
		if (strcmp(name, ptr->name)==0)
		{
			break;
		}
		ptr = ptr->next;
		
	}
	return ptr;
}



int delHT(pht H, char* name, Type typesys)
{
	int find = 0;
	int v = hash(name, H->N);		/* call hash */
	pterm ptr = H->a[v];	//prev
	pterm qtr = ptr;		//current
	if (qtr!=NULL)
	{
		if (strcmp(name, qtr->name)==0 && typeCMP(typesys, qtr->typesys))
		{
			find = 1;
			H->a[v] = qtr->next;
			free(qtr);
			H->Num[v] --;
			H->total --;
			return find;
		}
	}
	while (ptr!=NULL)
	{
		qtr = ptr->next;
		if (strcmp(name, qtr->name)==0 && typeCMP(typesys, qtr->typesys))
		{
			find = 1;
			ptr->next = qtr->next;
			free(qtr);
			H->Num[v] --;
			H->total --;
			break;
		}
		ptr = ptr->next;
	}

	return find;
}

Type getTypeByName(pht H, char* name)	// This function is for structure-object query!
{
	int v = hash(name, H->N);		/* call hash */
	pterm ptr = H->a[v];
	Type type_ptr;	// NOTE: Here only address will be returned!
	
	while (ptr!=NULL)
	{
		if (strcmp(name, ptr->name)==0)
		{
			type_ptr = ptr->typesys;
			break;
		}
		ptr = ptr->next;
	}

	return type_ptr;
}



void printSymbolTable()
{
	int i;
	int cnt = 0;
	pterm t;
	
	printf("******************  SymbolTable ******************\n");
	for (i = 0; i < SymbolTable->N; i++)
	{
		t = SymbolTable->a[i];
	
		for (; t!=NULL; t = t->next)
		{
			printf("\t[%d]%s\t\t\t", ++cnt, t->name);
			
			if (t->typesys!=NULL)
			{
				getstring(t->typesys);
				//printf("\t CORRECTION:");
				//getstring(getTypeByName(SymbolTable, t->name));
			}
			printf("\n");
		}
	}
	printf("******************  SymbolTable ******************\n");
}

void getstring(Type tp)
{
	FieldList f;
	switch (tp->kind)
	{
		case BASIC:
		printf("%s", tp->u.basic==INT_TYPE? "int " : "float ");
		break;

		case ARRAY:
		printf("array size %d elem ", tp->u.array.size);
		getstring(tp->u.array.elem);
		break;
		
		case STRUCTURE:
		printf("structure field \n");
		f = tp->u.structure;
		while (f != NULL)
		{
			printf("\t\t\t\t\t%s ", f->name);
			getstring(f->type);
			printf("\n");
			f = f->next;
		}
		break;
		
		case FUNCTION:
		printf("function %s ret ", tp->u.function.funcname);
		getstring(tp->u.function.ret);
		printf("param \n");
		f = tp->u.function.param;
		while (f != NULL)
		{
			printf("\t\t\t\t\t%s ", f->name);
			getstring(f->type);
			printf("\n");
			f = f->next;
		}
		break;
		
		case OBJECT:
		printf("structure-object from [struct %s] ", tp->u.object.structname);
		break;
		
		default:
		printf("NYI ");
	}
}
//Just for test
//
/*
int input(pht H, char* s[], int num)
{
	pterm ptr;
	int i = 0;
	for (i = 0; i < num; i++)
	{
		insertHT(H, s[i], 0);
	}


	return 0;
}
int printHT(pht H)
{
	int i = 0;
	pterm ptr;
	printf(">>Lines: %d\n", H->N);
	for (i = 0; i < H->N; i++)
	{
		//printf("******************************************************");
		printf("[%d]:", i);
		ptr = H->a[i];
		while (ptr!=NULL)
		{
			printf("%s  ", ptr->name);
			ptr = ptr->next;
		}
		printf("\n");
	}
	return 0;
}
*/
/*
int main(int argc, char* argv[])
{
	pht pHashTable = newHT(10);
	char* names[] = {
		"jack", "donald", "hilary", "obama", "john", "mike", "jerry", "walli", "anderson", "miller", "coco", "ballert", "handsome",
		"qiqi", "pipi", "C", "Java", "Python", "C plus plus", "SML", "PPAP", "Jian Hu", "MAGA", "economics", "machine learning",
		"cheeeeer", "why this", "hausdorf", "hilbert", "banach", "cauchy", "lambda", "asjqw", "wksqwq", "segregate"
	};
	printf(">>%d\n", sizeof(names)/sizeof(*names));
	input(pHashTable, names, sizeof(names)/sizeof(*names));
	printHT(pHashTable);
	int idx = -1, forward = 0;
	int x = 2131;
	
	int k;
	clock_t start, end;
	//start = clocK();
	srand((unsigned)time(NULL));
	int len = (sizeof(names)/sizeof(*names));
	char* s;
	x = (rand() + k) % len;
	printf("Total:%d terms     Starting calculation...\n", len);
	
	delHT(pHashTable, "Python", 0);
	delHT(pHashTable, "C plus plus", 0);
	
	printHT(pHashTable);
	int ok;
	for (k = 0; k < len; k++) {
		x = (x+1)%len;
		s = names[x];
		//s[0]='a';
		ok = getHT(pHashTable, s, 0, &idx, &forward);
		if (ok)
			printf("Got it! %15s:[%d][%d]\n", s, idx, forward);
		else
			printf("miss!%15s not found!\n", s);
	}
	
	//end = clock();
	//printf(">>time = %lf\n", (double)(end-start)/CLOCKS_PER_SEC);
	return 0;
}
*/

//clock_t start,end;  
//start = clock();  
////…calculating…  
//end = clock();  
//printf("time=%f\n",(double)end-start)/CLK_TCK);  "")

