#include "llvm-14/llvm/Pass.h"
#include "llvm-14/llvm/Passes/PassBuilder.h"
#include "llvm-14/llvm/Passes/PassPlugin.h"
#include "llvm-14/llvm/Support/raw_ostream.h"

#include "llvm-14/llvm/IR/DebugLoc.h"
#include "llvm-14/llvm/IR/DebugInfoMetadata.h"

#include "llvm-14/llvm/IR/Instructions.h"
#include <iostream>

using namespace llvm;

namespace {

struct SkeletonPass : public PassInfoMixin<SkeletonPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        for (auto &F : M) {
            errs() << "I saw a function called " << F.getName() << "!\n";
            //errs() << "Function body:\n" << F << "\n";
            for (auto& B : F) {
                errs() << "Basic block:\n" << B << "\n";
                for (auto& I : B) {
                    errs() << "Instruction: " << I << "\n";
                    // if (auto* op = dyn_cast<BinaryOperator>(&I)) {
                    //     errs() << "op\n" << op << "\n";
                    // }
                    if (auto* op = dyn_cast<BranchInst>(&I)) {

                        
                        if (op->isConditional()) {
                            // TODO: Did we cover every branch inst? switch...
                            Value* condition = op->getCondition();
                            BasicBlock* true_dst = op->getSuccessor(0);
                            BasicBlock* false_dst = op->getSuccessor(1);

                            auto* inst = dyn_cast<BinaryOperator>(&I);

                            errs() << "inst: " << inst << "\n";
                            errs() << "BranchInst: " << true_dst << ", " << false_dst << "\n";
                            if (true_dst->hasName()) {
                                errs() << "true dest: " << true_dst->getName() << "\n";
                            }
                            if (false_dst->hasName()) {
                                errs() << "false dest: " << false_dst->getName() << "\n";
                            }

                            // Get the line number
                            const DebugLoc &debugInfo = I.getDebugLoc();

                            std::string directory = std::string(debugInfo->getDirectory());
                            std::string filePath = std::string(debugInfo->getFilename());
                            int line = debugInfo->getLine();
                            int column = debugInfo->getColumn();

                            errs() << directory << ", " << filePath << ", " << line << ", " << column << "\n";

                            // Get first successor's line number
                            Instruction* firstLine = true_dst->getFirstNonPHI();
                            const DebugLoc &di = firstLine->getDebugLoc();
                            directory = std::string(di->getDirectory());
                            filePath = std::string(di->getFilename());
                            line = di->getLine();
                            column = di->getColumn();
                            errs() << directory << ", " << filePath << ", " << line << ", " << column << "\n";

                            // Get second successor's line number
                            firstLine = false_dst->getFirstNonPHI();
                            const DebugLoc &dg = firstLine->getDebugLoc();
                            directory = std::string(dg->getDirectory());
                            filePath = std::string(dg->getFilename());
                            line = dg->getLine();
                            column = dg->getColumn();
                            errs() << directory << ", " << filePath << ", " << line << ", " << column << "\n";

                        }
                    }
                    if (auto* op = dyn_cast<ICmpInst>(&I)) {
                        errs() << "ICmpInst instance" << "\n";
                        CmpInst::Predicate pr = op->getSignedPredicate();
                        errs() << pr << "\n";
                        for (Use& U: I.operands()) {
                            Value* v = U.get();
                            if (dyn_cast<Instruction>(v)) {
                                errs() << "\"" << *dyn_cast<Instruction>(v) << "\"" << " -> " << "\"" << I << "\"" << ";\n";
                            }
                            if (v->getName() != "") {
                                errs() << "\"" << v->getName() << "\"" << " -> " << "\"" << I << "\"" << ";\n";
                                errs() << "\"" << v->getName() << "\"" << " [ color = red ]\n";
                            }
                        }
                    }
                }
            }
        }
        // LoopInfo &LI = getAnalysis<LoopInfo>();
        // for (auto &loop : LoopInfo) {
        //     errs() << "loop:\n" << loop << "\n";
        // }
        return PreservedAnalyses::all();
    };
};

}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "Skeleton pass",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                    MPM.addPass(SkeletonPass());
                });
        }
    };
}
