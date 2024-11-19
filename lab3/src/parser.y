%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
    Type *currtype;
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    double ftype;
    long long int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    FuncFParams* funcparamtype;
    FuncRParams* funccallparamtype;
    Type* type;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER <ftype>FLOATNUM
%token IF ELSE WHILE CONTINUE BREAK CONST 
%token INT VOID FLOAT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON LSQUARE RSQUARE COMMA
%token ADD SUB OR AND LESS ASSIGN MUL DIV LARGE NOT UNEQUAL MOD LARGEEQUAL LESSEQUAL EQUAL
%token RETURN 

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt ContinueStmt EmptyStmt BreakStmt DeclStmt FuncDef WhileStmt VarDefs ConstDefs VarDef ConstDef Exprstmt
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp UnaryExp MulExp EqExp ConstExp ConstInitVal InitVal FuncCallExp   // ConstInitValList FuncRParams FuncRParam 
%nterm <type> Type
%type<funcparamtype> FuncFParams
%type<funccallparamtype> FuncRParams
%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt
    : AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | BreakStmt {$$=$1;}
    | ContinueStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    | WhileStmt {$$=$1;}
    | EmptyStmt {$$=$1;} 
    |Exprstmt{$$=$1;}
    ;
LVal
    : ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
         //               se = new IdentifierSymbolEntry(TypeSystem::voidType, $1, identifiers->getLevel());
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ;
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;
WhileStmt
    :
    WHILE LPAREN Cond RPAREN Stmt {
        $$ = new WhileStmt($3, $5);
    }
    ;
EmptyStmt
    : SEMICOLON {$$ = new EmptyStmt();}
    | LBRACE RBRACE {$$ = new EmptyStmt();}
    ;

BlockStmt
    :   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        Stmts RBRACE 
        {
            $$ = new CompoundStmt($3);
            SymbolTable *top = identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;
ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    ;
BreakStmt
    : BREAK SEMICOLON {$$ = new BreakStmt();}
    ;
ContinueStmt
    : CONTINUE SEMICOLON {$$ = new ContinueStmt();}
    ;

Exprstmt
: Exp SEMICOLON{
    $$=new Exprstmt($1);
}

Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;
PrimaryExp
    :
    LVal {
        $$ = $1;
    }
    | INTEGER {

        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    | FLOATNUM{
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::floatType, $1);
        $$ = new Constant(se);
    }
    | LPAREN Exp RPAREN
    {
         $$ = $2;
    }

    ;
UnaryExp
    :
    PrimaryExp {
        $$ = $1;
    }
    | ADD UnaryExp {
        if($2->getType()->isFloat())
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new UnaryExpr(se, UnaryExpr::ADD, $2);
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new UnaryExpr(se, UnaryExpr::ADD, $2);
        }
    }
    | SUB UnaryExp {
        if($2->getType()->isFloat())
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
        }
    }
    | NOT UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    | FuncCallExp {$$=$1;}
    ;
MulExp
     :
    UnaryExp {$$ = $1;}
    | MulExp MUL UnaryExp 
    {
        if($1->getType()->isFloat()||$3->getType()->isFloat())
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
        }
    }
    | MulExp DIV UnaryExp 
    {
        if($1->getType()->isFloat()||$3->getType()->isFloat())
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
        }
    }
    |
    MulExp MOD UnaryExp
    {
        if($1->getType()->isFloat()||$3->getType()->isFloat())
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
        }
    }
    ;
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {
        if($1->getType()->isFloat()||$3->getType()->isFloat())
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
        }
    }
    |
    AddExp SUB MulExp
    {
       if($1->getType()->isFloat()||$3->getType()->isFloat())
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
        }
    }
    ;
ConstExp
    :
    AddExp {$$ = $1;}
    ;
ConstInitVal
    :
    ConstExp {$$ = $1;}
    | LBRACE ConstExp RBRACE {$$ = $2;}
    ;


RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    |
    RelExp LARGE AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LARGE, $1, $3);
    }
    |
    RelExp LARGEEQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LARGEEQUAL, $1, $3);
    }
    |
    RelExp LESSEQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    ;
EqExp
    :
    RelExp {$$ = $1;}
    |
    EqExp EQUAL RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    |
    EqExp UNEQUAL RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::UNEQUAL, $1, $3);
    }
LAndExp
    :
    EqExp {$$ = $1;}
    |
    LAndExp AND EqExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
Type
    : INT {
        $$ = TypeSystem::intType;
        currtype=TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
    |FLOAT{
        $$ = TypeSystem::floatType;
        currtype=TypeSystem::floatType;;
    }
    ;
InitVal
    :
    Exp {$$ = $1;}
    |LBRACE Exp RBRACE {$$ = $2;}
    ;

DeclStmt
    :
    Type VarDefs SEMICOLON {
        $$ = $2;
    }
    |
    CONST Type ConstDefs SEMICOLON {
        $$ = $3;
    }
    ;
VarDefs
    :
    VarDefs COMMA VarDef {
        $$ = new SeqNode($1, $3);
      
    } 
    | 
    VarDef {$$ = $1;}
    ;
ConstDefs
    : 
    ConstDefs COMMA ConstDef {
        $$ = new SeqNode($1, $3);
    }
    | 
    ConstDef { $$ = $1; }
    ;
ConstDef
    : 
    ID ASSIGN ConstInitVal {
        SymbolEntry *se = new IdentifierSymbolEntry(currtype, $1, identifiers->getLevel());
        identifiers->install($1, se);
       $$ = new DeclStmt(new Id(se),$3); 
        delete []$1;
    }
    | ID LSQUARE ConstExp  RSQUARE ASSIGN ConstInitVal{
        SymbolEntry *se = new IdentifierSymbolEntry(currtype, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se),$3); 
        delete []$1;
    }
    ;
VarDef
    : 
    ID {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(currtype, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se));
        delete []$1;
    }
    | 
    ID ASSIGN InitVal {
        
        SymbolEntry *se = new IdentifierSymbolEntry(currtype, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se),$3); 
        delete []$1;
    }
    ;
 
FuncFParams
    :
    Type ID { 
        SymbolEntry *se;
        $$ = new FuncFParams();
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$->AddParams(se);
        delete []$2;
    }
    | 
    FuncFParams COMMA Type ID {
        $$ = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        identifiers->install($4, se);
        $$->AddParams(se);
    }
    |
    %empty { $$ = new FuncFParams(); }
    ;


FuncDef
    :
    Type ID {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    LPAREN FuncFParams RPAREN 
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, $7,$5);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();// xin jian zuo yong yu
        delete top;
        delete []$2;
    }
    ;

FuncCallExp 
    : 
     ID LPAREN FuncRParams RPAREN  {   
        
        SymbolEntry* se;   
        se = identifiers->lookup($1);
        SymbolEntry *temp = new TemporarySymbolEntry(static_cast<FunctionType *>(se->getType())->getRetType(), SymbolTable::getLabel());
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new FuncCallExp(se,temp, $3);
    }
    ;

FuncRParams
    :
    Exp { 
        $$ = new FuncRParams();
        $$->AddParams($1);
    }
    |
    FuncRParams COMMA Exp { 
        $$ = $1;
        $$->AddParams($3);
    }
    |
    %empty { $$ = new FuncRParams(); }
    ;

%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}

