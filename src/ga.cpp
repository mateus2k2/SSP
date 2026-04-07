#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <set>
#include <unordered_map>
#include <vector>

#include "headers/GA.h"

#define INT_MAX 2147483647

using namespace std;
using Clock = chrono::steady_clock;

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
GeneticAlgorithm::GeneticAlgorithm(const vector<Job>& groupedJobs,
                                   const GAParams&    params,
                                   EvalFn             evalFn,
                                   unsigned           seed)
    : jobs_(groupedJobs),
      params_(params),
      eval_(move(evalFn)),
      rng_(seed),
      n_((int)groupedJobs.size())
{}

// ─────────────────────────────────────────────────────────────────────────────
// Initialisation helpers
// ─────────────────────────────────────────────────────────────────────────────

Chromosome GeneticAlgorithm::makeRandom() {
    Chromosome c;
    c.perm.resize(n_);
    iota(c.perm.begin(), c.perm.end(), 0);
    shuffle(c.perm.begin(), c.perm.end(), rng_);
    c.fitness = eval_(c.perm);
    return c;
}

Chromosome GeneticAlgorithm::makePriorityFirst() {
    Chromosome c;
    vector<int> prio, nonPrio;
    for (int i = 0; i < n_; ++i) {
        if (jobs_[i].priority) prio.push_back(i);
        else                   nonPrio.push_back(i);
    }
    shuffle(prio.begin(),    prio.end(),    rng_);
    shuffle(nonPrio.begin(), nonPrio.end(), rng_);
    c.perm.insert(c.perm.end(), prio.begin(),    prio.end());
    c.perm.insert(c.perm.end(), nonPrio.begin(), nonPrio.end());
    c.fitness = eval_(c.perm);
    return c;
}

// ─────────────────────────────────────────────────────────────────────────────
// BnB initialisation helpers  (paper Section 4.1, BnB Grouping Method)
// ─────────────────────────────────────────────────────────────────────────────

// Returns the union of all tool indices required by grouped job i.
vector<int> GeneticAlgorithm::jobTools(int i) const {
    set<int> tools;
    const Job& j = jobs_[i];
    if (j.isGrouped && !j.toolSets.empty()) {
        for (const auto& ts : j.toolSets)
            for (int t : ts.tools) tools.insert(t);
    } else {
        for (int t : j.toolSetNormalized.tools) tools.insert(t);
    }
    return vector<int>(tools.begin(), tools.end());
}

// MIMU — Maximal Intersection, Minimal Union.
// Builds a maximal-class partition greedily: at each step pick the unassigned
// job that (1) maximises tool intersection with the current class, then
// (2) minimises the number of new tools added.  Produces upper bound U*.
vector<vector<int>> GeneticAlgorithm::runMIMU() {
    vector<bool> assigned(n_, false);
    vector<vector<int>> partition;

    // Precompute tool sets
    vector<vector<int>> tools(n_);
    for (int i = 0; i < n_; ++i) tools[i] = jobTools(i);

    while (true) {
        // Seed: first unassigned job
        int seed = -1;
        for (int i = 0; i < n_; ++i)
            if (!assigned[i]) { seed = i; break; }
        if (seed == -1) break;

        vector<int> cls = {seed};
        assigned[seed] = true;
        set<int> clsTools(tools[seed].begin(), tools[seed].end());

        // Greedily expand the class
        bool expanded = true;
        while (expanded) {
            expanded      = false;
            int bestJ     = -1;
            int bestInter = -1;
            int bestNewT  = INT_MAX;

            for (int j = 0; j < n_; ++j) {
                if (assigned[j]) continue;
                int inter   = 0;
                int newTools = 0;
                for (int t : tools[j]) {
                    if (clsTools.count(t)) ++inter;
                    else                   ++newTools;
                }
                // Feasibility: total tools must fit in magazine
                if ((int)(clsTools.size() + newTools) > params_.magazineCap) continue;

                // Prefer max intersection, break ties by min new tools
                if (inter > bestInter ||
                    (inter == bestInter && newTools < bestNewT)) {
                    bestInter = inter;
                    bestNewT  = newTools;
                    bestJ     = j;
                }
            }

            if (bestJ != -1) {
                cls.push_back(bestJ);
                assigned[bestJ] = true;
                for (int t : tools[bestJ]) clsTools.insert(t);
                expanded = true;
            }
        }
        partition.push_back(move(cls));
    }
    return partition;
}

