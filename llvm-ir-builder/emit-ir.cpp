#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include <memory>
#include "llvm/IR/Verifier.h"
#include "/home/yash007/Desktop/llvm-install/include/llvm/TargetParser/Host.h"

#include <map> 
#include <string>


static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::Module> TheModule;
static std::unique_ptr<llvm::IRBuilder<>> Builder;

static void initializeModule(){
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("ir-builder", *TheContext);

    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}


static void saveModuleIRToFile(const std::string& filename){
    std::error_code errorCode;
    llvm::raw_fd_ostream out(filename, errorCode);
    TheModule->print(out, nullptr, false);

}

typedef struct FunProto{
    llvm::Type *returnType;
    std::vector<llvm::Type*> params;
    bool isVarArg;
} FunProto;

static std::map<std::string, FunProto> funProtoMap;

void registerFunctionProto(){
    funProtoMap["main"] = {
        Builder->getInt32Ty(),
        {},
        false,
    };
}

llvm::Function *declareFunction(std::string name){
    auto func = TheModule->getFunction(name);
    if(func == nullptr){
        auto funProto = funProtoMap[name];
        auto funType = llvm::FunctionType::get(funProto.returnType, funProto.params, funProto.isVarArg);
        func = llvm::Function::Create(funType, llvm::Function::ExternalLinkage, name, TheModule.get());
        func->setDSOLocal(true);
    }
    return func;
}

void emitReturn(llvm::Type *ty, llvm::Value *value){
    if(ty->isVoidTy()){
        Builder->CreateRetVoid();
    }else{
        Builder->CreateRet(value);

    }

}

llvm::Value* localDemo();

llvm::Value* emitMainFunctionStatementList();
llvm::Value* globalDemo();
void emitFunctionBody(llvm::Function *fn){
    auto entry = llvm::BasicBlock::Create(*TheContext , "entry", fn);
    Builder->SetInsertPoint(entry);

    auto value = emitMainFunctionStatementList();
    // auto value = Builder->getInt32(10);
    // auto value = globalDemo();

    emitReturn(fn->getReturnType(), value);
}

void defineFunction(std::string name){
    auto fn = TheModule->getFunction(name);
    emitFunctionBody(fn);
    verifyFunction(*fn);
}

llvm::GlobalVariable* defineGlobalVariable(std::string name, llvm::Constant *init){
    TheModule->getOrInsertGlobal(name, init->getType());
    auto globalVar = TheModule->getNamedGlobal(name);
    globalVar->setInitializer(init);
    globalVar->setDSOLocal(true);
    return globalVar;
}


llvm::Value* emitLoadValue(llvm::GlobalVariable *value){
    return Builder->CreateLoad(value->getInitializer()->getType(), value);
}

llvm::Value* emitLoadValue(llvm::Value *value){
    //auto baseType = value->getType()->getPointerElementType();
    llvm::Type *baseType = Builder->getInt32Ty(); // Or another expected type

    return Builder->CreateLoad(baseType, value);
}



llvm::Value* emitLoadGlobalVar(std::string name){
    auto globalVar = TheModule->getGlobalVariable(name);
    auto rValue = emitLoadValue(globalVar);
    return rValue;
}





void emitAssign(llvm::Value *left , llvm::Value *right){
    Builder->CreateStore(right, left);
}

llvm::Value* emitStackLocalVariable(llvm::Type* type , std::string name ){
    return Builder->CreateAlloca(type, nullptr, name);
}


void emitConstant(llvm::Type *ty, std::string name, llvm::Constant *init){
    auto currentFunction = Builder->GetInsertBlock()->getParent();
    std::string funcName = currentFunction->getName().data();
    std::string constantVarName= "__constant." + funcName + "." + name;
////////////////////////////////////////////////////////////////
    auto constantVar = defineGlobalVariable(constantVarName, init);
    constantVar->setConstant(true);
    constantVar->setLinkage(llvm::GlobalValue::LinkageTypes::PrivateLinkage);
    

} 



