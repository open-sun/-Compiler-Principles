#include "Adce.h"


using namespace std;


void ADCE::initial(Function * func)
{
    for (auto block = func->begin(); block != func->end(); block++)
    {
        for(auto inst=(*block)->begin();inst!=(*block)->end();inst = inst->getNext())
        {
          
            if(inst->isret()||inst->isCall()||inst->isCond())
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
        else if (inst->isPhi())
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
      if((*block)->rbegin()->isCond())
      {
        CondBrInstruction * inst=static_cast<CondBrInstruction*>((*block)->rbegin());
        Operand * cond=inst->getUse()[0];
        if(cond->getsym()->isConstant())
        {
            int value=cond->getsym()->getValue();
            BasicBlock *truebb=inst->getTrueBranch();
            BasicBlock *falsebb=inst->getFalseBranch();
            if(value==1)
            {

               UncondBrInstruction* un=new UncondBrInstruction(truebb);
                (*block)->remove(inst);
                (*block)->insertBack(un);
                // delete falsebb;
                if(falsebb->getNumOfPred()==1)
                {
                    std::vector<BasicBlock*> succ=(*block)->getsucc();
                    for(size_t i=0;i<succ.size();i++)
                    {
                        if(succ[i]==falsebb)
                        {
                            succ.erase(std::remove(succ.begin(), succ.end(), falsebb), succ.end());
                            break;
                        }
                    }
                func->remove(falsebb);
                   (*block)->setsucc(succ);
                  

                }
                else
                {
                (*block)->removeSucc(falsebb);
                falsebb->removePred((*block));
                }
            }
            else if(value==0)
            {
                     UncondBrInstruction* un=new UncondBrInstruction(falsebb);
                (*block)->remove(inst);
                (*block)->insertBack(un);
                // delete falsebb;
                if(truebb->getNumOfPred()==1)
                {
                    std::vector<BasicBlock*> succ=(*block)->getsucc();
                    for(size_t i=0;i<succ.size();i++)
                    {
                        if(succ[i]==truebb)
                        {
                            succ.erase(std::remove(succ.begin(), succ.end(), truebb), succ.end());
                            break;
                        }
                    }
                func->remove(truebb);
                   (*block)->setsucc(succ);
                  

                }
                else
                {
                (*block)->removeSucc(truebb);
                truebb->removePred((*block));
                }

            }
        }
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
        deblock((*func));
      }
}