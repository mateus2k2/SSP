#pragma once
#include <vector>
#include <set>
#include <map>
#include <random>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <tuple>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include "SSP.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Forward declarations / types assumed from your existing code
// ─────────────────────────────────────────────────────────────────────────────
// struct ToolSet {
//     int indexToolSet;
//     std::vector<int> tools;
// };

// struct Job {
//     int indexJob;
//     int indexOperation;     // k-th operation of the job
//     int indexToolSet;
//     int processingTime;
//     bool priority;
//     bool flag;
//     ToolSet toolSet;
//     ToolSet toolSetNormalized;
//     bool isGrouped;
//     bool isReentrant;
//     std::vector<int> processingTimes;
//     std::vector<ToolSet> toolSets;
// };

// ─────────────────────────────────────────────────────────────────────────────
//  GA-specific data structures
// ─────────────────────────────────────────────────────────────────────────────

/**
 * A MaximalClass is a group of operations that can all fit in the tool
 * magazine at the same time (total tools <= TC).
 * Paper notation: S_g
 */
struct MaximalClass {
    std::vector<int> operationIndices;   // indices into the global operation list
    int machineId = 0;                   // machine assigned to this class (M_g)

    // Convenience: union of all tool sets of operations in this class
    std::set<int> allTools() const;       // filled lazily by GA
};

/**
 * One gene in a chromosome: v_g = {S_g, M_g}
 * Paper: chromosome v = (v_1, …, v_n) = ({S_1,M_1}, …, {S_n,M_n})
 */
struct Gene {
    MaximalClass cls;   // S_g
    int machineId = 0;  // M_g (duplicated from cls for convenience)
};

/**
 * A full chromosome.
 * V^S  = job vector  (sequence of MaximalClasses)
 * V^M  = machine vector (machine assigned to each class)
 * fitness = profit = Revenue - PenaltyCost - ToolSwitchCost
 */
struct Chromosome {
    std::vector<Gene> genes;            // length n = number of maximal classes
    double fitness = -std::numeric_limits<double>::infinity();

    int numGenes() const { return (int)genes.size(); }
};

// ─────────────────────────────────────────────────────────────────────────────
//  GA parameters (Section 7.2 of the paper)
// ─────────────────────────────────────────────────────────────────────────────
struct GAParams {
    int    popSize      = 400;   // N_p
    double gamma1       = 0.20;  // tournament selection rate
    double gamma2       = 0.10;  // elitism rate
    double probSwap     = 0.01;  // P_s  (swap mutation probability)
    double probUniform  = 0.01;  // P_u  (uniform mutation probability)
    int    Q            = 1;     // number of generations using POX after new best
    int    Gc           = 20;    // max non-improvement generations before stopping
    double maxTimeSec   = 3600.0;// maximum wall-clock time
    int    numMachines  = 1;     // |M|
    int    magazineCap  = 80;    // T_C
    int    horizon      = 10080; // H  (minutes; 7 days * 24 h * 60 = 10080)
    int    unsupHours   = 12;    // t_U  (unsupervised hours per day)
    // Cost / revenue parameters
    int    revenue      = 30;    // r  per finished operation
    int    penaltyCost  = 30;    // c_p per unfinished priority operation
    int    fixedSwitch  = 10;    // c_f per tool-switch instance
    int    varSwitch    = 1;     // c_v per individual tool switch
    // BnB seeding: 5 % of population
    double bnbSeedFrac  = 0.05;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Main GA class
// ─────────────────────────────────────────────────────────────────────────────
class GeneticAlgorithm {
public:
    // ── Construction ──────────────────────────────────────────────────────────
    /**
     * @param operations  flat list of all operations (O), ordered by job then
     *                    operation index.  Precedence constraints come from
     *                    consecutive operations with the same indexJob.
     * @param params      tunable hyper-parameters
     * @param seed        random seed (for reproducibility)
     */
    GeneticAlgorithm(const std::vector<Job>& operations,
                     const GAParams&         params,
                     unsigned int            seed = 42);

    // ── Public interface ──────────────────────────────────────────────────────
    /** Run the GA; returns the best chromosome found. */
    Chromosome run();

    /** Evaluate (and set) the fitness of a chromosome in-place. */
    void evaluateChromosome(Chromosome& chr);

    /** Return the all-time best chromosome. */
    const Chromosome& bestChromosome() const { return bestChr_; }

    // ── Exposed for unit-testing ───────────────────────────────────────────────
    std::vector<Chromosome> population_;    // current generation P_k

private:
    // ── Problem data ──────────────────────────────────────────────────────────
    std::vector<Job> ops_;          // all operations O
    GAParams         params_;
    std::mt19937     rng_;

    // Precomputed: for operation i, which tools does it need?
    // toolsOf_[i] = set of tool IDs
    std::vector<std::set<int>> toolsOf_;

    // Precomputed: for operation i, is it priority?
    std::vector<bool> isPriority_;

    // Precomputed: operation index → (jobIndex, operationIndex k)
    std::vector<std::pair<int,int>> opId_;

