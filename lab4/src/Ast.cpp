#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;
IRBuilder* Node::builder = nullptr;
bool InWhileStmt=false;
bool InFuncDef=false;
Type *retType = nullptr;

Node::Node()
{
    seq = counter++;
    next=nullptr;
}
void Node::gennext(Node *n)
{
    Node *node = this;
    while (node->getnext())
    {
        node = node->getnext();
    }
    node->next =n;
}
void Node::backPatch(std::vector<Instruction*> &list, BasicBlock*target)
{
    for(auto &ii:list)
    {
        if(ii->isCond())
        {
           dynamic_cast<CondBrInstruction *>(ii)->setTrueBranch(target);
            
        }
        else if(ii->isUncond())
        {
            dynamic_cast<UncondBrInstruction *>(ii)->setBranch(target);
        }
    }
}
void Node::falsebackPatch(std::vector<Instruction*> &list, BasicBlock*target)
{
    for(auto &ii:list)
    {
        if(ii->isCond())
        {
           dynamic_cast<CondBrInstruction *>(ii)->setFalseBranch(target);
            
        }
        else if(ii->isUncond())
        {
            dynamic_cast<UncondBrInstruction *>(ii)->setBranch(target);
        }
    }
}
std::vector<Instruction*> Node::merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2)
{
    std::vector<Instruction*> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void FunctionDef::genCode()
{
   Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry();
    builder->setInsertBB(entry);
    AllocaInstruction *alloca;
    if(!Params->se.empty())
    {
    for(auto *ss:Params->se)
    {
        Operand *addr,*addr2;
        SymbolEntry *addr_se,*addr_se2;
        Type *type,*type2;
        type = ss->getType();
        type2=new PointerType(ss->getType());
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr_se2 = new TemporarySymbolEntry(type2, SymbolTable::getLabel());
        addr = new Operand(addr_se);
        addr2=new Operand(addr_se2);
        static_cast<IdentifierSymbolEntry *>(ss)->setAddr(addr2);
        alloca = new AllocaInstruction(addr2,ss);
        new StoreInstruction(addr2,addr,builder->getInsertBB());
        func->addpa(addr);
        entry->insertFront(alloca);                               
    }
    }

    stmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */
     for (auto bb = func->begin(); bb != func->end(); bb++)
    {
        Instruction *ins = (*bb)->rbegin();
        if (ins->isUncond())
        {
            UncondBrInstruction *unBr = static_cast<UncondBrInstruction *>(ins);
            BasicBlock *branch = unBr->getBranch();
            (*bb)->addSucc(branch);
            branch->addPred(*bb);
        }
        else if (ins->isCond())
        {
            CondBrInstruction *condBr = static_cast<CondBrInstruction *>(ins);
            BasicBlock *trueBranch = condBr->getTrueBranch();
            BasicBlock *falseBranch = condBr->getFalseBranch();
            (*bb)->addSucc(trueBranch);
            (*bb)->addSucc(falseBranch);
            trueBranch->addPred(*bb);
            falseBranch->addPred(*bb);
        }
         else if(!ins->isCond()&&!ins->isUncond()&&!ins->isret())
        {
             BasicBlock* block=static_cast<BasicBlock *>(*bb);
        builder->setInsertBB(block);
        Type *type=builder->getInsertBB()->getParent()->getSymPtr()->getType();
        new RTinstruction(type,block);
        }

    }
      for (auto bb = func->begin(); bb != func->end(); bb++)
    {
        BasicBlock* block=static_cast<BasicBlock *>(*bb);
       if(block->empty()&&block->succEmpty())
       {
        builder->setInsertBB(block);
        Type *type=builder->getInsertBB()->getParent()->getSymPtr()->getType();
        new RTinstruction(type,block);
       }
    }
   

}


void BinaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == AND)
    {
        CondBrInstruction * un;
        BasicBlock  *falsetemp;
        falsetemp = new BasicBlock(func);
        BasicBlock *trueBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        if(expr1->isBool==0){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
            new CmpInstruction(CmpInstruction::NE, temp,  expr1->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), builder->getInsertBB());
            un=new CondBrInstruction(trueBB, falsetemp, temp, builder->getInsertBB());
        }  
        else{
            un=new CondBrInstruction(trueBB, falsetemp, expr1->getOperand(), builder->getInsertBB());
        }             
        expr1->addfalse(un);
        builder->setInsertBB(trueBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        if(expr2->isBool==0){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
            new CmpInstruction(CmpInstruction::NE, temp,  expr2->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), builder->getInsertBB());
             true_list = expr2->trueList();
            false_list = merge(expr1->falseList(), expr2->falseList());
            this->setdst(temp);
        }  
        else
        {
            true_list = expr2->trueList();
            false_list = merge(expr1->falseList(), expr2->falseList());
            this->setdst(expr2->getOperand());
        }

        
    }
    else if(op == OR)
    {
        CondBrInstruction * con;
        BasicBlock  *truetemp_bb;
        truetemp_bb=new BasicBlock(func);
        BasicBlock *falseBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        falsebackPatch(expr1->falseList(), falseBB);
        if(expr1->isBool==0){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
            new CmpInstruction(CmpInstruction::NE, temp,  expr1->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), builder->getInsertBB());
            con=new CondBrInstruction(truetemp_bb,falseBB, temp, builder->getInsertBB());
        }  
        else{
            con=new CondBrInstruction(truetemp_bb,falseBB,expr1->getOperand(),builder->getInsertBB());
        }
        
        expr1->addtrue(con);
        builder->setInsertBB(falseBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        false_list = expr2->falseList();
        true_list = merge(expr1->trueList(), expr2->trueList());
        this->setdst(expr2->getOperand());
        // Todo
    }
    else if(op >= LESS && op <= LARGE )
    {
        // Todo
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case LESS:
            opcode = CmpInstruction::L;
            break;
        case LARGE:
            opcode = CmpInstruction::G;
            break;
        default:
            opcode = -1;
            break;
        }
        new CmpInstruction(opcode, dst, src1, src2, bb);
    }
    else if(op >= LESSEQUAL && op <= LARGEEQUAL )
    {
        // Todo
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case LESSEQUAL:
            opcode = CmpInstruction::LE;
            break;
        case LARGEEQUAL:
            opcode = CmpInstruction::GE;
            break;
        default:
            opcode = -1;
            break;
        }
        new CmpInstruction(opcode, dst, src1, src2, bb);
    }
    else if(op >= EQUAL && op <=UNEQUAL )
    {
        // Todo
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case EQUAL:
            opcode = CmpInstruction::E;
            break;
        case UNEQUAL:
            opcode = CmpInstruction::NE;
            break;
        default:
            opcode = -1;
            break;
        }
        new CmpInstruction(opcode, dst, src1, src2, bb);
    }
    else if(op >= ADD && op <= SUB)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        default:
            opcode = -1;
            break;
        }

 //       Operand *temp1=expr1->getOperand();;  
   //     Operand *temp2=expr2->getOperand();  
  //      if(expr1->getType()!=dst->getType()){
   //        // Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));           
   //         new TypeConverInstruction(temp1,expr1->getOperand(),bb);            
   //     }
    //    if(expr2->getType()!=dst->getType()){
     //      // Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));           
    //        new TypeConverInstruction(temp2,expr1->getOperand(),bb);            
    //    }




        if (expr1->isBool==1&&expr2->isBool==1)
        {
            Operand *temp1=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction( temp1, src1,  bb);  
            Operand *temp2=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
           new ZextInstruction( temp2, src2,  bb);  
            new BinaryInstruction(opcode, dst, temp1, temp2, bb);
        }
        else if(expr1->isBool==1&&expr2->isBool==0){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction( temp, src1,  bb);
            new BinaryInstruction(opcode, dst, temp, src2, bb);
        }
        else if(expr1->isBool==0&&expr2->isBool==1){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction( temp, src2,  bb);
            new BinaryInstruction(opcode, dst, src1, temp, bb);
        }
        else {
            new BinaryInstruction(opcode, dst, src1, src2, bb);
        }

        //    new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
    else if(op >= MUL && op <= MOD)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case MUL:
            opcode = BinaryInstruction::MUL;
            break;
        case DIV:
            opcode = BinaryInstruction::DIV;
            break;
        case MOD:
            opcode = BinaryInstruction::MOD;
            break;
        default:
            opcode = -1;
            break;
        }
        if (expr1->isBool==1&&expr2->isBool==1)
        {
            Operand *temp1=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction( temp1, src1,  bb);  
            Operand *temp2=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction( temp2, src2,  bb);  
            new BinaryInstruction(opcode, dst, temp1, temp2, bb);
        }
        else if(expr1->isBool==1&&expr2->isBool==0){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction( temp, src1,  bb);
            new BinaryInstruction(opcode, dst, temp, src2, bb);
        }
        else if(expr1->isBool==0&&expr2->isBool==1){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction( temp, src2,  bb);
            new BinaryInstruction(opcode, dst, src1, temp, bb);
        }
        else {
            new BinaryInstruction(opcode, dst, src1, src2, bb);
        }
    }
}