// Sweeping — builds a partition starting from the most-constrained job.
// At each step, pick the unassigned job with the fewest compatible unassigned
// peers, then sweep all compatible jobs into its class.  Produces lower bound L*.
vector<vector<int>> GeneticAlgorithm::runSweeping() {
    // Precompute tool sets and pairwise compatibility
    vector<vector<int>> tools(n_);
    for (int i = 0; i < n_; ++i) tools[i] = jobTools(i);

    // compat[i][j] = true if jobs i and j can share a maximal class
    vector<vector<bool>> compat(n_, vector<bool>(n_, false));
    for (int i = 0; i < n_; ++i) {
        for (int j = i; j < n_; ++j) {
            set<int> u(tools[i].begin(), tools[i].end());
            for (int t : tools[j]) u.insert(t);
            bool ok = (int)u.size() <= params_.magazineCap;
            compat[i][j] = compat[j][i] = ok;
        }
    }

    vector<bool> assigned(n_, false);
    vector<vector<int>> partition;

    while (true) {
        // Find unassigned job with fewest compatible unassigned peers
        int seed    = -1;
        int minComp = INT_MAX;
        for (int i = 0; i < n_; ++i) {
            if (assigned[i]) continue;
            int cnt = 0;
            for (int j = 0; j < n_; ++j)
                if (!assigned[j] && compat[i][j]) ++cnt;
            if (cnt < minComp) { minComp = cnt; seed = i; }
        }
        if (seed == -1) break;

        // Sweep seed + all compatible unassigned jobs that still fit
        vector<int> cls = {seed};
        assigned[seed] = true;
        set<int> clsTools(tools[seed].begin(), tools[seed].end());

        for (int j = 0; j < n_; ++j) {
            if (assigned[j] || !compat[seed][j]) continue;
            // Check combined fit
            set<int> candidate = clsTools;
            for (int t : tools[j]) candidate.insert(t);
            if ((int)candidate.size() <= params_.magazineCap) {
                cls.push_back(j);
                assigned[j] = true;
                clsTools = move(candidate);
            }
        }
        partition.push_back(move(cls));
    }
    return partition;
}

// BnB — terminates when U* == L*.  For instances where MIMU already equals
// Sweeping (the common case, as in Example 4), this returns immediately.
// A lightweight branch step is used when a gap remains; the search depth is
// bounded to keep init time tractable.
vector<vector<int>> GeneticAlgorithm::runBnB() {
    auto mimu     = runMIMU();
    auto sweeping = runSweeping();
    int  U        = (int)mimu.size();
    int  L        = (int)sweeping.size();

    if (U == L) return mimu;   // optimal found at root — Example 4 case

    // Gap exists: simple branch-and-bound.
    // We branch by trying each unassigned job as the seed of the next class,
    // pruning when depth + remaining lower-bound estimate ≥ current U*.
    // Keep the best (fewest classes) partition found.
    vector<vector<int>> best = mimu;

    // Precompute tool sets
    vector<vector<int>> tools(n_);
    for (int i = 0; i < n_; ++i) tools[i] = jobTools(i);

    // Stack entry: (partial partition so far, assigned bitmask)
    using State = pair<vector<vector<int>>, vector<bool>>;
    vector<State> stack;
    stack.push_back({{}, vector<bool>(n_, false)});

    while (!stack.empty()) {
        auto [partial, assigned] = move(stack.back());
        stack.pop_back();

        // Count remaining unassigned
        int remaining = 0;
        for (int i = 0; i < n_; ++i) if (!assigned[i]) ++remaining;

        if (remaining == 0) {
            if ((int)partial.size() < (int)best.size())
                best = partial;
            continue;
        }

        // Lower-bound estimate: at least ⌈remaining_tools / TC⌉ more classes
        // Use a simple sweep estimate: count distinct tools still needed
        set<int> remainingTools;
        for (int i = 0; i < n_; ++i) {
            if (!assigned[i])
                for (int t : tools[i]) remainingTools.insert(t);
        }
        int lbExtra = max(1, (int)((remainingTools.size() + params_.magazineCap - 1) / params_.magazineCap));
        if ((int)partial.size() + lbExtra >= (int)best.size())
            continue;  // prune

        // Branch: try each unassigned job as the seed of the next class
        for (int seed = 0; seed < n_; ++seed) {
            if (assigned[seed]) continue;

            // Build a MIMU-style class starting from this seed
            vector<bool>  newAssigned = assigned;
            set<int>      clsTools(tools[seed].begin(), tools[seed].end());
            vector<int>   cls = {seed};
            newAssigned[seed] = true;

            // Greedily fill (same MIMU criterion)
            bool expanded = true;
            while (expanded) {
                expanded      = false;
                int bestJ     = -1;
                int bestInter = -1;
                int bestNewT  = INT_MAX;
                for (int j = 0; j < n_; ++j) {
                    if (newAssigned[j]) continue;
                    int inter = 0, newT = 0;
                    for (int t : tools[j]) {
                        if (clsTools.count(t)) ++inter;
                        else                   ++newT;
                    }
                    if ((int)(clsTools.size() + newT) > params_.magazineCap) continue;
                    if (inter > bestInter || (inter == bestInter && newT < bestNewT)) {
                        bestInter = inter; bestNewT = newT; bestJ = j;
                    }
                }
                if (bestJ != -1) {
                    cls.push_back(bestJ);
                    newAssigned[bestJ] = true;
                    for (int t : tools[bestJ]) clsTools.insert(t);
                    expanded = true;
                }
            }

            vector<vector<int>> newPartial = partial;
            newPartial.push_back(move(cls));
            stack.push_back({move(newPartial), move(newAssigned)});

            // Only push one branch per level to keep stack size manageable;
            // Sweeping already gives a good lower bound.
            break;
        }
    }
    return best;
}

