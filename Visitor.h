#ifndef VISITOR_H
#define VISITOR_H

#include <vector>
#include <iostream>

#include "Node.h"
#include "Exceptions.h"
#include "ExpressionManager.h"
#include "Environment.h"

class Environment;

class Program;
class Block;
class Type;
class vectorType;
class Decls;
class Decl;
class Id;

// Visitor astratto per la visita degli oggetti della gerarchia "Expression"
class Visitor {
public:
    virtual ~Visitor() {}
    virtual Constant* visitProgram(Program* program) = 0;
    virtual Constant* visitBlock(Block* program) = 0;
    virtual Constant* visitType(Type* type) = 0;
    virtual Constant* visitVectorType(vectorType* type) = 0;
    virtual Constant* visitDecls(Decls* decls) = 0;
    virtual Constant* visitDecl(Decl* decl) = 0;
    virtual Constant* visitId(Id* idNode) = 0;
    virtual Constant* visitSeq(Seq* seqNode) = 0;
    virtual Constant* visitIntConstant(intConstant* numNode) = 0;
    virtual Constant* visitBoolConstant(boolConstant* numNode) = 0;
    virtual Constant* visitBinOp(Arithm* arithNode) = 0;
    virtual Constant* visitUnaryOp(Unary* unaryNode) = 0;
    virtual Constant* visitAccess(Access* accessNode) = 0;
    virtual Constant* visitIf(If* ifNode) = 0;
    virtual Constant* visitElse(Else* elseNode) = 0;
    virtual Constant* visitWhile(While* whileNode) = 0;
    virtual Constant* visitDo(Do* doNode) = 0;
    virtual Constant* visitSet(Set* setNode) = 0;
    virtual Constant* visitSetElem(SetElem* setElemNode) = 0;
    virtual Constant* visitBreak(Break* breakNode) = 0;
    virtual Constant* visitPrint(Print* printNode) = 0;
    virtual Constant* visitNot(Not* notNode) = 0;
    virtual Constant* visitAnd(And* andNode) = 0;
    virtual Constant* visitOr(Or* andNode) = 0;
    virtual Constant* visitRel(Rel* relNode) = 0;



};

// Visitor concreto per la valutazione delle espressioni
class EvaluationVisitor : public Visitor {
public:
    EvaluationVisitor(Environment& e) : env {e}, 
    boolConst{new boolConstant()}, intConst{new intConstant()}, breakFlag{false}{
    }
    
    ~EvaluationVisitor(){
        delete intConst;
        delete boolConst;
    }

    EvaluationVisitor(EvaluationVisitor const&) = delete;
    EvaluationVisitor& operator=(EvaluationVisitor const&) = delete;

    Constant* visitProgram(Program* programNode) override {
        programNode->getBlock()->accept(this);
        return nullptr;
    }

    Constant* visitBlock(Block* block) override {
        if(block->getDecls()) 
            block->getDecls()->accept(this);
        if(block->getSeq()) 
            block->getSeq()->accept(this);
        return nullptr;
    }

     Constant* visitType(Type* type) {return nullptr;}
     Constant* visitVectorType(vectorType* vectorTypeNode) {return nullptr;}


    Constant* visitDecls(Decls* decls) override {
        //to ensure that declarations are done in order, it's important to visit the decl object first
        decls->getDecl()->accept(this);
        if(decls->getDecls())
            decls->getDecls()->accept(this);
        return nullptr;
    }

    Constant* visitDecl(Decl* decl) override { 
        //if downcasting succeeds, (vType != nullptr) = 1, so if statement is executed
        //if it doesnt succeed, vType = nullptr = 0, so else statement is executed  
        if(auto vType = dynamic_cast<vectorType*>(decl->getType()))
            env.declareArrayVar(decl->getId()->getName(), vType);
        else
            env.declareVar(decl->getId()->getName(),decl->getType());
        return nullptr;
    }

    Constant* visitSeq(Seq* seqNode) override {
        //statements that are after a break, are not to be executed until we encounter a while/do while 
        if(breakFlag) return nullptr;

        seqNode->getStmt()->accept(this);

        if(seqNode->getSeq())
            seqNode->getSeq()->accept(this);

        return nullptr;
    }    


     Constant* visitPrint(Print* printNode) {
        Constant* expr = printNode->getExp()->accept(this); 
        switch(expr->getTypeCode())
        {
            case Type::INT:
                std::cout<<expr->getInt()<<std::endl;
                break;
            case Type::BOOL:
                std::cout<<expr->getBool()<<std::endl;
                break;
            default:
                throw EvaluationError("Invalid expression type during print statement");
                break;
        }
        return nullptr;
    };

