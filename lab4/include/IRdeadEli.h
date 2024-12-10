#ifndef _IRDEADCODEELI_H
#define _IRDEADCODEELI_H
#include "Instruction.h"
#include "Type.h"
#include "Unit.h"
#include <set>
#include <unordered_set>
#include"BasicBlock.h"
#include"Operand.h"


class IRDeadEli {
    Unit *unit;

  public:
    IRDeadEli(Unit *_unit) : unit(_unit) {}
    bool execute(Function *func);
    void pass();
     void removeuse(AllocaInstruction* alloca)//当清除一个alloca的时候清除其use，如store啥的
    {
        std::vector<Instruction *> use=alloca->getDef()->getUse();
        for(size_t i=0;i<use.size();i++)
        {
            use[i]->getParent()->remove((use[i]));
            Instruction*temp=use[i];
            use.erase(std::find(use.begin(), use.end(), use[i]));
            delete temp;
            i--;
        }
        
    }
};

#endif