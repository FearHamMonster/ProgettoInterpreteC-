#ifndef EXPR_MANAGER_H
#define EXPR_MANAGER_H

#include <vector>

#include "Node.h"


// Per risolvere il problema delle possibili "perdite di memoria"
// creo un gestore dei nodi che provvede alla loro deallocazione
class ExpressionManager {
public:
    // Il costruttore di default va bene perch� invoca il costruttore
    // di default del vector che costruisce un vector vuoto
    ExpressionManager() = default;
    ~ExpressionManager() {
        clearMemory();
    }

    // Impedisco la copia e l'assegnamento per evitare problemi
    // di "ownership" dei puntatori (se facessi una copia di 
    // un oggetto ExpressionManager andrei incontro a deallocazioni multiple
    ExpressionManager(const ExpressionManager& other) = delete;
    ExpressionManager& operator=(const ExpressionManager& other) = delete;

    Program* makeProgram(Block* block)
    {
        Program* o = new Program(block);
        allocated.push_back(o);
        return o;
    }
    Block* makeBlock(Decls* decls, Seq* Seq){
        Block* o = new Block(decls, Seq);
        allocated.push_back(o);
        return o;
    }

    Decls* makeDecls(Decl* decl, Decls* decls){
        Decls* o = new Decls(decl, decls);
        allocated.push_back(o);
        return o;
    }

    Decl* makeDecl(Type* type, Id* idName){
        Decl* o = new Decl(type, idName);
        allocated.push_back(o);
        return o;
    }

    Type* makeType(Type::TypeCode type){
        Type* o = new Type(type);
        allocated.push_back(o);
        return o;
    }

    vectorType* makeVectorType(Type::TypeCode type, int index){
        vectorType* o = new vectorType(type, index);
        allocated.push_back(o);
        return o;
    }

    intConstant* makeIntConstant(int value) {
        intConstant* o = new intConstant(value);
        allocated.push_back(o);
        return o;
    }
    intConstant* makeIntConstant() {
        intConstant* o = new intConstant();
        allocated.push_back(o);
        return o;
    }

    boolConstant* makeBoolConstant(bool value) {
        boolConstant* o = new boolConstant(value);
        allocated.push_back(o);
        return o;
    }

    boolConstant* makeBoolConstant() {
        boolConstant* o = new boolConstant();
        allocated.push_back(o);
        return o;
    }

    Arithm* makeBinOp(Op::BinOpCode op, Expression* l, Expression* r) {
        Arithm* o = new Arithm(l, r, op);
        allocated.push_back(o);
        return o;
    }

    Access* makeAccess(Id* idName, Expression* index)
    {
        Access* o = new Access(idName, index);
        allocated.push_back(o);
        return o;
    }

    Unary* makeUnaryOp(Op::UnaryOpCode op, Expression* exp) {
        Unary* o = new Unary(exp, op);
        allocated.push_back(o);
        return o;
    }

    Id* makeId(std::string idName) {
        Id* o = new Id(idName);
        allocated.push_back(o);
        return o;
    }
    Not* makeNot(Expression* boolExpr) {
        Not* o = new Not(boolExpr);
        allocated.push_back(o);
        return o;
    }
    And* makeAnd(Expression* boolExpr1, Expression* boolExpr2 ) {
        And* o = new And(boolExpr1,boolExpr2);
        allocated.push_back(o);
        return o;
    }

    Or* makeOr(Expression* boolExpr1, Expression* boolExpr2 ) {
        Or* o = new Or(boolExpr1,boolExpr2);
        allocated.push_back(o);
        return o;
    }

    Rel* makeRel(Expression* boolExpr1, Expression* boolExpr2, Rel::OpCode relCode ) {
        
        Rel* o = new Rel(boolExpr1,boolExpr2, relCode);
        allocated.push_back(o);
        return o;
    }
    
    Seq* makeSeq(Seq* seq, Stmt* stmt)
    {
        Seq* o = new Seq(seq,stmt);
        allocated.push_back(o);
        return o;        
    }

    If* makeIf(Stmt* stmt, Expression* exp)
    {
        If* o = new If(stmt,exp);
        allocated.push_back(o);
        return o;        
    }

    Else* makeElse(Stmt* stmtIfTrue, Stmt* stmtIfFalse, Expression* condition)
    {
        Else* o = new Else(stmtIfTrue, stmtIfFalse, condition);
        allocated.push_back(o);
        return o;        
    }

    While* makeWhile(Stmt* stmt, Expression* condition)
    {
        While* o = new While(stmt, condition);
        allocated.push_back(o);
        return o;        
    }

    Do* makeDo(Stmt* stmt, Expression* condition)
    {
        Do* o = new Do(stmt, condition);
        allocated.push_back(o);
        return o;        
    }

    Set* makeSet(Id* idName, Expression* value)
    {
        Set* o = new Set(idName, value);
        allocated.push_back(o);
        return o;        
    }
    
    SetElem* makeSetElem(Id* vectorName, Expression* index, Expression* value)
    {
        SetElem* o = new SetElem(vectorName, value, index);
        allocated.push_back(o);
        return o;        
    }

    Break* makeBreak()
    {
        Break* o = new Break();
        allocated.push_back(o);
        return o;        
    }
    
    Print* makePrint(Expression* expToPrint)
    {
        Print* o = new Print(expToPrint);
        allocated.push_back(o);
        return o;
    }


    void clearMemory() {
        auto i = allocated.begin();
        // allocated.end() "marca" la fine del vettore
        // ++i sposta di un elemento la posizione "puntata" dall'iteratore
        for (; i != allocated.end(); ++i) {
            // *i restituisce l'elemento "puntato" dall'iteratore
            delete(*i);
        }
        allocated.resize(0);
    }

private:
    std::vector<Node*> allocated;
};



#endif
