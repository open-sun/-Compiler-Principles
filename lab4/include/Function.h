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

class Unit;

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
    std::map<BasicBlock*, BasicBlock*> idom;



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


    void computeDominators() {
        // 初始化支配集，入口块支配所有其他块
        for (auto block : block_list) {
            block->dominators.insert(block);
        }
        block_list[0]->dominators.clear(); // 入口块自己支配自己

        bool changed = true;
        while (changed) {
            changed = false;
            for (auto block : block_list) {
                if (block==getEntry()) continue; // 跳过入口块
                std::set<BasicBlock*> newDominators;
                newDominators.insert(block); // 基本块自己必然支配自己

                for (auto pred =block->pred_begin();pred!=block->pred_end();pred++) {
                    if (newDominators.empty()) {
                        newDominators = (*pred)->dominators;
                    } else {
                        std::set<BasicBlock*> intersection;
                        for (auto dom : (*pred)->dominators) {
                            if (newDominators.find(dom) != newDominators.end()) {
                                intersection.insert(dom);
                            }
                        }
                        newDominators = intersection;
                    }
                }

                if (newDominators != block->dominators) {
                    block->dominators = newDominators;
                    changed = true;
                }
            }
        }
    }

    // 计算直接支配者
    void computeImmediateDominators() {
        for (auto block : block_list) {
            if (block==getEntry()) continue; // 跳过入口块
            BasicBlock* immediateDominator = nullptr;

            for (auto dom : block->dominators) {
                if (dom != block && (immediateDominator == nullptr || dom->getNo()< immediateDominator->getNo())) {
                    immediateDominator = dom;
                }
            }
            idom[block] = immediateDominator;
        }
    }

    // 打印直接支配者
    void printImmediateDominators() {
        for (auto& pair : idom) {
            std::cout << "Block " << pair.first->getNo() << " has immediate dominator: "
                      << pair.second->getNo()<< std::endl;
        }
    }





};

#endif
