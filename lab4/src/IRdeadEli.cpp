#include "IRdeadEli.h"


using namespace std;




bool IRDeadEli::execute(Function * func) {
    bool result=true;
    
        for (auto block = func->begin(); block != func->end(); block++)
    {
        for(auto inst=(*block)->begin();inst!=(*block)->end();inst = inst->getNext())
        {
           if(inst->getDef()!=nullptr)
            {
             if(inst->isrAlloca())
            {
                AllocaInstruction *alloca=dynamic_cast<AllocaInstruction*>(inst);
                if(!alloca->isloaded())
                {
                    removeuse(alloca);
                     auto prevInst = inst->getPrev();
                    (*block)->remove(inst);
                    delete inst;
                    inst=prevInst;
                    result=false;
                }
            }else{
                if(inst->isCall()&&inst->getDef()->getType()->isVoid())// wu shang da ya 
                {
                    continue;
                }
                if(inst->getDef()->getUse().empty())
                {
                    auto prevInst = inst->getPrev();
                    (*block)->remove(inst);
                    delete inst;
                    inst=prevInst;
                    result=false;
                }
            }
            }
            
        }
        
    }
   

return result;
}
void IRDeadEli::pass()
{
    for (auto func = unit->begin(); func != unit->end(); func++)
    {
        while (!execute((*func)))
            ;
    }
}