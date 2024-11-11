#ifndef __UNIT_H__
#define __UNIT_H__

#include <vector>
#include "Function.h"
#include "Instruction.h"

//编译单元
class Unit
{
    // 定义迭代器类型别名
    typedef std::vector<Function *>::iterator iterator;
    typedef std::vector<Function *>::reverse_iterator reverse_iterator;

private:
    // 函数列表
    std::vector<Function *> func_list;
     std::vector< GlobalInstruction*> global_list;

public:
    // 默认构造函数
    Unit() = default;

    // 析构函数
    ~Unit();

    // 在单元中插入一个新的函数
    void insertFunc(Function *);
   // 从单元中移除一个函数
    void removeFunc(Function *);
    void insertglobal(GlobalInstruction *);


    // 输出单元的内容
    void output() const;

    // 正向遍历函数列表的开始迭代器
    iterator begin() { return func_list.begin(); };

    // 正向遍历函数列表的结束迭代器
    iterator end() { return func_list.end(); };

    // 反向遍历函数列表的开始迭代器
    reverse_iterator rbegin() { return func_list.rbegin(); };

    // 反向遍历函数列表的结束迭代器
    reverse_iterator rend() { return func_list.rend(); };

};
#endif