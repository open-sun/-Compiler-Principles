#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "Operand.h"
#include <vector>
#include <map>

//replace都给加上了，应该没问题。遇到报错时候可以试试makegdb的，看是不是这的问题。
class BasicBlock;

class Instruction
{
public:
    Instruction(unsigned instType, BasicBlock *insert_bb = nullptr);
    virtual ~Instruction();
    BasicBlock *getParent();
    bool isUncond() const {return instType == UNCOND;};
    bool isCond() const {return instType == COND;};
    bool isret()  const {return instType==RET;};
    bool isrAlloca()  const {return instType==ALLOCA;};
    bool isBin()  const {return instType==BINARY;};
    bool isLoad()  const {return instType==LOAD;};
    bool isStore()  const {return instType==STORE;};
    bool isCmp()  const {return instType==CMP;};
    bool isCall()  const {return instType==CALL;};
    void setParent(BasicBlock *);
    void setNext(Instruction *);
    void setPrev(Instruction *);
    Instruction *getNext();
    Instruction *getPrev();
    virtual Operand *getDef() { return nullptr; }
    virtual std::vector<Operand *> getUse() { return {}; }
    virtual void output() const = 0;
    virtual void replaceUse(Operand *, Operand *) {}
    virtual void replaceDef(Operand *) {}
    virtual unsigned getinsttype(){return instType;}
    virtual unsigned getopcode(){return opcode;}
    virtual bool defcanbeconst(){return false;}
    virtual double getdefvalue(){return 0;}
protected:
    unsigned instType;
    unsigned opcode;
    Instruction *prev;
    Instruction *next;
    BasicBlock *parent;
    std::vector<Operand*> operands;
    enum {BINARY, COND, UNCOND, RET, LOAD, STORE, CMP, ALLOCA,UNARY,GLOBAL,CALL,XOR,ZEXT, TYPECONVER,};
};

// meaningless instruction, used as the head node of the instruction list.
class DummyInstruction : public Instruction
{
public:
    DummyInstruction() : Instruction(-1, nullptr) {};
    void output() const {};
};

class AllocaInstruction : public Instruction
{
public:
    AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb = nullptr);
    ~AllocaInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    void replaceDef(Operand * temp)
    {
        operands[0]->setDef(nullptr);
        operands[0]=temp;
        temp->setDef(this);
    }
    bool isloaded()
    {
        Operand *temp=operands[0];
        for(auto ins=temp->use_begin();ins!=temp->use_end();ins++)
        {
            if((*ins)->isLoad())
            {
                return true;
            }
        }
        return false;

    }
    
private:
    SymbolEntry *se;
};


class LoadInstruction : public Instruction
{
public:
    LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb = nullptr);
    ~LoadInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1]}; }
     void replaceDef(Operand * temp)
    {
        operands[0]->setDef(nullptr);
        operands[0]=temp;
        temp->setDef(this);
    }
    void replaceUse(Operand *newuse,Operand* olduse)
    {
      for(size_t i=1;i<operands.size();i++)
      {
        if(operands[i]==olduse)
        {
            newuse->addUse(this);
            operands[i]->removeUse(this);
            operands[i]=newuse;
        }
      }
    }
};