// Convert a partition to a Chromosome.  Classes are emitted in order;
// optionally shuffle jobs within each class for diversity.
Chromosome GeneticAlgorithm::partitionToChromosome(const vector<vector<int>>& partition,
                                                    bool shuffleWithin) {
    Chromosome c;
    c.perm.reserve(n_);
    for (auto cls : partition) {            // copy so we can shuffle
        if (shuffleWithin) shuffle(cls.begin(), cls.end(), rng_);
        for (int j : cls) c.perm.push_back(j);
    }
    c.fitness = eval_(c.perm);
    return c;
}

// 5 % BnB-seeded + 5 % priority-seeded + 90 % random  (paper Section 4.1)
void GeneticAlgorithm::initPopulation(vector<Chromosome>& pop) {
    // BnB partition (computed once; diversified via within-class shuffling)
    vector<vector<int>> bnbPartition = runBnB();
    int bnbSeeded  = max(1, Np_ / 20);   // 5 % from BnB
    int prioSeeded = max(1, Np_ / 20);   // 5 % priority-first

    // First BnB chromosome: classes in their natural order
    pop.push_back(partitionToChromosome(bnbPartition, /*shuffleWithin=*/false));

    // Remaining BnB chromosomes: shuffle within classes for diversity
    for (int i = 1; i < bnbSeeded; ++i)
        pop.push_back(partitionToChromosome(bnbPartition, /*shuffleWithin=*/true));

    for (int i = 0; i < prioSeeded; ++i)
        pop.push_back(makePriorityFirst());

    while ((int)pop.size() < Np_)
        pop.push_back(makeRandom());
}

// ─────────────────────────────────────────────────────────────────────────────
// Selection — tournament (size ST_)
// ─────────────────────────────────────────────────────────────────────────────

const Chromosome& GeneticAlgorithm::tournamentSelect(const vector<Chromosome>& pop) {
    int sz  = (int)pop.size();
    int cap = min(ST_, sz);
    uniform_int_distribution<int> dist(0, sz - 1);
    int best = dist(rng_);
    for (int i = 1; i < cap; ++i) {
        int idx = dist(rng_);
        if (pop[idx].fitness > pop[best].fitness)
            best = idx;
    }
    return pop[best];
}

// ─────────────────────────────────────────────────────────────────────────────
// Combined Crossover (CX): Two-Point Crossover + PMX repair
// Paper Section 4.2 (Two-Point + Adapted Partial-Mapped Crossover)
// ─────────────────────────────────────────────────────────────────────────────

// child[l..r] is already filled from segParent[l..r].
// Fill remaining positions from fillParent, following the PMX mapping chain.
void GeneticAlgorithm::pmxRepair(vector<int>&       child,
                                  const vector<int>& segParent,
                                  const vector<int>& fillParent,
                                  int l, int r) {
    // Build position map for the copied segment
    unordered_map<int, int> segPos;
    segPos.reserve(r - l + 1);
    for (int i = l; i <= r; ++i)
        segPos[segParent[i]] = i;

    for (int i = 0; i < n_; ++i) {
        if (i >= l && i <= r) continue;
        int val = fillParent[i];
        while (segPos.count(val))        // val is in the segment → follow chain
            val = fillParent[segPos[val]];
        child[i] = val;
    }
}

