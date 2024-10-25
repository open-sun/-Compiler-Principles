#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(4);
VoidType TypeSystem::commonVoid = VoidType();
FloatType TypeSystem::commonFloat = FloatType(4);

Type* TypeSystem::floatType = &commonFloat;
Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;


std::string IntType::toStr()
{
    return "int";
}

std::string FloatType::toStr()
{
    return "float";
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr() << "()";
    return buffer.str();
}