void UnaryExpr::genCode()
{
   BasicBlock *bb = builder->getInsertBB();
    auto now_op = expr1->getOperand();
    auto now_dst = dst;
   if(op >= ADD && op <= NOT)
    {
        expr1->genCode();
        Operand *src = expr1->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            if (expr1->isBool==1)
            {
                Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
                new ZextInstruction( temp, now_op,  bb); 
                opcode = UnaryExprInstruction::ADD;
                new UnaryExprInstruction(opcode, dst, temp, bb);
                break;        
            }
        
            opcode = UnaryExprInstruction::ADD;
            new UnaryExprInstruction(opcode, dst, src, bb);
            break;
        case SUB:
            if (expr1->isBool==1)
            {
                Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
                new ZextInstruction( temp, now_op,  bb); 
                opcode = UnaryExprInstruction::SUB;
                new UnaryExprInstruction(opcode, dst, temp, bb);
                break;        
            }
            opcode = UnaryExprInstruction::SUB;
                        new UnaryExprInstruction(opcode, dst, src, bb);
            break;
        case NOT:
            opcode = UnaryExprInstruction::NOT;
            if (expr1->isBool==0)
            {
                new CmpInstruction(CmpInstruction::E, now_dst, now_op, new Operand(new ConstantSymbolEntry(TypeSystem::boolType, 0)), bb);       
            }
            else{
                 new UnaryExprInstruction(opcode, dst, src, bb);
            }
            break;
        default:
            opcode = -1;
            new UnaryExprInstruction(opcode, dst, src, bb);
            break;
        }
    }

}



