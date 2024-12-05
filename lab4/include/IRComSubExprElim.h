#ifndef __IRCOMSUBEXPRELIM_H__
#define __IRCOMSUBEXPRELIM_H__

#include "Unit.h"

struct Expr
{
    Instruction *inst;
    Expr(Instruction *inst) : inst(inst){};
    // 用于调用find函数
    bool operator==(const Expr &other) const
    {
        // TODO: 判断两个表达式是否相同
        // 两个表达式相同 <==> 两个表达式对应的指令的类型和操作数均相同
        if(inst->getinsttype()!=other.inst->getinsttype())
        {
            return false;
        }
       if(inst->getopcode()!=other.inst->getopcode())
       {
        return false;
       }
        auto operand1=inst->getUse();
        auto operand2=other.inst->getUse();
       if(operand1.size()==2)
       {
            if(operand1[0]->getsym()->isConstant()&&operand1[1]->getsym()->isConstant()&&operand2[0]->getsym()->isConstant()&&operand2[1]->getsym()->isConstant())
            {
                if(dynamic_cast<ConstantSymbolEntry*>(operand1[0]->getsym())->getValue()==dynamic_cast<ConstantSymbolEntry*>(operand2[0]->getsym())->getValue()
                &&dynamic_cast<ConstantSymbolEntry*>(operand1[1]->getsym())->getValue()==dynamic_cast<ConstantSymbolEntry*>(operand2[1]->getsym())->getValue())
                {
                    return true;
                }
                else if(dynamic_cast<ConstantSymbolEntry*>(operand1[0]->getsym())->getValue()==dynamic_cast<ConstantSymbolEntry*>(operand2[1]->getsym())->getValue()
                &&dynamic_cast<ConstantSymbolEntry*>(operand1[1]->getsym())->getValue()==dynamic_cast<ConstantSymbolEntry*>(operand2[0]->getsym())->getValue())
                {
                    return true;
                }
            }
            else if(operand1[0]->getsym()->isTemporary()&&operand1[1]->getsym()->isConstant()&&operand2[0]->getsym()->isTemporary()&&operand2[1]->getsym()->isConstant())
            {
                if(dynamic_cast<ConstantSymbolEntry*>(operand1[1]->getsym())->getValue()==dynamic_cast<ConstantSymbolEntry*>(operand2[1]->getsym())->getValue()
                &&operand1[0]==operand2[0])
                {
                        return true;
                }
            }
            else if(operand1[0]->getsym()->isConstant()&&operand1[1]->getsym()->isTemporary()&&operand2[0]->getsym()->isConstant()&&operand2[1]->getsym()->isTemporary())
            {
                if(dynamic_cast<ConstantSymbolEntry*>(operand1[0]->getsym())->getValue()==dynamic_cast<ConstantSymbolEntry*>(operand2[0]->getsym())->getValue()
                &&operand1[1]==operand2[1])
                {
                        return true;
                }
            }
       }
       else if(operand1.size()==1)
       {
        if(operand1[0]->getsym()->isConstant()&&operand2[0]->getsym()->isConstant())
        {
            if(dynamic_cast<ConstantSymbolEntry*>(operand1[0]->getsym())->getValue()==dynamic_cast<ConstantSymbolEntry*>(operand2[0]->getsym())->getValue())
            {
                return true;
            }
        }
        else if(operand1[0]==operand2[0])
        {
            return true;
        }
       }
       
        
        
        return false;
    };
};

class IRComSubExprElim
{
private:
    Unit *unit;

    std::vector<Expr> exprVec;
    std::map<Instruction *, int> ins2Expr;
    std::map<BasicBlock *, std::set<int>> genBB;
    std::map<BasicBlock *, std::set<int>> killBB;
    std::map<BasicBlock *, std::set<int>> inBB;
    std::map<BasicBlock *, std::set<int>> outBB;

    // 跳过无需分析的指令
    bool skip(Instruction *);

    // 局部公共子表达式消除
    bool localCSE(Function *);

    // 全局公共子表达式消除
    bool globalCSE(Function *);
    void calGenKill(Function*);
    void calInOut(Function*);
    bool removeGlobalCSE(Function*);

public:
    IRComSubExprElim(Unit *unit);
    ~IRComSubExprElim();
    void pass();
};

#endif