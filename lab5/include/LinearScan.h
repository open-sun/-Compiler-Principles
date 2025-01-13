/**
 * 线性扫描寄存器分配
 */

#ifndef _LINEARSCAN_H__  // 宏定义，防止头文件被重复包含
#define _LINEARSCAN_H__
#include <set>
#include <map>
#include <vector>
#include <list>

// 前向声明类
class MachineUnit;
class MachineOperand;
class MachineFunction;

// 线性扫描寄存器分配类
class LinearScan
{
private:
    // 结构体，表示一个虚拟寄存器的生命周期区间
    struct Interval
    {
        int start;  // 该虚拟寄存器生命周期的开始位置
        int end;    // 该虚拟寄存器生命周期的结束位置
        bool spill; // 是否需要将该虚拟寄存器溢出到内存
        int disp;   // 如果需要溢出到内存，存储该虚拟寄存器在栈中的偏移量
        int rreg;   // 如果该虚拟寄存器没有溢出到内存，表示分配给它的真实寄存器编号
        std::set<MachineOperand *> defs;  // 存储该虚拟寄存器的定义位置
        std::set<MachineOperand *> uses;  // 存储该虚拟寄存器的使用位置
    };

    MachineUnit *unit;  // 当前的机器代码单元
    MachineFunction *func;  // 当前的机器函数
    std::vector<int> regs;  // 存储可用寄存器的列表
    std::map<MachineOperand *, std::set<MachineOperand *>> du_chains;  // 定义使用链（Def-Use Chain）
    std::vector<Interval*> intervals;  // 存储虚拟寄存器的生命周期区间列表
    std::vector<Interval*> active; 

    // 比较函数，按照生命周期开始的位置对区间进行排序
    static bool compareStart(Interval* a, Interval* b);

    // 过期的区间，需要移除
    void expireOldIntervals(Interval *interval);

    // 溢出虚拟寄存器到内存
    void spillAtInterval(Interval *interval);

    // 构建定义使用链
    void makeDuChains();

    // 计算虚拟寄存器的生命周期区间
    void computeLiveIntervals();

    // 执行线性扫描寄存器分配
    bool linearScanRegisterAllocation();

    // 修改机器代码以反映寄存器分配
    void modifyCode();

    // 生成溢出代码（将寄存器溢出到内存）
    void genSpillCode();

public:
    // 构造函数，初始化线性扫描的机器代码单元
    LinearScan(MachineUnit *unit);

    // 执行寄存器分配
    void allocateRegisters();
};

#endif  // 结束宏定义，防止重复包含