    // Total number of operations
    int numOps_;
    int numMachines_;
    int TC_;            // magazine capacity

    // Best chromosome so far
    Chromosome bestChr_;

    // ── Initialisation (Section 5.2) ──────────────────────────────────────────
    void initialisePopulation();

    /** Generate one chromosome with a random permutation of maximal classes. */
    Chromosome randomChromosome();
    Chromosome randomChromosomeFromBnB();

    /** BnB grouping: partition all operations into min # of maximal classes.
     *  Returns the sequence of maximal classes (operations grouped). */
    std::vector<MaximalClass> bnbGrouping();

    // ── BnB helpers (Appendix E) ──────────────────────────────────────────────
    /** MIMU: sequential maximal partition (upper-bound heuristic). */
    std::vector<MaximalClass> MIMU(std::vector<int> opSet);

    /** Sweeping procedure: lower-bound heuristic. */
    int sweepingLowerBound(const std::vector<int>& opSet);

    /** Check if opSet forms a valid class (union of tools <= TC). */
    bool isValidClass(const std::vector<int>& opSet) const;

    /** Union of tools for a set of operation indices. */
    std::set<int> unionTools(const std::vector<int>& opSet) const;

    // ── Chromosome construction helpers ───────────────────────────────────────
    /**
     * Given a sequence (partition) of maximal classes, assign machines using
     * the constructive heuristic from Dang et al. 2021 (Section 5.3.2 step 3).
     * Machines are chosen by: (a) machine already has required tools; else
     *                         (b) machine with least accumulated workload.
     */
    void assignMachines(Chromosome& chr);

    /** Merge adjacent classes on the same machine if they still fit in T_C.
     *  Paper Section 5.5. */
    void mergeToMaximalClasses(Chromosome& chr);

    // ── Selection (Section 5.3) ───────────────────────────────────────────────
    /** Tournament selection; returns index of winner in population_. */
    int tournamentSelect();

    // ── Crossover (Section 5.3) ───────────────────────────────────────────────
    /** Combined crossover = 2X + APMX (Section 5.3.1). */
    std::pair<Chromosome,Chromosome> combinedCrossover(const Chromosome& p1,
                                                       const Chromosome& p2);

    /** Two-point crossover on the gene sequence. */
    std::pair<Chromosome,Chromosome> twoPointCrossover(const Chromosome& p1,
                                                       const Chromosome& p2);

    /** Adapted partial-mapped crossover: fix duplicates after 2X. */
    void adaptedPMX(Chromosome& offspring, const Chromosome& donor,
                    int cut1, int cut2);

    /** Problem-oriented crossover (Section 5.3.2). 
     *  Applies to a single chromosome (modifies it in-place). */
    void problemOrientedCrossover(Chromosome& chr);

    // ── Mutation (Section 5.4) ────────────────────────────────────────────────
    /** Swap mutation on gene-level and operation-level. */
    void swapMutation(Chromosome& chr);

    /** Uniform mutation on the machine vector. */
    void uniformMutation(Chromosome& chr);

    // ── KTNS tool-switching (Section 5.7) ─────────────────────────────────────
    /**
     * Given the sequence of finished maximal classes on one machine,
     * compute the total number of tool switches using KTNS.
     * Returns {totalSwitches, numSwitchInstances}.
     */
    std::pair<int,int> KTNS(const std::vector<MaximalClass>& seq,
                            const std::vector<int>& opIndices) const;

    // ── Fitness evaluation (Algorithm 2) ──────────────────────────────────────
    /**
     * Decode a chromosome, simulate the schedule respecting unsupervised
     * shifts, compute revenue, penalty, and tool-switch costs.
     */
    double fitnessEval(Chromosome& chr);

    // ── Elitism + immigration (Section 5.6) ───────────────────────────────────
    void nextGeneration(std::vector<Chromosome>& offspring);

    // ── Utility ───────────────────────────────────────────────────────────────
    int  uniformInt(int lo, int hi);           // inclusive
    double uniformReal(double lo, double hi);
    bool toolsFitInMagazine(const std::set<int>& toolsAlready,
                            const std::set<int>& toolsNeeded) const;
    /** Return tools union of a MaximalClass. */
    std::set<int> classTools(const MaximalClass& cls) const;

    /** Validate that a chromosome's precedence constraints are satisfied.
     *  (Each job j appears n_j times in the right order across genes.) */
    bool validatePrecedence(const Chromosome& chr) const;

    /** Decode the job-vector part of a chromosome into a flat ordered
     *  list of operations respecting precedence. */
    std::vector<int> decodeToOperationSequence(const Chromosome& chr) const;

    // Machine state during fitness evaluation
    struct MachineState {
        int availTime = 0;                // a_m
        std::set<int> magazine;           // current tools in magazine
        std::vector<MaximalClass> finishedClasses; // S_F_m
        std::vector<int> finishedOps;     // O_F_m
        int finishedPriority = 0;         // |O_P_m|
    };
};