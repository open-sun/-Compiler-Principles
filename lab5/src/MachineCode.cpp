#include "MachineCode.h"

extern FILE* yyout;

MachineOperand::MachineOperand(int tp, int val)
{
    this->type = tp;
    if(tp == MachineOperand::IMM)
        this->val = val;
    else 
        this->reg_no = val;
}

MachineOperand::MachineOperand(std::string label)
{
    this->type = MachineOperand::LABEL;
    this->label = label;
}

bool MachineOperand::operator==(const MachineOperand&a) const
{
    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand&a) const
{
    if(this->type == a.type)
    {
        if(this->type == IMM)
            return this->val < a.val;
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

void MachineOperand::PrintReg()
{
    switch (reg_no)
    {
    case 11:
        fprintf(yyout, "fp");
        break;
    case 13:
        fprintf(yyout, "sp");
        break;
    case 14:
        fprintf(yyout, "lr");
        break;
    case 15:
        fprintf(yyout, "pc");
        break;
    default:
        fprintf(yyout, "r%d", reg_no);
        break;
    }
}

void MachineOperand::output() 
{
    /* HINT：print operand
    * Example:
    * immediate num 1 -> print #1;
    * register 1 -> print r1;
    * lable addr_a -> print addr_a; */
    switch (this->type)
    {
    case IMM:
        fprintf(yyout, "#%d", this->val);
        break;
    case VREG:
        fprintf(yyout, "v%d", this->reg_no);
        break;
    case REG:
        PrintReg();
        break;
    case LABEL:
        if (this->label.substr(0, 2) == ".L")
            fprintf(yyout, "%s", this->label.c_str());
        else
            fprintf(yyout, "addr_%s", this->label.c_str());
    default:
        break;
    }
}

void MachineInstruction::PrintCond()
{
    // TODO
    switch (cond)
    {
    case LT:
        fprintf(yyout, "lt");
        break;
    case EQ:
        fprintf(yyout, "eq");
        break;
    case NE:
        fprintf(yyout, "ne");
        break;
    case GE:
        fprintf(yyout, "ge");
        break;
    case GT:
        fprintf(yyout, "gt");
        break;
    case LE:
        fprintf(yyout, "le");
        break;
    default:
        break;
    }
}

BinaryMInstruction::BinaryMInstruction(
    MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output() 
{
    // TODO: 
    // Complete other instructions
    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        fprintf(yyout, "\tadd ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::SUB:
        fprintf(yyout, "\tsub ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::MUL:
        fprintf(yyout, "\tmul ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::DIV:
        fprintf(yyout, "\tdiv ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::AND:
        fprintf(yyout, "\tand ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::OR:
        fprintf(yyout, "\torr ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    default:
        break;
    }
}

LoadMInstruction::LoadMInstruction(MachineBlock* p,
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2,
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}

void LoadMInstruction::output()
{
    fprintf(yyout, "\tldr ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
    if(this->use_list[0]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if( this->use_list.size() > 1 )
    {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

StoreMInstruction::StoreMInstruction(MachineBlock* p,
    MachineOperand* src1, MachineOperand* src2, MachineOperand* src3, 
    int cond)
{

    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = -1;
    this->cond = cond;
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    if (src3)
        this->use_list.push_back(src3);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
    if (src3)
        src3->setParent(this);

    // TODO
}

void StoreMInstruction::output()
{
    // TODO
    fprintf(yyout, "\tstr ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
   
    // Load address
    if(this->use_list[1]->isReg()||this->use_list[1]->isVReg())
        fprintf(yyout, "[");

    this->use_list[1]->output();
    if( this->use_list.size() > 2 )
    {
        fprintf(yyout, ", ");
        this->use_list[2]->output();
    }

    if(this->use_list[1]->isReg()||this->use_list[1]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

MovMInstruction::MovMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src,
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::MOV;
    this->op = -1;
    this->cond = cond;
     this->use_list.push_back(src);
     this->def_list.push_back(dst);

    
}

void MovMInstruction::output() 
{
    // TODO
     fprintf(yyout, "\tmov ");
    this->def_list[0]->output();
    fprintf(yyout, " ,");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, 
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::BRANCH;
    this->op =op;
    this->cond = cond;
    this->use_list.push_back(dst);
    dst->setParent(this);
}

void BranchMInstruction::output()
{
    // TODO
     switch (this->op)
    {
    case BranchMInstruction::B:
        fprintf(yyout, "\tb");
        this->PrintCond();
         fprintf(yyout, " ");
        this->use_list[0]->output();
        fprintf(yyout, "\n");
        break;
     case BranchMInstruction::BL:
        fprintf(yyout, "\tbl ");
        this->PrintCond();
         fprintf(yyout, " ");
        this->use_list[0]->output();
        fprintf(yyout, "\n");
        break;
     case BranchMInstruction::BX:
        fprintf(yyout, "\tbx ");
        this->PrintCond();
         fprintf(yyout, " ");
        this->use_list[0]->output();
        fprintf(yyout, "\n");
        break;
    default:
    break;
}
}

CmpMInstruction::CmpMInstruction(MachineBlock* p, 
    MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->op =op;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
    // TODO
}

void CmpMInstruction::output()
{
    // TODO
    // Jsut for reg alloca test
    // delete it after test
     fprintf(yyout, "\tcmp ");
    this->use_list[0]->output();
    fprintf(yyout, " ");
    this->use_list[1]->output();
     fprintf(yyout, "\n");
}

StackMInstrcuton::StackMInstrcuton(MachineBlock* p, int op, 
    MachineOperand* src,
    int cond)
{
    // TODO
     this->parent = p;
    this->type = MachineInstruction::BRANCH;
    this->op =op;
    this->cond = cond;

    this->use_list.push_back(src);
    src->setParent(this);
}

void StackMInstrcuton::output()
{
    // TODO
    switch (op)
    {
    case PUSH:
        fprintf(yyout, "\tpush ");
        fprintf(yyout, " {");
        use_list[0]->output();
        fprintf(yyout, "}");
        fprintf(yyout, "\n");
        break;
    case POP:
        fprintf(yyout, "\tpop ");
        fprintf(yyout, " {");
        use_list[0]->output();
        fprintf(yyout, "}");
        fprintf(yyout, "\n");
        break;
    default:
        break;
    }
}

// MachineFunction 类的构造函数，初始化 MachineFunction 对象
MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) 
{ 
    this->parent = p;  // 设置父级 MachineUnit 对象
    this->sym_ptr = sym_ptr;  // 设置符号表项，代表函数的名称或其他符号信息
    this->stack_size = 0;  // 初始化栈空间大小，可能会在后续使用
};

// MachineBlock 的输出函数，生成该代码块的汇编代码
void MachineBlock::output()
{
    fprintf(yyout, ".L%d:\n", this->no);  // 输出代码块的标签，格式为 .L<块编号>
    for(auto iter : inst_list)  // 遍历该代码块中的所有指令
        iter->output();  // 调用每条指令的输出函数，生成相应的汇编代码
}

// MachineFunction 的输出函数，生成函数的汇编代码
void MachineFunction::output()
{
    // 获取函数名，跳过前导的下划线（根据符号表项返回字符串）
    const char *func_name = this->sym_ptr->toStr().c_str() + 1;

    // 输出函数声明，声明为全局函数
    fprintf(yyout, "\t.global %s\n", func_name);
    // 输出函数类型声明
    fprintf(yyout, "\t.type %s , %%function\n", func_name);
    // 输出函数标签，表示函数的开始
    fprintf(yyout, "%s:\n", func_name);

    // TODO: 完成函数体的生成，完成以下操作
    /* 提示:
    *  1. 保存 fp 寄存器（当前的帧指针保存）
    *  2. 设置 fp = sp（将栈指针的值传递给帧指针）
    *  3. 保存被调用者保存的寄存器（根据需要保存某些寄存器）
    *  4. 为局部变量分配栈空间（在栈上为函数局部变量分配内存）
    */

    auto fp=new MachineOperand(MachineOperand::REG, 11);
    auto sp=new MachineOperand(MachineOperand::REG, 13);
    MachineInstruction *cur_ins=nullptr;
    cur_ins= new StackMInstrcuton(nullptr,StackMInstrcuton::PUSH,fp);
    cur_ins->output();
    cur_ins=new MovMInstruction(nullptr,MovMInstruction::MOV,fp,sp);
    cur_ins->output();
    auto  offest=new MachineOperand(MachineOperand::IMM,AllocSpace(0));
    cur_ins=new BinaryMInstruction(nullptr,BinaryMInstruction::SUB,sp,sp,offest);
    cur_ins->output();
    // 遍历当前函数的所有代码块并输出其汇编代码
    for(auto iter : block_list)
        iter->output();  // 每个代码块的输出函数生成其相应的汇编代码
}

// MachineUnit 的全局声明输出函数，用于打印全局变量或常量的声明
void MachineUnit::PrintGlobalDecl()
{
    // TODO:
    // 需要输出全局变量和常量的声明代码
    // 例如: .data 部分的声明或初始化
    
    
}

// MachineUnit 的输出函数，生成整个程序的汇编代码
void MachineUnit::output()
{
    // TODO: 完善输出过程
    /* 提示:
    * 1. 输出全局变量/常量的声明代码；例如 .data 部分声明数据
    * 2. 遍历所有函数并输出它们的汇编代码
    * 3. 在汇编代码的末尾，别忘了输出桥接标签（用于程序的入口和出口处理） */
    
    // 输出目标架构指令，设置为 armv8-a 架构
    fprintf(yyout, "\t.arch armv8-a\n");
    // 输出附加的架构扩展支持，如 CRC (循环冗余校验)
    fprintf(yyout, "\t.arch_extension crc\n");
    // 设置程序使用 ARM 指令集
    fprintf(yyout, "\t.arm\n");
    
    // 输出全局声明部分的汇编代码
    PrintGlobalDecl();
    
    // 遍历所有函数并输出它们的汇编代码
    for(auto iter : func_list)
        iter->output();  // 调用每个函数的输出函数，生成汇编代码

    
}
