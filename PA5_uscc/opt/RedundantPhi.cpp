#include "Graph.h"
#include "SSABuilder.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include <cstdint>
#include <set>

namespace llvm {
namespace uscc {
namespace opt {

class RedundantPhiRemoval : public FunctionPass {
public:
  static char ID;
  RedundantPhiRemoval() : FunctionPass(ID) {}
  virtual bool runOnFunction(llvm::Function &F) override;
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;

  // my added helpers
  void removeRedundantPhis(const std::set<PHINode*>& phis); // to remove not needed phi nodes
  void processSCC(const std::set<Value*>& strongConnections);  // 
  void replaceSCCByValue(const std::set<Value*>& strongConnections, Value* toReplaceWith); // replace all phi with value toReplaceWith and then clean up phis
  bool changeFlag;  // tracks if changes were made or not to help with runOnFunctoin
};



char RedundantPhiRemoval::ID = 0;

void RedundantPhiRemoval::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
  AU.setPreservesCFG();
}


bool RedundantPhiRemoval::runOnFunction(Function &F) {

  // PA5: Implement
  //
  // This is the entry point of the redudant phi removal pass.
  // Your implementation should start here.
  // Feel free to add functions to the RedundantPhiRemoval class.
  
  changeFlag = false; // no changes made yet
  std::set<PHINode*> phiSet;

  for(auto& bb : F){
    for(auto& node : bb.phis()){
      phiSet.insert(&node);
    }
  }

  removeRedundantPhis(phiSet);

  return changeFlag;
}

void RedundantPhiRemoval::removeRedundantPhis(const std::set<PHINode*>& phis){
  Graph graph;

  for(PHINode* phiNode : phis){
    GraphNode* graphNode = graph.getOrAddToGraph(phiNode);
    for(uint32_t i = 0; i < phiNode->getNumIncomingValues(); i++){
      auto operand = phiNode->getIncomingValue(i);
      if(isa<PHINode>(operand) && phis.count(cast<PHINode>(operand))){
        GraphNode* operandGraphNode = graph.getOrAddToGraph(operand);
        graphNode->addEdge(operandGraphNode);
      }
    }
  }

  auto strongConnections = graph.getSCCs();
  for(const auto& connection : strongConnections){
    processSCC(connection);
  }
}

void RedundantPhiRemoval::processSCC(const std::set<Value*>& strongConnections){
  std::set<PHINode*> inner;
  std::set<Value*> outerOps;

  for(Value* connection : strongConnections){
    PHINode* phi = cast<PHINode>(connection);
    bool isInner = true; // tracks inner loop

    for(uint32_t i = 0; i < phi->getNumIncomingValues(); i++){
      auto operand = phi->getIncomingValue(i);
      if(!strongConnections.count(operand)){
        outerOps.insert(operand);
        isInner = false;
      }
    }

    if(isInner){
      inner.insert(phi);
    }
  }
  if(outerOps.size() == 1){
    replaceSCCByValue(strongConnections, *outerOps.begin());
  }
  else if(outerOps.size() > 1){
    removeRedundantPhis(inner);
  }
}

void RedundantPhiRemoval::replaceSCCByValue(const std::set<Value*>& strongConnections, Value* toReplaceWith){
  for(Value* connection : strongConnections){
    PHINode* phiNode = cast<PHINode>(connection);
    phiNode->replaceAllUsesWith(toReplaceWith);
  }

  for(Value* connectoin : strongConnections){
    PHINode* phiNode = cast<PHINode>(connectoin);
    phiNode->eraseFromParent();
  }
  changeFlag = true;
}


} // namespace opt
} // namespace uscc

FunctionPass *createRedundantPhiRemovalPass() {
  return new uscc::opt::RedundantPhiRemoval();
}

} // namespace llvm