pair<Chromosome, Chromosome>
GeneticAlgorithm::combinedCrossover(const Chromosome& p1, const Chromosome& p2) {
    if (n_ < 2) return {p1, p2};

    uniform_int_distribution<int> posDist(0, n_ - 1);
    int l = posDist(rng_);
    int r = posDist(rng_);
    if (l > r) swap(l, r);

    Chromosome c1, c2;
    c1.perm.resize(n_, -1);
    c2.perm.resize(n_, -1);

    for (int i = l; i <= r; ++i) {
        c1.perm[i] = p1.perm[i];
        c2.perm[i] = p2.perm[i];
    }

    pmxRepair(c1.perm, p1.perm, p2.perm, l, r);
    pmxRepair(c2.perm, p2.perm, p1.perm, l, r);

    return {c1, c2};
}

// ─────────────────────────────────────────────────────────────────────────────
// Problem-Oriented Crossover (POX) — Paper Section 4.3
//
// Step 1: Priority jobs first (stable sort preserves relative order).
// Step 2: Greedy nearest-neighbour tool-similarity ordering of the rest.
// Step 3: Machine allocation handled implicitly by the KTNS evaluator.
// ─────────────────────────────────────────────────────────────────────────────

int GeneticAlgorithm::toolOverlap(int i, int j) const {
    set<int> toolsI;
    const Job& ji = jobs_[i];
    if (ji.isGrouped && !ji.toolSets.empty()) {
        for (const auto& ts : ji.toolSets)
            for (int t : ts.tools) toolsI.insert(t);
    } else {
        for (int t : ji.toolSetNormalized.tools) toolsI.insert(t);
    }

    int overlap = 0;
    const Job& jj = jobs_[j];
    if (jj.isGrouped && !jj.toolSets.empty()) {
        for (const auto& ts : jj.toolSets)
            for (int t : ts.tools)
                if (toolsI.count(t)) ++overlap;
    } else {
        for (int t : jj.toolSetNormalized.tools)
            if (toolsI.count(t)) ++overlap;
    }
    return overlap;
}

