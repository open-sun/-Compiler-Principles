#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include "Function.h"
#include "Type.h"
#include "Ast.h"
extern FILE* yyout;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
    else
    {
        parent=nullptr;
    }
    live=false;

}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}









BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    case MUL:
        op = "mul";
        break;
    case DIV:
        op = "sdiv";
        break;
    case MOD:
        op = "srem";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}
UnaryExprInstruction::UnaryExprInstruction(unsigned opcode, Operand *dst, Operand *src1, BasicBlock *insert_bb) : Instruction(UNARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    dst->setDef(this);
    src1->addUse(this);
}
void UnaryExprInstruction::output() const
{
    std::string s1, s2, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    case NOT:
        op = "xor";
        break;
    default:
        break;
    }
    if(op=="xor")
    {
        fprintf(yyout, "  %s = %s i1 %s,true\n", s1.c_str(), op.c_str(), s2.c_str());
    }
    else{
        fprintf(yyout, "  %s = %s %s 0,%s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str());
    }
}
UnaryExprInstruction::~UnaryExprInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}



CallInstruction::CallInstruction(Operand *dst, SymbolEntry *src,std::vector<Operand *> canshus, BasicBlock *insert_bb) : Instruction(CALL, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    name=src;
    params=canshus;
    for(auto opera=params.begin();opera!=params.end();opera++)
    {
        (*opera)->addUse(this);
    }
}
void CallInstruction::output() const
{
    std::string s1, s2, type,type1;
    s1 = operands[0]->toStr();
    s2= name->toStr();
    type = operands[0]->getType()->toStr();
    type1 =type;
    std::string paramStr;
    if(type1=="void")
    {
         if(params.size()==0)
    {
        fprintf(yyout, "call %s %s\n",type1.c_str(), s2.c_str());
    }
    else{
          for (long unsigned int i = 0; i<params.size(); ++i)
	    {
            if (i > 0) {
            paramStr += ", ";  // 在参数之间添加逗号和空格
                    }
        paramStr += params[i]->getType()->toStr()+" "+ params[i]->toStr();
        }
          fprintf(yyout, "call %s %s(%s)\n",type1.c_str(), s2.c_str(),paramStr.c_str());
        
    }
    }
    else
    {
        if(params.size()==0)
    {
        fprintf(yyout, "  %s =call %s %s()\n", s1.c_str(),type1.c_str(), s2.c_str());
    }
    else{
          for (long unsigned int i = 0; i<params.size(); ++i)
	    {
            if (i > 0) {
            paramStr += ", ";  // 在参数之间添加逗号和空格
                    }
        paramStr += params[i]->getType()->toStr()+" "+ params[i]->toStr();
        }
          fprintf(yyout, "  %s =call %s %s(%s)\n", s1.c_str(),type1.c_str(), s2.c_str(),paramStr.c_str());
        
    }
    }
}
CallInstruction::~CallInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
}




CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb): Instruction(CMP, insert_bb){
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
    switch (opcode)
    {
    case E:
        op = "eq";
        break;
    case NE:
        op = "ne";
        break;
    case L:
        op = "slt";
        break;
    case LE:
        op = "sle";
        break;
    case G:
        op = "sgt";
        break;
    case GE:
        op = "sge";
        break;
    default:
        op = "";
        break;
    }

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock*true_branch, BasicBlock*false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb){
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if(src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if(!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if(operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}
RTinstruction::RTinstruction(Type *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    type=src;
}

RTinstruction::~RTinstruction()
{
   
}

void RTinstruction::output() const
{
    if(type->toStr()=="void()")
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string typeStr = type->toStr();
        std::string ret, type1;
       type1 = typeStr.substr(0, typeStr.size() - 2);
        fprintf(yyout, "  ret %s 0\n", type1.c_str());
    }
}
AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}
StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type1 = operands[1]->getType()->toStr();
     std::string src_type;
    if(src_type1=="i32()"||src_type1=="void()")
    {
          src_type = src_type1.substr(0, src_type1.size() - 2);
    }
    else
    {
        src_type=src_type1;
    }
    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
}
GlobalInstruction::GlobalInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(GLOBAL, insert_bb)
{
    if(src!=nullptr)
    {
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->setDef(this);
    src->addUse(this);
    }
    else{
        operands.push_back(dst_addr);
        dst_addr->setDef(this);
         operands.push_back(nullptr);
    }
}
GlobalInstruction::~GlobalInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void GlobalInstruction::output() const
{

     std::string dst_type = operands[0]->getType()->toStr();
     if(operands[0]->getType()->toStr()=="i32*")
    {
        dst_type="i32";
    }
    else if(operands[0]->getType()->toStr()=="float*")
    {
        dst_type="float";
    }
    if(operands[1]!=nullptr)
    {
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = dso_local global %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src.c_str());
    }
    else
    {
         std::string dst = operands[0]->toStr();
         fprintf(yyout, "  %s =dso_local global %s 0, align 4\n", dst.c_str(), dst_type.c_str());

    }
}


TypeConverInstruction::TypeConverInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(TYPECONVER, insert_bb), dst(dst), src(src)
{
    dst->setDef(this);
    src->addUse(this);
}

TypeConverInstruction::~TypeConverInstruction()
{
    dst->setDef(nullptr);
    src->addUse(this);
}