    Constant* visitIf(If* ifNode) override {

        if (ifNode->getCondition()->accept(this)->getBool())
            ifNode->getStmt()->accept(this);
        return nullptr;
    }


    Constant* visitElse(Else* elseNode) override {

        if (elseNode->getCondition()->accept(this)->getBool())
            elseNode->getifTrueStmt()->accept(this);
        else
            elseNode->getifFalseStmt()->accept(this);
        return nullptr;
    }

    Constant* visitWhile(While* whileNode) override {
        while(whileNode->getCondition()->accept(this)->getBool())
        {
            whileNode->getStmt()->accept(this);
            if(breakFlag) 
            {
                breakFlag = false;
                break;
            }
        }
        return nullptr;
    }

    Constant* visitDo(Do* doNode) override {
        do
        {   //we exit the loop and deactivate the breakFlag
            if(breakFlag)
            {
                breakFlag = false;
                break;
            }
            doNode->getStmt()->accept(this);
        } while(doNode->getCondition()->accept(this)->getBool());
        return nullptr;
    }

    Constant* visitSet(Set* setNode) override {
        std::string idName =  setNode->getId()->getName();
        Constant* value = setNode->getExp()->accept(this);

        if(env.getIdValue(idName)->getTypeCode() != value->getTypeCode())
            throw EvaluationError("Trying to assign an expression of type different to that of identifier");

        env.assignConstant(idName, value);
        return nullptr;
    }

    Constant* visitSetElem(SetElem* setElemNode) override {
        std::string idName =  setElemNode->getId()->getName();
        
        Constant* value = setElemNode->getExp()->accept(this);
        if(env.getArray(idName)->typeCode != value->getTypeCode())
            throw EvaluationError("Trying to assign an expression of type different to that of identifier");
     
        env.assignConstantToArray(idName,value,setElemNode->getIndex()->accept(this)->getInt());
        return nullptr;
    }

     Constant* visitBreak(Break* breakNode) {
        breakFlag = true;
        return nullptr;
    }

     Constant* visitId(Id* idNode) {
        return env.getIdValue(idNode->getName());
    }


    Constant* visitNot(Not* notNode) override {
        return boolConst->set(!notNode->getExp()->accept(this)->getBool());
    }

    Constant* visitAnd(And* andNode) override {
        return boolConst->set(
            andNode->getLeftExp()->accept(this)->getBool() && 
            andNode->getRightExp()->accept(this)->getBool());
    }

    Constant* visitOr(Or* orNode) override {
        return boolConst->set(
            orNode->getLeftExp()->accept(this)->getBool() || 
            orNode->getRightExp()->accept(this)->getBool());
    }

    Constant* visitRel(Rel* relNode) override {
        
        switch(relNode->getOp())
        {
            case Rel::MORE:
                return boolConst->set(relNode->getLeftExp()->accept(this)->getInt() > 
                relNode->getRightExp()->accept(this)->getInt());
                break;

            case Rel::MORE_EQ:
                return boolConst->set(relNode->getLeftExp()->accept(this)->getInt() >= 
                relNode->getRightExp()->accept(this)->getInt());
                break;

            case Rel::LESS:
                return boolConst->set(relNode->getLeftExp()->accept(this)->getInt() <
                relNode->getRightExp()->accept(this)->getInt());
                break;

            case Rel::LESS_EQ:
                return boolConst->set(relNode->getLeftExp()->accept(this)->getInt() <= 
                relNode->getRightExp()->accept(this)->getInt());
                break;
    
            default:
                throw EvaluationError("Invalid relational operator");
        }
    }

