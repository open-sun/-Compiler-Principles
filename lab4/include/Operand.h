#ifndef __OPERAND_H__
#define __OPERAND_H__

#include "SymbolTable.h"
#include <vector>

class Instruction;
class Function;



class Operand
{
    // 定义迭代器类型别名
    typedef std::vector<Instruction *>::iterator use_iterator;

private:
    // 操作数的定义指令
    Instruction *def;                // The instruction where this operand is defined.
    
    // 使用该操作数的所有指令
    std::vector<Instruction *> uses; // Instructions that use this operand.
    
    // 操作数的符号表条目
    SymbolEntry *se;                 // The symbol entry of this operand.

public:
    // 构造函数：初始化符号表条目
    Operand(SymbolEntry* se) : se(se) { def = nullptr; };
    
    // 设置定义该操作数的指令
    void setDef(Instruction *inst) { def = inst; };
    
    // 添加使用该操作数的指令
    void addUse(Instruction *inst) { uses.push_back(inst); };
    
    // 移除使用该操作数的指令
    void removeUse(Instruction *inst);
    
    // 获取使用该操作数的指令数量
    int usersNum() const { return uses.size(); };
    
    // 获取定义该操作数的指令
    Instruction *getDef() { return def; };
    
    // 获取使用该操作数的指令列表
    std::vector<Instruction *> &getUse() { return uses; };
    
    // 获取使用该操作数的指令的开始迭代器
    use_iterator use_begin() { return uses.begin(); };
    
    // 获取使用该操作数的指令的结束迭代器
    use_iterator use_end() { return uses.end(); };
    
    // 获取操作数的类型
    Type* getType() { return se->getType(); };
     SymbolEntry* getsym(){return se;};
    
    // 将操作数转换为字符串表示
    std::string toStr() const;
};

#endif