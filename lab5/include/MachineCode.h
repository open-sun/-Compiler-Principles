#ifndef __MACHINECODE_H__
#define __MACHINECODE_H__
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>
#include "SymbolTable.h"


/* 提示：
* MachineUnit：编译器单元，表示一个机器代码单元。
* MachineFunction：表示汇编代码中的一个函数。
* MachineInstruction：表示一条单独的汇编指令。
* MachineOperand：表示汇编指令中的操作数，例如立即数、寄存器、地址标签等。 */

/* 待完成部分：
* 我们只提供了 "BinaryMInstruction" 和 "AccessMInstruction" 类的示例代码（因为我们相信你!!!），
* 你需要完成其他成员函数，特别是 "output()" 函数，
* 完成后，你可以使用 "output()" 函数打印汇编代码。 */

class MachineUnit;
class MachineFunction;
class MachineBlock;
class MachineInstruction;

// 机器指令操作数类，表示机器指令中的每个操作数（立即数、寄存器、标签等）
class MachineOperand
{
private:
    MachineInstruction* parent;  // 操作数所属的指令
    int type;  // 操作数类型
    int val;  // 立即数的值
    int reg_no; // 寄存器编号
    std::string label; // 地址标签
public:
    // 枚举，表示操作数的类型
    enum { IMM, VREG, REG, LABEL };

    // 构造函数：根据类型初始化操作数
    MachineOperand(int tp, int val);  // 立即数类型
    MachineOperand(std::string label);  // 标签类型

    // 操作符重载：用于比较操作数
    bool operator == (const MachineOperand&) const;
    bool operator < (const MachineOperand&) const;

    // 判断操作数类型的辅助函数
    bool isImm() { return this->type == IMM; };  // 是否为立即数
    bool isReg() { return this->type == REG; };  // 是否为寄存器
    bool isVReg() { return this->type == VREG; };  // 是否为虚拟寄存器
    bool isLabel() { return this->type == LABEL; };  // 是否为标签

    // 获取操作数的值或寄存器编号
    int getVal() {return this->val; };
    int getReg() {return this->reg_no; };

    // 设置寄存器编号
    void setReg(int regno) {this->type = REG; this->reg_no = regno;};

    // 获取标签
    std::string getLabel() {return this->label; };

    // 设置父指令
    void setParent(MachineInstruction* p) { this->parent = p; };
    MachineInstruction* getParent() { return this->parent;};

    // 打印寄存器信息
    void PrintReg();

    // 输出操作数
    void output();
};

// 机器指令类，是所有指令的基类，定义了一些共性成员
class MachineInstruction
{
protected:
    MachineBlock* parent;  // 指令所属的代码块
    int no;  // 指令编号
    int type;  // 指令类型
    int cond = MachineInstruction::NONE;  // 指令的执行条件（可选）
    int op;  // 指令的操作码（opcode）
    // 操作数列表，按在汇编指令中出现的顺序排序
    std::vector<MachineOperand*> def_list;  // 定义列表
    std::vector<MachineOperand*> use_list;  // 使用列表

    // 添加定义和使用的操作数
    void addDef(MachineOperand* ope) { def_list.push_back(ope); };
    void addUse(MachineOperand* ope) { use_list.push_back(ope); };

    // 打印条件语句
    void PrintCond();

    // 指令类型枚举
    enum instType { BINARY, LOAD, STORE, MOV, BRANCH, CMP, STACK };
public:
    // 条件类型枚举
    enum condType { EQ, NE, LT, LE , GT, GE, NONE };

    // 纯虚函数，所有子类需要实现的输出函数
    virtual void output() = 0;

    // 设置和获取指令编号
    void setNo(int no) {this->no = no;};
    int getNo() {return no;};

    // 获取定义和使用的操作数列表
    std::vector<MachineOperand*>& getDef() {return def_list;};
    std::vector<MachineOperand*>& getUse() {return use_list;};

    // 获取指令所属的代码块
    MachineBlock* getParent() {return this->parent;};
};

// 二元指令类（例如加法、减法等）
class BinaryMInstruction : public MachineInstruction
{
public:
    // 操作类型枚举
    enum opType { ADD, SUB, MUL, DIV, AND, OR };

    // 构造函数：初始化操作数和指令类型
    BinaryMInstruction(MachineBlock* p, int op, 
                    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, 
                    int cond = MachineInstruction::NONE);

    // 输出二元指令
    void output();
};

// 加载指令类（例如从内存中加载数据）
class LoadMInstruction : public MachineInstruction
{
public:
    // 构造函数：初始化操作数和指令类型
    LoadMInstruction(MachineBlock* p,
                    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2 = nullptr, 
                    int cond = MachineInstruction::NONE);

    // 输出加载指令
    void output();
};

// 存储指令类（例如将数据存储到内存中）
class StoreMInstruction : public MachineInstruction
{
public:
    // 构造函数：初始化操作数和指令类型
    StoreMInstruction(MachineBlock* p,
                    MachineOperand* src1, MachineOperand* src2, MachineOperand* src3 = nullptr, 
                    int cond = MachineInstruction::NONE);

    // 输出存储指令
    void output();
};

// 移动指令类（例如寄存器间的数据移动）
class MovMInstruction : public MachineInstruction
{
public:
    // 操作类型枚举（例如 MOV 或 MVN）
    enum opType { MOV, MVN };