    Constant* visitBinOp(Arithm* arithmNode)
    {
        switch(arithmNode->getOp())
        {
            case Op::ADD:
                return intConst->set(arithmNode->getLeftExp()->accept(this)->getInt() 
                + arithmNode->getRightExp()->accept(this)->getInt());
                break;
            case Op::SUB:
                return intConst->set(arithmNode->getLeftExp()->accept(this)->getInt() 
                - arithmNode->getRightExp()->accept(this)->getInt());
                break;
            case Op::MUL:
                return intConst->set(arithmNode->getLeftExp()->accept(this)->getInt() 
                * arithmNode->getRightExp()->accept(this)->getInt());
                break;
            case Op::DIV:
                if(arithmNode->getRightExp()->accept(this)->getInt()==0)
                   throw EvaluationError("Division by 0");
                return intConst->set(arithmNode->getLeftExp()->accept(this)->getInt() 
                / arithmNode->getRightExp()->accept(this)->getInt());
                break;
            case Op::EQ:               
                if(arithmNode->getRightExp()->accept(this)->getTypeCode() == Type::INT)
                    return boolConst->set(arithmNode->getLeftExp()->accept(this)->getInt() 
                    == arithmNode->getRightExp()->accept(this)->getInt());

                //eq operations between bools are also allowed
                else if(arithmNode->getRightExp()->accept(this)->getTypeCode() == Type::BOOL) 
                    return boolConst->set(arithmNode->getLeftExp()->accept(this)->getBool()
                     == arithmNode->getRightExp()->accept(this)->getBool());
                break;

            case Op::NOT_EQ:
                if(arithmNode->getRightExp()->accept(this)->getTypeCode() == Type::INT)
                    return boolConst->set(arithmNode->getLeftExp()->accept(this)->getInt() 
                    != arithmNode->getRightExp()->accept(this)->getInt());
                else if(arithmNode->getRightExp()->accept(this)->getTypeCode() == Type::BOOL)
                    return boolConst->set(arithmNode->getLeftExp()->accept(this)->getBool() 
                    != arithmNode->getRightExp()->accept(this)->getBool());
                break;
            
            default:
                throw EvaluationError("Invalid arithmetic operator");
            
        }
        return nullptr;
    }

    Constant* visitUnaryOp(Unary* unaryNode)
    {   
        switch (unaryNode->getOp())
        {
        case Op::UNARY_MIN:
            return intConst->set(-unaryNode->getExp()->accept(this)->getInt());
            break;
        
        default:
            throw EvaluationError("Invalid unary arithmetic operator");
            break;
        }
    }

    Constant* visitIntConstant(intConstant* numNode) 
    {
        return numNode;
    }
    
    Constant* visitBoolConstant(boolConstant* numNode)
    {
        return numNode;
    } 

    Constant* visitAccess(Access* accessNode)
    {
        return env.getArrayValue(accessNode->getId()->getName(), accessNode->getIndex()->accept(this)->getInt());
    }


    
private:
    //these consts are used to store temporary evaluations of expressions, they are used 
    //to avoid making heap allocations in every visit function 
    intConstant* intConst;
    boolConstant* boolConst;
        
    //Used to tell the visitor when a break is encountered 
    bool breakFlag;

    //The environment in which all the dynamically allocated variables are stored
    Environment& env;
};


// Visitor concreto per la stampa delle espressioni
class PrintVisitor : public Visitor {
public:
    PrintVisitor() = default;
    ~PrintVisitor() = default;
    PrintVisitor(PrintVisitor const&) = delete;
    PrintVisitor& operator=(PrintVisitor const&) = delete;

    Constant* visitProgram(Program* program) override {
        std::cout<<"Program(";
        program->getBlock()->accept(this);
        std::cout<<")";
        return nullptr;
    }

    Constant* visitBlock(Block* block) override {
        std::cout<<"Block(";
        if(block->getDecls()) 
            block->getDecls()->accept(this);
        else 
            std::cout<<"NULL";
        std::cout<<", ";
        if(block->getSeq()) 
            block->getSeq()->accept(this);
        else 
            std::cout<<"NULL";
        std::cout<<")";
        return nullptr;
    }

    Constant* visitDecls(Decls* decls) override {

        std::cout<<"Decls(";
        decls->getDecl()->accept(this);
        std::cout<<", ";
        if(decls->getDecls())
            decls->getDecls()->accept(this);
        else 
            std::cout<<"NULL";
        std::cout<<")";
        return nullptr;
    }

    Constant* visitDecl(Decl* decl) override {
        std::cout<<"Decl(";
        decl->getType()->accept(this);
        std::cout<<", ";
        decl->getId()->accept(this);
        std::cout<<")";

        return nullptr;
    }

    Constant* visitType(Type* type) override {
        std::cout<<"Type(";
        std::cout<<Type::typeid2String[type->getTypeCode()];
        std::cout<<")";

        return nullptr;
    }

    Constant* visitVectorType(vectorType* type) override {
        std::cout<<"VectorType(";
        std::cout<<Type::typeid2String[type->getTypeCode()];
        std::cout<<", ["<<type->getSize()<<"]";
        std::cout<<")";

        return nullptr;
    }


    Constant* visitId(Id* id) override {
        std::cout<<"Id(";
        std::cout<<id->getName();
        std::cout<<")";

        return nullptr;
    }