class StoreInstruction : public Instruction
{
public:
    StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb = nullptr);
    ~StoreInstruction();
    void output() const;
    std::vector<Operand *> getUse() { return {operands[0], operands[1]}; }
     void replaceUse(Operand *newuse,Operand* olduse)
    {
      for(size_t i=0;i<operands.size();i++)
      {
        if(operands[i]==olduse)
        {
            newuse->addUse(this);
            operands[i]->removeUse(this);
            operands[i]=newuse;
        }
      }
    }
};
class GlobalInstruction : public Instruction
{
public:
    GlobalInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb = nullptr);
    ~GlobalInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return { operands[1]}; }
    void replaceDef(Operand * temp)
    {
        operands[0]->setDef(nullptr);
        operands[0]=temp;
        temp->setDef(this);
    }
    void replaceUse(Operand *newuse,Operand* olduse)
    {
      for(size_t i=1;i<operands.size();i++)
      {
        if(operands[i]==olduse)
        {
            newuse->addUse(this);
            operands[i]->removeUse(this);
            operands[i]=newuse;
        }
      }
    }
};
class BinaryInstruction : public Instruction
{
public:
    BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~BinaryInstruction();
    void output() const;
    enum {SUB, ADD, AND, OR,MUL,DIV,MOD};
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1], operands[2]}; }
    void replaceDef(Operand * temp)
    {
        operands[0]->setDef(nullptr);
        operands[0]=temp;
        temp->setDef(this);
    }
    void replaceUse(Operand *newuse,Operand* olduse)
    {
      for(size_t i=1;i<operands.size();i++)
      {
        if(operands[i]==olduse)
        {
            newuse->addUse(this);
            operands[i]->removeUse(this);
            operands[i]=newuse;
        }
      }
    }
    bool defcanbeconst()
    {
        if(operands[1]->getsym()->isConstant()&&operands[2]->getsym()->isConstant())
        {
            return true;
        }
        return false;
    }
    double getdefvalue()
    {
        double result=0;
    switch (opcode)
    {
    case ADD:
        result=operands[1]->getsym()->getValue()+operands[2]->getsym()->getValue();
        break;
    case SUB:
         result=operands[1]->getsym()->getValue()-operands[2]->getsym()->getValue();
        break;
    case MUL:
        result=operands[1]->getsym()->getValue()*operands[2]->getsym()->getValue();
        break;
    case DIV:
         result=operands[1]->getsym()->getValue()/operands[2]->getsym()->getValue();
        break;
    case MOD:
         result=static_cast<int>(operands[1]->getsym()->getValue()) % 
         static_cast<int>(operands[2]->getsym()->getValue());
        break;
    default:
        break;
    }
        
        return result;
    }
};
class UnaryExprInstruction : public Instruction
{
public:
    UnaryExprInstruction(unsigned opcode, Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~UnaryExprInstruction();
    void output() const;
    enum {ADD,SUB,NOT};
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1]}; }
    void replaceDef(Operand * temp)
    {
        operands[0]->setDef(nullptr);
        operands[0]=temp;
        temp->setDef(this);
    }
    void replaceUse(Operand *newuse,Operand* olduse)
    {
      for(size_t i=1;i<operands.size();i++)
      {
        if(operands[i]==olduse)
        {
            newuse->addUse(this);
            operands[i]->removeUse(this);
            operands[i]=newuse;
            
        }
      }
    }
};
class CallInstruction : public Instruction
{
public:
    CallInstruction(Operand *dst,SymbolEntry *src,std::vector<Operand *> canshu, BasicBlock *insert_bb = nullptr);
    ~CallInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
     void replaceDef(Operand * temp)
    {
        operands[0]->setDef(nullptr);
        operands[0]=temp;
        temp->setDef(this);
    }
    void replaceUse(Operand *newuse,Operand* olduse)
    {
      for(size_t i=0;i<params.size();i++)
      {
        if(params[i]==olduse)
        {
            newuse->addUse(this);
            params[i]->removeUse(this);
            params[i]=newuse;
        }
      }
    }
private:
    SymbolEntry *name;
    std::vector<Operand *> params;

};
class CmpInstruction : public Instruction
{
public:
    CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~CmpInstruction();
    void output() const;
    enum {E, NE, L, GE, G, LE};
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1], operands[2]}; }
     void replaceDef(Operand * temp)
    {
        operands[0]->setDef(nullptr);
        operands[0]=temp;
        temp->setDef(this);
    }
    void replaceUse(Operand *newuse,Operand* olduse)
    {
      for(size_t i=1;i<operands.size();i++)
      {
        if(operands[i]==olduse)
        {
        newuse->addUse(this);
        operands[i]->removeUse(this);
        operands[i]=newuse;
        }
      }
    }
    
};

