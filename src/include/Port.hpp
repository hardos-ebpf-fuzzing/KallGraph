#ifndef PORT_H
#define PORT_H

#include "llvm/IR/Function.h"
#include <llvm/IR/Type.h>
#include <llvm/Support/Casting.h>
#include "SVF-LLVM/LLVMModule.h"
#include "SVFIR/SVFValue.h"

#include "llvm/ADT/DenseMap.h"
#include <shared_mutex>
#include <unordered_map>

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

static inline const llvm::Type*
fromSVFTypeToLLVMType(const SVF::SVFType* svfType) noexcept {
  if (!svfType) return nullptr;

  thread_local std::unordered_map<const SVF::SVFType*, const llvm::Type*> tlCache;
  if (auto it = tlCache.find(svfType); it != tlCache.end())
    return it->second;

  static llvm::DenseMap<const SVF::SVFType*, const llvm::Type*> gCache;
  static std::shared_mutex gCacheMutex;

  {
    std::shared_lock rlk(gCacheMutex);
    if (auto it = gCache.find(svfType); it != gCache.end()) {
      tlCache.emplace(svfType, it->second);
      return it->second;
    }
  }

  const auto* ms = SVF::LLVMModuleSet::getLLVMModuleSet();
  const llvm::Type* ty = ms->getLLVMType(svfType);

  {
    std::unique_lock wlk(gCacheMutex);
    auto [it, _] = gCache.try_emplace(svfType, ty);
    tlCache.emplace(svfType, it->second);
    return it->second;
  }
}

static inline const SVF::SVFType *fromLLVMTypeToSVFType(const llvm::Type *llvmType) {
    return SVF::LLVMModuleSet::getLLVMModuleSet()->getSVFType(llvmType);
}

#endif
