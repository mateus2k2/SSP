https://data.mendeley.com/datasets/xbyd8tj9yn/1?utm_source=chatgpt.com

Set of 2880 instances named IPMTC-I and IPMTC-II, and used in the paper "Scheduling identical parallel machines with tooling constraints", by Beezão, Cordeau, Laporte and Yanasse, published at EJOR, 2017 (https://www.sciencedirect.com/science/article/abs/pii/S0377221716306233). The test instances are separated according the number of jobs: 8, 15, 25 (composing set IPMTC-I) and 50, 100 and 200 (composing set IPMTC-II). 

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
