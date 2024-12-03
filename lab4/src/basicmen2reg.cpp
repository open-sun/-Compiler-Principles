#include "basicmem2reg.h"
#include <unordered_set>
#include <vector>
#include"BasicBlock.h"


using namespace std;

void Mem2reg::execute() {
    for (auto func = unit->begin(); func != unit->end(); func++){
        BasicBlock *temp=(*func)->getEntry();
        Instruction *ins=temp->begin();
        for(;ins!=temp->end();)
        {
            if(ins->isrAlloca())
            {
                allocalist.push_back(ins);
            }
            ins=ins->getNext();
        }
    }

    for(size_t i=0;i<allocalist.size();i++)
    {
        AllocaInstruction *alloca=dynamic_cast<AllocaInstruction*>(allocalist[i]);
        if(!alloca->isloaded())
        {
            removeuse(alloca);
           
            alloca->getParent()->remove(alloca);
        }
    }
}