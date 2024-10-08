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

    typedef struct SymbolTableEntry {
        char* name; 
        struct SymbolTableEntry* next;                           
    } SymbolTableEntry;

    typedef struct SymbolTable {
        SymbolTableEntry* head; 
        int size;                            
    } SymbolTable;

    SymbolTable* createSymbolTable() {
        SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
        table->head = NULL;
        table->size = 0;
        return table;
    }

      
    void insertSymbol(SymbolTable* table, const char* name) {
       
        SymbolTableEntry* current = table->head;
        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {
                return; 
            }
            current = current->next;
        }

        // 创建新节点
        SymbolTableEntry* newNode = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
        newNode->name = strdup(name); 
        newNode->next = table->head;   
        table->head = newNode;    
        table->size++;
    }

    // 查找符号
    SymbolTableEntry* lookupSymbol(SymbolTable* table, const char* name) {
        SymbolTableEntry* current = table->head;
        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {
                return current; 
            }
            current = current->next;
        }
        return NULL; 
    }



    typedef struct Scope {
        SymbolTable* table;    // 当前作用域的符号表
        struct Scope* parent;  // 上一级作用域
    } Scope;

    Scope* enterScope(Scope* parent) {
        Scope* scope = (Scope*)malloc(sizeof(Scope));
        scope->parent = parent;
        scope->table = createSymbolTable();
        return scope;
    }
    Scope* exitScope(Scope* currentScope) {
        Scope* parent = currentScope->parent;
        SymbolTableEntry* current = currentScope->table->head;
        while (current != NULL) {
            SymbolTableEntry* temp = current;
            current = current->next;
            free(temp->name); 
            free(temp);     
        }
        free(currentScope->table); 
        free(currentScope);        
        return parent;
    }




    Scope* currentScope = enterScope(NULL);
    /* Your code here, if desired (lab3). */

    
%}

/* definitions section */
DECIMIAL ([1-9][0-9]*|0)
OCTAL 0[0-7]+
HEXADECIMAL 0[xX][0-9a-fA-F]+
FLOAT ([0-9]+(\.[0-9]+)?([eE][-+]?[0-9]+)?)


%x COMMENT
%x SCOMMENT
ID [[:alpha:]_][[:alpha:][:digit:]_]*

EOL (\r\n|\n|\r)

WHITE [\t ]

/*  Your code here (lab3). */

%%
    /* rules section */
<COMMENT>
{
    "*/" {
    yycolno+=yyleng;
    BEGIN INITIAL;
}
}
<COMMENT>
{
    . {
    yycolno+=yyleng;
}
}
<COMMENT>
{
    {EOL} {
    yycolno=0;

}
}
<SCOMMENT>
{
    {EOL} {
    yycolno=0;
    BEGIN INITIAL;

}
}
<SCOMMENT>
{
    . {
    yycolno+=yyleng;
}
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

"==" {
    dump_tokens("EQUAL\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
"!=" {
    dump_tokens("UNEQUAL\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
">=" {
    dump_tokens("LARGEEQUAL\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
"<=" {
    dump_tokens("LESSEQUAL\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
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
"%" {
    dump_tokens("MOD\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
"!" {
    dump_tokens("NOT\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
"&&" {
    dump_tokens("AND\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    yycolno+=yyleng;
}
"||" {
    dump_tokens("OR\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
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

    currentScope = enterScope(currentScope);
    yycolno+=yyleng;
}

"}" {
    dump_tokens("RBRACE\t%s\t%d\t%d\n", yytext,yylineno,yycolno);
    currentScope = exitScope(currentScope);
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

    SymbolTableEntry* entry = lookupSymbol(currentScope->table, yytext);
        if (!entry) {
            // 如果标识符不存在，插入到当前作用域的符号表中
            insertSymbol(currentScope->table, yytext);
            entry = lookupSymbol(currentScope->table, yytext);
        }

    dump_tokens("ID\t%s\t%d\t%d\t%p\n", yytext, yylineno, yycolno, (void*)entry);

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
