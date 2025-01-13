#ifndef _ADCE_H
#define _ADCE_H
#include "Instruction.h"
#include "Type.h"
#include "Unit.h"
#include <set>
#include <unordered_set>
#include"BasicBlock.h"
#include"Operand.h"
#include<vector>


class ADCE {
    Unit *unit;

  public:
    ADCE(Unit *_unit) : unit(_unit) {}
    void execute(Function *func);
    std::vector<Instruction*> worklist;
    void initial(Function * func);
    void deblock(Function *func);
    void deinst(Function *func);
    void pass();
};

#endif