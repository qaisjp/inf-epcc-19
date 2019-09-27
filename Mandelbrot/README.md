# Exercise 2: Area of the Mandelbrot Set
The aim of this exercise is to use the OpenMP directives learned so far and apply them to a real problem.

It will demonstrate some of the issues which need to be taken into account when adapting serial code to a parallel version.

## The Mandelbrot Set
The Mandelbrot Set is the set of complex numbers c for which the iteration `z = (z^2) + c` does not diverge, from the initial condition `z = c`.

To determine (approximately) whether a point `c` lies in the set, a finite number of iterations are performed, and if the condition `|z| > 2` is satisfied, then the point is considered to be outside the Mandelbrot Set.

What we are interested in is calculating the area 
of the Mandelbrot Set. There is no known theoretical value for this, and estimates are based on a procedure similar to that used here.

## The Code
The method we shall use generates a grid of points in a box of the complex plane containing the upper of the (symmetric) Mandelbrot Set.

Then each point is iterated using the equation above a finite number of times (say 2000).

If within that number of iterations the threshold condition `|z| > 2` is satisfied then that  point is considered to be outside of the Mandelbrot Set.

Then counting the number of points within the Set and those outside will lead to an estimate of the area of the Set.

Parallelise the serial code using the OpenMP directives and library routines that  you have learned so far. The method for doing this is as follows:

1. Start a parallel region before the main loop, nest making sure that any  private, shared or reduction variables within the region are correctly declared.
2. Distribute the outermost loop across the threads available so that each thread has an equal number of the points.

    For this you will need to use some of the OpenMP library routines.
    
    Once you have written the code try it out using 1, 2, 3 and 4 threads.
    
    Check that the results are identical in each case, and compare the time taken for the calculations using different number of threads.

## Extra Exercise
Is your solution well load balanced? Try different ways of mapping iterations to threads.
