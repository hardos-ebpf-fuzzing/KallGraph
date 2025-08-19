#ifndef PORT_H
#define PORT_H

#include "llvm/IR/Function.h"
#include <llvm/Support/Casting.h>
#include "SVF-LLVM/LLVMModule.h"
#include "SVFIR/SVFValue.h"
// static inline const Function *fromSVFFuncToLLVMFunc(const SVFFunction *svfFunc) {
//     // SVFFunction is a subclass of SVFValue;
//     // llvm::Function is a subclass of llvm::Value
//     // There is a map called `SVFValue2LLVMValue`, which maps SVFValue to llvm::Value, traversed by `getLLVMValue`;
//     return dyn_cast<Function>(LLVMModuleSet::getLLVMModuleSet()->getLLVMValue(svfFunc));
// }

template <typename T>
static inline const T *fromLLVMValueToSVFValue(const llvm::Value *llvmValue) {
    static_assert(std::is_base_of<SVF::SVFValue, T>::value, "T must be a subclass of SVFValue");

    // SVF/svf-llvm/lib/LLVMModule.cpp::1336
    return llvm::dyn_cast<T>(SVF::LLVMModuleSet::getLLVMModuleSet()->getSVFValue(llvmValue));
}

template <typename T>
static inline const T *fromSVFValueToLLVMValue(const SVF::SVFValue *svfValue) {
    static_assert(std::is_base_of<llvm::Value, T>::value, "T must be a subclass of llvm::Value");

    // SVFValue2LLVMValue
    return llvm::dyn_cast<T>(SVF::LLVMModuleSet::getLLVMModuleSet()->getLLVMValue(svfValue));
}

#endif
