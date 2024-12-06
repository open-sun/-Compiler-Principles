#ifndef _IRDEADCODEELI_H
#define _IRDEADCODEELI_H
#include "Instruction.h"
#include "Type.h"
#include "Unit.h"
#include <set>
#include <unordered_set>
#include"BasicBlock.h"
#include"Operand.h"


class IRDeadEli {
    Unit *unit;

  public:
    IRDeadEli(Unit *_unit) : unit(_unit) {}
    void execute();
};

#endif