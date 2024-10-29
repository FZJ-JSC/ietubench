# Instruction Execution Throughput MicroBenchmarks

This repository contains a template-based microbenchmark generation framework,
and a family of microbenchmarks for the A64 instruction set.

The microbenchmark code generation uses the [Inja](https://github.com/pantor/inja)
template engine [[1](https://github.com/pantor/inja)]
and [JSON for Modern C++](https://github.com/nlohmann/json)
header-only libraries [[2](https://github.com/nlohmann/json)].

Benchmarks can be instrumented to be run on the [gem5 simulator](https://github.com/gem5/gem5)
[[3](https://github.com/gem5/gem5)] or for use with the [LIKWID](https://github.com/RRZE-HPC/likwid)
performance monitoring suite [[4](https://github.com/RRZE-HPC/likwid)].

## Quick Start

```bash
# Clone this repository
git clone https://github.com/FZJ-JSC/ietubench.git

# Configure build using CMake
cmake -B build/ietubench ietubench -D CMAKE_BUILD_TYPE="Release"

# Optional: Configure build using CMake and link with LIKWID
# cmake -B build/ietubench-lk ietubench -D CMAKE_BUILD_TYPE="Release" -D PROJECT_USE_LIKWID="ON"

# Build all microbenchmarks defined in src/micro subdirectory
make -C build/ietubench -j$(nproc)
```


## Available A64 Microbenchmarks

| Benchmark | Directory | 
| --------- | --------- |
| Instruction Execution Throughput | [micro/iet](src/micro/iet)  |
| Core-to-core Latency             | [micro/c2c](src/micro/c2c)  |
| Branch Prediction                | [micro/bp](src/micro/bp)    |

### Instruction Execution Throughput

#### Description

Instruction Execution Throughput microbenchmarks execute a single instruction
or small group of instructions over a loop of length $L$ and measure the time $t$
to compute $N$ loop iterations. The measurement is repeated $M$ times to calculate
the average latency $\bar{t}_L$

```math
\bar{t}_L = \frac{\sum^M t_i}{ M L}
```

and execution throughput $\bar{I}_L \equiv  \frac{1}{\bar{t}_L}$.

Other examples of microbenchmark implementations to measure instruction execution rate are
[[5](https://github.com/ChipsandCheese/Microbenchmarks)] and [[6](https://github.com/lelegard/cryptobench)].

#### Usage

```bash
# Benchmark latency/throughput of integer ADD using a loop of length L=64,
# iterating the main loop 100 times and repeating the benchmark 150 times
./src/micro/iet/add_a_64.x -l 100 -r 150

# Benchmark latency/throughput of integer ADD and 64 bit FP fadd for loops
# of length L=16,64,256,1024,4096 and print the output in CSV format
for L in 16 64 256 1024 4096 ; do ./src/micro/iet/add_fadd64_b_$L.x -l 100 -r 100 -o | awk -vT="$L" '{ print T "," $0 }' ; done | tee iet.csv

```

### Core-to-Core latency

#### Description

The *core-to-core latency* $L_c(m,n)$  between two *Processing Elements* (PE)
$P_m$ and $P_n$ denotes the delay between $P_m$ updating a memory location $x$,
and another PE $P_n$ observing the update.
Because of the weakly-ordered memory model of the ARMv8 architecture,
it is not a priori obvious how to operationally define the observation delay.
A proper implementation needs to enforce memory ordering.

We might define the *core-to-core latency* $L_c(m,n)$ as *message passing* between $P_m$ and $P_n$

```armasm
WAIT_ACQ:
  ldar x1, [#local]
  cmp  x1, x2
  bls  WAIT_ACQ
  add  x2, x1, #1
  stlr x2, [#remote]
```

The WAIT_ACQ loop is described in section K14.2.1 of [[7](https://developer.arm.com/documentation/ddi0487/latest)]
as a way to implement message passing for weakly-ordered memory architectures.
The WAIT_ACQ loop is implemented in [c2c/ldar](src/micro/c2c/ldar.j2).

#### Usage

```bash
# Benchmark core-to-core message passing latency for CPUs m and n,
# iterating the main loop l times and repeating the benchmark r times
./src/micro/c2c/ldar_256.x -t m,n -l l -r r

# Run the c2c benchmark for all pairs [(0,1), .. ,(0,63)]
for n in $(seq 1 63); do ./src/micro/c2c/ldar_256.x  -t 0,$n -l 1000 -r 10 -o  | awk -vT="$n" '{ print T "," $0 }' ; done | tee c2c.csv
```

### Branch Prediction

#### Description

Consider a sequence $(a_i)$ of integers $a_i \in \\{0..99\\}$,
randomly distributed with constant probability distribution $P(a_i = p) = \frac{1}{100}$
for all $p \in \\{0..99\\}$. Then the probability of encountering
a value $a_i$ strictly less than $p$ is given by
```math
P(n_i < p) = \sum_{0}^{p-1} \frac{1}{100} = \frac{p}{100}
```

Consider the loop shown below and let ``[x10]`` point
to an integer array constructed as described above.
Each array element read is compared to a given integer input parameter $p \in \\{0,100\\}$.
If the array element is strictly less than $p$, then a branch is taken.
Otherwise the next element is evaluated, and so on.

The integer parameter $p$ can then be directly interpreted
as the average branching probability at each branch inside the loop,
and the benchmark can be used to characterize branch prediction accuracy
and branch misprediction penalties for different branching probabilities $p$.

```armasm
LOOP:
  ldrb  w9, [x10], 1
  cmp   x9,  x11
  bcc   .TARGET_1
.BACK_FROM_TARGET_1:
  ldrb  w9, [x10], 1
  cmp   x9,  x11
  bcc   .TARGET_2
.BACK_FROM_TARGET_2:
  ...
```

The loop is implemented in [bp/bcc](src/micro/bp/bcc.j2).

A similar experiment using arrays of random integers to benchmark
branch prediction is presented in [[8](https://en.algorithmica.org/hpc/pipelining/branching/)].


#### Usage

```bash
# Use LIKWID to measure selected Armv8 performance counters on processor 8 for all values of p=1..100
export CTR="CPU_CYCLES:PMC0,INST_RETIRED:PMC1,BR_PRED:PMC2,BR_RETIRED:PMC3,BR_MIS_PRED:PMC4,BR_MIS_PRED_RETIRED:PMC5"
for p in $(seq 1 100);
  do likwid-perfctr -C 8 -c 8 -g $CTR -o csv/bcc_b_4096_p${p}.csv -m ./src/micro/bp/bcc_b_4096-lk.x -r 100 -l 100 -f s
rc/data/data_s409600_r100.dat -p $p;
done 
```

## References

[1] [https://github.com/pantor/inja](https://github.com/pantor/inja)

[2] [https://github.com/nlohmann/json](https://github.com/nlohmann/json)

[3] [gem5 computer-system architecture simulator](https://github.com/gem5/gem5)

[4] [LIKWID Performance monitoring and benchmarking suite ](https://github.com/RRZE-HPC/likwid)

[5] [ChipsandCheese/Microbenchmarks](https://github.com/ChipsandCheese/Microbenchmarks)

[6] [Cryptographic libraries comparative benchmarks](https://github.com/lelegard/cryptobench)

[7] [Arm Architecture Reference Manual for A-profile architecture](https://developer.arm.com/documentation/ddi0487/latest)

[8] [The Cost of Branching](https://en.algorithmica.org/hpc/pipelining/branching/)
