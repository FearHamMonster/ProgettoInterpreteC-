#ifndef NODE_H
#define NODE_H
#include <string>
#include <map>
#include "Exceptions.h"
//forward declaration essenziali per evitare errori di compilazipone
class Visitor;  
class Id;
class Seq;
class Stmt;
class Block;
class Constant;

class Node
{
    public:
    virtual ~Node() = default;
    Node() = default;
    virtual Constant* accept(Visitor* v) = 0;
};

//Type
class Type : public Node{

public:
    enum TypeCode{INT, BOOL};
    static const int numOfTypes = 2;
    static std::string  typeid2String [numOfTypes]; 

    Type(TypeCode t) : type{t}{}

    TypeCode getTypeCode() {return type;}

    Constant* accept(Visitor* v) override;   

private:
    TypeCode type;
};

class vectorType : public Type{
public: 

    vectorType(Type::TypeCode t, int s) : Type(t), size{s}{}
    
    int getSize(){return size;}

    Constant* accept(Visitor* v) override;   

private:
    int size;
    
};


//Decls
class Decl : public Node{
public:

    Decl(Type* t, Id* i): type{t}, id{i}{}
    Type* getType(){return type;}
    Id* getId(){return id;}

    Constant* accept(Visitor* v) override;
private:
    Type* type;
    Id* id;
};

class Decls : public Node{
public:

    //it's better to treat the empty decls object as something more abstract than just nullptr
    static constexpr Decls* EMPTY_DECLS = nullptr; 
    Decls(Decl* dec, Decls* decs ): declaration{dec}, declarations{decs}{}

    Decl* getDecl(){return declaration;}
    Decls* getDecls(){return declarations;}

    Constant* accept(Visitor* v) override;

private:
    Decl* declaration;
    Decls* declarations; 
};



//Program
class Program : public Node{
public:

    Program(Block* b) : block{b}{}

    Block* getBlock() {return block;}
    
    Constant* accept(Visitor* v) override;

private:
    Block* block;
};



//Expression
class Expression : public Node{
};
    
class Constant : public Expression {
    
public: 
    Constant(Type::TypeCode t) : typeCode{t}{}

    Type::TypeCode getTypeCode() {return typeCode;}

    virtual bool getBool()
    {
        throw  EvaluationError("Expecting a boolean");
    }
    
    virtual int getInt()
    {
        throw  EvaluationError("Expecting an integer");
    }

  Constant* accept(Visitor* v) override;

private:
    Type::TypeCode typeCode;

};

class Id: public Expression
{
public:
  Id(std::string name_) : name{name_} {};
  Id& operator= (const Id& other) = default;
  
  std::string getName() {
    return name;
  }

  Constant* accept(Visitor* v) override;

private:
  std::string name; 
};

class intConstant : public Constant{
public:

    intConstant() : value{0}, Constant(Type::INT){}
    intConstant(int v) : value{v}, Constant(Type::INT){}

    int getInt() override {
        return value;
    }

    intConstant* set (int val)
    {
        value = val;
        return this;
    }

    Constant* accept(Visitor* v) override;

private:
    int value;
};

class boolConstant : public Constant{
public:
    
    boolConstant() : value{false}, Constant(Type::BOOL) {}
    boolConstant(bool v) : value{v}, Constant(Type::BOOL) {}

    bool getBool() override {
        return value;

    }
    boolConstant* set (bool val)
    {
        value = val;
        return this;
    }


    Constant* accept(Visitor* v) override;

private:
    bool value;
};


//Logical
class Logical : public Expression{

};

class Not : public Logical{

public:

    Not(Expression* e) : exp{e}{}
    Expression* getExp() {return exp;}

    Constant* accept(Visitor* v) override;

private:
    Expression* exp;

};

class And : public Logical{

public:

    And(Expression* l, Expression* r) : left{l}, right{r}{}
    Expression* getLeftExp() {return left;}
    Expression* getRightExp() {return right;}

    Constant* accept(Visitor* v) override;

private:
    Expression* left;
    Expression* right;

};

class Or : public Logical{

public:

    Or(Expression* l, Expression* r) : left{l}, right{r}{}
    Expression* getLeftExp() {return left;}
    Expression* getRightExp() {return right;}

    Constant* accept(Visitor* v) override;


private:
    Expression* left;
    Expression* right;

};

class Rel : public Logical{
    
public:
    enum OpCode {MORE, MORE_EQ, LESS, LESS_EQ};
    static const int numOfOps = 4;
    static std::string opCode2String[numOfOps];

    Rel(Expression* l, Expression* r, OpCode op) :
    left{l}, right{r}, operation{op}{}
    Expression* getLeftExp() {return left;}
    Expression* getRightExp() {return right;}
    OpCode getOp(){return operation;} 

    Constant* accept(Visitor* v) override;

private:
    Expression* left;
    Expression* right;
    OpCode operation;
};




