#include "Unit.h"

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
    for (auto &global : global_list)
        global->output();


    for (auto &func : func_list)
        func->output();
}

Unit::~Unit()
{
    auto delete_list = func_list;
    for(auto &func:delete_list)
        delete func;
}
