#include "basicmem2reg.h"
#include <unordered_set>
#include <vector>
#include <queue>
#include"BasicBlock.h"


using namespace std;

void Mem2reg::execute() {

    basicMem2reg();
 
  //  auto func = unit->begin();
 
      insertPhi((*unit->begin()));

 
  //  for (auto func = unit->begin(); func != unit->end(); func++){
      //  std::cout<<"in";
       // insertPhi((*func));

       // std::cout<<"1ok";
   // }
}

void Mem2reg::basicMem2reg(){
    for (auto func = unit->begin(); func != unit->end(); func++){//形成allocalist，alloca只在entry里
        BasicBlock *temp=(*func)->getEntry();
        Instruction *ins=temp->begin();
        for(;ins!=temp->end();)
        {
            if(ins->isrAlloca())
            {
                allocalist.push_back(ins);
            }
            ins=ins->getNext();
        }
        (*func)->computeDFSTree();
        (*func)->computeSdom();
        (*func)->computeIdom();
        (*func)->computeDomFrontier();
       // std::cout<<"1ok";
    }

    for(size_t i=0;i<allocalist.size();i++)
    //清除noload，一些普适的判断或操作直接在instruction加了函数，不过当函数里用到block的相关函数就不大行，会发生error，所以这里remove在mem这个类定义了
    {
        AllocaInstruction *alloca=dynamic_cast<AllocaInstruction*>(allocalist[i]);
        if(!alloca->isloaded())
        {
            removeuse(alloca);
           
            alloca->getParent()->remove(alloca);
        }
        BasicBlock *block = alloca->getParent();


        if (alloca->defAndUse()) {
            // 维护一个变量 val，初始为 undef
            Operand *val = nullptr;
         //   Operand *use=alloca->getDef();

            // 遍历基本块中的指令
            for (Instruction *ins = block->begin(); ins != block->end(); ) {
                Instruction *next = ins->getNext();

                if (ins->isStore() && ins->getUse()[0] == alloca->getDef()) {
                    // 遇到 store 指令，更新 val 并删除 store
                  
                    val = ins->getUse()[1];
                    block->remove(ins);
                } 
                else if (ins->isLoad() && ins->getUse()[0] == alloca->getDef()) {
                                      
                    // 遇到 load 指令，替换其结果的所有使用，并删除 load
                    Operand *loadResult = ins->getDef();
                    for (auto use = loadResult->use_begin(); use != loadResult->use_end(); ) {
                                        
                        (*use)->replaceUse(val,loadResult);
                         use = loadResult->use_begin(); // 更新迭代器
                    }
                    block->remove(ins);
                }
                ins = next; // 继续遍历下一条指令
            }

            // 删除优化后的 alloca
            block->remove(alloca);

        }
    }
}

void Mem2reg::insertPhi(Function *function){
    // 获取函数的入口基本块
  //  BasicBlock *entry = function->getEntry();

    // 定义工作队列和相关集合，用于记录需要处理的基本块
    std::set<BasicBlock *> worklist;                       // 工作队列，保存需要处理的基本块
    std::unordered_set<BasicBlock *> inWorklist;   // inWorklist：已加入队列的基本块； 

    // 遍历所有分配指令（alloca）
    for (auto &alloca : allocalist)
    {
        //  std::cout<<alloca->getDef()->toStr()<<std::endl;
        // 清空工作队列和集合，为当前指令的处理做准备
        inWorklist.clear();


        // 获取分配指令所在的基本块，并将其从基本块中移除
        auto block = alloca->getParent();
        block->remove(alloca);

        // 获取分配指令定义的操作数，并清除其定义信息
        auto operand = alloca->getDef();
      //  operand->setDef(nullptr);
 
        for(auto ins=operand->use_begin();ins!=operand->use_end();ins++){
            if((*ins)->isStore()){
                worklist.insert((*ins)->getParent());
            }
        }
        while(!worklist.empty()){
               
            auto bb=(*worklist.begin());
            auto df=bb->dominators;
            for(auto d=df.begin();d!=df.end();d++){
                if(inWorklist.find((*d))==nullptr){
                //    std::cout<<(*d)->getNo()<<endl;
                       
                        auto phi = new PhiInstruction(operand);
                  
                        ((AllocaInstruction*)alloca)->phiIns.push_back(phi);
                        //
                        (*d)->insertFront(phi);
                        worklist.insert((*d));
                        inWorklist.insert((*d));

                }
            }
            inWorklist.insert(bb);
            worklist.erase(bb);

        }


   
    }



}