    Constant* visitSeq(Seq* Seq) override {

        std::cout<<"Seq(";
        Seq->getStmt()->accept(this);
        std::cout<<", ";
        if(Seq->getSeq())
            Seq->getSeq()->accept(this);
        else 
            std::cout<<"NULL";
        std::cout<<")";
        return nullptr;
    }    
    
    Constant* visitIf(If* ifNode) override {
        std::cout<<"If(";
        ifNode->getCondition()->accept(this);
        std::cout<<", ";
        ifNode->getStmt()->accept(this);
        std::cout<<")";
        return nullptr;
    }


    Constant* visitElse(Else* elseNode) override {
        std::cout<<"Else(";
        elseNode->getCondition()->accept(this);
        std::cout<<", ";
        elseNode->getifTrueStmt()->accept(this);
        std::cout<<", ";
        elseNode->getifFalseStmt()->accept(this);
        std::cout<<")";
        return nullptr;
    }

    Constant* visitWhile(While* whileNode) override {
        std::cout<<"While(";
        whileNode->getCondition()->accept(this);
        std::cout<<", ";
        whileNode->getStmt()->accept(this);
        std::cout<<")";
        return nullptr;
    }

    Constant* visitDo(Do* doNode) override {
        std::cout<<"Do(";
        doNode->getCondition()->accept(this);
        std::cout<<", ";
        doNode->getStmt()->accept(this);
        std::cout<<")";
        return nullptr;
    }

    Constant* visitSet(Set* setNode) override {
        std::cout<<"Set(";
        setNode->getId()->accept(this);
        std::cout<<", ";        
        setNode->getExp()->accept(this);
        std::cout<<")";
        return nullptr;
    }

    Constant* visitSetElem(SetElem* setElemNode) override {
        std::cout<<"SetElem(";
        setElemNode->getId()->accept(this);
        std::cout<<",[";        
        setElemNode->getIndex()->accept(this);
        std::cout<<"], ";
        setElemNode->getExp()->accept(this);
        std::cout<<")";        
        return nullptr;
    }

    Constant* visitBreak(Break* breakNode) override {
        std::cout<<"Break()";
        return nullptr;
    }

    Constant* visitPrint(Print* printNode) override {
        std::cout<<"Print(";
        printNode->getExp()->accept(this);
        std::cout<<")";
        return nullptr;
    }

    Constant* visitNot(Not* notNode) override {
        std::cout<<"Not(";
        notNode->getExp()->accept(this);
        std::cout<<")";        
        return nullptr;
    }

    Constant* visitAnd(And* andNode) override {
        std::cout<<"And(";
        andNode->getLeftExp()->accept(this);
        std::cout<<", ";        
        andNode->getRightExp()->accept(this);
        std::cout<<")";        
        return nullptr;
    }

    Constant* visitOr(Or* orNode) override {
        std::cout<<"Or(";
        orNode->getLeftExp()->accept(this);
        std::cout<<", ";        
        orNode->getRightExp()->accept(this);
        std::cout<<")";        
        return nullptr;
    }

    Constant* visitRel(Rel* relNode) override {
        std::cout<<"Rel(";
        std::cout<<Rel::opCode2String[relNode->getOp()];
        std::cout<<", ";        
        relNode->getLeftExp()->accept(this);
        std::cout<<", ";        
        relNode->getRightExp()->accept(this);
        std::cout<<")";                
        return nullptr;
    }

    Constant* visitIntConstant(intConstant* numNode) 
    {
        std::cout<<"IntConstant("<<numNode->getInt()<<")";
        return nullptr;
    }
    
    Constant* visitBoolConstant(boolConstant* numNode)
    {
        std::cout<<"BoolConstant("<<numNode->getBool()<<")";
        return nullptr;
    } 
    
    Constant* visitBinOp(Arithm* arithmNode)
    {
        std::cout<<"Arithm(";
        std::cout<<Op::binOp2String[arithmNode->getOp()];
        std::cout<<", ";        
        arithmNode->getLeftExp()->accept(this);
        std::cout<<", ";        
        arithmNode->getRightExp()->accept(this);
        std::cout<<")";                
        return nullptr;
    }
    
    Constant* visitUnaryOp(Unary* unaryNode)
    {
        std::cout<<"Unary(";
        std::cout<<Op::unaryOp2String[unaryNode->getOp()];
        std::cout<<", ";        
        unaryNode->getExp()->accept(this);        
        std::cout<<")";                
        return nullptr;
    }

    Constant* visitAccess(Access* accessNode)
    {
        std::cout<<"Access(";
        accessNode->getId()->accept(this);
        std::cout<<",[";        
        accessNode->getIndex()->accept(this);
        std::cout<<",] )";        
        return nullptr;
    }

};


#endif
