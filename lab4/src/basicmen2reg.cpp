#include "basicmem2reg.h"
#include <unordered_set>
#include <vector>
#include"BasicBlock.h"


using namespace std;

void Mem2reg::execute() {
    for (auto func = unit->begin(); func != unit->end(); func++){//形成allocalist，alloca只在entry里
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
    //清除noload，一些普适的判断或操作直接在instruction加了函数，不过当函数里用到block的相关函数就不大行，会发生error，所以这里remove在mem这个类定义了
    {
        AllocaInstruction *alloca=dynamic_cast<AllocaInstruction*>(allocalist[i]);
        if(!alloca->isloaded())
        {
            removeuse(alloca);
            alloca->getParent()->remove(alloca);
        }
    }
}