void emitStringPtr(std::string content, std::string name){
    auto strPtr = Builder->CreateGlobalString(content, "." + name);
    ///////////////////////////////////////////////////////////
}

void emitIntegers(){
    //char i_8 = 1
    defineGlobalVariable(Builder->gerInt8Ty(),"i_8" Builder->gerInt8(1));

    //short i_16 = 2
    defineGlobalVariable(Builder->gerInt16Ty(), "i_16", Builder->getInt16(2));

    //int i_32= 3
    defineGlobalVariable(Builder->getInt32Ty(),"i_32", Builder->getInt32(3));

    //long i_64 = 4
    defineGlobalVariable(Builder->getInt64Ty(), "i_64", Builder->getInt64(4));

    //unsigned char ui_8 = 1
    defineGlobalVariable(Builder->getInt8Ty(),"ui_8", Builder->getInt8(1));

    //unsigned int ui_32 = 3
    defineGlobalVariable(Builder->getInt32Ty(),"ui_32", Builder->getInt32(3));

}

void emitFloats(){
    //float f=1.0
    defineGlobalVariable(Builder->getFloatTy(), "f", llvm::ConstantFP::get(Builder->getFloatTy(), 1.0));

    //double d= 2.0
    defineGlobalVariable(Builder->getDoubleTy(), "d", llvm::ConstantFPt)

}




void emitConstantArray(){
    int num = 4;
    llvm::Type *ty = Builder->getInt32Ty();
    llvm::ArrayType *arrType = llvm::ArrayType::get(ty, num);

    llvm::SmallVector<llvm::Constant*, 16> list;
    list.reserve(num);
    list.push_back(Builder->getInt32(1));
    list.push_back(Builder->getInt32(2));
    list.push_back(Builder->getInt32(3));
    list.push_back(Builder->getInt32(4));

    llvm::Constant *c = llvm::ConstantArray::get(arrType, list);
    emitConstant(arrType, "init_array", c);

    
}

void emitConstStruct(){
    llvm::StructType *structTy = llvm::StructType::create(*TheContext,"struct.point");
    structTy->setBody({Builder->getInt32Ty(), Builder->getInt32Ty()});


    int num = 2;
    llvm::SmallVector<llvm::Constant*, 16> list;
    list.reserve(num);
    list.push_back(Builder->getInt32(1));
    list.push_back(Builder->getInt32(2));

    llvm::Constant *c = llvm::ConstantStruct::get(structTy, list);
    emitConstant(structTy, "point", c);
}

void emitConstString(){
    emitStringPtr("hello", "string");
}









llvm::Value* constantsDemo(){
    emitConstantArray();
    emitConstStruct();
    emitConstString();

    auto value = emitLoadGlobalVar("global_a");
    return value;
}



// llvm::Value* localDemo(){
//     auto local_b = emitStackLocalVariable(Builder->getInt32Ty(), "local_b");

//     emitAssign(local_b, Builder->getInt32(2));
//     auto global_a = TheModule->getGlobalVariable("global_a");
//     auto global_a_rvalue = emitLoadValue(global_a);

//     emitAssign(local_b, global_a_rvalue);

//     auto value = emitLoadValue(local_b);
//     return value;
// }

// llvm::Value* globalDemo(){
//     auto global_a = TheModule->getGlobalVariable("global_a");
//     auto value_2 = Builder->getInt32(2);


//     emitAssign(global_a, value_2);

//     auto value = emitLoadValue(global_a);

//     return value;
// }













llvm::Value* emitMainFunctionStatementList(){
    // return globalDemo();
    // return localDemo();
    return constantsDemo();
    
}


void emitProgram(){
    defineGlobalVariable("global_a", Builder->getInt32(1));

    declareFunction("main");
    defineFunction("main");

}


int main(int argc, char *argv[]){
    initializeModule();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    TheModule->setTargetTriple(targetTriple);

    registerFunctionProto();
    emitProgram();

    TheModule->print(llvm::outs(), nullptr);
    saveModuleIRToFile("out.ll");
    return 0;

}