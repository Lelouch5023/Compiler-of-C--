/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ASSIGNOP = 258,
     OR = 259,
     AND = 260,
     RELOP = 261,
     MINUS = 262,
     PLUS = 263,
     DIV = 264,
     STAR = 265,
     NOT = 266,
     RB = 267,
     LB = 268,
     RP = 269,
     LP = 270,
     DOT = 271,
     ELSE = 272,
     INT = 273,
     FLOAT = 274,
     ID = 275,
     SEMI = 276,
     COMMA = 277,
     TYPE = 278,
     LC = 279,
     RC = 280,
     STRUCT = 281,
     RETURN = 282,
     IF = 283,
     WHILE = 284,
     EOP = 285
   };
#endif
/* Tokens.  */
#define ASSIGNOP 258
#define OR 259
#define AND 260
#define RELOP 261
#define MINUS 262
#define PLUS 263
#define DIV 264
#define STAR 265
#define NOT 266
#define RB 267
#define LB 268
#define RP 269
#define LP 270
#define DOT 271
#define ELSE 272
#define INT 273
#define FLOAT 274
#define ID 275
#define SEMI 276
#define COMMA 277
#define TYPE 278
#define LC 279
#define RC 280
#define STRUCT 281
#define RETURN 282
#define IF 283
#define WHILE 284
#define EOP 285




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

