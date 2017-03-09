%{
#include <stdio.h>

#include "../include/vslc.h"
%}
%option noyywrap
%option array
%option yylineno
%%

[\t\n\r\v ]		{ } //ignoring whitespace
\/\/.*\n		{ } //ignoring comments

FUNC                    { return FUNC; }
BEGIN			{return OPENBLOCK; }
END			{ return CLOSEBLOCK; }
RETURN			{ return RETURN; }
PRINT			{ return PRINT; }
CONTINUE		{ return CONTINUE; }
IF			{  return IF; }
THEN			{ return THEN; }
ELSE			{ return ELSE; }
WHILE			{ return WHILE; }
DO			{  return DO; }
VAR			{  return VAR; }

[\:\=\+\-\*\/\=\<\>\,\(\)]	{  return yytext[0]; } //return operator


\"(\\.|[^"])*\"		{  return STRING; }
[0-9]+           	{ return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* {  return IDENTIFIER; }
.                       { printf("Unknown character [%c]\n", yytext[0]); }

%%
