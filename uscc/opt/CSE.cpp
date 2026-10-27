/**
 * USCC Compiler
 *
 * A client of Available Expressions Analysis to perform
 * Common Subexpression Elimination.
 */

#include "Passes.h"
#include "AvailableExpressions.h"

using namespace llvm;
using namespace std;

bool enableCSE;

namespace llvm {
void initializeCommonSubexpressionEliminationPass(PassRegistry&);
}
namespace  {
bool sameOperand(Value* Op1, Value* Op2) {
    if (Op1 == Op2) return true; // Operands are the same Value*

    LoadInst* LI1 = dyn_cast<LoadInst>(Op1);
    LoadInst* LI2 = dyn_cast<LoadInst>(Op2);
    Constant* C1 = dyn_cast<Constant>(Op1);
    Constant* C2 = dyn_cast<Constant>(Op2);

    if (LI1 && LI2) {
        // Both are loads, compare the pointer operands
        return LI1->getPointerOperand() == LI2->getPointerOperand();
    }
    if (C1 && C2) {
        // Both are constants, compare their values
        return C1 == C2;
    }
    // Operands are different Value* and not comparable loads/constants
    return false;
}

bool Search(Instruction* Inst1, Instruction* Inst2) {
    if (!Inst1 || !Inst2) return false;
    if (Inst1->getOpcode() != Inst2->getOpcode()) return false;
    if (Inst1->getNumOperands() != Inst2->getNumOperands()) return false;
    if (Inst1->getType() != Inst2->getType()) return false; // Ensure result types match

    bool inOrder = true;
    for (unsigned i = 0; i < Inst1->getNumOperands(); ++i) {
        if (!sameOperand(Inst1->getOperand(i), Inst2->getOperand(i))) {
            inOrder = false;
            break;
        }
    }

    bool crossed = Inst1->getNumOperands() == 2 &&
                   sameOperand(Inst1->getOperand(0), Inst2->getOperand(1)) &&
                   sameOperand(Inst1->getOperand(1), Inst2->getOperand(0));

    CmpInst* Cmp1 = dyn_cast<CmpInst>(Inst1);
    CmpInst* Cmp2 = dyn_cast<CmpInst>(Inst2);
    if (Cmp1 && Cmp2) {
        // Ensure comparison predicates match
        if (inOrder && Cmp1->getPredicate() == Cmp2->getPredicate()) return true;
        return crossed && Cmp1->getPredicate() == Cmp2->getSwappedPredicate();
    }

    if (inOrder) return true;

    return crossed && Inst1->isCommutative();
}

}


namespace {
class CommonSubexpressionElimination : public FunctionPass {
public:
    static char ID;
    CommonSubexpressionElimination() : FunctionPass(ID) {
        initializeCommonSubexpressionEliminationPass(*PassRegistry::getPassRegistry());
    }
    bool runOnFunction(llvm::Function &F) override;
    void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
};
}

char CommonSubexpressionElimination::ID = 0;
INITIALIZE_PASS(CommonSubexpressionElimination, "cse", "Common Subexpression Elimination", false, false)

void CommonSubexpressionElimination::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
    // This pass is a client of Available Expressions Analysis.
    AU.addRequired<AvailableExpressions>();
    AU.setPreservesCFG();
}

FunctionPass *llvm::createCSEPass() {
    return new CommonSubexpressionElimination();
}

bool CommonSubexpressionElimination::runOnFunction(llvm::Function &F) {
    if (F.empty())
        return false;

    AvailableExpressions &ae = getAnalysis<AvailableExpressions>();
    bool globalChanged = false;
    // PA4: Implement

    return globalChanged;
}
