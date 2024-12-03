#ifndef _BLOCKMERGE_H
#define _BLOCKMERGE_H
#include "Instruction.h"
#include "Type.h"
#include "Unit.h"
#include <set>
#include <unordered_set>
#include"BasicBlock.h"
#include"Operand.h"

class Mem2reg {
    Unit *unit;
    std::vector<Instruction *> allocalist;

  public:
    Mem2reg(Unit *_unit) : unit(_unit) {}
    void execute();
    void removeuse(AllocaInstruction* alloca)
    {
        for(auto ins=alloca->getDef()->use_begin();ins!=alloca->getDef()->use_end();ins++)
        {
            (*ins)->getParent()->remove((*ins));
        }
    }
};

#endif