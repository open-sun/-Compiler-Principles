#include "Adce.h"


using namespace std;


void ADCE::initial(Function * func)
{
    func->idom.clear();
    func->sdom.clear();
    func->DFSTree.clear();
    for(auto bb:func->getBlockList())
    {
        bb->dominators.clear();
        bb->indexInFunc=-1;
    }
    func->computeRDomFrontier();
    for (auto block = func->begin(); block != func->end(); block++)
    {
        for(auto inst=(*block)->begin();inst!=(*block)->end();inst = inst->getNext())
        {
          
            if(inst->isret()||inst->isCall())
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
        if(inst->isGlobal())
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
            

            PhiInstruction *phi=dynamic_cast<PhiInstruction*>(inst);
            auto uses=phi->getUse();
            for(size_t i=0;i<uses.size();i++)
        {
            if(uses[i]->getDef()!=nullptr&&!uses[i]->getDef()->islive())
            {
                worklist.push_back(uses[i]->getDef());
                uses[i]->getDef()->setlive();
                if(!uses[i]->getDef()->getParent()->rbegin()->islive())
                {
                      uses[i]->getDef()->getParent()->rbegin();//you shu cai huo yue
                }
              
            }
        }
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
                  auto block = inst->getParent();
         if(!block->dominators.empty())
        {
          
        for (auto bb: block->dominators) {
          if((bb->rbegin()->isCond()||bb->rbegin()->isUncond())&&!bb->rbegin()->islive())
          {
            printf("woshi%d,wo de yilai qian qu shi %d \n",block->getNo(),bb->getNo());
            worklist.push_back(bb->rbegin());
            bb->rbegin()->setlive();
          }
        }
        }
            }


        if(inst->getParent()!=nullptr)
        {
          
              inst->getParent()->setlive();
        }
    }
        
}
void ADCE::deblock(Function *func)
{
    // func->idom.clear();
    // func->sdom.clear();
    // func->DFSTree.clear();
    //  for(auto bb:func->getBlockList())
    // {
    //     bb->dominators.clear();
    // }

    //     func->computeDFSTree();
    //     func->computeSdom();
    //     func->computeIdom();
    //     func->computeDomFrontier();
    // func->outputDomTreeIteratively();
     for (auto block = func->begin(); block != func->end(); block++)//zui duo liang ge hou ji 
    {
        auto inst=(*block)->rbegin();
       if(!inst->islive()&&((*block)->getNumOfPred()!=0||(*block)==func->getEntry()))
       {
      
        
        if(func->getfisrtlivesucc((*block))!=nullptr)
        {
          
            
             UncondBrInstruction * newjump=new UncondBrInstruction(func->getfisrtlivesucc((*block)));
             newjump->setlive();
         (*block)->insertBack(newjump);
         (*block)->cleansucc();
        (*block)->addSucc(func->getfisrtlivesucc((*block)));
        func->getfisrtlivesucc((*block))->addPred((*block));
        }
       }
        
    }
}
void ADCE::deinst(Function *func)
{
     for (auto block = func->begin(); block != func->end(); block++)
    {
        if((*block)->islive())
        {
            printf("%dshihuodde\n",(*block)->getNo());
        }
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
        
        deblock((*func));
        deinst((*func));
      }
}