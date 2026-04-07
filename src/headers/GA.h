#pragma once

#include <functional>
#include <random>
#include <unordered_map>
#include <vector>

#include "GlobalVars.h"

// ─────────────────────────────────────────────────────────────────────────────
// GA parameter bundle (filled by main.cpp from SSP accessors)
// ─────────────────────────────────────────────────────────────────────────────
struct GAParams {
    int    numMachines = 2;
    int    magazineCap = 80;
    int    horizon     = 10080;   // planning horizon in minutes (7 days)
    double unsupHours  = 12.0;   // unsupervised hours per day
    int    revenue     = 30;
    int    penaltyCost = 30;
    int    fixedSwitch = 10;
    int    varSwitch   = 1;
    double maxTimeSec  = 3600.0; // 1 hour
};

// ─────────────────────────────────────────────────────────────────────────────
// Chromosome: permutation of grouped-job indices + fitness (profit, higher=better)
// ─────────────────────────────────────────────────────────────────────────────
struct Chromosome {
    std::vector<int> perm;
    double           fitness = 0.0;
};

// ─────────────────────────────────────────────────────────────────────────────
// Genetic Algorithm  — Dang et al. 2023, Section 4
//
// Chromosome encoding:  permutation of grouped-job indices.
//   The KTNS scheduler (inside evalFn) assigns jobs to machines via a time-
//   based split, so no explicit machine vector is needed here.
//
// Operators (paper → implementation):
//   CX  = Two-Point Crossover + Partially-Mapped Crossover repair
//   POX = Problem-Oriented Crossover (priority sort + greedy tool-similarity)
//   SM  = Swap Mutation  (Ps = 0.01 per locus)
//   UM  = Uniform / Reinsertion Mutation  (Pu = 0.01 per locus)
//
// Parameters (Table A.2-A.5):
//   Np = 400, SE = 40, ST = 80, Gc = 20, Ω = 1, Ps = Pu = 0.01
// ─────────────────────────────────────────────────────────────────────────────
class GeneticAlgorithm {
public:
    /// evalFn(perm) must return the PROFIT (positive value, higher = better).
    using EvalFn = std::function<double(const std::vector<int>&)>;

    GeneticAlgorithm(const std::vector<Job>& groupedJobs,
                     const GAParams&         params,
                     EvalFn                  evalFn,
                     unsigned                seed = 42);

    /// Run the GA and return the best chromosome found.
    Chromosome run();

private:
    // ── problem data ──────────────────────────────────────────────────────────
    std::vector<Job> jobs_;   // grouped jobs
    GAParams         params_;
    EvalFn           eval_;
    std::mt19937     rng_;
    int              n_;      // number of grouped jobs

    // ── hyper-parameters (tuned values from paper) ────────────────────────────
    static constexpr int    Np_    = 400;   // population size
    static constexpr int    SE_    = 40;    // elitism count  (10 % of Np)
    static constexpr int    ST_    = 80;    // tournament size (20 % of Np)
    static constexpr int    Gc_    = 200;    // max generations without improvement
    static constexpr int    Omega_ = 1;     // POX generations after improvement
    static constexpr double Ps_    = 0.01;  // swap-mutation prob per locus
    static constexpr double Pu_    = 0.01;  // reinsertion-mutation prob per locus

    // ── initialisation ────────────────────────────────────────────────────────
    Chromosome makeRandom();
    Chromosome makePriorityFirst();
    void       initPopulation(std::vector<Chromosome>& pop);

    // ── crossover ─────────────────────────────────────────────────────────────
    std::pair<Chromosome, Chromosome>
        combinedCrossover(const Chromosome& p1, const Chromosome& p2);

    void pmxRepair(std::vector<int>&       child,
                   const std::vector<int>& segParent,
                   const std::vector<int>& fillParent,
                   int l, int r);

    Chromosome problemOrientedCrossover(const Chromosome& c);

    // ── mutation ──────────────────────────────────────────────────────────────
    void swapMutation       (Chromosome& c);
    void reinsertionMutation(Chromosome& c);

    // ── selection ─────────────────────────────────────────────────────────────
    const Chromosome& tournamentSelect(const std::vector<Chromosome>& pop);

    // ── BnB initialization (MIMU + Sweeping) ─────────────────────────────────
    // Returns the union of all tools required by grouped job i.
    std::vector<int> jobTools(int i) const;

    // MIMU: greedily builds a maximal-class partition (upper bound U*).
    // Maximise intersection with current class, break ties by min new tools.
    std::vector<std::vector<int>> runMIMU();

    // Sweeping: partition starting from the most-constrained unassigned job.
    std::vector<std::vector<int>> runSweeping();

    // BnB: if U*==L*, return either partition; otherwise branch on the job
    // with fewest remaining compatible peers and pick the child with lower
    // depth + sweep-estimate bound.  Returns the best partition found or
    // the MIMU partition if the time budget for BnB is exceeded.
    std::vector<std::vector<int>> runBnB();

    // Convert a partition (list of maximal classes) to a GA chromosome.
    // Jobs within each class are optionally shuffled for diversity.
    Chromosome partitionToChromosome(const std::vector<std::vector<int>>& partition,
                                     bool shuffleWithin = false);

    // ── utilities ─────────────────────────────────────────────────────────────
    int  toolOverlap   (int i, int j) const;
    void removeDuplicates(std::vector<Chromosome>& pop);

    static bool fitnessDesc(const Chromosome& a, const Chromosome& b)
        { return a.fitness > b.fitness; }
};
