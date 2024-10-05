#include "Node.h"


Constant*  Unary::accept(Visitor* v)
{
    return v->visitUnaryOp(this);
}

Constant*  Arithm::accept(Visitor* v)
{
    return v->visitBinOp(this);
}

Constant*  Access::accept(Visitor* v)
{
    return v->visitAccess(this);
}

Constant*  If::accept(Visitor* v)
{
    return v->visitIf(this);
}

Constant*  Else::accept(Visitor* v)
{
    return v->visitElse(this);
}

Constant*  While::accept(Visitor* v)
{
    return v->visitWhile(this);
}

Constant*  Do::accept(Visitor* v)
{
    return v->visitDo(this);
}

Constant*  Set::accept(Visitor* v)
{
    return v->visitSet(this);
}

Constant*  SetElem::accept(Visitor* v)
{
    return v->visitSetElem(this);
}

Constant*  Break::accept(Visitor* v)
{
    return v->visitBreak(this);
}

Constant*  Print::accept(Visitor* v)
{
    return v->visitPrint(this);
}

Constant*  Id::accept(Visitor* v)
{
    return v->visitId(this);
}

Constant*  intConstant::accept(Visitor* v)
{
    return v->visitIntConstant(this);
}
Constant*  boolConstant::accept(Visitor* v)
{
    return v->visitBoolConstant(this);
}


//Logical
Constant*  Not::accept(Visitor* v)
{
    return v->visitNot(this);
}
Constant*  And::accept(Visitor* v)
{
    return v->visitAnd(this);
}
Constant*  Rel::accept(Visitor* v)
{
    return v->visitRel(this);
}

Constant*  Or::accept(Visitor* v)
{
    return v->visitOr(this);
}

//Decls
Constant*  Decls::accept(Visitor* v) {
    return v->visitDecls(this);
}

Constant*  Decl::accept(Visitor* v){
    return v->visitDecl(this);
}

//Block
Constant*  Block::accept(Visitor* v)
{
    return v->visitBlock(this);
}

//Type
Constant* Type::accept(Visitor* v) {
    return v->visitType(this);
}

Constant*  vectorType::accept(Visitor* v) {
    return v->visitVectorType(this);
}

//Stmt
Constant* Seq::accept(Visitor* v) {
    return v->visitSeq(this);
}

//this ensures that only subclasses of Stmt can call accept
Constant* Stmt::accept(Visitor* v) {
    throw EvaluationError("accept method of Stmt didn't get redefined in subclass");
}

//this ensures that only subclasses of Constant can call accept
Constant* Constant::accept(Visitor* v){
    throw EvaluationError("accept method of Constant didn't get redefined in subclass");
}



Constant*  Program::accept(Visitor* v)
{
    return v->visitProgram(this);
}


std::string Type::typeid2String[Type::numOfTypes] = {"int","bool"};

std::string Rel::opCode2String[Rel::numOfOps] = {">",">=","<","<="};

std::string Op::binOp2String[Op::numOfBinOps] = {"+","-","*","/","==","!="};
std::string Op::unaryOp2String[Op::numOfUnaryOps] = {"-"};