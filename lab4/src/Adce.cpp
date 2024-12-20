#include "Adce.h"


using namespace std;


void ADCE::initial(Function * func)
{
    for (auto block = func->begin(); block != func->end(); block++)
    {
        for(auto inst=(*block)->begin();inst!=(*block)->end();inst = inst->getNext())
        {
          
            if(inst->isret()||inst->isCall()||inst->isUncond())
            {
                inst->setlive();
                inst->getParent()->setlive();
                worklist.push_back(inst);
            }
        }
        
    }
}

void ADCE::execute(Function * func) {
    
    while(!worklist.empty())
    {
        Instruction *inst=worklist.back();
        worklist.pop_back() ;// 从工作列表中弹出该指令
        if(inst->isAlloca()||inst->isGlobal())
        {
            Operand * def=inst->getDef();
            std::vector<Instruction*> uses=def->getUse();
              for(size_t i=0;i<uses.size();i++)
            {
            if(uses[i]->isStore()&&!uses[i]->islive())
            {
                worklist.push_back(uses[i]);
                uses[i]->setlive();
            }
        }
        }
        else if(inst->isPhi())
        {
            
        }
        else
        {
        std::vector<Operand*> uses=inst->getUse();
        for(size_t i=0;i<uses.size();i++)
        {
            if(uses[i]->getDef()!=nullptr&&!uses[i]->getDef()->islive())
            {
                worklist.push_back(uses[i]->getDef());
                uses[i]->getDef()->setlive();
            }
        }
        }
        if(inst->getParent()!=nullptr)
        {
          
              inst->getParent()->setlive();
         for(auto pre=inst->getParent()->pred_begin();pre!=inst->getParent()->pred_end();pre++)
                {
                    (*pre)->setlive();
                    Instruction *lastinst=(*pre)->rbegin();
                    if(!lastinst->islive())
                    {
                    lastinst->setlive();
                    worklist.push_back(lastinst);
                    }
                   
                }
        }
    }
        
}
void ADCE::deblock(Function *func)
{
     for (auto block = func->begin(); block != func->end(); block++)
    {
       if(!(*block)->islive())
       {
        
       }
        
    }
}
void ADCE::deinst(Function *func)
{
     for (auto block = func->begin(); block != func->end(); block++)
    {
        for(auto inst=(*block)->begin();inst!=(*block)->end();inst = inst->getNext())
        {
          if(!inst->islive())
          {
              auto prevInst = inst->getPrev();
                    (*block)->remove(inst);
                    delete inst;
                    inst=prevInst;
          }
        }
    }
}
void ADCE::pass()
{
      for (auto func = unit->begin(); func != unit->end(); func++){
        initial((*func));
        execute((*func));
        deinst((*func));
      }
}