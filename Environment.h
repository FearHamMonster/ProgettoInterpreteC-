#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <map>
#include "Node.h"
#include "ExpressionManager.h"

//A struct is created to store additional information of an array object, such as size and type
struct arrayStruct
{
  arrayStruct( Type::TypeCode t, const int s, std::vector<Constant**>& allocatedArrs) : typeCode{t}, size{s}
  {
    Constant** arr = new Constant*[size]; 
    
    //Individual cells are later declared, during assignment 
    for(int i=0; i<size; i++)
    {
      arr[i] = nullptr;
    }
   array = arr;
   //an array of consts is deallocated when destructor is called
   allocatedArrs.push_back(array);
  }
    
  const int size;
  Constant** array;  //pointer points to arrays of Constants 
  Type::TypeCode typeCode;
};


//This is the class that handles creation and manipulation of variables and arrays 
class Environment {
public:
  Environment(ExpressionManager& e) : em{e}{}
  
  ~Environment(){
    clearMemory();
  }

  void declareVar(std::string name_, Type* type_){
    if(isAlreadyDeclared(name_))
      return;
   Type::TypeCode typeCode = type_->getTypeCode();
    switch (typeCode)
    {
        case Type::INT:
          declaredVars[name_] = em.makeIntConstant();
          break;
        case Type::BOOL:
          declaredVars[name_] = em.makeBoolConstant();
          break;
        default:
          throw EvaluationError("identifier "+ name_+ " is being declared with invalid type");
          break;
    }
  }         

 //declaring an array of name "x" corresponds to adding a tuple ("x",arraystruct) in the dictionary
  void declareArrayVar(std::string name_, vectorType* type){
    //double declaration is illegal, the reason that here an error is not thrown is because
    //double declaration checking is done at parsing time, not execution time
    if(isAlreadyDeclared(name_))
      return;

    auto arrayStrct = new arrayStruct(type->getTypeCode(),type->getSize(),allocatedArraysOfConstants);
    declaredArrays[name_] = arrayStrct;
    allocatedArrayStructs.push_back(arrayStrct); 
  }         

  //it's illegal to declare multiple variables with the same name, even if they don't share type
  //or one of them is a array and the other a primitive type
  bool isAlreadyDeclared(std::string idName){
    return (declaredVars.find(idName) != declaredVars.end()) ||
          (declaredArrays.find(idName) != declaredArrays.end());
  }

  Constant* getIdValue(std::string idName){
    auto it = declaredVars.find(idName);
    if(it == declaredVars.end())
      throw EvaluationError("Trying to access identifier " + idName + " , which has not been declared");
    return it->second; 
  }

  void assignConstant(std::string idName, Constant* value)
  {
    auto it = declaredVars.find(idName);
    if(it == declaredVars.end())
      throw EvaluationError("Trying to access identifier " + idName + " , which has not been declared");
    switch(value->getTypeCode())
    {
      //dynamic casting is necessary to do a deep copy instead of a shallow one 
      case Type::INT:
      dynamic_cast<intConstant*>(it->second)->set(dynamic_cast<intConstant*>(value)->getInt());
      break;

      case Type::BOOL:
      dynamic_cast<boolConstant*>(it->second)->set(dynamic_cast<boolConstant*>(value)->getBool());
    }
  }

  void assignConstantToArray(std::string idName, Constant* value, int index){
    auto it = declaredArrays.find(idName);
    if(it == declaredArrays.end())
      throw EvaluationError("Trying to access identifier " + idName + " , which has not been declared");

   if(index < 0 || index >= it->second->size)
      throw EvaluationError("Out of bounds error on " + idName + " array");
  
    switch(value->getTypeCode())
    {
      case Type::INT:
      if(!it->second->array[index]) //if element has not yet been declared we declare it
        it->second->array[index] = em.makeIntConstant(); 
      dynamic_cast<intConstant*>(it->second->array[index])->set(dynamic_cast<intConstant*>(value)->getInt());
      break;

      case Type::BOOL:
      if(!it->second->array[index]) //if element has not yet been declared we declare it
        it->second->array[index] = em.makeBoolConstant(); 
      dynamic_cast<boolConstant*>(it->second->array[index])->set(dynamic_cast<boolConstant*>(value)->getBool());
    }
    
  }

  Constant* getArrayValue(std::string idName, int index){
    auto it = declaredArrays.find(idName);
    if(it == declaredArrays.end())
      throw EvaluationError("Trying to access identifier " + idName + " , which has not been declared");

    if(index < 0 || index >= it->second->size)
      throw EvaluationError("Out of bounds error on " + idName + " array");

    if(!it->second->array[index]) //if array cell has not been declared, error
      throw EvaluationError("Trying to retrieve a cell from an array which has not been declared");
   
    return it->second->array[index];
  }

  arrayStruct* getArray(std::string idName){
    auto it = declaredArrays.find(idName);
    if(it == declaredArrays.end())
      throw EvaluationError("Trying to access identifier " + idName + " , which has not been declared");

    return (it->second);
  }



private:

  //the recipients of the actual data of variables and vectors
  std::map<std::string, Constant*> declaredVars;
  std::map<std::string, arrayStruct*> declaredArrays;

  //expression manager handles pointers of type Constant*, the other vectors handle pointers of different type
  ExpressionManager& em;
  std::vector<arrayStruct*> allocatedArrayStructs;
  std::vector<Constant**> allocatedArraysOfConstants;

  void clearMemory()
  {
    for (auto i = allocatedArraysOfConstants.begin(); i != allocatedArraysOfConstants.end(); ++i) 
        delete(*i);
    
    for (auto i = allocatedArrayStructs.begin(); i != allocatedArrayStructs.end(); ++i) 
        delete(*i);

    allocatedArrayStructs.resize(0);
    allocatedArraysOfConstants.resize(0);
  }
};


#endif