Chromosome GeneticAlgorithm::problemOrientedCrossover(const Chromosome& c) {
    Chromosome result = c;
    vector<int>& perm = result.perm;

    // Step 1: priority jobs first
    stable_sort(perm.begin(), perm.end(), [&](int a, int b) {
        return (int)jobs_[a].priority > (int)jobs_[b].priority;
    });

    // Step 2: greedy tool-similarity ordering for non-priority jobs
    int boundary = 0;
    while (boundary < n_ && jobs_[perm[boundary]].priority) ++boundary;

    if (boundary < n_) {
        int np = n_ - boundary;
        vector<int>  pool(perm.begin() + boundary, perm.end());
        vector<bool> placed(np, false);

        vector<int> ordered;
        ordered.reserve(np);

        placed[0] = true;
        ordered.push_back(pool[0]);

        while ((int)ordered.size() < np) {
            int last     = ordered.back();
            int bestIdx  = -1;
            int bestOver = -1;

            for (int k = 0; k < np; ++k) {
                if (placed[k]) continue;
                int ov = toolOverlap(last, pool[k]);
                if (ov > bestOver) { bestOver = ov; bestIdx = k; }
            }
            if (bestIdx == -1) {  // fallback: first unplaced
                for (int k = 0; k < np; ++k)
                    if (!placed[k]) { bestIdx = k; break; }
            }
            placed[bestIdx] = true;
            ordered.push_back(pool[bestIdx]);
        }

        for (int i = 0; i < np; ++i)
            perm[boundary + i] = ordered[i];
    }

    result.fitness = eval_(result.perm);
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
// Mutation
// ─────────────────────────────────────────────────────────────────────────────

// Swap Mutation (SM): with probability Ps_ per locus, swap with random position.
void GeneticAlgorithm::swapMutation(Chromosome& c) {
    uniform_real_distribution<double> prob(0.0, 1.0);
    uniform_int_distribution<int>     pos(0, n_ - 1);
    for (int i = 0; i < n_; ++i) {
        if (prob(rng_) < Ps_)
            swap(c.perm[i], c.perm[pos(rng_)]);
    }
}

// Reinsertion / Uniform Mutation (UM): with probability Pu_ per locus, remove
// and re-insert at a random position.  Adapts the paper's machine-vector
// uniform mutation to our implicit-machine permutation encoding.
void GeneticAlgorithm::reinsertionMutation(Chromosome& c) {
    uniform_real_distribution<double> prob(0.0, 1.0);
    uniform_int_distribution<int>     pos(0, n_ - 1);
    for (int i = n_ - 1; i >= 0; --i) {   // backwards keeps later indices valid
        if (prob(rng_) < Pu_) {
            int j   = pos(rng_);
            int val = c.perm[i];
            c.perm.erase(c.perm.begin() + i);
            if (j >= (int)c.perm.size()) j = (int)c.perm.size();
            c.perm.insert(c.perm.begin() + j, val);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Duplicate removal
// ─────────────────────────────────────────────────────────────────────────────

void GeneticAlgorithm::removeDuplicates(vector<Chromosome>& pop) {
    set<vector<int>> seen;
    for (auto& ch : pop) {
        if (!seen.insert(ch.perm).second)
            ch = makeRandom();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Main GA loop — Algorithm 1 of Dang et al. 2023
// ─────────────────────────────────────────────────────────────────────────────

Chromosome GeneticAlgorithm::run() {
    if (n_ == 0) return Chromosome{};

    auto startTime  = Clock::now();
    auto elapsedSec = [&]() -> double {
        return chrono::duration<double>(Clock::now() - startTime).count();
    };

    // ── Initialise ──
    vector<Chromosome> pop;
    pop.reserve(Np_);
    initPopulation(pop);
    sort(pop.begin(), pop.end(), fitnessDesc);

    Chromosome best       = pop[0];
    int  gensNoImprove    = 0;
    bool poxActive        = false;
    int  poxGenCount      = 0;

    cerr << "[GA] n=" << n_ << "  Np=" << Np_
         << "  init_best=" << best.fitness << "\n";

    // ── Main loop ──
    while (elapsedSec() < params_.maxTimeSec && gensNoImprove < Gc_) {

        bool usePOX = poxActive && (poxGenCount < Omega_);

        // Build offspring
        vector<Chromosome> offspring;
        offspring.reserve(Np_);

        while ((int)offspring.size() < Np_) {
            const Chromosome& p1 = tournamentSelect(pop);
            const Chromosome& p2 = tournamentSelect(pop);

            auto [c1, c2] = combinedCrossover(p1, p2);

            if (usePOX) {
                // POX already evaluates; mutation below will re-evaluate
                c1 = problemOrientedCrossover(c1);
                c2 = problemOrientedCrossover(c2);
            }

            swapMutation(c1);
            reinsertionMutation(c1);
            c1.fitness = eval_(c1.perm);

            swapMutation(c2);
            reinsertionMutation(c2);
            c2.fitness = eval_(c2.perm);

            offspring.push_back(move(c1));
            if ((int)offspring.size() < Np_)
                offspring.push_back(move(c2));
        }

        // Elitism: sort offspring ascending (worst first),
        // overwrite SE_ worst with SE_ best parents.
        sort(offspring.begin(), offspring.end(),
             [](const Chromosome& a, const Chromosome& b) {
                 return a.fitness < b.fitness;
             });
        for (int i = 0; i < SE_ && i < Np_; ++i)
            offspring[i] = pop[i];   // pop is descending: pop[0] = best

        // Deduplicate, then sort for next generation
        removeDuplicates(offspring);
        pop = move(offspring);
        sort(pop.begin(), pop.end(), fitnessDesc);

        // Track improvement
        if (pop[0].fitness > best.fitness) {
            best          = pop[0];
            gensNoImprove = 0;
            poxActive     = true;
            poxGenCount   = 0;
            cerr << "[GA] New best=" << best.fitness
                 << "  t=" << (int)elapsedSec() << "s\n";
        } else {
            ++gensNoImprove;
            if (poxActive && ++poxGenCount >= Omega_)
                poxActive = false;
        }
    }

    cerr << "[GA] Done  best=" << best.fitness
         << "  no_improve=" << gensNoImprove
         << "  t=" << (int)elapsedSec() << "s\n";
    return best;
}
