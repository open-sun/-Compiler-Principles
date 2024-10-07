%option noyywrap
%option nounput
%option noinput
%option yylineno
%top{
    #include <stdarg.h>
    #include "common.h"
}
%{
    int yycolno=0;
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
OCTAL 0[0-7]+
HEXADECIMAL 0[xX][0-9a-fA-F]+
FLOAT ([0-9]+(\.[0-9]+)?([eE][-+]?[0-9]+)?)




ID [[:alpha:]_][[:alpha:][:digit:]_]*

EOL (\r\n|\n|\r)

WHITE [\t ]

/*  Your code here (lab3). */

%%
    /* rules section */
"int" {
    dump_tokens("INT\t%s\t%d\t%d\n", yytext, yylineno,yycolno);
    yycolno+=yyleng;
}

"void" {
    dump_tokens("VOID\t%s\t%d\t%d\n", yytext, yylineno,yycolno);
        yycolno+=yyleng;
}

"if" {
    dump_tokens("IF\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

"else" {
    dump_tokens("ELSE\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

"return" {
    dump_tokens("RETURN\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

"=" {
    dump_tokens("ASSIGN\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

"<" {
    dump_tokens("LESS\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

"+" {
    dump_tokens("ADD\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
"-" {
    dump_tokens("MINUS\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
"*" {
    dump_tokens("MULTIPLE\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
"/" {
    dump_tokens("DIVISION\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
";" {
    dump_tokens("SEMICOLON\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

"(" {
    dump_tokens("LPAREN\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

")" {
    dump_tokens("RPAREN\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

"{" {
    dump_tokens("LBRACE\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

"}" {
    dump_tokens("RBRACE\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}

{DECIMIAL} {
    int decimal;
    decimal = atoi(yytext);
    dump_tokens("DECIMAL\t%s\t%d\t%d\t%d\n", yytext,yylineno,yycolno, decimal);
    yycolno+=yyleng;
}
{OCTAL} {
    int octal = strtol(yytext, NULL, 8);
    dump_tokens("OCTAL\t%s\t%d\t%d\t%d\n", yytext ,yycolno,octal);

    yycolno+=yyleng;
}   

{HEXADECIMAL} {
    int hex = strtol(yytext, NULL, 16);
    dump_tokens("HEXADECIMAL\t%s\t%d\t%d\t%d\n", yytext,yycolno, hex);
    yycolno+=yyleng;
}

{FLOAT} {
    float float_value = atof(yytext);
    dump_tokens("FLOAT\t%s\t%d\t%.6f\n", yytext, yylineno,yycolno, float_value);
    yycolno+=yyleng;
}




{ID} {
    dump_tokens("ID\t%s\t%d\t%d\n", yytext,yylineno,yycolno);





    yycolno+=yyleng;
}

{EOL} {
    yycolno=0;

}


{WHITE} {
    yycolno+=yyleng;
}

    /*  Your code here (lab3). */
%%
/* user code section */