void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}

void IfStmt::genCode()
{



    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);
    if(cond->isBool==0){
        cond->genCode();

        Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel()));
        backPatch(cond->trueList(), then_bb);
        falsebackPatch(cond->falseList(), end_bb);


        new CmpInstruction(CmpInstruction::NE, temp,  cond->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), builder->getInsertBB());


        new CondBrInstruction(then_bb, end_bb, temp, builder->getInsertBB()); 
    }
    else{
        cond->genCode();
        backPatch(cond->trueList(), then_bb);
        falsebackPatch(cond->falseList(), end_bb);
        new CondBrInstruction(then_bb, end_bb, cond->getOperand(), builder->getInsertBB());
    }


    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);

}

void IfElseStmt::genCode()
{

   // new CondBrInstruction(then_bb, else_bb, cond->getOperand(), builder->getInsertBB());
 //   if(cond->isBool==0){
   //         Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel()));
    //        new CmpInstruction(CmpInstruction::NE, temp,  cond->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), builder->getInsertBB());  
    //        new CondBrInstruction(then_bb, end_bb, temp, builder->getInsertBB()); 
  //  }
  //  else{
  //      new CondBrInstruction(then_bb, end_bb, cond->getOperand(), builder->getInsertBB());
  //  }


     Function *func;
    BasicBlock *then_bb, *end_bb,*else_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);
    else_bb = new BasicBlock(func);

    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    falsebackPatch(cond->falseList(), else_bb);
 //   new CondBrInstruction(then_bb, else_bb, cond->getOperand(), builder->getInsertBB());


    if(cond->isBool==0){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel()));
            new CmpInstruction(CmpInstruction::NE, temp,  cond->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), builder->getInsertBB());  
            new CondBrInstruction(then_bb, else_bb, temp, builder->getInsertBB()); 
    }
    else{
        new CondBrInstruction(then_bb, else_bb,cond->getOperand(), builder->getInsertBB());
    }



    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);

    builder->setInsertBB(end_bb);



}
void   WhileStmt::genCode()
{
 // she zhi yi ge tiaoh zhuan de kuai
    Function *func;
    BasicBlock *then_bb, *end_bb,*cond_bb;
      builder->whilelist.push_back(this);

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);
    cond_bb=new BasicBlock(func);
   
    
    new UncondBrInstruction(cond_bb, builder->getInsertBB());
    builder->setInsertBB(cond_bb);
    cond->genCode();

    backPatch(cond->trueList(), then_bb);
    falsebackPatch(cond->falseList(), end_bb);
     if(cond->isBool==0){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::boolType,SymbolTable::getLabel()));
            new CmpInstruction(CmpInstruction::NE, temp,  cond->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), builder->getInsertBB());  
            new CondBrInstruction(then_bb, end_bb, temp, builder->getInsertBB()); 
    }
    else{
        new CondBrInstruction(then_bb, end_bb,cond->getOperand(), builder->getInsertBB());
    }

    builder->setInsertBB(then_bb);
    Stmt->genCode();
    then_bb=builder->getInsertBB();
    new UncondBrInstruction(cond_bb,then_bb);
    then_bb = builder->getInsertBB();
    falsebackPatch(Stmt->falseList(),end_bb);
     falsebackPatch(this->falseList(),end_bb);
     backPatch(this->trueList(),then_bb);
    
    builder->setInsertBB(end_bb);
    builder->whilelist.pop_back();

    
    
}
void   BreakStmt::genCode()
{
    UncondBrInstruction * uncon;
    Function *func;
    func = builder->getInsertBB()->getParent();
    BasicBlock *end_bb;
    end_bb=new BasicBlock(func);
    uncon=new UncondBrInstruction(end_bb, builder->getInsertBB());
    builder->whilelist.back()->addfalse(uncon);


}
void   ContinueStmt::genCode()
{
    // Todo
    UncondBrInstruction * uncon;
    Function *func;
    func = builder->getInsertBB()->getParent();
    BasicBlock *end_bb;
    end_bb=new BasicBlock(func);
    uncon=new UncondBrInstruction(end_bb, builder->getInsertBB());
    builder->whilelist.back()->addtrue(uncon);
}