    // 构造函数：初始化操作数和指令类型
    MovMInstruction(MachineBlock* p, int op, 
                MachineOperand* dst, MachineOperand* src,
                int cond = MachineInstruction::NONE);

    // 输出移动指令
    void output();
};

// 分支指令类（例如跳转到标签或调用函数）
class BranchMInstruction : public MachineInstruction
{
public:
    // 操作类型枚举（例如 B、BL 或 BX）
    enum opType { B, BL, BX };

    // 构造函数：初始化操作数和指令类型
    BranchMInstruction(MachineBlock* p, int op, 
                MachineOperand* dst, 
                int cond = MachineInstruction::NONE);

    // 输出分支指令
    void output();
};

// 比较指令类（例如比较两个值）
class CmpMInstruction : public MachineInstruction
{
public:
    // 操作类型枚举（例如 CMP）
    enum opType { CMP };

    // 构造函数：初始化操作数和指令类型
    CmpMInstruction(MachineBlock* p, 
                MachineOperand* src1, MachineOperand* src2, 
                int cond = MachineInstruction::NONE);

    // 输出比较指令
    void output();
};

// 栈操作指令类（例如推送或弹出栈数据）
class StackMInstrcuton : public MachineInstruction
{
public:
    // 操作类型枚举（例如 PUSH 或 POP）
    enum opType { PUSH, POP };

    // 构造函数：初始化操作数和指令类型
    StackMInstrcuton(MachineBlock* p, int op, 
                MachineOperand* src,
                int cond = MachineInstruction::NONE);

    // 输出栈操作指令
    void output();
};

// 机器代码块类，表示机器代码中的一个基本块
class MachineBlock
{
private:
    MachineFunction* parent;  // 所属函数
    int no;  // 基本块编号
    std::vector<MachineBlock *> pred, succ;  // 前驱和后继基本块
    std::vector<MachineInstruction*> inst_list;  // 当前基本块中的指令列表
    std::set<MachineOperand*> live_in;  // 活跃输入变量
    std::set<MachineOperand*> live_out;  // 活跃输出变量
public:
    // 获取指令列表
    std::vector<MachineInstruction*>& getInsts() {return inst_list;};

    // 遍历指令列表的迭代器
    std::vector<MachineInstruction*>::iterator begin() { return inst_list.begin(); };
    std::vector<MachineInstruction*>::reverse_iterator rbegin() { return inst_list.rbegin(); };
    std::vector<MachineInstruction*>::reverse_iterator rend() { return inst_list.rend(); };
    std::vector<MachineInstruction*>::iterator end() { return inst_list.end(); };

    // 构造函数：初始化基本块
    MachineBlock(MachineFunction* p, int no) { this->parent = p; this->no = no; };

    // 插入指令
    void InsertInst(MachineInstruction* inst) { this->inst_list.push_back(inst); };

    // 添加前驱和后继基本块
    void addPred(MachineBlock* p) { this->pred.push_back(p); };
    void addSucc(MachineBlock* s) { this->succ.push_back(s); };

    // 获取活跃输入和输出变量
    std::set<MachineOperand*>& getLiveIn() {return live_in;};
    std::set<MachineOperand*>& getLiveOut() {return live_out;};

    // 获取前驱和后继基本块
    std::vector<MachineBlock*>& getPreds() {return pred;};
    std::vector<MachineBlock*>& getSuccs() {return succ;};

    // 输出当前基本块
    void output();
};

// 机器函数类，表示机器代码中的一个函数
class MachineFunction
{
private:
    MachineUnit* parent;  // 所属机器单元
    std::vector<MachineBlock*> block_list;  // 函数中的基本块列表
    int stack_size;  // 栈空间大小
    std::set<int> saved_regs;  // 保存的寄存器集合
    SymbolEntry* sym_ptr;  // 对应的符号表项
public:
    // 获取基本块列表
    std::vector<MachineBlock*>& getBlocks() {return block_list;};

    // 遍历基本块列表的迭代器
    std::vector<MachineBlock*>::iterator begin() { return block_list.begin(); };
    std::vector<MachineBlock*>::iterator end() { return block_list.end(); };

    // 构造函数：初始化函数
    MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr);

    // 分配栈空间并返回当前栈偏移
    int AllocSpace(int size) { this->stack_size += size; return this->stack_size; };

    // 插入基本块
    void InsertBlock(MachineBlock* block) { this->block_list.push_back(block); };

    // 添加保存的寄存器
    void addSavedRegs(int regno) {saved_regs.insert(regno);};

    // 输出当前函数
    void output();
};

// 机器单元类，表示整个机器代码单元
class MachineUnit
{
private:
    std::vector<MachineFunction*> func_list;  // 机器单元中的函数列表
     std::vector<SymbolEntry*> global_list;
    void PrintGlobalDecl();  // 打印全局声明
public:
    // 获取函数列表
    std::vector<MachineFunction*>& getFuncs() {return func_list;};

    // 遍历函数列表的迭代器
    std::vector<MachineFunction*>::iterator begin() { return func_list.begin(); };
    std::vector<MachineFunction*>::iterator end() { return func_list.end(); };
    void insetglobal(SymbolEntry * newglobal){global_list.push_back(newglobal);};

    // 插入函数
    void InsertFunc(MachineFunction* func) { func_list.push_back(func);};

    // 输出整个机器单元
    void output();
};

#endif
