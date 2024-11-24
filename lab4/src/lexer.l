%option noyywrap
%option nounput
%option noinput
%top{
    #include <stdarg.h>
    #include "common.h"
    #include "parser.h"
    #include <vector>
    #include <Type.h>
    #include <SymbolTable.h>
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
ID [[:alpha:]_][[:alpha:][:digit:]_]*


%x COMMENT
%x SCOMMENT


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
    dump_tokens("INT\t%s\n", yytext);
    return INT;
}

"void" {
    dump_tokens("VOID\t%s\n", yytext);
    return VOID;
}

"if" {
    dump_tokens("IF\t%s\n", yytext);
    return IF;
}

"else" {
    dump_tokens("ELSE\t%s\n", yytext);
    return ELSE;
}
"while" {
    dump_tokens("WHILE\t%s\n", yytext);
    return WHILE;
}

"return" {
    dump_tokens("RETURN\t%s\n", yytext);
    return RETURN;
}

"break" {
    dump_tokens("BREAK\t%s\n", yytext);
    return BREAK;
}

"continue" {
    dump_tokens("CONTINUE\t%s\n", yytext);
    return CONTINUE;
}
"float" {
    dump_tokens("FLOAT\t%s\n", yytext);
    return FLOAT;
}
"const" {
    dump_tokens("CONST\t%s\n", yytext);
    return CONST;
}
"," {
    dump_tokens("COMMA\t%s\n", yytext);
    return COMMA;
}
"==" {
    dump_tokens("EQUAL\t%s\n", yytext);
    return EQUAL;
}
">=" {
    dump_tokens("LARGEEQUAL\t%s\n", yytext);
    return LARGEEQUAL;
}
"<=" {
    dump_tokens("LESSEQUAL\t%s\n", yytext);
    return LESSEQUAL;
}
"&&" {
    dump_tokens("AND\t%s\n", yytext);
    return AND;
}
"||" {
    dump_tokens("OR\t%s\n", yytext);
    return OR;
}
"!=" {
    dump_tokens("UNEQUAL\t%s\n", yytext);
    return UNEQUAL;
}
"!" {
    dump_tokens("NOT\t%s\n", yytext);
    return NOT;
}

"=" {
    dump_tokens("ASSIGN\t%s\n", yytext);
    return ASSIGN;
}

"<" {
    dump_tokens("LESS\t%s\n", yytext);
    return LESS;
}

">" {
    dump_tokens("LARGE\t%s\n", yytext);
    return LARGE;
}
"+" {
    dump_tokens("ADD\t%s\n", yytext);
    return ADD;
}

"-" {
    dump_tokens("SUB\t%s\n", yytext);
    return SUB;
}
"*" {
    dump_tokens("MUL\t%s\n", yytext);
    return MUL;
}
"/" {
    dump_tokens("DIV\t%s\n", yytext);
    return DIV;
}
"%" {
    dump_tokens("MOD\t%s\n", yytext);
    return MOD;
}
";" {
    dump_tokens("SEMICOLON\t%s\n", yytext);
    return SEMICOLON;
}

"(" {
    dump_tokens("LPAREN\t%s\n", yytext);
    return LPAREN;
}

")" {
    dump_tokens("RPAREN\t%s\n", yytext);
    return RPAREN;
}

"{" {
    dump_tokens("LBRACE\t%s\n", yytext);
    return LBRACE;
}

"}" {
    dump_tokens("RBRACE\t%s\n", yytext);
    return RBRACE;
}
"[" {
    dump_tokens("LSQUARE\t%s\n", yytext);
    return LSQUARE;
}
"]" {
    dump_tokens("RSQUARE\t%s\n", yytext);
    return RSQUARE;
}
"getint" {
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    Type* funcType = new FunctionType(TypeSystem::intType, vec);
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext,0);
    globals->install(yytext, se);
    return ID;
}
"getch" {
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    Type* funcType = new FunctionType(TypeSystem::intType, vec);
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext,0);
    globals->install(yytext, se);
    return ID;
}
"getfloat" {
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    Type* funcType = new FunctionType(TypeSystem::floatType, vec);
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext,0);
    globals->install(yytext, se);
    return ID;
}

"putint" {
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    vec.push_back(TypeSystem::intType);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec);
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext,0);
    globals->install(yytext, se);
    return ID;
}
"putfloat" {
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    vec.push_back(TypeSystem::floatType);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec);
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext,0);
    globals->install(yytext, se);
    return ID;
}
"putch" {
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    std::vector<Type*> vec;
    vec.push_back(TypeSystem::intType);
    Type* funcType = new FunctionType(TypeSystem::voidType, vec);
    SymbolEntry* se = new IdentifierSymbolEntry(funcType, yytext,0);
    globals->install(yytext, se);
    return ID;
}
{DECIMIAL} {
    long long int decimal;
    decimal = atoll(yytext);
    dump_tokens("DECIMAL\t%s\t%lld\n", yytext, decimal);
    yylval.itype = decimal;
    return INTEGER;
}

{ID} {
    char *lexeme;
    dump_tokens("ID\t%s\n", yytext);
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}
{OCTAL} {
    long long octal = strtoll(yytext, NULL, 8);
    dump_tokens("%-30s%-30s%-5d%-5d%lld\n", "OCTAL", yytext, yylineno, yycolno, octal);
    yycolno+=yyleng;
    yylval.itype = octal;
    return INTEGER;
}

{HEXADECIMAL} {
    long long hex = strtoll(yytext, NULL, 16);
    dump_tokens("%-30s%-30s%-5d%-5d%lld\n", "HEXADECIMAL", yytext, yylineno, yycolno, hex);
    yycolno+=yyleng;
    yylval.itype = hex;
    return INTEGER;
}

{DECIMAL_FLOATING_CONSTANT} {
    double DECIMAL_value = atof(yytext);
    dump_tokens("%-30s%-30s%-5d%-5d%.6f\n", "DECIMAL_FLOATING_CONSTANT", yytext, yylineno, yycolno, DECIMAL_value);
    yycolno+=yyleng;
    yylval.ftype = DECIMAL_value;
    return FLOATNUM;
}

{HEXADECIMAL_FLOATING_CONSTANT} {
    double HEXADECIMAL_value = atof(yytext);
    dump_tokens("%-30s%-30s%-5d%-5d%.6f\n", "HEXADECIMAL_FLOATING_CONSTANT", yytext, yylineno, yycolno, HEXADECIMAL_value);
    yycolno+=yyleng;
     yylval.ftype = HEXADECIMAL_value;
    return FLOATNUM;
}

{EOL} {
    yycolno=0;
}

{WHITE} {
    yycolno+=yyleng;
}

%%

/* user code section */