void   FuncFParams::genCode()
{
    // Todo
}
void   FuncCallExp::genCode()
{
    std::vector<Operand *> canshu;
    BasicBlock *bb = builder->getInsertBB();
    for (long unsigned int i = 0; i<params->params.size(); ++i)
	{
        params->params[i]->genCode();
        if(params->params[i]->isBool==1){
            Operand *temp=new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction( temp, params->params[i]->getOperand(),  bb); 
            canshu.push_back(temp);
        }
        else{
            canshu.push_back(params->params[i]->getOperand());
        }
    }

    new CallInstruction(dst,symbolEntry,canshu,bb);

}
void   EmptyStmt::genCode()
{
    // Todo
}
void   Exprstmt::genCode()
{
    expr->genCode();
}
void CompoundStmt::genCode()
{
    // Todo
     stmt->genCode();
}
void   FuncRParams::genCode()
{
    // Todo
}
void SeqNode::genCode()
{
    // Todo
    stmt1->genCode();
    stmt2->genCode();

}

void DeclStmt::genCode()
{
    IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
    if(se->isGlobal())
    {
        GlobalInstruction* global;
        Operand *addr,*src;
        SymbolEntry *addr_se;
        addr_se = new IdentifierSymbolEntry(*se);
        addr_se->setType(new PointerType(se->getType()));
        addr = new Operand(addr_se);
        if(value!=nullptr)
        {
            src=value->getOperand();
            global=new GlobalInstruction(addr,src,builder->getInsertBB());
        }
       else
       {
        global=new GlobalInstruction(addr,nullptr,builder->getInsertBB());
       }
        se->setAddr(addr);
        builder->getUnit()->insertglobal(global);
    }
    else if(se->isLocal())
    {
        Function *func = builder->getInsertBB()->getParent();
        BasicBlock *entry = func->getEntry();
        Instruction *alloca;
        Operand *addr;
        SymbolEntry *addr_se;
        Type *type;
        type = new PointerType(se->getType());
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr = new Operand(addr_se);
        alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
        entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
        if(value!=nullptr)
        {
            value->genCode();
            Operand *src = value->getOperand();
            new StoreInstruction(addr, src, builder->getInsertBB());
        }
        se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
    }
}

