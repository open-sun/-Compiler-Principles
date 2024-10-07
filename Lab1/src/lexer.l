%option noyywrap
%option nounput
%option noinput
%option yylineno
%top{
    #include <stdarg.h>
    #include "common.h"
}
%{
    extern dump_type_t dump_type;

    inline void dump_tokens(const char* format, ...){
        va_list args;
        va_start(args, format);
        if (dump_type == TOKENS)
            vfprintf(yyout, format, args);
        va_end(args);
    }


    /* Your code here, if desired (lab3). */

    
%}

/* definitions section */
DECIMIAL ([1-9][0-9]*|0)

ID [[:alpha:]_][[:alpha:][:digit:]_]*

EOL (\r\n|\n|\r)

WHITE [\t ]

/*  Your code here (lab3). */

%%
    /* rules section */
"int" {
    dump_tokens("INT\t%s\t%d\n", yytext, yylineno);
}

"void" {
    dump_tokens("VOID\t%s\t%d\n", yytext, yylineno);
}

"if" {
    dump_tokens("IF\t%s\t%d\n", yytext,yylineno);
}

"else" {
    dump_tokens("ELSE\t%s\t%d\n", yytext,yylineno);
}

"return" {
    dump_tokens("RETURN\t%s\t%d\n", yytext,yylineno);
}

"=" {
    dump_tokens("ASSIGN\t%s\t%d\n", yytext,yylineno);
}

"<" {
    dump_tokens("LESS\t%s\t%d\n", yytext,yylineno);
}

"+" {
    dump_tokens("ADD\t%s\t%d\n", yytext,yylineno);
}
"-" {
    dump_tokens("MINUS\t%s\t%d\n", yytext,yylineno);
}
"*" {
    dump_tokens("MULTIPLE\t%s\t%d\n", yytext,yylineno);
}
"/" {
    dump_tokens("DIVISION\t%s\t%d\n", yytext,yylineno);
}
";" {
    dump_tokens("SEMICOLON\t%s\t%d\n", yytext,yylineno);
}

"(" {
    dump_tokens("LPAREN\t%s\t%d\n", yytext,yylineno);
}

")" {
    dump_tokens("RPAREN\t%s\t%d\n", yytext,yylineno);
}

"{" {
    dump_tokens("LBRACE\t%s\t%d\n", yytext,yylineno);
}

"}" {
    dump_tokens("RBRACE\t%s\t%d\n", yytext,yylineno);
}

{DECIMIAL} {
    int decimal;
    decimal = atoi(yytext);
    dump_tokens("DECIMAL\t%s\t%d\t%d\n", yytext,yylineno, decimal);
}

{ID} {
    dump_tokens("ID\t%s\t%d\n", yytext,yylineno);
}

{EOL} ;

{WHITE}

    /*  Your code here (lab3). */
%%
/* user code section */
