#ifndef _SCCP_H
#define _SCCP_H
#include "Instruction.h"
#include "Type.h"
#include "Unit.h"
#include <set>
#include <unordered_set>

class SCCP {
    Unit *unit;
    
   

  public:
    SCCP(Unit *_unit) : unit(_unit) {};
    bool funcsccp(Function *func);
    bool skip(Instruction *inst);
    void execute();
};

#endif