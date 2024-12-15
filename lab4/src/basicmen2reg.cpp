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
        (*func)->computeDFSTree();
         (*func)->computeSdom();
       (*func)->computeIdom();
       // std::cout<<"1ok";
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
        BasicBlock *block = alloca->getParent();


        if (alloca->defAndUse()) {
            // 维护一个变量 val，初始为 undef
            Operand *val = nullptr;
         //   Operand *use=alloca->getDef();

            // 遍历基本块中的指令
            for (Instruction *ins = block->begin(); ins != block->end(); ) {
                Instruction *next = ins->getNext();

                if (ins->isStore() && ins->getUse()[0] == alloca->getDef()) {
                    // 遇到 store 指令，更新 val 并删除 store
                  
                    val = ins->getUse()[1];
                    block->remove(ins);
                } 
                else if (ins->isLoad() && ins->getUse()[0] == alloca->getDef()) {
                                      
                    // 遇到 load 指令，替换其结果的所有使用，并删除 load
                    Operand *loadResult = ins->getDef();
                    for (auto use = loadResult->use_begin(); use != loadResult->use_end(); ) {
                                        
                        (*use)->replaceUse(val,loadResult);
                         use = loadResult->use_begin(); // 更新迭代器
                    }
                    block->remove(ins);
                }
                ins = next; // 继续遍历下一条指令
            }

            // 删除优化后的 alloca
            block->remove(alloca);

        }


    }
}