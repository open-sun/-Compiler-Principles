%option noyywrap
%option nounput
%option noinput
%top{
    #include <stdarg.h>
    #include "common.h"
    #include "parser.h"
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

    /* Your code here, if desired (lab1). */
%}

/* definitions section */
DECIMIAL ([1-9][0-9]*|0)
OCTAL 0[0-7]+
HEXADECIMAL 0[xX][0-9a-fA-F]+
DECIMAL_FLOATING_CONSTANT ((((([0-9]+)?(\.[0-9]*))|([0-9]+\.))([eE][+-]?[0-9]+)?)|([0-9]+([eE][+-]?[0-9]+)?))
HEXADECIMAL_FLOATING_CONSTANT (0[xX](([0-9a-fA-F]+(\.[0-9a-fA-F]*)?)|(\.[0-9a-fA-F]+))[pP][+-]?[0-9]+)



%x COMMENT
%x SCOMMENT
ID [[:alpha:]_][[:alpha:][:digit:]_]*

EOL (\r\n|\n|\r)

WHITE [\t ]

%%

<COMMENT>"*/" {
    yycolno+=yyleng;
    BEGIN INITIAL;
}
<COMMENT>. {
    yycolno+=yyleng;
}
<COMMENT>{EOL} {
    yycolno=0;
}
<SCOMMENT>{EOL} {
    yycolno=0;
    BEGIN INITIAL;
}
<SCOMMENT>. {
    yycolno+=yyleng;
}
"/*" {
    yycolno+=yyleng;
    BEGIN COMMENT;
}
"//" {
    yycolno+=yyleng;
    BEGIN SCOMMENT;
}

"int" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "INT", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"void" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "VOID", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"if" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "IF", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"else" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "ELSE", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"return" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "RETURN", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}
"while" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "WHILE", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}
"break" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "BREAK", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}
"continue" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "CONTINUE", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}
"float" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "FLOAT", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}
"const" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "CONST", yytext, yylineno, yycolno);
	yycolno+= yyleng;
}
"," {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "FFR", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}
"==" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "EQUAL", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"!=" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "UNEQUAL", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

">=" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "LARGEEQUAL", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"<=" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "LESSEQUAL", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"=" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "ASSIGN", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"<" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "LESS", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"+" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "ADD", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"-" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "MINUS", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"*" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "MULTIPLE", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"/" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "DIVISION", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"%" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "MOD", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"!" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "NOT", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"&&" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "AND", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"||" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "OR", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

";" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "SEMICOLON", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"(" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "LPAREN", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

")" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "RPAREN", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"[" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "LSQUARE", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"]" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "RSQUARE", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}
"{" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "LBRACE", yytext, yylineno, yycolno);
    currentScope = enterScope(currentScope);
    yycolno+=yyleng;
}

"}" {
    dump_tokens("%-30s%-30s%-5d%-5d\n", "RBRACE", yytext, yylineno, yycolno);
    currentScope = exitScope(currentScope);
    yycolno+=yyleng;
}

{DECIMIAL} {
    dump_tokens("%-30s%-30s%-5d%-5d%lld\n", "DECIMAL", yytext, yylineno, yycolno, atoll(yytext));
    yycolno+=yyleng;
}
{OCTAL} {
    long long octal = strtoll(yytext, NULL, 8);
    dump_tokens("%-30s%-30s%-5d%-5d%lld\n", "OCTAL", yytext, yylineno, yycolno, octal);
    yycolno+=yyleng;
}

{HEXADECIMAL} {
    long long hex = strtoll(yytext, NULL, 16);
    dump_tokens("%-30s%-30s%-5d%-5d%lld\n", "HEXADECIMAL", yytext, yylineno, yycolno, hex);
    yycolno+=yyleng;
}

{DECIMAL_FLOATING_CONSTANT} {
    double DECIMAL_value = atof(yytext);
    dump_tokens("%-30s%-30s%-5d%-5d%.6f\n", "DECIMAL_FLOATING_CONSTANT", yytext, yylineno, yycolno, DECIMAL_value);
    yycolno+=yyleng;
}

{HEXADECIMAL_FLOATING_CONSTANT} {
    double HEXADECIMAL_value = atof(yytext);
    dump_tokens("%-30s%-30s%-5d%-5d%.6f\n", "HEXADECIMAL_FLOATING_CONSTANT", yytext, yylineno, yycolno, HEXADECIMAL_value);
    yycolno+=yyleng;
}

{ID} {
    SymbolTableEntry* entry = lookupSymbol(currentScope->table, yytext);
    if (!entry) {
        insertSymbol(currentScope->table, yytext);
        entry = lookupSymbol(currentScope->table, yytext);
    }
    dump_tokens("%-30s%-30s%-5d%-5d%p\n", "ID", yytext, yylineno, yycolno, (void*)entry);
    yycolno+=yyleng;
}

{EOL} {
    yycolno=0;
}

{WHITE} {
    yycolno+=yyleng;
}

%%

/* user code section */