void TypeConverInstruction::output() const
{
    






    std::string typeConver;
    if (src->getType() == TypeSystem::boolType && dst->getType()->isInt())
    {
        typeConver = "zext";
    }
    else if (src->getType()->isFloat() && dst->getType()->isInt())
    {
        typeConver = "fptosi";
    }
    else if (src->getType()->isInt() && dst->getType()->isFloat())
    {
        typeConver = "sitofp";
    }
    fprintf(yyout, "  %s = %s %s %s to %s\n", dst->toStr().c_str(), typeConver.c_str(), src->getType()->toStr().c_str(), src->toStr().c_str(), dst->getType()->toStr().c_str());






}





XorInstruction::XorInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(XOR,insert_bb)
{
    dst->setDef(this);
    src->addUse(this);
    operands.push_back(dst);
    operands.push_back(src);
}

void XorInstruction::output() const
{
    std::string dst, src;
    dst = operands[0]->toStr();
    src = operands[1]->toStr();
    fprintf(yyout, "  %s = xor i1 %s, true\n", dst.c_str(), src.c_str());
}


ZextInstruction::ZextInstruction(Operand *dst, Operand *src,  BasicBlock *insert_bb) : Instruction(ZEXT, insert_bb)
{
    dst->setDef(this);
    src->addUse(this);
    operands.push_back(dst);
    operands.push_back(src);
}

void ZextInstruction::output() const
{
    std::string dst, src;
    dst = operands[0]->toStr();
    src = operands[1]->toStr();
    fprintf(yyout, "  %s = zext i1 %s to i32\n", dst.c_str(), src.c_str());
}


void PhiInstruction::addSrc(BasicBlock* block, Operand* src) {
    operands.push_back(src);
    srcs.insert(std::make_pair(block, src));
    src->addUse(this);
}

void PhiInstruction::removeSrc(BasicBlock* block){
    for (auto it = srcs.begin(); it != srcs.end(); it++) {
        if(it->first==block){
            //使用erase时容器失效
            srcs.erase(block);
            removeUse(it->second);
            it->second->removeUse(this);
            return;
        }
    }
    return;
}

PhiInstruction::PhiInstruction(Operand* dst, BasicBlock* insert_bb ):Instruction(PHI,insert_bb)
{
    dst->setDef(this);
  //  src->addUse(this);
    operands.push_back(dst);
   // operands.push_back(src);
}

bool PhiInstruction::findSrc(BasicBlock* block){
    for (auto it = srcs.begin(); it != srcs.end(); it++) {
        if(it->first==block){
            return true;
        }
    }
    return false;
}

void PhiInstruction::replaceUse(Operand* old, Operand* new_) {
    for (auto& it : srcs) {
        if (it.second == old) {
            it.second->removeUse(this);
            it.second = new_;
            new_->addUse(this);
        }
    }
    for (auto it = operands.begin() + 1; it != operands.end(); it++)
        if (*it == old)
            *it = new_;
}

void PhiInstruction::removeUse(Operand* use) {
    auto it = find(operands.begin() + 1, operands.end(), use);
    if (it != operands.end())
        operands.erase(it);
}

void PhiInstruction::replaceDef(Operand* new_) {
    //!!
    dst->removeUse(this);
    dst = new_;
    new_->setDef(this);
}

void PhiInstruction::replaceOriginDef(Operand* new_) {
    this->originDef = new_;
}


Operand* PhiInstruction::getSrc(BasicBlock* block) {
    if (srcs.find(block) != srcs.end())
        return srcs[block];
    return nullptr;
}

void PhiInstruction::output() const {
   // fprintf(yyout,"PHI");
    fprintf(yyout, "  %s = phi %s", dst->toStr().c_str(),
            dst->getType()->toStr().c_str());
    bool first = true;
    for (auto it = srcs.begin(); it != srcs.end(); it++) {
        if (!first)
            fprintf(yyout, ", ");
        else
            first = false;
        fprintf(yyout, "[ %s , %%B%d ]", it->second->toStr().c_str(),
                it->first->getNo());
    }
    fprintf(yyout, "\n");
}

PhiInstruction::~PhiInstruction()
{
   // dst->setDef(nullptr);
  //  src->addUse(this);
}



MachineOperand* Instruction::genMachineOperand(Operand* ope)
{
    auto se = ope->getEntry();
    MachineOperand* mope = nullptr;
    if(se->isConstant())
        mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry*>(se)->getValue());
    else if(se->isTemporary())
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());
    else if(se->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        if(id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().c_str());
        else
            exit(0);
    }
    return mope;
}

MachineOperand* Instruction::genMachineReg(int reg) 
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand* Instruction::genMachineVReg() 
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand* Instruction::genMachineImm(int val) 
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand* Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder)
{
    /* HINT:
    * Allocate stack space for local variabel
    * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    int offset = cur_func->AllocSpace(4);
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    // Load global operand
    if(operands[1]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        // example: load r1, [r0]
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if(operands[1]->getEntry()->isTemporary()
    && operands[1]->getDef()
    && operands[1]->getDef()->isAlloca())
    {
        // example: load r1, [r0, #4]
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())->getOffset());
        cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // Load operand from temporary variable
    else
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO:
    // complete other instructions
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    /* HINT:
    * The source operands of ADD instruction in ir code both can be immediate num.
    * However, it's not allowed in assembly code.
    * So you need to insert LOAD/MOV instrucrion to load immediate num into register.
    * As to other instructions, such as MUL, CMP, you need to deal with this situation, too.*/
    MachineInstruction* cur_inst = nullptr;
    if(src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);
    }
    switch (opcode)
    {
    case ADD:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
        break;
    default:
        break;
    }
    cur_block->InsertInst(cur_inst);
}

void CmpInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
}

void CondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
}

void RetInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    /* HINT:
    * 1. Generate mov instruction to save return value in r0
    * 2. Restore callee saved registers and sp, fp
    * 3. Generate bx instruction */
}