void ReturnStmt::genCode()
{
    // Todo
    retValue->genCode();
    new RetInstruction(retValue->getOperand(),builder->getInsertBB());
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
}

void Ast::typeCheck()
{
        SymbolEntry* se =globals->lookup("main");
        if(se==nullptr||!(se->getType()->isFunc())){
            printf("main function is not defined.\n");
         //   exit(-1);
        }
        root->typeCheck();
}








int UnaryExpr::getValue()
{
    double value = expr1->getValue();
    switch (op)
    {
    case NOT:
        value = (!value);
        break;
    case SUB:
        value = (-value);
        break;
    }
    return value;
}

int Constant::getValue()
{
    return  dynamic_cast<ConstantSymbolEntry *>(symbolEntry)->getValue();
}

int Id::getValue()
{
    return  dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getValue();
}



int BinaryExpr::getValue()
{
    int value1 = expr1->getValue();
    int value2 = expr2->getValue();
    int value=-1;

        switch (op)
        {
        case ADD:
            value = value1 + value2;
            break;
        case SUB:
            value = value1 - value2;
            break;
        case MUL:
            value = value1 * value2;
            break;
        case DIV:
            if(value2==0)
            {
                printf("error ,chushushi 0\n");
                exit(-1);
            }
            value = value1 / value2;

            break;
        case MOD:
            value = (int)value1 % (int)value2;
            break;
        case AND:
            value = value1 && value2;
            break;
        case OR:
            value = value1 || value2;
            break;
        case LESS:
            value = value1 < value2;
            break;
        case LESSEQUAL:
            value = value1 <= value2;
            break;
        case LARGE:
            value = value1 > value2;
            break;
        case LARGEEQUAL:
            value = value1 >= value2;
            break;
        case EQUAL:
            value = value1 == value2;
            break;
        case UNEQUAL:
            value = value1 != value2;
            break;
        }
    return value;
}




void BinaryExpr::typeCheck()
{
    expr1->typeCheck(); 
    expr2->typeCheck(); 
    
    Type *leftType = expr1->getType();  
    Type *rightType = expr2->getType(); 
    if(leftType->isFunc()){
        leftType=((FunctionType*)leftType)->getRetType();
        if(leftType->isVoid()){
        printf("BinaryExpr can not use voidType \n");

    }
    }
    if(rightType->isFunc()){
        rightType=((FunctionType*)rightType)->getRetType();
        if(rightType->isVoid()){
        printf("BinaryExpr can not use voidType \n");
    }
    }
    if (leftType != rightType) {
        
        printf("Operands of binary operator must have the same type.\n");
    }
    if(op==DIV){ 
             int value=expr2->getValue();
            if(value==0){
                printf("div cant use 0\n");
                return;
            }
            this->isBool=0;
    }

    switch (op) {
       case ADD:
               this->isBool=0;
            break;
        case SUB:
               this->isBool=0;
            break;
        case MUL:
               this->isBool=0;
            break;
        case DIV:
               this->isBool=0;
            break;
        case MOD:
            this->isBool=0;
            break;
        case AND:
                    
            this->isBool=1;
            break;
        case OR:
            break;
            this->isBool=1;
        case LESS:            
        this->isBool=1;
            break;
        case LESSEQUAL:            
        this->isBool=1;
            break;
        case LARGE:            
        this->isBool=1;
            break;
        case LARGEEQUAL:            
        this->isBool=1;
            break;
        case EQUAL:            
        this->isBool=1;
            break;
        case UNEQUAL:            
        this->isBool=1;
            break;
        default:

            break;
    }
}



