Set of 2880 instances named IPMTC-I and IPMTC-II, and used in the paper "Scheduling identical parallel machines with tooling constraints", 
by Beezão, Cordeau, Laporte and Yanasse, published at EJOR, 2017 (https://www.sciencedirect.com/science/article/abs/pii/S0377221716306233). 
The test instances are separated according the number of jobs: 8, 15, 25 (composing set IPMTC-I) and 50, 100 and 200 (composing set IPMTC-II). 

An example of a test instance follows:

2 8 15 5

15

56 10 42 22 44 20 21 40 

0 0 0 0 1 0 0 0 

1 0 0 0 0 0 0 0 

0 0 1 0 0 1 1 0 

0 0 0 0 0 0 1 1 

0 0 0 0 0 0 1 1 

0 0 0 0 0 1 0 0 

0 0 1 0 0 0 0 1 

1 1 0 0 0 0 0 0 

1 0 0 0 1 1 0 1 

0 1 0 0 1 0 1 0 

1 0 0 0 0 1 0 0 

0 0 0 1 1 0 0 0 

1 0 0 1 0 0 0 0 

0 0 0 0 1 1 1 0 

0 0 0 1 0 0 0 1

Row 1: #machines, #jobs, #tools and capacity of the machine's magazine.

Row 2: switching time.

Row 3: processing time of each job.

Remaining: tool requirement matrix, where each column i indicates the need (or not) of each tool (row) to the processing of job i.



To illustrate the generalizability and broader applicability of our proposed GA, we conduct an additional numerical experiment based on the
Beezão instances5 from the literature (Beezão et al., 2017). Their instances contain the number of machines, jobs, and required tools. However,
Beezão’s instances does not contain information about unsupervised hours, priority jobs, and a fixed scheduling horizon. Therefore, in order to
experiment Beezão’s instances in our paper, we provided some modifications. We first set the unsupervised shift ratio 𝜌𝑈 = 0 (no unsupervised
hours), which means we only consider supervised hours in which tool switches can always occur. Second, we set the priority ratio 𝜌𝑃 = 1, meaning
that all jobs have the same priority. Also, since Beezão et al. (2017) does not provide revenue and cost rates, we set 𝑟 = 0 and 𝑐𝑓 = 0, while keeping
𝑐𝑣 = 1 and 𝑐𝑝 = 30 as in our paper. Finally, we set a fixed scheduling horizon 𝐻 to the obtained makespan by Beezão et al. (2017), excluding
the idle time caused by the tool switches (i.e., time of completing all jobs without tool switching time; let denote it by 𝛥∗, so 𝐻 = 𝛥∗).6 This is
because our problem setting does not consider delay caused by tool switches. Among the largest Beezão instances, we selected 12 instances, with
6 machines, 200 jobs, and 40 tools.

unsupervised shift ratio 𝜌𝑈 = 0
priority ratio 𝜌𝑃 = 1, meaning that all jobs have the same priority
set 𝑟 = 0 and 𝑐𝑓 = 0, while keeping 𝑐𝑣 = 1 and 𝑐𝑝 = 30 as in our paper
set a fixed scheduling horizon 𝐻 to the obtained makespan by Beezão et al. (2017), excluding the idle time caused by the tool switches
we selected 12 instances, with 6 machines, 200 jobs, and 40 tools.
