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
              //  std::cout<<(*it)->getNo()<<" ";
                TreeNode* child = new TreeNode(*it);
                DFSTree.push_back(child);
                child->parent = node;
                                    
             //  node->addChild(child);
                search(child);
            }
        }
    }

