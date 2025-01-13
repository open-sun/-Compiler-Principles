#ifndef __LIVE_VARIABLE_ANALYSIS_H__  // 防止头文件重复包含的宏定义
#define __LIVE_VARIABLE_ANALYSIS_H__

#include <set>  // 引入set容器
#include <map>  // 引入map容器

class MachineFunction;  // 前向声明MachineFunction类
class MachineUnit;  // 前向声明MachineUnit类
class MachineOperand;  // 前向声明MachineOperand类
class MachineBlock;  // 前向声明MachineBlock类

// 该类用于实现活跃变量分析
class LiveVariableAnalysis
{
private:
    // all_uses：每个操作数的使用位置集合
    std::map<MachineOperand, std::set<MachineOperand *>> all_uses;  

    // def：每个代码块的定义操作数集合
    std::map<MachineBlock *, std::set<MachineOperand *>> def;  

    // use：每个代码块的使用操作数集合
    std::map<MachineBlock *, std::set<MachineOperand *>> use;  

    // 计算每个函数中的使用位置
    void computeUsePos(MachineFunction *);  

    // 计算每个函数中的定义和使用
    void computeDefUse(MachineFunction *);  

    // 迭代处理函数中的各个代码块
    void iterate(MachineFunction *);  

public:
    // 对整个机器单元进行活跃变量分析
    void pass(MachineUnit *unit);  

    // 对某个函数进行活跃变量分析
    void pass(MachineFunction *func);  

    // 获取所有使用位置的集合
    std::map<MachineOperand, std::set<MachineOperand *>> &getAllUses() { return all_uses; };  
};

#endif  // 结束宏定义，防止重复包含
