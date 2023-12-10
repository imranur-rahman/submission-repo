#include "llvm-14/llvm/Pass.h"
#include "llvm-14/llvm/Passes/PassBuilder.h"
#include "llvm-14/llvm/Passes/PassPlugin.h"
#include "llvm-14/llvm/Support/raw_ostream.h"

#include "llvm-14/llvm/IR/DebugLoc.h"
#include "llvm-14/llvm/IR/DebugInfoMetadata.h"
#include "llvm-14/llvm/IR/DebugInfo.h"

//#include "llvm-14/llvm/Analysis/DebugInfo.h"

#include "llvm-14/llvm/IR/Instructions.h"
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>

using namespace llvm;

namespace {

std::map<int, std::string>variable_map;
std::map<Metadata*, std::string>metadata_map;

struct SkeletonPass : public PassInfoMixin<SkeletonPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        for (auto &F : M) {
            errs() << "I saw a function called " << F.getName() << "!\n";
            //errs() << "Function body:\n" << F << "\n";
            for (auto& B : F) {
                errs() << "Basic block:\n" << B << "\n";
                for (auto& I : B) {
                    errs() << "Instruction: " << I << "\n";
                    if (DbgDeclareInst* ddi = dyn_cast<DbgDeclareInst>(&I)) {
                        errs() << "debug declare inst: " << I << "\n";
                        errs() << ddi->getVariable()->getName() << "\n";
                        //errs() << ddi->getAddress() << "\n";
                        const DebugLoc& dl = ddi->getDebugLoc();
                        int line = dl.getLine();
                        errs() << "line no: " << line << "\n";
                        variable_map[line] = ddi->getVariable()->getName();

                        Metadata *Meta = cast<MetadataAsValue>(I.getOperand(0))->getMetadata();
                        errs() << "metadata: " << *Meta << "\n";
                        metadata_map[Meta] = ddi->getVariable()->getName();
                    }
                    if (auto* op = dyn_cast<BranchInst>(&I)) {

                        
                        if (op->isConditional()) {
                            errs() << "######################################################################################################\n";
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

                            // do the analysis here
                            std::deque<Instruction*> worklist;
                            std::set<Instruction*> visited;
                            for (Use& U: I.operands()) {
                                Value* v = U.get();
                                if (Instruction* inst = dyn_cast<Instruction>(v)) {
                                    errs() << "\"" << *dyn_cast<Instruction>(v) << "\"" << " -> " << "\"" << I << "\"" << ";\n";
                                    errs() << *U << "\n";
                                    worklist.push_back(inst);
                                }
                            }
                            // It should be one at this point
                            errs() << "worklist size: " << worklist.size() << "\n";

                            // Do a BFS type analysis here
                            errs() << "bfs" << "\n";
                            while (worklist.empty() == false) {
                                Instruction* curr_inst = worklist.front();
                                errs() << "\ncurr_inst: " << *curr_inst << "\n";
                                worklist.pop_front();

                                // if already visited, no need to do the analysis again
                                if (visited.find(curr_inst) != visited.end()) {
                                    continue;
                                }
                                visited.insert(curr_inst);

                                // base condition
                                if (AllocaInst* AI = dyn_cast<AllocaInst>(curr_inst)) {
                                    //stack allocation
                                    errs() << "allocation instruction: " << *AI << "\n";
                                    // check if any of it's users are scanf
                                    errs() << "users of this allocation instruction: \n";
                                    for (User* user: curr_inst->users()) {
                                        // errs() << *user << "\n";
                                        Instruction* user_inst = dyn_cast<Instruction>(user);
                                        errs() << *user_inst << "\n";

                                        // for scanf
                                        if (auto *CI = dyn_cast<CallInst>(user_inst)) {
                                            StringRef callee_name = CI->getCalledFunction()->getName();
                                            errs() << "called function name: " << callee_name << "\n";
                                            if (callee_name == "__isoc99_scanf") {
                                                TinyPtrVector<DbgDeclareInst *> DIs = FindDbgDeclareUses(AI);
                                                for (DbgDeclareInst *ddi: DIs) {
                                                    dbgs() << "dbg inst: " << *ddi << '\n';
                                                    errs() << ddi->getVariable()->getName() << "\n";
                                                    const DebugLoc& dl = ddi->getDebugLoc();
                                                    int line = dl.getLine();
                                                    errs() << "\n\n" << "Line " << line << ": " << ddi->getVariable()->getName() << "\n\n";
                                                }
                                                

                                            }
                                        }

                                        // keep the store instructions to find the load instructions of <store, load> chain
                                        if (StoreInst* storeInst = dyn_cast<StoreInst>(user_inst)) {
                                            worklist.push_back(user_inst);
                                        }
                                    }
                                    errs() << "users of this allocation instruction end \n";
                                }
                                if (StoreInst* storeInst = dyn_cast<StoreInst>(curr_inst)) {
                                    errs() << "found a store instruction\n";
                                    // TinyPtrVector<DbgDeclareInst *> DIs = FindDbgDeclareUses(storeInst);
                                    // for (DbgDeclareInst *ddi: DIs) {
                                    //     dbgs() << "dbg inst: " << *ddi << '\n';
                                    //     errs() << ddi->getVariable()->getName() << "\n";
                                    //     const DebugLoc& dl = ddi->getDebugLoc();
                                    //     int line = dl.getLine();
                                    //     errs() << "\n\n" << "Line " << line << ": " << ddi->getVariable()->getName() << "\n\n";
                                    // }
                                    errs() << "value operand" << *storeInst->getValueOperand() << "\n";
                                    errs() << "printing operands\n";
                                    for (Use& U: curr_inst->operands()) {
                                        errs() << *U << "\n";
                                    }

                                    // if (Metadata *Meta = cast<MetadataAsValue>(curr_inst->getOperand(0))->getMetadata()) {
                                    //     errs() << "metadata: " << *Meta << "\n";
                                    // }
                                    errs() << "printing metadata\n";
                                    if (storeInst->hasMetadata()) {
                                        SmallVector<int>s;
                                        SmallVector< std::pair< unsigned, MDNode* >> MDs;
                                        storeInst->getAllMetadata(MDs);
                                        for (std::pair< unsigned, MDNode* >& it: MDs) {
                                            errs() << it.first << " " << *it.second << "\n";

                                            // DILocation Loc(it->second);
                                            // unsigned Line = Loc.getLineNumber();
                                            // errs() << "Line " << line << ": size of file " << variable_map[line] << "\n";
                                            errs() << "Metadata ID: " << it.second->getMetadataID() << "\n";
                                            const DebugLoc* debugLoc = new DebugLoc(it.second);
                                            unsigned line = debugLoc->getLine();
                                            errs() << "Line " << line << ": size of file " << variable_map[line] << "\n";
                                        }
                                    }
                                    
                                }
                                if (CallInst* callInst = dyn_cast<CallInst>(curr_inst)) {
                                    errs() << "callInst: " << *callInst << "\n";
                                    Function* called_function = callInst->getCalledFunction();
                                    StringRef callee_name = called_function->getName();
                                    errs() << "called function name: " << callee_name << "\n";
                                    if (callee_name == "fopen") {
                                        TinyPtrVector<DbgDeclareInst *> DIs = FindDbgDeclareUses(callInst);
                                        // There is no debug usage for this case
                                        for (DbgDeclareInst *ddi: DIs) {
                                            dbgs() << "dbg inst: " << *ddi << '\n';
                                            errs() << ddi->getVariable()->getName() << "\n";
                                            const DebugLoc& dl = ddi->getDebugLoc();
                                            int line = dl.getLine();
                                            errs() << "\n\n" << "Line " << line << ": " << ddi->getVariable()->getName() << "\n\n";
                                        }
                                        errs() << "args of this callInst\n";
                                        for (Use& args: callInst->data_ops()) {
                                            errs() << *args << "\n";
                                        }
                                        // errs() << "called func: " << callInst->getCalledFunction()->getOperand(0) << "\n";
                                        // errs() << "arguments \n";
                                        // for (Argument& it: called_function->args()) {
                                        //     errs() << it.dump() << "\n";
                                        // }
                                    }
                                }

                                // Use the load inst of <store, load> chain to find out the seminal input
                                if (LoadInst* loadInst = dyn_cast<LoadInst>(curr_inst)) {
                                    errs() << "LoadInst: " << *loadInst << "\n";
                                    errs() << "users of this load instruction: \n";
                                    for (User* user: curr_inst->users()) {
                                        errs() << *user << "\n";
                                        Instruction* user_inst = dyn_cast<Instruction>(user);
                                        errs() << *user_inst << "\n";
                                        // if (auto *CI = dyn_cast<CallInst>(user_inst)) {
                                        //     StringRef callee_name = CI->getCalledFunction()->getName();
                                        //     errs() << "called function name: " << callee_name << "\n";
                                        //     if (callee_name == "__isoc99_scanf") {
                                        //         TinyPtrVector<DbgDeclareInst *> DIs = FindDbgDeclareUses(AI);
                                        //         for (DbgDeclareInst *ddi: DIs) {
                                        //             dbgs() << "dbg inst: " << *ddi << '\n';
                                        //             errs() << ddi->getVariable()->getName() << "\n";
                                        //             const DebugLoc& dl = ddi->getDebugLoc();
                                        //             int line = dl.getLine();
                                        //             errs() << "\n\n" << "Line " << line << ": " << ddi->getVariable()->getName() << "\n\n";
                                        //         }
                                                

                                        //     }
                                        // }
                                        worklist.push_back(user_inst);
                                    }
                                    errs() << "users of this load instruction end \n";
                                }

                                errs() << "Add the previous instructions to the worklist.\n";
                                for (Use& U: curr_inst->operands()) {
                                    errs() << "operands: " << *U << "\n";
                                    Value* v = U.get();
                                    
                                    if (Instruction* prev_inst = dyn_cast<Instruction>(v)) {
                                        errs() << "prev _inst " << " -> " << *prev_inst << "\n";
                                        worklist.push_back(prev_inst);
                                    }
                                }
                            }

                            errs() << "######################################################################################################\n";

                        }
                    }
                    if (auto* op = dyn_cast<ICmpInst>(&I)) {
                        errs() << "-----------------------------------------------------------------------------------------------------\n";
                        errs() << "ICmpInst instance" << "\n";
                        // CmpInst::Predicate pr = op->getSignedPredicate();
                        // errs() << pr << "\n";
                        for (Use& U: I.operands()) {
                            Value* v = U.get();
                            if (auto* inst = dyn_cast<Instruction>(v)) {
                                errs() << "\"" << *dyn_cast<Instruction>(v) << "\"" << " -> " << "\"" << I << "\"" << ";\n";
                                //errs() << inst->dump() << "\n";
                            }
                            // if (v->getName() != "") {
                            //     errs() << "\"" << v->getName() << "\"" << " -> " << "\"" << I << "\"" << ";\n";
                            //     errs() << "\"" << v->getName() << "\"" << " [ color = red ]\n";
                            // }
                            
                        }
                        errs() << "-----------------------------------------------------------------------------------------------------\n";
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