// unconditional branch
class UncondBrInstruction : public Instruction
{
public:
    UncondBrInstruction(BasicBlock*, BasicBlock *insert_bb = nullptr);
    void output() const;
    void setBranch(BasicBlock *);
    BasicBlock *getBranch();
    BasicBlock **patchBranch() {return &branch;};
protected:
    BasicBlock *branch;
};

// conditional branch
class CondBrInstruction : public Instruction
{
public:
    CondBrInstruction(BasicBlock*, BasicBlock*, Operand *, BasicBlock *insert_bb = nullptr);
    ~CondBrInstruction();
    void output() const;
    void setTrueBranch(BasicBlock*);
    BasicBlock* getTrueBranch();
    void setFalseBranch(BasicBlock*);
    BasicBlock* getFalseBranch();
    BasicBlock **patchBranchTrue() {return &true_branch;};
    BasicBlock **patchBranchFalse() {return &false_branch;};
    std::vector<Operand *> getUse() { return {operands[0]}; }
    void replaceUse(Operand *newuse,Operand* olduse)
    {
        newuse->addUse(this);
        operands[0]->removeUse(this);
        operands[0]=newuse;
       
    }
protected:
    BasicBlock* true_branch;
    BasicBlock* false_branch;
};

class RetInstruction : public Instruction
{
public:
    RetInstruction(Operand *src, BasicBlock *insert_bb = nullptr);
    ~RetInstruction();
    std::vector<Operand *> getUse()
    {
        if (operands.size())
            return {operands[0]};
        else
            return {};
    }
    void replaceUse(Operand *newuse,Operand* olduse)
    {
        newuse->addUse(this);
        operands[0]->removeUse(this);
        operands[0]=newuse;
    }
    void output() const;
};
class RTinstruction : public Instruction
{
public:
    RTinstruction(Type *src, BasicBlock *insert_bb = nullptr);
    ~RTinstruction();
    void output() const;
    Type*type;
};

class XorInstruction : public Instruction // not指令
{
public:
    XorInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    void output() const;
    Operand *getDef()
    {
        return operands[0];
    }
    std::vector<Operand *> getUse() { return {operands[1]}; }
    void replaceUse(Operand *rep, Operand *old)
    {
        if (operands[1] == old)
        {
            operands[1]->removeUse(this);
            operands[1] = rep;
            rep->addUse(this);
        }
    }
    void replaceDef(Operand *rep)
    {
        operands[0]->setDef(nullptr);
        operands[0] = rep;
        operands[0]->setDef(this);
    }
    void setDef(Operand *rep)
    {
        operands[0] = rep;
        operands[0]->setDef(this);
    }
    Instruction *copy() { return new XorInstruction(*this); }
};

class ZextInstruction : public Instruction 
{
public:
    ZextInstruction(Operand *dst, Operand *src,  BasicBlock *insert_bb = nullptr);
    void output() const;
    Operand *getDef()
    {
        return operands[0];
    }
    std::vector<Operand *> getUse() { return {operands[1]}; }
    void replaceUse(Operand *rep, Operand *old)
    {
        if (operands[1] == old)
        {
            operands[1]->removeUse(this);
            operands[1] = rep;
            rep->addUse(this);
        }
    }
    void replaceDef(Operand *rep)
    {
        operands[0]->setDef(nullptr);
        operands[0] = rep;
        operands[0]->setDef(this);
    }
    void setDef(Operand *rep)
    {
        operands[0] = rep;
        operands[0]->setDef(this);
    }
    Instruction *copy() { return new ZextInstruction(*this); }

private:
};


class TypeConverInstruction : public Instruction
{
public:
    TypeConverInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~TypeConverInstruction();
    void output() const;

private:
    Operand *dst;
    Operand *src;
};


#endif