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

    inline void dump_tokens(const char* format, ...) {
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

        SymbolTableEntry* newNode = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
        newNode->name = strdup(name); 
        newNode->next = table->head;   
        table->head = newNode;    
        table->size++;
    }

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
        SymbolTable* table;
        struct Scope* parent;
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
    dump_tokens("%-30s%-10s%-5d%-5d\n", "INT", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"void" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "VOID", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"if" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "IF", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"else" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "ELSE", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"return" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "RETURN", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"==" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "EQUAL", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"!=" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "UNEQUAL", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

">=" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "LARGEEQUAL", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"<=" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "LESSEQUAL", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"=" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "ASSIGN", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"<" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "LESS", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"+" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "ADD", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"-" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "MINUS", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"*" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "MULTIPLE", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"/" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "DIVISION", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"%" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "MOD", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"!" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "NOT", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"&&" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "AND", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"||" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "OR", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

";" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "SEMICOLON", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"(" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "LPAREN", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

")" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "RPAREN", yytext, yylineno, yycolno);
    yycolno+=yyleng;
}

"{" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "LBRACE", yytext, yylineno, yycolno);
    currentScope = enterScope(currentScope);
    yycolno+=yyleng;
}

"}" {
    dump_tokens("%-30s%-10s%-5d%-5d\n", "RBRACE", yytext, yylineno, yycolno);
    currentScope = exitScope(currentScope);
    yycolno+=yyleng;
}

{DECIMIAL} {
    int decimal = atoi(yytext);
    dump_tokens("%-30s%-10s%-5d%-5d%d\n", "DECIMIAL", yytext, yylineno, yycolno, decimal);
    yycolno+=yyleng;
}

{OCTAL} {
    int octal = strtol(yytext, NULL, 8);
    dump_tokens("%-30s%-10s%-5d%-5d%d\n", "OCTAL", yytext, yylineno, yycolno, octal);
    yycolno+=yyleng;
}

{HEXADECIMAL} {
    int hex = strtol(yytext, NULL, 16);
    dump_tokens("%-30s%-10s%-5d%-5d%d\n", "HEXADECIMAL", yytext, yylineno, yycolno, hex);
    yycolno+=yyleng;
}

{DECIMAL_FLOATING_CONSTANT} {
    double DECIMAL_value = atof(yytext);
    dump_tokens("%-30s%-10s%-5d%-5d%.6f\n", "DECIMAL_FLOATING_CONSTANT", yytext, yylineno, yycolno, DECIMAL_value);
    yycolno+=yyleng;
}

{HEXADECIMAL_FLOATING_CONSTANT} {
    double HEXADECIMAL_value = atof(yytext);
    dump_tokens("%-30s%-10s%-5d%-5d%.6f\n", "HEXADECIMAL_FLOATING_CONSTANT", yytext, yylineno, yycolno, HEXADECIMAL_value);
    yycolno+=yyleng;
}

{ID} {
    SymbolTableEntry* entry = lookupSymbol(currentScope->table, yytext);
    if (!entry) {
        insertSymbol(currentScope->table, yytext);
        entry = lookupSymbol(currentScope->table, yytext);
    }
    dump_tokens("%-30s%-10s%-5d%-5d%p\n", "ID", yytext, yylineno, yycolno, (void*)entry);
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
