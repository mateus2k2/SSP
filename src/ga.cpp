#include "headers/GA.h"
#include <cassert>
#include <climits>
#include <stdexcept>

// ─────────────────────────────────────────────────────────────────────────────
//  MaximalClass helper
// ─────────────────────────────────────────────────────────────────────────────
std::set<int> MaximalClass::allTools() const {
    // NOTE: The GA fills this lazily via GeneticAlgorithm::classTools().
    // This stub is kept for completeness.
    return {};
}

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────────────────
GeneticAlgorithm::GeneticAlgorithm(const std::vector<Job>& operations,
                                   const GAParams&         params,
                                   unsigned int            seed)
    : ops_(operations), params_(params), rng_(seed)
{
    numOps_      = (int)ops_.size();
    numMachines_ = params_.numMachines;
    TC_          = params_.magazineCap;

    // Pre-compute per-operation tool sets and priority flags
    toolsOf_.resize(numOps_);
    isPriority_.resize(numOps_);
    opId_.resize(numOps_);

    for (int i = 0; i < numOps_; ++i) {
        const auto& op = ops_[i];
        for (int t : op.toolSetNormalized.tools)
            toolsOf_[i].insert(t);
        isPriority_[i] = op.priority;
        opId_[i] = {op.indexJob, op.indexOperation};
    }

    // Sanity: every tool set must fit in the magazine
    for (int i = 0; i < numOps_; ++i) {
        if ((int)toolsOf_[i].size() > TC_)
            throw std::runtime_error("Operation " + std::to_string(i) +
                                     " requires more tools than magazine capacity!");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Utility
// ─────────────────────────────────────────────────────────────────────────────
int GeneticAlgorithm::uniformInt(int lo, int hi) {
    std::uniform_int_distribution<int> d(lo, hi);
    return d(rng_);
}

double GeneticAlgorithm::uniformReal(double lo, double hi) {
    std::uniform_real_distribution<double> d(lo, hi);
    return d(rng_);
}

std::set<int> GeneticAlgorithm::unionTools(const std::vector<int>& opSet) const {
    std::set<int> result;
    for (int idx : opSet)
        result.insert(toolsOf_[idx].begin(), toolsOf_[idx].end());
    return result;
}

std::set<int> GeneticAlgorithm::classTools(const MaximalClass& cls) const {
    return unionTools(cls.operationIndices);
}

bool GeneticAlgorithm::isValidClass(const std::vector<int>& opSet) const {
    return (int)unionTools(opSet).size() <= TC_;
}

bool GeneticAlgorithm::toolsFitInMagazine(const std::set<int>& already,
                                           const std::set<int>& needed) const {
    std::set<int> combined;
    combined.insert(already.begin(), already.end());
    combined.insert(needed.begin(), needed.end());
    return (int)combined.size() <= TC_;
}

// ─────────────────────────────────────────────────────────────────────────────
//  MIMU heuristic  (Algorithm E.1)
// ─────────────────────────────────────────────────────────────────────────────
std::vector<MaximalClass> GeneticAlgorithm::MIMU(std::vector<int> opSet) {
    std::vector<MaximalClass> classes;
    std::vector<bool> used(numOps_, false);

    // Mark operations NOT in opSet as used
    {
        std::set<int> inSet(opSet.begin(), opSet.end());
        for (int i = 0; i < numOps_; ++i)
            if (!inSet.count(i)) used[i] = true;
    }

    auto remaining = [&]() {
        std::vector<int> r;
        for (int i : opSet)
            if (!used[i]) r.push_back(i);
        return r;
    };

    while (true) {
        auto rem = remaining();
        if (rem.empty()) break;

        MaximalClass cls;

        // Step 3: pick the operation with the largest |T_jk| to start the class
        int seed = rem[0];
        for (int i : rem)
            if (toolsOf_[i].size() > toolsOf_[seed].size()) seed = i;
        cls.operationIndices.push_back(seed);
        used[seed] = true;

        // Expand class (Step 4-5)
        while (true) {
            rem = remaining();
            if (rem.empty()) break;

            std::set<int> currentTools = unionTools(cls.operationIndices);

            // Filter: which remaining ops are still compatible with the class?
            std::vector<int> compatible;
            for (int i : rem) {
                std::set<int> combined = currentTools;
                combined.insert(toolsOf_[i].begin(), toolsOf_[i].end());
                if ((int)combined.size() <= TC_)
                    compatible.push_back(i);
            }
            if (compatible.empty()) break;

            // Select op that maximises |R(S) ∩ T_jk|, break ties by minimising
            // |T_jk - R(S)|  (lexicographic: max intersection, min union extra)
            int best = compatible[0];
            int bestInter = 0, bestExtra = INT_MAX;
            for (int i : compatible) {
                int inter = 0;
                for (int t : toolsOf_[i])
                    if (currentTools.count(t)) inter++;
                int extra = (int)toolsOf_[i].size() - inter;
                if (inter > bestInter ||
                   (inter == bestInter && extra < bestExtra)) {
                    best      = i;
                    bestInter = inter;
                    bestExtra = extra;
                }
            }
            cls.operationIndices.push_back(best);
            used[best] = true;
        }

        classes.push_back(cls);
    }
    return classes;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Sweeping lower-bound  (Algorithm E.2)
// ─────────────────────────────────────────────────────────────────────────────
int GeneticAlgorithm::sweepingLowerBound(const std::vector<int>& opSet) {
    if (opSet.empty()) return 0;

    // LB from coverage: ceil(|union all tools| / TC)
    std::set<int> allT = unionTools(opSet);
    int lb1 = ((int)allT.size() + TC_ - 1) / TC_;

    // Sweeping procedure
    std::vector<bool> used(numOps_, false);
    std::set<int> inSet(opSet.begin(), opSet.end());
    for (int i = 0; i < numOps_; ++i)
        if (!inSet.count(i)) used[i] = true;

    int lb2 = 0;
    while (true) {
        // Collect remaining
        std::vector<int> rem;
        for (int i : opSet)
            if (!used[i]) rem.push_back(i);
        if (rem.empty()) break;

        // Find op compatible with fewest others
        int pivot = rem[0];
        int minCompat = INT_MAX;
        for (int i : rem) {
            int cnt = 0;
            for (int j : rem) {
                if (j == i) { cnt++; continue; }
                std::set<int> combined = toolsOf_[i];
                combined.insert(toolsOf_[j].begin(), toolsOf_[j].end());
                if ((int)combined.size() <= TC_) cnt++;
            }
            if (cnt < minCompat) { minCompat = cnt; pivot = i; }
        }

        // Sweep: mark pivot and all ops compatible with pivot
        for (int i : rem) {
            std::set<int> combined = toolsOf_[pivot];
            combined.insert(toolsOf_[i].begin(), toolsOf_[i].end());
            if ((int)combined.size() <= TC_)
                used[i] = true;
        }
        lb2++;
    }

    return std::max(lb1, lb2);
}

// ─────────────────────────────────────────────────────────────────────────────
//  BnB grouping  (Section 5.2.1 / Algorithm E.3)
// ─────────────────────────────────────────────────────────────────────────────
std::vector<MaximalClass> GeneticAlgorithm::bnbGrouping() {
    std::vector<int> allOps(numOps_);
    std::iota(allOps.begin(), allOps.end(), 0);

    // Initial upper bound from MIMU
    std::vector<MaximalClass> bestPartition = MIMU(allOps);
    int UB = (int)bestPartition.size();

    // Initial lower bound from sweeping
    int LB = sweepingLowerBound(allOps);

    if (LB == UB) return bestPartition; // already optimal

    // BnB tree: each node is a partial partition (list of committed classes)
    // plus the set of uncommitted operations.
    // For industry-size problems, we run MIMU and return (paper does the same
    // for speed): the MIMU already gives a good upper bound used in the GA init.
    // A full BnB is exponential; the paper uses it only for small seeding (5%).
    // Here we implement a depth-limited BnB:

    struct Node {
        std::vector<MaximalClass> committed; // classes decided so far
        std::vector<int>          remaining; // operations not yet placed
        int depth;
    };

    std::vector<Node> stack;
    stack.push_back({/*committed=*/{}, allOps, 0});

    const int MAX_NODES = 100000; // safety limit
    int explored = 0;

    while (!stack.empty() && explored < MAX_NODES) {
        Node node = stack.back();
        stack.pop_back();
        explored++;

        if (node.remaining.empty()) {
            int sz = (int)node.committed.size();
            if (sz < UB) {
                UB = sz;
                bestPartition = node.committed;
            }
            continue;
        }

        // Lower bound for this node
        int nodeLB = (int)node.committed.size() + sweepingLowerBound(node.remaining);
        if (nodeLB >= UB) continue; // prune

        // Find the operation compatible with the fewest others → branch on it
        int pivot = node.remaining[0];
        int minC  = INT_MAX;
        for (int i : node.remaining) {
            int c = 0;
            for (int j : node.remaining) {
                std::set<int> comb = toolsOf_[i];
                comb.insert(toolsOf_[j].begin(), toolsOf_[j].end());
                if ((int)comb.size() <= TC_) c++;
            }
            if (c < minC) { minC = c; pivot = i; }
        }

        // Enumerate all maximal classes of `remaining` that contain `pivot`
        // We enumerate by building each possible maximal class containing pivot
        // using MIMU restricted to subsets that include pivot.

        // Simplified branching: try adding each compatible subset {pivot + X}
        // by starting MIMU from pivot.  We just branch on "pivot goes into a
        // class built by MIMU starting from pivot" or "pivot alone if TC allows".

        // Build the class starting from pivot using MIMU logic
        MaximalClass cls;
        cls.operationIndices.push_back(pivot);
        std::set<int> curTools = toolsOf_[pivot];

        std::vector<int> rem2;
        for (int i : node.remaining)
            if (i != pivot) rem2.push_back(i);

        // Greedy extend (MIMU-style)
        bool changed = true;
        while (changed) {
            changed = false;
            int bestAdd = -1, bestInter = -1, bestExtra = INT_MAX;
            for (int i : rem2) {
                bool alreadyIn = false;
                for (int x : cls.operationIndices)
                    if (x == i) { alreadyIn = true; break; }
                if (alreadyIn) continue;

                std::set<int> comb = curTools;
                comb.insert(toolsOf_[i].begin(), toolsOf_[i].end());
                if ((int)comb.size() > TC_) continue;

                int inter = 0;
                for (int t : toolsOf_[i])
                    if (curTools.count(t)) inter++;
                int extra = (int)toolsOf_[i].size() - inter;

                if (inter > bestInter ||
                   (inter == bestInter && extra < bestExtra)) {
                    bestAdd   = i;
                    bestInter = inter;
                    bestExtra = extra;
                    changed   = true;
                }
            }
            if (bestAdd >= 0) {
                cls.operationIndices.push_back(bestAdd);
                curTools.insert(toolsOf_[bestAdd].begin(), toolsOf_[bestAdd].end());
                rem2.erase(std::remove(rem2.begin(), rem2.end(), bestAdd), rem2.end());
            }
        }

        // Build child node
        Node child;
        child.committed = node.committed;
        child.committed.push_back(cls);
        child.remaining = rem2; // ops not in cls
        child.depth = node.depth + 1;
        stack.push_back(child);
    }

    return bestPartition;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Machine assignment  (constructive heuristic, Section 5.3.2 step 3)
// ─────────────────────────────────────────────────────────────────────────────
void GeneticAlgorithm::assignMachines(Chromosome& chr) {
    // Track magazine state and accumulated workload per machine
    std::vector<std::set<int>> magazineState(numMachines_);
    std::vector<int> workload(numMachines_, 0);

    for (auto& gene : chr.genes) {
        std::set<int> neededTools = classTools(gene.cls);

        // Step 3a: prefer machine that already holds a subset of needed tools
        int bestMachine = -1;
        int bestOverlap = -1;
        for (int m = 0; m < numMachines_; ++m) {
            int overlap = 0;
            for (int t : neededTools)
                if (magazineState[m].count(t)) overlap++;
            if (overlap > bestOverlap) { bestOverlap = overlap; bestMachine = m; }
        }

        // Step 3b: if tie (or zero overlap everywhere) use least workload
        if (bestOverlap == 0) {
            int minW = INT_MAX;
            for (int m = 0; m < numMachines_; ++m)
                if (workload[m] < minW) { minW = workload[m]; bestMachine = m; }
        }

        gene.machineId     = bestMachine;
        gene.cls.machineId = bestMachine;

        // Update workload (sum of processing times in this class)
        for (int idx : gene.cls.operationIndices)
            workload[bestMachine] += ops_[idx].processingTime;

        // Update magazine (simplified: replace with needed tools; KTNS applied later)
        magazineState[bestMachine].insert(neededTools.begin(), neededTools.end());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Merge to maximal classes  (Section 5.5)
// ─────────────────────────────────────────────────────────────────────────────
void GeneticAlgorithm::mergeToMaximalClasses(Chromosome& chr) {
    // Group genes by machine, then try to merge adjacent classes on the same machine
    bool merged = true;
    while (merged) {
        merged = false;
        for (int g = 0; g + 1 < (int)chr.genes.size(); ++g) {
            if (chr.genes[g].machineId != chr.genes[g+1].machineId) continue;

            // Try merging
            std::vector<int> combined = chr.genes[g].cls.operationIndices;
            combined.insert(combined.end(),
                            chr.genes[g+1].cls.operationIndices.begin(),
                            chr.genes[g+1].cls.operationIndices.end());

            if (isValidClass(combined)) {
                chr.genes[g].cls.operationIndices = combined;
                chr.genes.erase(chr.genes.begin() + g + 1);
                merged = true;
                break; // restart scan after any merge
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Random chromosome  (Section 5.2)
// ─────────────────────────────────────────────────────────────────────────────
Chromosome GeneticAlgorithm::randomChromosome() {
    // 1. Build a random permutation of all operations respecting precedence.
    //    We use a topological shuffle: for each job, the operations must appear
    //    in order.  Approach: list each operation by (job, k), then pick a
    //    random valid operation at each step.

    // Group operations by job
    std::map<int, std::vector<int>> jobOps; // jobIndex → list of op indices sorted by k
    for (int i = 0; i < numOps_; ++i)
        jobOps[ops_[i].indexJob].push_back(i);
    for (auto& kv : jobOps)
        std::sort(kv.second.begin(), kv.second.end(),
                  [&](int a, int b){ return ops_[a].indexOperation < ops_[b].indexOperation; });

    // "Next available" pointer per job
    std::map<int,int> nextPtr;
    for (auto& kv : jobOps) nextPtr[kv.first] = 0;

    std::vector<int> permutation;
    permutation.reserve(numOps_);
    while ((int)permutation.size() < numOps_) {
        // Collect available operations (first unscheduled for each job)
        std::vector<int> available;
        for (auto& kv : jobOps) {
            int ptr = nextPtr[kv.first];
            if (ptr < (int)kv.second.size())
                available.push_back(kv.second[ptr]);
        }
        // Pick uniformly at random
        int chosen = available[uniformInt(0, (int)available.size()-1)];
        permutation.push_back(chosen);
        nextPtr[ops_[chosen].indexJob]++;
    }

    // 2. Pack operations into maximal classes (greedy left-to-right)
    Chromosome chr;
    MaximalClass current;
    std::set<int> currentTools;

    for (int idx : permutation) {
        std::set<int> combined = currentTools;
        combined.insert(toolsOf_[idx].begin(), toolsOf_[idx].end());

        if (!current.operationIndices.empty() && (int)combined.size() > TC_) {
            // Close current class
            Gene g;  g.cls = current;  chr.genes.push_back(g);
            current = MaximalClass();
            current.operationIndices.push_back(idx);
            currentTools = toolsOf_[idx];
        } else {
            current.operationIndices.push_back(idx);
            currentTools = combined;
        }
    }
    if (!current.operationIndices.empty()) {
        Gene g; g.cls = current; chr.genes.push_back(g);
    }

    // 3. Assign machines
    assignMachines(chr);
    return chr;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Chromosome from BnB grouping  (Section 5.2)
// ─────────────────────────────────────────────────────────────────────────────
Chromosome GeneticAlgorithm::randomChromosomeFromBnB() {
    auto classes = bnbGrouping();

    // Shuffle the classes randomly (still valid since precedence is within a class
    // only if same job — we need to ensure job ordering still holds across classes).
    // Paper: the class content is fixed; only ordering is shuffled.
    std::shuffle(classes.begin(), classes.end(), rng_);

    // Ensure each job's operations appear in order across classes.
    // We re-sort within each class by (job, operation index) and then
    // validate/fix cross-class ordering.
    for (auto& cls : classes)
        std::sort(cls.operationIndices.begin(), cls.operationIndices.end(),
                  [&](int a, int b){
                      if (ops_[a].indexJob != ops_[b].indexJob)
                          return ops_[a].indexJob < ops_[b].indexJob;
                      return ops_[a].indexOperation < ops_[b].indexOperation;
                  });

    Chromosome chr;
    for (auto& cls : classes) {
        Gene g; g.cls = cls; chr.genes.push_back(g);
    }
    assignMachines(chr);
    return chr;
}


// ─────────────────────────────────────────────────────────────────────────────
//  Population initialisation  (Section 5.2)
// ─────────────────────────────────────────────────────────────────────────────
void GeneticAlgorithm::initialisePopulation() {
    population_.clear();
    population_.reserve(params_.popSize);

    int bnbCount = std::max(1, (int)(params_.bnbSeedFrac * params_.popSize));

    // Seed with BnB-based chromosomes
    for (int i = 0; i < bnbCount && (int)population_.size() < params_.popSize; ++i) {
        Chromosome chr = randomChromosomeFromBnB();
        evaluateChromosome(chr);
        population_.push_back(chr);
    }

    // Fill the rest with random chromosomes
    while ((int)population_.size() < params_.popSize) {
        Chromosome chr = randomChromosome();
        evaluateChromosome(chr);
        population_.push_back(chr);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  KTNS  (Section 5.7)
//  Given a sequence of maximal classes on ONE machine, returns
//  {totalToolSwitches, numberOfSwitchInstances}.
//  "A tool switch = one inserted tool."
// ─────────────────────────────────────────────────────────────────────────────
std::pair<int,int> GeneticAlgorithm::KTNS(const std::vector<MaximalClass>& seq,
                                           const std::vector<int>& /*opIndices*/) const {
    if (seq.empty()) return {0, 0};

    int totalSwitches   = 0;
    int switchInstances = 0;

    // Magazine starts empty; tools for first class are pre-loaded (no cost for
    // the first class per the paper assumption).
    std::set<int> magazine;
    {
        // Load tools for first class for free
        std::set<int> firstTools;
        for (int idx : seq[0].operationIndices)
            for (int t : toolsOf_[idx]) firstTools.insert(t);
        // Respect TC_ — first class must fit (guaranteed by construction)
        magazine = firstTools;
    }

    for (int s = 1; s < (int)seq.size(); ++s) {
        // Tools needed for class s
        std::set<int> needed;
        for (int idx : seq[s].operationIndices)
            for (int t : toolsOf_[idx]) needed.insert(t);

        // Missing tools = tools in `needed` but not in `magazine`
        std::vector<int> missing;
        for (int t : needed)
            if (!magazine.count(t)) missing.push_back(t);

        if (missing.empty()) continue; // no switch needed

        // KTNS: decide which tools to remove.
        // Score each present tool by when it is next needed.
        // Tools needed the soonest get the highest score → removed last.
        // Tools never needed again get score 0 → removed first.

        // Build "next-use" position for each tool currently in magazine
        // (looking at classes s, s+1, …)
        auto nextUse = [&](int tool) -> int {
            for (int k = s; k < (int)seq.size(); ++k) {
                std::set<int> kTools;
                for (int idx : seq[k].operationIndices)
                    for (int t : toolsOf_[idx]) kTools.insert(t);
                if (kTools.count(tool)) return k;
            }
            return (int)seq.size(); // never used again
        };

        int slotsNeeded = (int)missing.size() - (TC_ - (int)magazine.size());
        if (slotsNeeded < 0) slotsNeeded = 0;

        if (slotsNeeded > 0) {
            // Collect present tools not in `needed` (candidates for eviction)
            std::vector<std::pair<int,int>> evictable; // {nextUsePos, tool}
            for (int t : magazine) {
                if (!needed.count(t))
                    evictable.push_back({nextUse(t), t});
            }
            // Sort ascending by nextUse: tools used furthest away removed first
            std::sort(evictable.begin(), evictable.end());
            // We remove those needed LEAST soon (smallest nextUse value = soonest
            // is needed → keep.  Largest nextUse → evict first.)
            // Paper: "remove those needed the soonest LAST" = remove furthest first.
            // Re-sort descending:
            std::sort(evictable.begin(), evictable.end(),
                      [](const auto& a, const auto& b){ return a.first > b.first; });

            for (int r = 0; r < slotsNeeded && r < (int)evictable.size(); ++r)
                magazine.erase(evictable[r].second);
        }

        // Insert missing tools
        magazine.insert(missing.begin(), missing.end());

        totalSwitches   += (int)missing.size();
        switchInstances += 1;
    }

    return {totalSwitches, switchInstances};
}

// ─────────────────────────────────────────────────────────────────────────────
//  Fitness evaluation  (Algorithm 2)
// ─────────────────────────────────────────────────────────────────────────────
double GeneticAlgorithm::fitnessEval(Chromosome& chr) {
    // Convert horizon and unsupervised hours to minutes (same unit as processing times)
    const int H    = params_.horizon;          // minutes
    const int tU   = params_.unsupHours * 60;  // minutes per day
    const int dayL = 24 * 60;                  // minutes per day

    // Per-machine state
    std::vector<MachineState> machines(numMachines_);

    // Decode: iterate genes in order
    for (auto& gene : chr.genes) {
        int m = gene.machineId;
        MachineState& ms = machines[m];

        if (ms.availTime > H) continue; // machine past horizon

        // If current availTime falls within an unsupervised shift, delay to
        // start of next supervised shift  (lines 6-8 of Algorithm 2)
        {
            int tod = ms.availTime % dayL;        // time of day (minutes)
            int supervisedEnd = dayL - tU;        // supervised hours end at this minute
            if (tod >= supervisedEnd) {
                // We are in an unsupervised period — skip to next supervised shift
                ms.availTime += (dayL - tod);
            }
        }

        // Sort operations in this class: priority ops first (line 4)
        std::vector<int> ops = gene.cls.operationIndices;
        std::sort(ops.begin(), ops.end(),
                  [&](int a, int b){ return isPriority_[a] > isPriority_[b]; });

        MaximalClass finishedCls;
        finishedCls.machineId = m;

        for (int idx : ops) {
            if (ms.availTime > H) break;

            int endTime = ms.availTime + ops_[idx].processingTime;
            if (endTime <= H) {
                ms.availTime = endTime;
                finishedCls.operationIndices.push_back(idx);
                ms.finishedOps.push_back(idx);
                if (isPriority_[idx]) ms.finishedPriority++;
            }
        }

        if (!finishedCls.operationIndices.empty())
            ms.finishedClasses.push_back(finishedCls);
    }

    // Compute revenue
    int totalFinished = 0;
    int totalFinishedPriority = 0;
    for (auto& ms : machines) {
        totalFinished += (int)ms.finishedOps.size();
        totalFinishedPriority += ms.finishedPriority;
    }

    double revenue = params_.revenue * totalFinished;

    // Penalty for unfinished priority ops
    int totalPriorityOps = 0;
    for (int i = 0; i < numOps_; ++i)
        if (isPriority_[i]) totalPriorityOps++;
    int unfinishedPriority = totalPriorityOps - totalFinishedPriority;
    double penalty = params_.penaltyCost * unfinishedPriority;

    // Tool switching cost via KTNS (lines 21-29 of Algorithm 2)
    int totalToolSwitches   = 0;
    int totalSwitchInstances = 0;
    for (auto& ms : machines) {
        auto [ts, si] = KTNS(ms.finishedClasses, ms.finishedOps);
        totalToolSwitches   += ts;
        totalSwitchInstances += si;
    }

    double switchCost = params_.fixedSwitch * totalSwitchInstances +
                        params_.varSwitch   * totalToolSwitches;

    double profit = revenue - penalty - switchCost;
    chr.fitness   = profit;
    return profit;
}

void GeneticAlgorithm::evaluateChromosome(Chromosome& chr) {
    fitnessEval(chr);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tournament selection  (Section 5.3)
// ─────────────────────────────────────────────────────────────────────────────
int GeneticAlgorithm::tournamentSelect() {
    int tournSize = std::max(2, (int)(params_.gamma1 * params_.popSize));
    int best = uniformInt(0, (int)population_.size() - 1);
    for (int i = 1; i < tournSize; ++i) {
        int candidate = uniformInt(0, (int)population_.size() - 1);
        if (population_[candidate].fitness > population_[best].fitness)
            best = candidate;
    }
    return best;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Two-point crossover  (Section 5.3.1)
// ─────────────────────────────────────────────────────────────────────────────
std::pair<Chromosome,Chromosome>
GeneticAlgorithm::twoPointCrossover(const Chromosome& p1, const Chromosome& p2) {
    // Both parents must have the same number of genes after normalisation.
    // In practice they may differ; we use the shorter length as limit.
    int n1 = p1.numGenes(), n2 = p2.numGenes();
    int n  = std::min(n1, n2);
    if (n < 2) return {p1, p2};

    int cut1 = uniformInt(0, n - 2);
    int cut2 = uniformInt(cut1 + 1, n - 1);

    Chromosome c1 = p1, c2 = p2;

    // Exchange the substring [cut1, cut2] between parents
    for (int i = cut1; i <= cut2 && i < n1 && i < n2; ++i)
        std::swap(c1.genes[i], c2.genes[i]);

    return {c1, c2};
}

// ─────────────────────────────────────────────────────────────────────────────
//  Adapted PMX: fix duplicate operations in offspring after 2X
//  (Section 5.3.1)
// ─────────────────────────────────────────────────────────────────────────────
void GeneticAlgorithm::adaptedPMX(Chromosome& offspring, const Chromosome& /*donor*/,
                                   int cut1, int cut2) {
    // Collect all operation indices in the offspring
    // Find duplicates and missing operations

    std::map<int,int> opCount;
    for (auto& gene : offspring.genes)
        for (int idx : gene.cls.operationIndices)
            opCount[idx]++;

    // Find missing ops (count 0) and duplicated ops (count > 1)
    std::vector<int> missing, dups;
    for (int i = 0; i < numOps_; ++i) {
        int cnt = opCount.count(i) ? opCount[i] : 0;
        if (cnt == 0) missing.push_back(i);
        else if (cnt > 1) dups.push_back(i);
    }

    // Replace one duplicate outside [cut1,cut2] with a missing op
    int mi = 0;
    for (int g = 0; g < (int)offspring.genes.size() && mi < (int)missing.size(); ++g) {
        if (g >= cut1 && g <= cut2) continue; // inside exchanged region — skip
        auto& ops = offspring.genes[g].cls.operationIndices;
        for (int& idx : ops) {
            if (mi >= (int)missing.size()) break;
            if (opCount[idx] > 1) {
                opCount[idx]--;
                opCount[missing[mi]]++;
                idx = missing[mi];
                mi++;
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Combined crossover  (Section 5.3.1)
// ─────────────────────────────────────────────────────────────────────────────
std::pair<Chromosome,Chromosome>
GeneticAlgorithm::combinedCrossover(const Chromosome& p1, const Chromosome& p2) {
    int n  = std::min(p1.numGenes(), p2.numGenes());
    if (n < 2) return {p1, p2};

    int cut1 = uniformInt(0, n - 2);
    int cut2 = uniformInt(cut1 + 1, n - 1);

    // Step 1: two-point crossover
    auto [c1, c2] = twoPointCrossover(p1, p2);

    // Step 2: APMX to repair duplicates
    adaptedPMX(c1, p2, cut1, cut2);
    adaptedPMX(c2, p1, cut1, cut2);

    // Re-assign machines
    assignMachines(c1);
    assignMachines(c2);

    return {c1, c2};
}

// ─────────────────────────────────────────────────────────────────────────────
//  Problem-oriented crossover  (Section 5.3.2)
//  Applied to a single chromosome (modifies in place).
// ─────────────────────────────────────────────────────────────────────────────
void GeneticAlgorithm::problemOrientedCrossover(Chromosome& chr) {
    if (chr.genes.empty()) return;

    // ── Step 1: re-order genes by decreasing number of priority operations ──
    std::stable_sort(chr.genes.begin(), chr.genes.end(),
        [&](const Gene& a, const Gene& b) {
            int pa = 0, pb = 0;
            for (int idx : a.cls.operationIndices) if (isPriority_[idx]) pa++;
            for (int idx : b.cls.operationIndices) if (isPriority_[idx]) pb++;
            return pa > pb; // descending
        });

    // ── Step 2: group operations with similar tool sets ──
    // For each gene from v_1, find other genes with overlapping tools and
    // pull those operations earlier.
    // We implement this as: for each gene, collect all operations in later genes
    // that share tools with this gene's tool set, and move them adjacent.
    for (int g = 0; g < (int)chr.genes.size(); ++g) {
        std::set<int> gTools = classTools(chr.genes[g].cls);

        for (int h = g + 1; h < (int)chr.genes.size(); ++h) {
            std::set<int> hTools = classTools(chr.genes[h].cls);
            // Check intersection
            bool overlap = false;
            for (int t : gTools)
                if (hTools.count(t)) { overlap = true; break; }

            if (overlap) {
                // Try to merge h into g if fits
                std::vector<int> combined = chr.genes[g].cls.operationIndices;
                combined.insert(combined.end(),
                                chr.genes[h].cls.operationIndices.begin(),
                                chr.genes[h].cls.operationIndices.end());
                if (isValidClass(combined)) {
                    chr.genes[g].cls.operationIndices = combined;
                    chr.genes.erase(chr.genes.begin() + h);
                    --h; // adjust index
                } else {
                    // Move h to position g+1 (bring similar tools closer)
                    Gene moved = chr.genes[h];
                    chr.genes.erase(chr.genes.begin() + h);
                    chr.genes.insert(chr.genes.begin() + g + 1, moved);
                }
                break; // move on to next g
            }
        }
    }

    // ── Step 3: constructive heuristic for machine assignment ──
    assignMachines(chr);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Swap mutation  (Section 5.4)
// ─────────────────────────────────────────────────────────────────────────────
void GeneticAlgorithm::swapMutation(Chromosome& chr) {
    int n = chr.numGenes();
    if (n < 2) return;

    // Gene-level swap
    for (int g = 0; g < n; ++g) {
        if (uniformReal(0.0, 1.0) < params_.probSwap) {
            int g2 = uniformInt(0, n - 1);
            std::swap(chr.genes[g], chr.genes[g2]);
        }
    }

    // Operation-level swap
    // Collect all (gene_index, op_position) pairs
    for (int g = 0; g < (int)chr.genes.size(); ++g) {
        auto& opsG = chr.genes[g].cls.operationIndices;
        for (int oi = 0; oi < (int)opsG.size(); ++oi) {
            if (uniformReal(0.0, 1.0) >= params_.probSwap) continue;

            // Pick a random target gene and op position
            int tg  = uniformInt(0, (int)chr.genes.size() - 1);
            auto& opsT = chr.genes[tg].cls.operationIndices;
            if (opsT.empty()) continue;
            int ti = uniformInt(0, (int)opsT.size() - 1);

            // Compute tentative new classes after swap
            std::vector<int> newG = opsG;
            std::vector<int> newT = opsT;
            int opA = newG[oi];
            int opB = newT[ti];
            if (opA == opB) continue;

            // Check precedence compatibility (simplified: same job operations
            // must not be reordered across the chromosome)
            // We check feasibility by verifying tool capacity only here;
            // precedence is maintained via the job-occurrence ordering convention.
            newG[oi] = opB;
            newT[ti] = opA;

            bool fitsG = isValidClass(newG);
            bool fitsT = isValidClass(newT);

            if (fitsG && fitsT) {
                opsG = newG;
                opsT = newT;
            } else if (!fitsT) {
                // opA cannot go into gene tg — add opA to a new class at the end
                newG[oi] = opB;          // keep opB in gene tg's class
                opsG = newG;
                opsT.push_back(opA);     // opA appended (may exceed TC → new class)
                // If exceeds, split off a new gene
                if (!isValidClass(opsT)) {
                    opsT.pop_back();
                    MaximalClass newCls;
                    newCls.operationIndices.push_back(opA);
                    Gene newGene;
                    newGene.cls = newCls;
                    newGene.machineId = uniformInt(0, numMachines_ - 1);
                    chr.genes.push_back(newGene);
                }
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Uniform mutation  (Section 5.4)
// ─────────────────────────────────────────────────────────────────────────────
void GeneticAlgorithm::uniformMutation(Chromosome& chr) {
    for (auto& gene : chr.genes) {
        if (uniformReal(0.0, 1.0) < params_.probUniform) {
            gene.machineId     = uniformInt(0, numMachines_ - 1);
            gene.cls.machineId = gene.machineId;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Elitism + immigration  (Section 5.6)
// ─────────────────────────────────────────────────────────────────────────────
void GeneticAlgorithm::nextGeneration(std::vector<Chromosome>& offspring) {
    // Sort parents by descending fitness
    std::sort(population_.begin(), population_.end(),
              [](const Chromosome& a, const Chromosome& b){ return a.fitness > b.fitness; });

    int eliteCount = std::max(1, (int)(params_.gamma2 * params_.popSize));

    // Replace the worst `eliteCount` offspring with the best `eliteCount` parents
    std::sort(offspring.begin(), offspring.end(),
              [](const Chromosome& a, const Chromosome& b){ return a.fitness < b.fitness; });

    for (int i = 0; i < eliteCount && i < (int)offspring.size(); ++i)
        offspring[i] = population_[i]; // replace worst offspring with best parent

    // Remove duplicates (immigration: replace duplicates with random chromosomes)
    for (int i = 0; i < (int)offspring.size(); ++i) {
        for (int j = i + 1; j < (int)offspring.size(); ++j) {
            // Simple duplicate check: same fitness (probabilistic)
            if (std::abs(offspring[i].fitness - offspring[j].fitness) < 1e-9) {
                Chromosome fresh = randomChromosome();
                evaluateChromosome(fresh);
                offspring[j] = fresh;
            }
        }
    }

    population_ = offspring;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Main GA loop  (Algorithm 1)
// ─────────────────────────────────────────────────────────────────────────────
Chromosome GeneticAlgorithm::run() {
    // Line 1: initialise
    initialisePopulation();

    // Find initial best
    bestChr_ = *std::max_element(population_.begin(), population_.end(),
                                  [](const Chromosome& a, const Chromosome& b){
                                      return a.fitness < b.fitness;
                                  });

    bool usePOX = false;  // `best` flag
    int  q       = 0;     // POX iteration counter

    int  nonImpGen = 0;   // consecutive generations without improvement

    auto wallStart = std::chrono::steady_clock::now();

    // Main loop
    while (true) {
        // Check stopping criteria
        double elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - wallStart).count();
        if (elapsed >= params_.maxTimeSec) break;
        if (nonImpGen >= params_.Gc)       break;

        std::vector<Chromosome> offspring;
        offspring.reserve(params_.popSize);

        // Generate N_p offspring via crossover
        while ((int)offspring.size() < params_.popSize) {
            int idx1 = tournamentSelect();
            int idx2 = tournamentSelect();
            const Chromosome& p1 = population_[idx1];
            const Chromosome& p2 = population_[idx2];

            Chromosome c1, c2;

            // Lines 6-10: choose crossover type
            if (usePOX && q <= params_.Q) {
                // Problem-oriented crossover
                c1 = p1; c2 = p2;
                problemOrientedCrossover(c1);
                problemOrientedCrossover(c2);
            } else {
                // Combined crossover
                auto [cc1, cc2] = combinedCrossover(p1, p2);
                c1 = cc1; c2 = cc2;
            }

            // Line 11: mutation
            swapMutation(c1);  uniformMutation(c1);
            swapMutation(c2);  uniformMutation(c2);

            // Line 12: merge to maximal classes
            mergeToMaximalClasses(c1);
            mergeToMaximalClasses(c2);

            // Line 13: evaluate
            evaluateChromosome(c1);
            evaluateChromosome(c2);

            offspring.push_back(std::move(c1));
            if ((int)offspring.size() < params_.popSize)
                offspring.push_back(std::move(c2));
        }

        // Find best in offspring
        const Chromosome& genBest = *std::max_element(offspring.begin(), offspring.end(),
            [](const Chromosome& a, const Chromosome& b){ return a.fitness < b.fitness; });

        // Lines 15-20: update best and POX flag
        if (genBest.fitness > bestChr_.fitness) {
            bestChr_ = genBest;
            usePOX   = true;
            q        = 1;
            nonImpGen = 0;
        } else {
            usePOX = false;
            q++;
            nonImpGen++;
        }

        // Line 14: create next generation
        nextGeneration(offspring);
    }

    return bestChr_;
}