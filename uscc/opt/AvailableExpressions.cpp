
/**
 * USC Compiler
 *
 * An iterative forward available expressions analysis.
 */

#include "Passes.h"
#include "AvailableExpressions.h"
#include "llvm/IR/CFG.h"

using namespace std;
using namespace llvm;

bool enableAE;

namespace {
// Helper for set intersection
std::set<uint32_t> intersect(const std::set<uint32_t>& A, const std::set<uint32_t>& B) {
    std::set<uint32_t> result;
    std::set_intersection(A.begin(), A.end(),
                          B.begin(), B.end(),
                          std::inserter(result, result.begin()));
    return result;
}

// Helper for set union (needed for transfer function)
std::set<uint32_t> set_union(const std::set<uint32_t>& A, const std::set<uint32_t>& B) {
    std::set<uint32_t> result = A;
    result.insert(B.begin(), B.end());
    return result;
}

// Helper for set difference (needed for transfer function)
std::set<uint32_t> set_difference(const std::set<uint32_t>& A, const std::set<uint32_t>& B) {
    std::set<uint32_t> result;
    std::set_difference(A.begin(), A.end(),
                        B.begin(), B.end(),
                        std::inserter(result, result.begin()));
    return result;
}

// Helper for comparing sets
bool areSetsEqual(const std::set<uint32_t>& A, const std::set<uint32_t>& B) {
    return A == B;
}

// Helper for finding the object an address is derived from
Value* baseObject(Value* ptr) {
    while (auto* gep = dyn_cast<GetElementPtrInst>(ptr)) {
        ptr = gep->getPointerOperand();
    }
    return ptr;
}

// Helper for deciding whether a write can reach a read
bool mayOverlap(Value* written, Value* read) {
    Value* a = baseObject(written);
    Value* b = baseObject(read);
    if (a == b) return true;
    return !isa<AllocaInst>(a) || !isa<AllocaInst>(b);
}

// Helper for deciding whether a callee can reach a location
bool escapesToCallee(Value* ptr) {
    if (auto* alloca = dyn_cast<AllocaInst>(baseObject(ptr))) {
        return alloca->getAllocatedType()->isArrayTy() ||
               alloca->getAllocatedType()->isPointerTy();
    }
    return true;
}

// Helper for computing post order
void computePostOrder(BasicBlock *entry, set<BasicBlock *> &visited, deque<BasicBlock *> &order) {
  visited.insert(entry);
  auto succItr = succ_begin(entry), end = succ_end(entry);
  for (; succItr != end; ++succItr) {
    if (!visited.count(*succItr))
      computePostOrder(*succItr, visited, order);
  }
  order.push_back(entry);
}
}

char AvailableExpressions::ID = 0;
INITIALIZE_PASS(AvailableExpressions, "ae", "Available Expressions Analysis", true, true)

AvailableExpressions::AvailableExpressions() : FunctionPass(ID) {
    initializeAvailableExpressionsPass(*PassRegistry::getPassRegistry());
}

FunctionPass *llvm::createAEPass() {
    return new AvailableExpressions();
}

// Helper for computing IN set query for the basic block BB
std::vector<Instruction*> AvailableExpressions::getInSet(BasicBlock* BB) {
    std::vector<Instruction*> result;

    // 1. Find the IN set (of IDs) for the requested block
    if (bb2In.find(BB) == bb2In.end()) {
        // This block has no IN set (e.g., unreachable)
        return result;
    }

    // Get the set of uint32_t IDs
    const std::set<uint32_t>& inSetIDs = bb2In.at(BB);

    // 2. Add the expressions computed within this block
    for (auto &Inst : *BB) {
        if (isa<BinaryOperator>(&Inst) || isa<CmpInst>(&Inst) ||
                isa<GetElementPtrInst>(&Inst)) {
            result.push_back(&Inst);
        }
    }

    // 3. Convert IDs back to Instruction*
    for (uint32_t id : inSetIDs) {
        // 4. Find Instruction* from id2Expr
        if (id2Expr[id] != nullptr) {
            // 5. Add to result vector
            result.push_back(id2Expr[id]);
        }
    }

    // 6. Return the vector of available instructions
    return result;
}

void AvailableExpressions::dumpAvailableExpressions(Function &F, unsigned iterationCount) {
    llvm::outs() << "********** Available Expressions Information **********\n";
    llvm::outs() << "********** Function: " << F.getName() << ", analysis iterates " << iterationCount << " times\n";

    for (auto &bb : F) {
        llvm::outs() << bb.getName() << ":\n";
        llvm::outs() << "  GEN:";
        for (uint32_t id : this->bb2Gen[&bb]) {
            if (id < this->id2Expr.size() && this->id2Expr[id]) {
                 llvm::outs() << " [" << id << ": " << *this->id2Expr[id] << "]";
            } else {
                 llvm::outs() << " [ID:" << id << "?]";
            }
        }
        llvm::outs() << "\n";

        llvm::outs() << "  KILL:";
        for (uint32_t id : this->bb2Kill[&bb]) {
            if (id < this->id2Expr.size() && this->id2Expr[id]) {
                 llvm::outs() << " [" << id << ": " << *this->id2Expr[id] << "]";
            } else {
                 llvm::outs() << " [ID:" << id << "?]";
            }
        }
        llvm::outs() << "\n";

        // Print IN set
        llvm::outs() << "  IN:";
        if (this->bb2In.count(&bb)) {
            for (uint32_t id : this->bb2In[&bb]) {
                if (id < this->id2Expr.size() && this->id2Expr[id]) {
                     llvm::outs() << " [" << id << ": " << *this->id2Expr[id] << "]";
                } else {
                     llvm::outs() << " [ID:" << id << "?]";
                }
            }
        }
        llvm::outs() << "\n";

        // Print OUT set
        llvm::outs() << "  OUT:";
         if (this->bb2Out.count(&bb)) {
            for (uint32_t id : this->bb2Out[&bb]) {
                 if (id < this->id2Expr.size() && this->id2Expr[id]) {
                     llvm::outs() << " [" << id << ": " << *this->id2Expr[id] << "]";
                } else {
                     llvm::outs() << " [ID:" << id << "?]";
                }
            }
        }
        llvm::outs() << "\n";
    }
    llvm::outs() << "\n";
}


bool AvailableExpressions::runOnFunction(Function &F) {
    if (F.empty())
        return false;

    // Step 1: Identify all expressions and create mappings
    // PA4: Implement

    // Step 2: Calculate GEN and KILL sets
    // PA4: Implement

    // Step 3: Initialize IN and OUT sets
    // PA4: Implement

    // Step 4: Worklist algorithm
    unsigned iterationCount = 0;
    // PA4: Implement

    if (enableAE) {
        dumpAvailableExpressions(F, iterationCount);
    }

    return false;
}

// Helper function to use for CSE
bool AvailableExpressions::isAvailableAfter(Instruction &expr, Instruction &point) {
    if (expr2Id.find(&expr) == expr2Id.end()) {
        return false; // Not an expression we are tracking
    }
    BasicBlock* bb = point.getParent();
    if (!bb || bb2In.find(bb) == bb2In.end()) {
        return false; // Should not happen
    }

    // PA4: Implement
    return false;
}