void UnaryExpr::typeCheck()
{
    expr1->typeCheck(); 
    
    Type *operandType = expr1->getType();

    if(operandType->isFunc()){
        operandType=((FunctionType*)operandType)->getRetType();
        
    }
    if(operandType->isVoid() ){
        printf("UnaryExpr can not use voidType \n");

    }  
 
    
    switch (op) {
        case ADD:
                       this->isBool=0;
        case SUB:
                       this->isBool=0;
            if (!(operandType->isFloat()&&operandType->isInt())) {
            //    printf("Operand of unary operator must be numeric.");
            }
            break;
        case NOT:
        //    this->settype(TypeSystem::boolType);
         //   printf("set bool");
         //   this->isBool=1;
            this->settype(TypeSystem::boolType);
    
            this->isBool=1;
        //强制转哈换
            break;
        default:
            break;
    }
}



void Constant::typeCheck()
{
    // Todo
}

void Id::typeCheck()
{
    // Todo
}

void IfStmt::typeCheck()
{
    Type *condType = cond->getType();
    if (condType->isFunc()) {
        condType=((FunctionType*)condType)->getRetType();

    }
    if(condType->isVoid()){
        printf("ifstmt condition can't use void type.\n");
        exit(-1);
    }

    cond->typeCheck();
    thenStmt->typeCheck();
}

void IfElseStmt::typeCheck()
{
        Type *condType = cond->getType();
    if (condType->isFunc()) {
        condType=((FunctionType*)condType)->getRetType();

    }
    if(condType->isVoid()){
        printf("ifstmt condition can't use void type.\n");
        exit(-1);
    }

    cond->typeCheck();
    thenStmt->typeCheck();
    elseStmt->typeCheck();
}

void CompoundStmt::typeCheck()
{
    stmt->typeCheck();
    //语法分析居然没用过这个东西
}

void SeqNode::typeCheck()
{
    stmt1->typeCheck();
    stmt2->typeCheck();
}

void DeclStmt::typeCheck()
{

    if(value){
        value->typeCheck();
        Type *valueType = value->getType();
        if (valueType->isFunc()) {
            valueType=((FunctionType*)valueType)->getRetType();
        
        }
        if(valueType->isVoid()){
            printf("DeclStmt condition can't use void type.\n");
                exit(-1);
    } 

    }


 //  ((IdentifierSymbolEntry *)id->getSymPtr())->setValue(value->getValue());
    // Todo
}

void ReturnStmt::typeCheck()
{
    retValue->typeCheck();
    if(InFuncDef==true&& retValue->getType()!=retType&&retValue->getType()!=TypeSystem::voidType){
        printf("return type dosen`t match\n");
 //      exit(-1);
    }
}

void AssignStmt::typeCheck()

{
    lval->typeCheck();
    expr->typeCheck();

    Type *lhsType = lval->getType();
  //  ((IdentifierSymbolEntry *)lval->getSymPtr())->setValue(expr->getValue());

    // expr->typeCheck();  
     Type *rhsType = expr->getType();
    if (rhsType->isFunc()) {
        rhsType=((FunctionType*)rhsType)->getRetType();
    if(rhsType->isVoid() ){
        printf("assignstmt can't use void type.\n");

    } 
        }
    if (lhsType->isFunc()) {
        printf("lhstype must have assignable\n");

    }

}

