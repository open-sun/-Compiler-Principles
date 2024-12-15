#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

extern FILE* yyout;

int TreeNode::Num = 0;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
}

Function::~Function()
{
    auto delete_list = block_list;
    for (auto &i : delete_list)
        delete i;
    parent->removeFunc(this);
}

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::output() const
{
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    fprintf(yyout, "define %s %s(", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    std::string pas;
    bool first = true;
   for (auto ss : params) {
    if (!first) {
        pas += ',';
    }
    pas += ss->getType()->toStr() + " " + ss->toStr();
    first = false;
    }
    fprintf(yyout, "%s){\n", pas.c_str());
    std::set<BasicBlock *> v;
    std::list<BasicBlock *> q;
    q.push_back(entry);
    v.insert(entry);
    while (!q.empty())
    {
        auto bb = q.front();
        q.pop_front();
        bb->output();
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            if (v.find(*succ) == v.end())
            {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }
    fprintf(yyout, "}\n");
}

     void Function::computeDFSTree() {
   
        TreeNode::Num = 0;
        //int len = block_list.size();
       // DFSTree.resize(len);
      //  bool* visited = new bool[len]{};
        DFSRoot = new TreeNode(entry);
        DFSTree.push_back(DFSRoot);
        search(DFSRoot);
      //  delete[] visited;
    }

    void Function::search(TreeNode* node) {

      //  int n = node->block->indexInFunc;
        auto block = node->block;

        for (auto it = block->succ_begin(); it != block->succ_end(); it++) {
            
            int idx = (*it)->indexInFunc;
                                 
            if (idx==-1) {
        //        std::cout<<(*it)->getNo()<<" ";
                TreeNode* child = new TreeNode(*it);
                DFSTree.push_back(child);
                child->parent = node;
                                    
             //  node->addChild(child);
                search(child);
            }
        }
    }
void Function::computeSdom() {
    int len = block_list.size();
    sdom.resize(len);
    int* ancestors = new int[len];
    for (int i = 0; i < len; i++) {
        sdom[i] = i;        // 初始时每个块的 sdom 是自身
        ancestors[i] = -1;  // 初始化所有块的祖先为 -1
    }
    
    // 后序遍历 DFS 树（从叶子到根）
    for (auto it = DFSTree.rbegin(); (*it)->block != entry; it++) {
        auto block = (*it)->block;
        int s = block->indexInFunc;

        // 遍历当前块的所有前驱块
        for (auto it1 = block->pred_begin(); it1 != block->pred_end(); it1++) {
            int z = (*it1)->indexInFunc;

            // 直接在此实现路径追踪逻辑
            while (ancestors[z] != -1) {  // 遍历 z 的祖先链
                if (sdom[z] > sdom[ancestors[z]])
                    z = ancestors[z];  // 更新为更优祖先
                else
                    break;  // 祖先路径已经满足条件
            }

            // 更新 sdom[s] 为最小值
            if (sdom[z] < sdom[s])
                sdom[s] = sdom[z];
        }

        // 设置当前块的祖先为其父节点
        ancestors[s] = (*it)->parent->num;
    }
   // for(int i=0;i<len;i++){
      //  std::cout<<DFSTree[i]->num<<" "<<sdom[i]<<std::endl;
  //  }

    delete[] ancestors;  // 释放动态数组
}

void Function::computeIdom() {
    int len = block_list.size();
    idom.resize(len);
    idom[entry->indexInFunc] = 0;

    for (auto it = DFSTree.begin() + 1; it != DFSTree.end(); it++) {
        // 获取当前节点编号和父节点编号
        int current = (*it)->num;
        int parentNum = (*it)->parent->num;
        int sdomNum = sdom[current];

        // 展开 LCA 逻辑
        int p = parentNum; // 初始为父节点
        while (p != sdomNum) {
            if (p > sdomNum) {
                p = idom[p]; // 向上追溯父节点的 idom
            } else {
                sdomNum = idom[sdomNum]; // 向上追溯半支配节点的 idom
            }
        }

        // 将计算得到的最近公共祖先作为当前节点的 idom
        idom[current] = p;

        // 构造支配树节点
    }

  //  for(int i=0;i<len;i++){
  //      std::cout<<DFSTree[i]->num<<" "<<idom[i]<<std::endl;
   // }

}
