#include "head.h"
#ifndef HEAD_H
	
enum MIPS_INS {	/* reuse of data structure Code->kind */
	add = 100, addu, sub, subu, and, or, xor, nor, slt, sltu, sll, srl, sra, sllv, srlv, srav, jl,	/* R-type Instruction */
	addi, addiu, andi, ori, xori, lui, lw, sw, beq, bne, slti, sltiu,							/* I-type Instruction */				
	j, jal,																						/* J-type Instruction */
	sle, sgt, sge, mflo																			// I don't know
	li, la, move, bgt, bge, blt, ble, _label, _fun													/* Pseudo Instruction */
};



enum MIPS_OPS {
	REGS = 10, 	/* Variable:  $s0, $s1, $s2, $s3, ...., $s7, $t9, $t8,...$t3 */
	REGT, 	//>> special usage as temporary storage
	REGA,	/* $a0, $a1, $a2, $a3 */
	REGV,	/* $v0 */
	REGRA,	/* $ra */
	REGSP,	/* $sp */
	MEM,	/* Tempaddr:  */
	IMME,	//CONSTANT
	LBL,	//LABELJMP
	FUND,   //FUNCADDR
};




#endif

/*
**
*/
extern int seed_t;

Code getMipsCode(Code line)
{
	int i;
	int regt_no = 0;
	Code res = NULL;	// as results
	int sn = 0;	// s register 
	int a_no = 0;
	Operand lop;
	Operand rop;
	Operand rop2;
	
	FILE *stream ;
    if((stream = freopen("mips.s", "w+", stdout)) == NULL)
    {
    	printf("Failure in open mips.s\n");
    	exit(-1);
    }


	int j;
	printf(".data\n");
	for (j = 0; j < seed_t+1; j++)
		printf("_T%d: .word 0\n", j);
	printf("_prompt: .asciiz \"Enter an integer:\"\n");
	printf("_ret: .asciiz \"\\n\"\n");
	printf(".globl main\n");
	printf(".text\n");

	printf("_read:\n");
	printf("  li $v0, 4\n");
	printf("  la $a0, _prompt\n");
	printf("  syscall\n");
	printf("  li $v0, 5\n");
	printf("  syscall\n");
	printf("  jr $ra\n");
	printf("\n");

	printf("_write:\n");
	printf("  li $v0, 1\n");
	printf("  syscall\n");
	printf("  li $v0, 4\n");
	printf("  la $a0, _ret\n");
	printf("  syscall\n");
	printf("  move $v0, $0\n");
	printf("  jr $ra\n");
	printf("\n");

	while (line!=NULL)
	{
		lop = line->op[0];
		rop = line->op[1];	
		rop2 = line->op[2];
		
		switch (line->kind)
		{

			case ASSIGNIC:
			{
				switch (rop->kind)	//right op
				{
					case VARIABLE:
						if (lop->kind==VARIABLE)
						{
							printf("  move\t$s%d, $s%d\n", lop->value, rop->value);
						}
						else	//lop->kind == TEMPADDR
						{
							printf("  move\t$t%d, $s%d\n", 0, rop->value);
							printf("  sw\t$t%d, _T%d\n", 0, lop->value);
						}
					break;

					case CONSTANT:
						if (lop->kind==VARIABLE)
						{
							printf("  li  \t$t%d, %d\n", 0, rop->value);
							printf("  move\t$s%d, $t%d\n", lop->value, 0);
						}
						else	//lop->kind == TEMPADDR
						{
							printf("  li\t$t%d, %d\n", 0, rop->value);
							printf("  sw\t$t%d, _T%d\n", 0, lop->value);
						}
					break;

					case TEMPADDR: 
						if (lop->kind==VARIABLE)	// op s _T 
						{
							printf("  lw\t$s%d, _T%d\n", lop->value, rop->value);
						}
						else	//kind == TEMPADDR 	// op _T _T
						{
							printf("  lw\t$t%d, _T%d\n", 0, rop->value);
							printf("  sw\t$t%d, _T%d\n", 0, lop->value);
						}
					break;
				}
			}
			break;
			
			case ADDIC:
			{
				if (rop2->kind==IMME)	// addi reg(x), reg(y), k
				{
					switch (rop->kind)	
					{
						case VARIABLE:
							if (lop->kind==VARIABLE)
							{
								res = gen(move, 2, REGS, lop->value, REGS, rop->value);
								//
								printf("  move\t$s%d, $s%d\n", lop->value, rop->value);
							}
							else
							{
								res = gen(move, 2, REGT, 0, REGS, rop->value);
								res = join(res, gen(sw, 2, REGT, 0, MEM, lop->value));
								//
								printf("  move\t$t%d, $s%d\n", 0, rop->value);
								printf("  sw  \t$t%d, _T%d\n", 0, lop->value);
							}
						break;
						case CONSTANT:
							if (line->op[0]->kind==VARIABLE)
							{
								res = gen(lw, 2, REGT, 0, MEM, rop->value);
								res = join(res, gen(move, 2, REGS, lop->value, REGT, 0));
								//
								printf("  lw\t$t%d, _T%d\n", 0, rop->value);
								printf("  move\t$s%d, $t%d\n", lop->value, 0);
							}
							else
							{
								res = gen(lw, 2, REGT, 0, MEM, rop->value);
								res = join(res, gen(sw, 2, REGT, 0, MEM, lop->value));
								//
								printf("  lw  \t$t%d, _T%d\n", 0, rop->value);
								printf("  sw  \t$t%d, _T%d\n", 0, lop->value);
							}
						break;
						case TEMPADDR: 
							if (line->op[0]->kind==VARIABLE)
							{
								res = gen(li, 2, REGS, lop->value, IMME, rop->value);
								//
								printf("  li\t$s%d, %d\n", lop->value, rop->value);
							}
							else
							{
								res = gen(li, 2, REGT, 0, IMME, rop->value);
								res = join(res, gen(sw, 2, REGT, 0, MEM, lop->value));
								//
								printf("  li  \t$t%d, %d\n", 0, rop->value);
								printf("  sw  \t$t%d, _T%d\n", 0, lop->value);	
							}
						break;
					}
				}
				else	// add reg(x), reg(y), reg(z)
				{
					res = gen(lw, 2, REGT, 0, MEM, rop2->value);
					res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
					//
					printf("  lw  \t$t%d, _T%d\n", 0, rop2->value); 
					printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
					if (lop->kind==TEMPADDR)
					{
						res = join(res, gen(add, 3, REGT, 2, REGT, 1, REGT, 0));
						res = join(res, gen(sw, 2, REGT, 2, MEM, lop->value));
						//
						printf("  add \t$t%d, $t%d, $t%d\n", 2, 1, 0);
						printf("  sw  \t$t%d, _T%d\n", 2, lop->value);
					}	
					else//lop->kind==VARIABLE
					{
						res = join(res, gen(add, 3, REGS, lop->value, REGT, 1, REGT, 0));
						//
						printf("  add \t$s%d, $t%d, $t%d\n", lop->value, 1, 0);
					}
				}
			}
			break;
			
			case SUBIC:
			{
				if (rop2->kind==IMME)	// addi reg(x), reg(y), -k
				{
					/* Not Needed */
				}
				else					// sub reg(x), reg(y), reg(z)
				{
					res = gen(lw, 2, REGT, 0, MEM, rop2->value);
					res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
					//
					printf("  lw  \t$t%d, _T%d\n", 0, rop2->value);
					printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
					
					if (lop->kind==TEMPADDR)
					{
						res = join(res, gen(sub, 3, REGT, 2, REGT, 1, REGT, 0));
						res = join(res, gen(sw, 2, REGT, 2, MEM, lop->value));
						//
						printf("  sub \t$t%d, $t%d, $t%d\n", 2, 1, 0);
						printf("  sw  \t$t%d, _T%d\n", 2, lop->value);

					}	
					else//lop->kind==VARIABLE
					{
						res = join(res, gen(sub, 3, REGS, lop->value, REGT, 1, REGT, 0));
						//
						printf("  sub \t$s%d, $t%d, $t%d\n", lop->value, 1, 0);
					}
				}
			}
			break;
			
			case MULIC:	// mul reg(x), reg(y), reg(z)
			{
				res = join(res, gen(lw, 2, REGT, 0, MEM, rop2->value));
				res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
				//
				printf("  lw  \t$t%d, _T%d\n", 0, rop2->value);
				printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
				
				switch (lop->kind)	
				{
					case VARIABLE:
						res = join(res, gen(mul, 3, REGS, lop->value, REGT, 1, REGT, 0));
						//
						printf("  mul \t$s%d, $t%d, $t%d\n", lop->value, 1, 0);
					break;
					
					case TEMPADDR: 
						res = join(res, gen(mul, 3, REGT, 2, REGT, 1, REGT, 0));
						res = join(res, gen(sw, 2, REGT, 2, MEM, lop->value));
						//
						printf("  mul \t$t%d, $t%d, $t%d\n", 2, 1, 0);
						printf("  sw  \t$t%d, _T%d\n", 2, lop->value);
					break;
				}
			}
			break;
			
			case DIVIC:	//	div 	reg(y), reg(z)
			{			//	mflo	reg(x)
				res = join(res, gen(lw, 2, REGT, 0, MEM, rop2->value));
				res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
				res = join(res, gen(divm, 2, REGT, 1, REGT, 0));
				//
				printf("  lw  \t$t%d, _T%d\n", 0, rop2->value);
				printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
				printf("  div \t$t%d, $t%d\n", 1, 0);
				switch (lop->kind)	
				{
					case VARIABLE:
						res = join(res, gen(mflo, 1, REGS, lop->value));
						//
						printf("  mflo\t$s%d\n", lop->value);
					break;
					
					case TEMPADDR: 
						res = join(res, gen(mflo, 1, REGT, 0));
						res = join(res, gen(sw, 2, REGT, 0, MEM, lop->value));
						//
						printf("  mflo\t$t%d\n", 2);
						printf("  sw  \t$t%d, _T%d\n", 2, lop->value);
					break;
				}

			}
			break;

 			case DECIC:
 			{
 				
 			}
 			break;

			case ADDRIC: 
			{
				
			}
			break;

 			case PTRRDIC: // lw reg(x), 0(reg(y))
 			{
 				/* Not needed */
 			}
 			break;
			
			case PTRWTIC: // sw reg(y), 0(reg(x))
			{
				/* Not needed */	
			}
			break;
			
			case LABELIC: // x:
			{
				res = gen(_label, 1, LBL, lop->value);
				//
				printf("label%d:\n", lop->value);
			}
			break;
 			
 			case GOTOIC: //	j x
 			{
 				res = gen(j, 1, LBL, lop->value);
 				//
 				printf("  j   \tlabel%d\n", lop->value);
 			}
 			break;

 			case CONDGIC: //bgt	reg(x), reg(y), z
 			{
 				res = join(res, gen(lw, 2, REGT, 0, MEM, lop->value));
 				res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
 				res = join(res, gen(bgt, 3, REGT, 0, REGT, 1, LBL, rop2->value));
 				//
 				printf("  lw  \t$t%d, _T%d\n", 0, lop->value);
 				printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
 				printf("  bgt \t$t%d, $t%d, label%d\n", 0, 1, rop2->value);
 			}
 			break;

			case CONDEIC: //beq	reg(x), reg(y), z
			{
				res = join(res, gen(lw, 2, REGT, 0, MEM, lop->value));
 				res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
 				res = join(res, gen(beq, 3, REGT, 0, REGT, 1, LBL, rop2->value));	
 				//
 				printf("  lw  \t$t%d, _T%d\n", 0, lop->value);
 				printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
 				printf("  beq \t$t%d, $t%d, label%d\n", 0, 1, rop2->value);
			}
			break;

			case CONDLIC: //blt	reg(x), reg(y), z
			{
				res = join(res, gen(lw, 2, REGT, 0, MEM, lop->value));
 				res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
 				res = join(res, gen(blt, 3, REGT, 0, REGT, 1, LBL, rop2->value));	
 				//
 				printf("  lw  \t$t%d, _T%d\n", 0, lop->value);
 				printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
 				printf("  blt \t$t%d, $t%d, label%d\n", 0, 1, rop2->value);
			}
			break;

			case CONDGEIC: //bge reg(x), reg(y), z
			{
				res = join(res, gen(lw, 2, REGT, 0, MEM, lop->value));
 				res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
 				res = join(res, gen(bge, 3, REGT, 0, REGT, 1, LBL, rop2->value));	
 				//
 				printf("  lw  \t$t%d, _T%d\n", 0, lop->value);
 				printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
 				printf("  bge \t$t%d, $t%d, label%d\n", 0, 1, rop2->value);
			}
			break;
			
			case CONDLEIC: //ble reg(x), reg(y), z
			{
				res = join(res, gen(lw, 2, REGT, 0, MEM, lop->value));
 				res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
 				res = join(res, gen(ble, 3, REGT, 0, REGT, 1, LBL, rop2->value));	
 				//
 				printf("  lw  \t$t%d, _T%d\n", 0, lop->value);
 				printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
 				printf("  ble \t$t%d, $t%d, label%d\n", 0, 1, rop2->value);
			}
			break;

			case CONDNEIC: //bne reg(x), reg(y), z
			{
				res = join(res, gen(lw, 2, REGT, 0, MEM, lop->value));
 				res = join(res, gen(lw, 2, REGT, 1, MEM, rop->value));
 				res = join(res, gen(bne, 3, REGT, 0, REGT, 1, LBL, rop2->value));	
 				//
 				printf("  lw  \t$t%d, _T%d\n", 0, lop->value);
 				printf("  lw  \t$t%d, _T%d\n", 1, rop->value);
 				printf("  bne \t$t%d, $t%d, label%d\n", 0, 1, rop2->value);
			}
			break;

			/********************** Function **********************/


			case FUNDEFIC:	// x:
			{
				res = join(res, gen(_fun, 1, FUND, lop->value));
				//
				char* pc = getFunctionNameByVarNo(lop->value);
				printf("\n%s%s:\n", strcmp(pc, "main")==0?"":"_", pc);
			}
			break;

			case RETIC:		// move $v0, reg(x)
			{				// jr $ra
				

				//
				printf("  lw  \t$v0, _T%d\n", lop->value);
				printf("  jr  \t$ra\n");
			}
			break;
			
			case ARGIC:		//
			{
				
				res = join(res, gen(lw, 2, REGA, a_no, MEM, lop->value));
				//
				printf("  lw\t$a%d, _T%d\n", a_no, lop->value);
				a_no ++;
				if (line->next->kind!=ARGIC)
					a_no = 0;
			}
			break;
			
			case CALLIC:	//
			{
				res = join(res, gen(addi, 3, REGSP, 0, REGSP, 0, IMME, -4));
				//res = join(res, gen(sw, 2, REGRA, 0, ));
				//
				printf("  addi\t$sp, $sp, -4\n");
				printf("  sw  \t$ra, 0($sp)\n");
				printf("  jal \t_%s\n", getFunctionNameByVarNo(rop->value));
				printf("  lw  \t$ra, 0($sp)\n");
				printf("  addi\t$sp, $sp, 4\n");
				if (lop->kind==VARIABLE)
				{
					printf("  move\t$s%d, $v0\n", lop->value);
				}
				else	//  ===TEMPADDR
				{
					printf("  sw  \t$v0, _T%d\n", lop->value);
				}
			}
			break;
			
			case PARAMIC:	//PARAM v2  => move $s2, $a0
			{
				res = join(res, gen(move, 2, REGS, lop->value, REGA, a_no));
				//
				int total = 0;
				Code current = line;
				while (current->next!=NULL && current->next->kind==PARAMIC)
				{
					total++;
					current = current->next;
				}
				printf("  move\t$s%d, $a%d\n", lop->value, total);
				
				//if (line->next->kind!=PARAMIC)
				//	a_no = 0;
			}
			break;

			case READIC:
			{
				//
				printf("  addi\t$sp, $sp, -4\n");
				printf("  sw  \t$ra, 0($sp)\n");
				printf("  jal \t_%s\n", "read");
				printf("  lw  \t$ra, 0($sp)\n");
				printf("  addi\t$sp, $sp, 4\n");
				if (lop->kind==VARIABLE)
				{
					printf("  move\t$s%d, $v0\n", lop->value);
				}
				else	//  ===TEMPADDR
				{
					printf("  sw  \t$v0, _T%d\n", lop->value);
				}
			}
			break;

			case WRITEIC:
			{
				//
				printf("  lw  \t$a0, _T%d\n", lop->value);

				//printf("  lw  \t$a0\n");
				printf("  addi\t$sp, $sp, -4\n");
				printf("  sw  \t$ra, 0($sp)\n");
				printf("  jal \t_%s\n", "write");
				printf("  lw  \t$ra, 0($sp)\n");
				printf("  addi\t$sp, $sp, 4\n");
				

			}
			break;
		}

		line->ins = res;
		res = NULL;

		//fprintf(f, "%s\n", line->MIPS);
		//printf("%s\n", line->MIPS);
		line = line->next;
	}


	 stream = freopen("/dev/tty", "w", stdout);
	 printf("Well done\n");


	return res;
}





