void   WhileStmt::typeCheck()
{
     Type *condType = cond->getType();
    if (condType->isFunc()) {
        condType=((FunctionType*)condType)->getRetType();
    if(condType->isVoid() ){
        printf("whilestmt condition can't use void type.\n");
        exit(-1);
    }
    }



    cond->typeCheck();
    InWhileStmt = true;
    Stmt->typeCheck();
    InWhileStmt = false;
}
void   BreakStmt::typeCheck()
{
    if (!InWhileStmt)
    {
        printf("break should in whileStmt\n");
    }
}
void   ContinueStmt::typeCheck()
{
    if (!InWhileStmt)
    {
        printf("continue should in whileStmt\n");
    }
}
void   FuncFParams::typeCheck()
{
    // Todo
}

void FunctionDef::typeCheck()
{

    InFuncDef=true;
    for (long unsigned int i = 0; i<Params->se.size(); ++i)
	{   Type *ptype=Params->se[i]->getType();
//        std::cout<<"fparamsdef "<<ptype->toStr()<<std::endl;   

        ((FunctionType*)se->getType())->setparams(ptype);
    }
    retType = ((FunctionType *)se->getType())->getRetType();
    stmt->typeCheck();
    // Todo
}


void   FuncCallExp::typeCheck()
{

   std::vector<Type*> FParams= ((FunctionType *)this->getSymPtr()->getType())->getparamsType();
//   std::cout<<"type check "<<((IdentifierSymbolEntry*)this->getSymPtr())->getName()<<std::endl;
   if(FParams.size()!=params->params.size()){
        printf("FuncCallExp param number error\n");
   }

    for (long unsigned int i = 0; i<params->params.size()&&i<FParams.size(); ++i)
	{
        Type * rtype=params->params[i]->getSymPtr()->getType();
        if(params->params[i]->getSymPtr()->getType()->isFunc()){
            rtype=((FunctionType*)params->params[i]->getSymPtr()->getType())->getRetType();
        }
 //       std::cout<<"fparams "<<FParams[i]->toStr()<<"  "<<"rtype "<<params->params[i]->getSymPtr()->getType()->toStr()<<std::endl;       
        if(FParams[i]!=rtype){
            printf("FuncCallExp param type error\n");
        }
        params->params[i]->typeCheck();
    }
    // Todos
}
void   EmptyStmt::typeCheck()
{
    // Todo
}
void   Exprstmt::typeCheck()
{
    expr->typeCheck();
}
void   FuncRParams::typeCheck()
{
    // Todo
}



void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case AND:
            op_str = "and";
            break;
        case OR:
            op_str = "or";
            break;
        case LESS:
            op_str = "less";
            break;
        case LARGE:
            op_str="large";
            break;
        case LESSEQUAL:
            op_str="lessequal";
            break;
        case LARGEEQUAL:
            op_str="largeequal";
            break;
        case MUL:
            op_str="MUL";
            break;
        case DIV:
            op_str="DIV";
            break;
        case MOD:
            op_str="MOD";
            break;
        case EQUAL:
            op_str="equal";
            break;
        case UNEQUAL:
            op_str="unequal";
            break;
        

    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}


void UnaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break; 
        case NOT:
            op_str = "not";
            break; 
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
}



void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    stmt1->output(level);
    stmt2->output(level);
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    id->output(level + 4);
     if (value)
    {
        value->output(level + 4);
    }
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    stmt->output(level + 4);
}
void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}


void EmptyStmt::output(int level)
{
    fprintf(yyout, "%*cEmptyStmt\n", level, ' ');
}


void FuncFParams::output(int level){
}

void FuncCallExp ::output(int level)
{
    std::string name, type;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cCallExpr function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(params) params->output(level + 4);
}

void FuncRParams::output(int level){
    fprintf(yyout, "%*cFuncRParams\n", level, ' ');
    for (long unsigned int i = 0; i<params.size(); ++i)
	{
        params[i]->output(level+4);
    }

}
void Exprstmt::output(int level){
    expr->output(level);
}

void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    Stmt->output(level + 4);
}