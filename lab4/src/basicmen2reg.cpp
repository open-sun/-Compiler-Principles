#include "basicmem2reg.h"
#include <unordered_set>
#include <vector>
#include <queue>
#include"BasicBlock.h"


using namespace std;

void Mem2reg::execute() {

    basicMem2reg();
 
  //  auto func = unit->begin();
 
    for (auto func = unit->begin(); func != unit->end(); func++){//形成allocalist，alloca只在entry里
        CompMem2reg((*func));
       // std::cout<<"1ok";
    }

 
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
            if(ins->isAlloca())
            {
                allocalist.push_back(ins);
            }
            ins=ins->getNext();
        }

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

void Mem2reg::CompMem2reg(Function *function){
    allocalist.clear();
    for (auto func = unit->begin(); func != unit->end(); func++){//形成allocalist，alloca只在entry里
        BasicBlock *temp=(*func)->getEntry();
        Instruction *ins=temp->begin();
        for(;ins!=temp->end();)
        {
            if(ins->isAlloca())
            {
                allocalist.push_back(ins);
            }
            ins=ins->getNext();
        }
    }

        function->computeDFSTree();
        function->computeSdom();
        function->computeIdom();
        function->computeDomFrontier();
        insertPhi(function);
        rename(function);

     //  std::cout<<"1ok"<<std::endl;;
    
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
                        phi->alloca=((AllocaInstruction*)alloca);
                        Operand* newOperand = new Operand(new TemporarySymbolEntry( ((PointerType*)(operand->getType()))->getValueType(),SymbolTable::getLabel()));
                        phi->setDst(newOperand);
                       // std::cout<<newOperand->toStr()<<std::endl;
                         //std::cout<<operand->getType()->toStr()<<std::endl;
                        // Type *t= ((PointerType*)(operand->getType()))->getValueType();
                        // std::cout<<operand->getType()->toStr()<<std::endl;
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


void Mem2reg::rename(Function *function){

    std::unordered_set<BasicBlock *> Worklist;
    std::unordered_set<BasicBlock *> inWorklist; 
    BasicBlock* entry=function->getEntry();
  //  std::map<AllocaInstruction*,Operand*>incomingVals;
    Worklist.insert(entry);

    while(!Worklist.empty()){
  
        BasicBlock *bb=(*Worklist.begin());
      //  std::cout<<bb->getNo()<<std::endl;
        Worklist.erase(bb);
        if(inWorklist.find(bb)!=nullptr){
            continue;
        } 
        inWorklist.insert(bb);
        for(Instruction *ins=bb->begin();ins!=bb->end();ins=ins->getNext()){
            if(ins->isAlloca()){
                bb->remove(ins);
            }
            else if(ins->isLoad()){
               // std::cout<<"load"<<std::endl;
                for(auto alloca=allocalist.begin();alloca!=allocalist.end();alloca++){
               
              //      Operand *dst=(*alloca)->getDef();
                    if(ins->getUse()[0] == (*alloca)->getDef()){
                      
                       // std::cout<<((AllocaInstruction*)(*alloca))->getDef()->toStr()<<std::endl;
                       // std::cout<<((AllocaInstruction*)(*alloca))->incomingVals->toStr()<<std::endl;
                        Operand *newOp=((AllocaInstruction*)(*alloca))->incomingVals;

                       // std::cout<<newOp->toStr()<<std::endl;
                        //替换使用
                        Operand *loadResult = ins->getDef();
                       // std::cout<<loadResult->toStr()<<std::endl;
                   
                        for (auto use = loadResult->use_begin(); use != loadResult->use_end(); ) {        
                            (*use)->replaceUse(newOp,loadResult);
                            use = loadResult->use_begin(); // 更新迭代器
                        }
                       
                        bb->remove(ins);
                    }
                   
                }
                
            }
            else if(ins->isStore()){
                 
                for(auto alloca=allocalist.begin();alloca!=allocalist.end();alloca++){
                    if(ins->getUse()[0] == (*alloca)->getDef()){
                        ((AllocaInstruction*)(*alloca))->incomingVals=ins->getUse()[1];
                       
                        bb->remove(ins);
                    }
                }
            }
            else if(ins->isPhi()){
             //    std::cout<<"phi"<<std::endl;
                for(auto alloca=allocalist.begin();alloca!=allocalist.end();alloca++){
                    if(((PhiInstruction*)ins)->alloca== (*alloca)){
                       
                        ((AllocaInstruction*)(*alloca))->incomingVals=ins->getDef();
                        
                    }
                }
            };
        }
        for(auto succ=bb->succ_begin();succ!=bb->succ_end();succ++){
          //  std::cout<<"133"<<std::endl;
            Worklist.insert((*succ));
            for(auto ins=(*succ)->begin();ins!=(*succ)->end();ins=ins->getNext()){
                if(ins->isPhi()){
                    Operand* inval=((PhiInstruction*)ins)->alloca->incomingVals;
                    if(inval!=nullptr){
                        ((PhiInstruction*)ins)->addSrc(bb,inval);
                    }
                }
            }

        }
    }


}

