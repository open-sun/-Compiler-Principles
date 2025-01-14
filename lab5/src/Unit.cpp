#include "Unit.h"
extern FILE* yyout;
void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}
void Unit::insertglobal(GlobalInstruction *g)
{
    global_list.push_back(g);
}
void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::output() const
{
    fprintf(yyout, "target triple = \"x86_64-pc-linux-gnu\"\n");
    fprintf(yyout, "declare i32 @getint()\n");
    fprintf(yyout, "declare void @putint(i32)\n");
    fprintf(yyout, "declare i32 @getch()\n");
    fprintf(yyout, "declare void @putch(i32)\n");
    fprintf(yyout, "declare void @putf(i32)\n\n");
    for (auto &global : global_list)
        global->output();


    for (auto &func : func_list)
        func->output();
}

void Unit::genMachineCode(MachineUnit* munit) 
{
    AsmBuilder* builder = new AsmBuilder();
    builder->setUnit(munit);
    for(auto global:global_list)
    {
        auto sym=global->getDef()->getEntry();
        builder->getUnit()->insetglobal(sym);
    }
    for (auto &func : func_list)
        func->genMachineCode(builder);
}
Unit::~Unit()
{
    auto delete_list = func_list;
    for(auto &func:delete_list)
        delete func;
}
