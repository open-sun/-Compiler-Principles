#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include "BasicBlock.h"
#include "SymbolTable.h"
#include "Operand.h"
#include<queue>
extern FILE* yyout;
class Unit;


struct TreeNode {
    static int Num;

    int num;
    BasicBlock* block;
    std::vector<TreeNode*> children;
    TreeNode* parent = nullptr;

    TreeNode(BasicBlock* block) : block(block) {
        num=Num;
        Num++;
        block->indexInFunc=num;
     
    }
    // only use for dom tree node
    TreeNode(BasicBlock* block, int num) : block(block) {
        this->num = block->getNo();
         
    }
    void addChild(TreeNode* child) { children.push_back(child); }
    // only use for dom tree node
    int getHeight() {
        int height = 0;
        TreeNode* temp = this;
        while (temp) {
            height++;
            temp = temp->parent;
        }
        return height;
    }
};




class Function
{
    // 定义迭代器类型别名
    typedef std::vector<BasicBlock *>::iterator iterator;
    typedef std::vector<BasicBlock *>::reverse_iterator reverse_iterator;

private:
    // 基本块列表
    std::vector<BasicBlock *> block_list;
    
    // 符号表条目，通常代表函数本身
    SymbolEntry *sym_ptr;
    
    // 函数的入口基本块
    BasicBlock *entry;
    
    // 父单元（例如模块或文件）
    Unit *parent;
    std::vector< Operand*> params;

public:
    std::vector<int> idom;
    std::vector<int> sdom;
    TreeNode* DFSRoot;
    std::vector<TreeNode*> DFSTree;

    // 构造函数：初始化父单元和符号表条目
    Function(Unit *, SymbolEntry *);
    
    // 析构函数
    ~Function();
    
    // 在函数中插入一个新的基本块
    void insertBlock(BasicBlock *bb) { block_list.push_back(bb); };
    
    // 获取函数的入口基本块
    BasicBlock *getEntry() { return entry; };
    
    // 从函数中移除一个基本块
    void remove(BasicBlock *bb);
    
    // 输出函数的内容
    void output() const;
    
    // 获取基本块列表的引用
    std::vector<BasicBlock *> &getBlockList() { return block_list; };
    
    // 正向遍历基本块列表的开始迭代器
    iterator begin() { return block_list.begin(); };
    
    // 正向遍历基本块列表的结束迭代器
    iterator end() { return block_list.end(); };
    
    // 反向遍历基本块列表的开始迭代器
    reverse_iterator rbegin() { return block_list.rbegin(); };
    
    // 反向遍历基本块列表的结束迭代器
    reverse_iterator rend() { return block_list.rend(); };
    
    // 获取符号表条目
    SymbolEntry *getSymPtr() { return sym_ptr; };
    void addpa(Operand * op){params.push_back(op);return;};

    void computeDFSTree();
    

    void search(TreeNode* node) ;

    void computeSdom();
    int LCA(int i, int j);
    void computeIdom();
    void computeDomFrontier() ;


    void computeRDFSTree(BasicBlock* exit);
    void RSearch(TreeNode* node);
    void computeRSdom(BasicBlock* exit);
    void computeRIdom(BasicBlock* exit);
    void computeRDomFrontier();
    BasicBlock* getfisrtlivesucc(BasicBlock* block);
    void outputDomTreeIteratively() const {
       // 通过 idom 数组输出每个块的支配关系
    for (auto block : block_list) {
        int currentBlockNum = block->indexInFunc;
        int domBlockNum = idom[currentBlockNum]; // 获取当前块的支配者

        // 输出当前块和它的支配者
        printf("Block %d is dominated by Block %d\n", currentBlockNum, domBlockNum);

        // 如果需要，可以遍历支配树的每个节点，输出完整的树结构
        // 例如：对于每个块，输出它支配的块（如果有）
     
    }
    }
};

#endif
