Project 06: Concurrency
==========================

Members
-------

1. Chris Petrella (clpetrella@wpi.edu)
2. student B (email@wpi.edu)

Design: Correctness (40 points)
------

> 1. How do you parallelize the compression process to maximize the underlying physical resources (i.e., CPUs)? 
>    - How did you determine the number of threads to use for the wpzip?  (4 points)
>    - How did you break down the compression process into different tasks?  (4 points) 
>    - What are those tasks? (4 points)
>    - Which tasks were parallelized and which are serial? (4 points)

Response.
The number of threads is determined by a parameter in the command line input.
The compression process was difficult to break up, but we tried to parallelize as much as possible.
The main split is the compression and writing portions. The compression portion was parallelized so multiple files could be compressed
at once. Writing the result to the output file however, had to be serial. Trying to write in parallel resulted in the output of multiple
threads being mixed together.

> 2. How did you employ concurrency mechanisms to ensure correctness, avoid deadlocks, and coordinate threads?
>    - What synchronization primitives (mutex, condition variables, semaphores) did you use? (4 points)
>    - How was each primitive used? (6 points)
>    - Justify how your design ensures correctness. (10 points)
>    - How did you design to maximize the parallelism? (4 points)

Responses.
To ensure the individual threads didn't interfere with each other, we used mutex and condition variables. These were used together to ensure
that each file wrote to the final output in order. Without these, multiple threads would attempt to write to the file at the same time.
This caused the outputs to overlap and corrupt each other. To maximise parallelism, the locks only take effect at the end of the thread,
right before the write command.


> 3. Additional design details that are not covered in the previous two questions. 

Responses.



Performance Optimization (10 points)
------

> 1. What optimizations have you tried and which optimizations were useful in improving the compression performance over your previous parallelized implementation?

Responses.

> 2. Description of any new workloads you used to understand the performance bottlenecks.

Responses.

Errata
------

> Describe any known errors, bugs, or deviations from the requirements.

Responses.