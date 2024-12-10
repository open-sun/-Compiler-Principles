#include "IRSCCP.h"
#include <unordered_set>
#include <vector>


using namespace std;


bool SCCP::skip(Instruction *inst)
{
    /**
     * 判断当前指令是否可以当成一个表达式
     * 当前只将二元运算指令当作表达式
     * 纯函数及一些一元指令也可当作表达式
     */
    if (dynamic_cast<BinaryInstruction *>(inst) != nullptr||dynamic_cast<UnaryExprInstruction *>(inst) != nullptr)
        return false;
    return true;
}



 bool SCCP::funcsccp(Function *func)
 {
    bool result=true;;
     for (auto block = func->begin(); block != func->end(); block++)
    {
        
        for (auto inst = (*block)->begin(); inst != (*block)->end(); inst = inst->getNext())
        {
            if(!skip(inst))
            {
                if(inst->defcanbeconst())
                {
                    SymbolEntry *se = new ConstantSymbolEntry(inst->getDef()->getType(),inst->getdefvalue());
                    Operand *newuse=new Operand(se);
                    Operand *olduse=inst->getDef();
                    std::vector<Instruction *> oldtobereplace=olduse->getUse();
                    if(oldtobereplace.size()!=0)
                    {
                    
                        for(size_t i=0;i<oldtobereplace.size();i++)
                        {
                        oldtobereplace[i]->replaceUse(newuse,olduse);
                        }   
                    }
                    auto pre=inst->getPrev();
                    (*block)->remove(inst);
                    delete inst;
                    inst=pre;
                    result=false;
            }
        }


    }

    }




    return result;
 }

void SCCP::execute() {
   for (auto func = unit->begin(); func != unit->end(); func++)
    {
        while (!funcsccp(*func))
            ;
    }
}
