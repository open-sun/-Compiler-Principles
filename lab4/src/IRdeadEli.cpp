#include "IRdeadEli.h"


using namespace std;




void IRDeadEli::execute() {
     for (auto func = unit->begin(); func != unit->end(); func++){
        for (auto block = (*func)->begin(); block != (*func)->end(); block++)
    {
        for(auto inst=(*block)->begin();inst!=(*block)->end();inst = inst->getNext())
        {
           if(inst->getDef()!=nullptr)
            {
                if(inst->isCall())
                {
                    continue;
                }
                if(inst->getDef()->getUse().empty())
                {
                    auto prevInst = inst->getPrev();
                    (*block)->remove(inst);
                    delete inst;
                    inst=prevInst;
                }
            }
        }
    }
   
}
}