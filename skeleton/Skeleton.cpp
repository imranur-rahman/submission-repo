#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

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
