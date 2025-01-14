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
    auto se = ope->getEntry();  // 获取操作数的符号条目
    MachineOperand* mope = nullptr;  // 定义一个指向 MachineOperand 的指针，用来保存生成的机器操作数
    if(se->isConstant())  // 如果符号条目是常量
        mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry*>(se)->getValue());  // 创建立即数操作数
    else if(se->isTemporary())  // 如果符号条目是临时变量
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());  // 创建虚拟寄存器操作数
    else if(se->isVariable())  // 如果符号条目是变量
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);  // 强制转换为标识符符号条目
        if(id_se->isGlobal())  // 如果是全局变量
            mope = new MachineOperand(id_se->toStr().c_str());  // 创建标签操作数
        else
            exit(0);  // 非全局变量，退出程序
    }
    return mope;  // 返回生成的机器操作数
}

MachineOperand* Instruction::genMachineReg(int reg) 
{
    return new MachineOperand(MachineOperand::REG, reg);  // 创建一个寄存器操作数
}

MachineOperand* Instruction::genMachineVReg() 
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());  // 创建一个虚拟寄存器操作数，标签通过符号表生成  //cai yong zhe ge lai tong yi xunijicunqi mingzi 
}

MachineOperand* Instruction::genMachineImm(int val) 
{
    return new MachineOperand(MachineOperand::IMM, val);  // 创建一个立即数操作数
}

MachineOperand* Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;  // 生成以 .L 开头的标签，后跟块编号
    std::string label = buf.str();  // 将其转为字符串
    return new MachineOperand(label);  // 返回标签操作数
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder)
{
    /* HINT:
    * 为局部变量分配栈空间
    * 将栈帧偏移量存储在符号条目中 */
    auto cur_func = builder->getFunction();  // 获取当前函数
    int offset = cur_func->AllocSpace(4);  // 分配4字节的栈空间
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);  // 将偏移量设置到临时符号条目
}

void LoadInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();  // 获取当前代码块
    MachineInstruction* cur_inst = nullptr;  // 定义机器指令
    // 处理全局变量加载操作
    if(operands[1]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);  // 获取目标操作数
        auto internal_reg1 = genMachineVReg();  // 生成一个虚拟寄存器
        auto internal_reg2 = new MachineOperand(*internal_reg1);  // 复制虚拟寄存器
        auto src = genMachineOperand(operands[1]);  // 获取源操作数
        // 生成加载指令: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);  // 插入指令到代码块
        // 生成加载指令: load r1, [r0]
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // 处理局部变量加载操作
    else if(operands[1]->getEntry()->isTemporary()
    && operands[1]->getDef()
    && operands[1]->getDef()->isAlloca())
    {
        // 生成加载指令: load r1, [r0, #4]
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);  // 使用寄存器 r11
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())->getOffset());  // 获取偏移量
        cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // 处理临时变量加载操作
    else
    {
        // 生成加载指令: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO: 处理存储指令生成代码
    auto cur_block = builder->getBlock();  // 获取当前代码块
    MachineInstruction* cur_inst = nullptr;  // 定义机器指令
    if(operands[1]->getEntry()->isConstant())
    {
        auto src = genMachineOperand(operands[1]);
        auto def1=genMachineVReg();
        cur_inst=new LoadMInstruction(cur_block,def1,src);
        cur_block->InsertInst(cur_inst);
        if(operands[0]->getEntry()->isTemporary()
    && operands[0]->getDef()
    && operands[0]->getDef()->isAlloca())
    {
       auto src2= genMachineReg(11);  // 使用寄存器 r11
       auto src3=genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->getOffset());  // 获取偏移量
       cur_inst=new StoreMInstruction(cur_block,def1,src2,src3);
        cur_block->InsertInst(cur_inst);

    }
        else if(operands[0]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getEntry())->isGlobal())// quanjv xian load
    {
        auto src21 = genMachineOperand(operands[0]);  // 获取目标操作数

         auto src2=genMachineVReg();
        cur_inst=new LoadMInstruction(cur_block,src2,src21);
           cur_block->InsertInst(cur_inst);  // 插入指令到代码块
        cur_inst = new StoreMInstruction(cur_block, def1, src2);
        cur_block->InsertInst(cur_inst);  // 插入指令到代码块
    }
    else
    {
         auto dst = genMachineOperand(operands[1]);
        auto src = genMachineOperand(operands[0]);
        cur_inst = new StoreMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
    }
    // 处理局部变量加载操作
    else if(operands[0]->getEntry()->isTemporary()
    && operands[0]->getDef()
    && operands[0]->getDef()->isAlloca())
    {
        // 生成加载指令: load r1, [r0, #4]
        auto def1=genMachineOperand(operands[1]);
        auto src2= genMachineReg(11);  // 使用寄存器 r11
       auto src3=genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->getOffset());  // 获取偏移量
       cur_inst=new StoreMInstruction(cur_block,def1,src2,src3);
        cur_block->InsertInst(cur_inst);
    }
    else if(operands[0]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getEntry())->isGlobal())// quanjv xian load
    {
         auto def1=genMachineOperand(operands[1]);
        auto src21 = genMachineOperand(operands[0]);  // 获取目标操作数

         auto src2=genMachineVReg();
        cur_inst=new LoadMInstruction(cur_block,src2,src21);
           cur_block->InsertInst(cur_inst);  // 插入指令到代码块
        cur_inst = new StoreMInstruction(cur_block, def1, src2);
        cur_block->InsertInst(cur_inst);  // 插入指令到代码块
    }
    // 处理临时变量加载操作
    else
    {
        // 生成加载指令: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new StoreMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO:
    // 完成其他指令的处理
    auto cur_block = builder->getBlock();  // 获取当前代码块
    auto dst = genMachineOperand(operands[0]);  // 获取目标操作数
    auto src1 = genMachineOperand(operands[1]);  // 获取第一个源操作数
    auto src2 = genMachineOperand(operands[2]);  // 获取第二个源操作数
    /* HINT:
    * 在IR代码中，ADD指令的源操作数可以是立即数。
    * 但是在汇编代码中，不能直接使用立即数作为操作数。
    * 所以需要插入LOAD/MOV指令将立即数加载到寄存器中。
    * 对于其他指令，例如MUL、CMP，也需要处理类似情况。*/

     MachineOperand *dst1 = nullptr,  *src22 = nullptr,*dst2=nullptr;
    MachineInstruction* cur_inst = nullptr;
    if(src1->isImm())  // 如果第一个源操作数是立即数
    {
        auto internal_reg = genMachineVReg();  // 创建一个虚拟寄存器
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);  // 将立即数加载到寄存器
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);  // 更新源操作数为加载到的寄存器
    }
    if(src2->isImm())
    {
        auto internal_reg2 = genMachineVReg();  // 创建一个虚拟寄存器
        cur_inst = new LoadMInstruction(cur_block, internal_reg2, src2);  // 将立即数加载到寄存器
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*internal_reg2);  // 更新源操作数为加载到的寄存器
    }
    switch (opcode)  // 根据操作码选择生成的指令
    {
    case ADD:  // 对于ADD指令
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);  // 创建加法指令
        break;
    case SUB:  // 对于ADD指令
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);  // 创建加法指令
        break;
    case MUL:  // 对于ADD指令
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);  // 创建加法指令
        break;
    case DIV:  // 对于ADD指令
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);  // 创建加法指令
        break;
    case AND:  // 对于ADD指令
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::AND, dst, src1, src2);  // 创建加法指令
        break;
    case OR:  // 对于ADD指令
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::OR, dst, src1, src2);  // 创建加法指令
        break;
    case MOD:
        cur_inst=new BinaryMInstruction(cur_block,BinaryMInstruction::DIV,dst,src1,src2);
        cur_block->InsertInst(cur_inst);
        dst1 = new MachineOperand(*dst);
         
       src22 = new MachineOperand(*src2);
       cur_inst=new BinaryMInstruction(cur_block,BinaryMInstruction::MUL,dst1,dst,src22);
        cur_block->InsertInst(cur_inst);
        dst2 = new MachineOperand(*dst1);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst2, src1, dst1);
        break;
    default:
        break;
    }
    cur_block->InsertInst(cur_inst);  // 将指令插入到代码块
}


void CmpInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO: 生成比较指令的机器代码
    auto src1=genMachineOperand(operands[1]);
    auto src2=genMachineOperand(operands[2]);
      MachineInstruction* cur_inst = nullptr;
       auto cur_block = builder->getBlock();  // 获取当前代码块
    cur_inst= new CmpMInstruction(cur_block,src1,src2);
    builder->setCmpOpcode(this->getopcode());
    cur_block->InsertInst(cur_inst);

}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO: 生成无条件跳转指令的机器代码
     auto cur_block = builder->getBlock();  // 获取当前代码块
     MachineInstruction* cur_inst = nullptr;
    auto target=genMachineLabel(this->getBranch()->getNo());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, target);
     cur_block->InsertInst(cur_inst);

}

void CondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO: 生成条件跳转指令的机器代码
     auto cur_block = builder->getBlock();  // 获取当前代码块
     MachineInstruction* cur_inst = nullptr;
    auto target=genMachineLabel(this->getTrueBranch()->getNo());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, target,builder->getCmpOpcode());
    cur_block->InsertInst(cur_inst);
    target=genMachineLabel(this->getFalseBranch()->getNo());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, target);
    cur_block->InsertInst(cur_inst);

}

void RetInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO: 生成返回指令的机器代码
    /* HINT:
    * 1. 生成MOV指令，将返回值保存到寄存器r0
    * 2. 恢复被调用者保存的寄存器和栈指针、帧指针
    * 3. 生成BX指令 */
     auto cur_block = builder->getBlock();  // 获取当前代码块
     MachineInstruction* cur_inst = nullptr;
   if(operands.size()>0)
   {
    auto dst=genMachineReg(0);//shi reg ba ying gai 
    auto src=genMachineOperand(operands[0]);
    cur_inst=new MovMInstruction(cur_block,MovMInstruction::MOV,dst,src);
    cur_block->InsertInst(cur_inst);
   }
    auto sp=genMachineReg(13);
   auto fp=genMachineReg(11);
//    cur_inst=new MovMInstruction(cur_block,MovMInstruction::MOV,sp,fp);  hao xiang bu xu yao zhe jv
//    cur_block->InsertInst(cur_inst);
   
   auto offest=genMachineImm(builder->getFunction()->AllocSpace(0));
   cur_inst= new BinaryMInstruction(cur_block,BinaryMInstruction::ADD,sp,sp,offest);
   cur_block->InsertInst(cur_inst);
    cur_inst=new StackMInstrcuton(cur_block,StackMInstrcuton::POP,fp);
    cur_block->InsertInst(cur_inst);
   auto lr=genMachineReg(14);
   cur_inst=new BranchMInstruction(cur_block,BranchMInstruction::BX,lr);
   cur_block->InsertInst(cur_inst);

}
void   PhiInstruction::genMachineCode(AsmBuilder* builder)
{
    
}
void ZextInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src));
}
void TypeConverInstruction::genMachineCode(AsmBuilder* builder)
{   
    
}
void RTinstruction::genMachineCode(AsmBuilder* builder)
{
      auto cur_block = builder->getBlock();  // 获取当前代码块
     MachineInstruction* cur_inst = nullptr;
   if(operands.size()>0)
   {
    auto dst=genMachineReg(0);//shi reg ba ying gai 
    auto src=genMachineOperand(operands[0]);
    cur_inst=new MovMInstruction(cur_block,MovMInstruction::MOV,dst,src);
    cur_block->InsertInst(cur_inst);
   }
    auto sp=genMachineReg(13);
   auto fp=genMachineReg(11);
//    cur_inst=new MovMInstruction(cur_block,MovMInstruction::MOV,sp,fp);  hao xiang bu xu yao zhe jv
//    cur_block->InsertInst(cur_inst);
   
   auto offest=genMachineImm(builder->getFunction()->AllocSpace(0));
   cur_inst= new BinaryMInstruction(cur_block,BinaryMInstruction::ADD,sp,sp,offest);
   cur_block->InsertInst(cur_inst);
    cur_inst=new StackMInstrcuton(cur_block,StackMInstrcuton::POP,fp);
    cur_block->InsertInst(cur_inst);
   auto lr=genMachineReg(14);
   cur_inst=new BranchMInstruction(cur_block,BranchMInstruction::BX,lr);
   cur_block->InsertInst(cur_inst);
}
void CallInstruction::genMachineCode(AsmBuilder* builder)
{
    
}
void UnaryExprInstruction::genMachineCode(AsmBuilder* builder)
{
    
}
void GlobalInstruction::genMachineCode(AsmBuilder* builder)
{
    
}
void XorInstruction::genMachineCode(AsmBuilder* builder)
{
    
}