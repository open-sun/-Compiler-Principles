#ifndef _MEM2REG_H
#define _MEM2REG_H
#include "Instruction.h"
#include "Type.h"
#include "Unit.h"
#include <set>
#include <unordered_set>
#include"BasicBlock.h"
#include"Operand.h"

//消除不可达指令和块在gencode完成的，在完成noload的时候，没有影响。未出现return后有load的指令未被消除的情况。在ast的funcgencode
class Mem2reg {
    Unit *unit;
    std::vector<Instruction *> allocalist;//alloca的表

  public:
    Mem2reg(Unit *_unit) : unit(_unit) {}
    void execute();
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