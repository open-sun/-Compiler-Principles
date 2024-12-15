#ifndef __BASIC_BLOCK_H__
#define __BASIC_BLOCK_H__
#include <vector>
#include <set>
#include "Instruction.h"

class Function;

//基本快，包含各种指令，块与块之间进行跳转
class BasicBlock
{
    // 定义迭代器类型别名
    typedef std::vector<BasicBlock *>::iterator bb_iterator;

private:
    // 前驱基本块列表
    std::vector<BasicBlock *> pred;
    
    // 后继基本块列表
    std::vector<BasicBlock *> succ;
    
    // 头部哨兵指令
    Instruction *head;
    
    // 父函数
    Function *parent;
    
    // 基本块编号
    int no;

public:

    std::vector<BasicBlock*> idom;
    std::vector<BasicBlock*> sdom;
    std::set<BasicBlock*> dominators;


    // 构造函数：初始化父函数和编号
    BasicBlock(Function *);
    
    // 析构函数
    ~BasicBlock();
    
    // 在头部插入指令
    void insertFront(Instruction *);
    
    // 在尾部插入指令
    void insertBack(Instruction *);
    
    // 在指定指令之前插入新指令
    void insertBefore(Instruction *, Instruction *);
    
    // 从基本块中移除指令
    void remove(Instruction *);
    
    // 检查基本块是否为空
    bool empty() const { return head->getNext() == head; }
    
    // 输出基本块内容
    void output() const;
    
    // 检查后继基本块列表是否为空
    bool succEmpty() const { return succ.empty(); };
    
    // 检查前驱基本块列表是否为空
    bool predEmpty() const { return pred.empty(); };
    
    // 添加后继基本块
    void addSucc(BasicBlock *);
    
    // 移除后继基本块
    void removeSucc(BasicBlock *);
    
    // 添加前驱基本块
    void addPred(BasicBlock *);
    
    // 移除前驱基本块
    void removePred(BasicBlock *);
    // 获取基本块编号
    int getNo() { return no; };
    
    // 获取父函数
    Function *getParent() { return parent; };
    
    // 获取第一个指令
    Instruction* begin() { return head->getNext(); };
    
    // 获取最后一个指令（实际是头哨兵）
    Instruction* end() { return head; };
    
    // 反向获取第一个指令
    Instruction* rbegin() { return head->getPrev(); };
    
    // 反向获取最后一个指令（实际是头哨兵）
    Instruction* rend() { return head; };
    
    // 获取后继基本块列表的开始迭代器
    bb_iterator succ_begin() { return succ.begin(); };
    
    // 获取后继基本块列表的结束迭代器
    bb_iterator succ_end() { return succ.end(); };
    
    // 获取前驱基本块列表的开始迭代器
    bb_iterator pred_begin() { return pred.begin(); };
    
    // 获取前驱基本块列表的结束迭代器
    bb_iterator pred_end() { return pred.end(); };
    
    // 获取前驱基本块的数量
    int getNumOfPred() const { return pred.size(); };
    
    // 获取后继基本块的数量
    int getNumOfSucc() const { return succ.size(); };
};

#endif