//Operations
class Op : public Expression {

public:
  enum BinOpCode {ADD, SUB, MUL, DIV, EQ, NOT_EQ};
  static const int numOfBinOps = 6;
  static std::string binOp2String[numOfBinOps];

  enum UnaryOpCode {UNARY_MIN};
  static const int numOfUnaryOps = 1; 
  static std::string unaryOp2String[numOfUnaryOps];
};

//operatori unari
class Unary : public Op{

public:
    Unary( Expression* ex, UnaryOpCode op) : exp{ex}, operation{op} {}

    Expression* getExp() {return exp;}
    UnaryOpCode getOp(){return operation;} 

    Constant* accept(Visitor* v) override;

private:
    Expression* exp;
    UnaryOpCode operation;

};

//operatori binari
class Arithm : public Op{

public:

    Arithm(Expression* l, Expression* r, BinOpCode op) :
    left{l}, right{r}, operation{op}{} 
    Expression* getLeftExp() {return left;}
    Expression* getRightExp() {return right;}
    BinOpCode getOp() {return operation;} 

    Constant* accept(Visitor* v) override;

private:
    Expression* left;
    Expression* right;
    BinOpCode operation;    
};

//operazione di accesso indicizzato a vettore
class Access : public Op{
public:

    Access(Id* vec, Expression* ind) : vector{vec}, index{ind}{}
    Id* getId() {return vector;}
    Expression* getIndex() {return index;}

    Constant* accept(Visitor* v) override;

private:
    Id* vector;
    Expression* index;

};



//Seq
class Seq: public Node{
public:

//it's better to treat the empty Seq object as something more abstract than just nullptr
static constexpr Seq* EMPTY_SEQ = nullptr; 
Seq(Seq* seq_, Stmt* stmt_) : seq{seq_}, stmt{stmt_}{}

Seq* getSeq() {return seq;}
Stmt* getStmt() {return stmt;}

Constant* accept(Visitor* v) override;   

private:
Seq* seq;
Stmt* stmt;
};

class Stmt : public Node{
  
public:
    Constant* accept(Visitor* v) override;   

};

class If : public Stmt {
public:

    If(Stmt* s, Expression* e) : stmt{s}, condition{e}{}

    Stmt* getStmt() {return stmt;}
    Expression* getCondition () {return condition;}

    Constant* accept(Visitor* v) override;   

private:
    Stmt* stmt;
    Expression* condition;
};

class Else : public Stmt{
public:

    Else(Stmt* stmtTrue, Stmt* stmtFalse, Expression* e)
     : stmtIfTrue{stmtTrue}, stmtIfFalse{stmtFalse}, condition{e}{}

    Stmt* getifTrueStmt() {return stmtIfTrue;}
    Stmt* getifFalseStmt() {return stmtIfFalse;}
    Expression* getCondition () {return condition;}

    Constant* accept(Visitor* v) override;   

private:
    Expression* condition;
    Stmt* stmtIfTrue;
    Stmt* stmtIfFalse;
};

class While : public Stmt{
public:

    While(Stmt* s, Expression* e) : stmt{s}, condition{e}{}
    
    Stmt* getStmt() {return stmt;}
    Expression* getCondition () {return condition;}

 
    Constant* accept(Visitor* v) override;   

private:
    Stmt* stmt;
    Expression* condition;
};

class Do : public Stmt{
public:

    Do(Stmt* s, Expression* e) : stmt{s}, condition{e}{}
    Stmt* getStmt() {return stmt;}
    Expression* getCondition () {return condition;}


    Constant* accept(Visitor* v) override;   

private:
    Stmt* stmt;
    Expression* condition;
  
};

class Set : public Stmt{
public:

    Set(Id* var, Expression* e) : variable{var}, exp{e}{}
    Id* getId(){return variable;}
    Expression* getExp(){return exp;}

    Constant* accept(Visitor* v) override;   

private:
    Id* variable;
    Expression* exp;    
};

class SetElem : public Stmt{
public:

    SetElem(Id* v, Expression* e, Expression* i) :
         arrayName{v}, exp{e}, index{i} {}

    Id* getId(){return arrayName;}
    Expression* getExp(){return exp;}
    Expression* getIndex(){return index;}


    Constant* accept(Visitor* v) override;   


private:
    Id* arrayName;
    Expression* exp;    
    Expression* index;
};

class Break : public Stmt{
public:
    Break() = default;
    Constant* accept(Visitor* v) override;   

};

class Print : public Stmt{
public:

    Print(Expression* e) : expToPrint{e}{}
    Expression* getExp(){return expToPrint;}
   
    Constant* accept(Visitor* v) override;   

private:
    Expression* expToPrint;    
};

class Block : public Stmt{
public:

    Block(Decls* decs, Seq* Seq) : declarations{decs}, statements{Seq}{}
    Decls* getDecls(){return declarations;}
    Seq* getSeq(){return statements;}

    Constant* accept(Visitor* v) override;   

private:
    Decls* declarations;
    Seq* statements;

};

//Visitor is declared here to avoid circular dependency
#include "Visitor.h"


#endif