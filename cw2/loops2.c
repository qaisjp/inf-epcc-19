#include <stdio.h>
#include <math.h>


#define N 729
#define reps 1000 
#include <omp.h> 

double a[N][N], b[N][N], c[N];
int jmax[N];  


void init1(void);
void init2(void);
void runloop(int); 
void loop1chunk(int, int);
void loop2chunk(int, int);
void valid1(void);
void valid2(void);


int main(int argc, char *argv[]) { 

  double start1,start2,end1,end2;
  int r;

  init1(); 

  start1 = omp_get_wtime(); 

  for (r=0; r<reps; r++){ 
    runloop(1);
  } 

  end1  = omp_get_wtime();  

  valid1(); 

  printf("Total time for %d reps of loop 1 = %f\n",reps, (float)(end1-start1)); 


  init2(); 

  start2 = omp_get_wtime(); 

  for (r=0; r<reps; r++){ 
    runloop(2);
  } 

  end2  = omp_get_wtime(); 

  valid2(); 

  printf("Total time for %d reps of loop 2 = %f\n",reps, (float)(end2-start2)); 

} 

void init1(void){
  int i,j; 

  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      a[i][j] = 0.0; 
      b[i][j] = 3.142*(i+j); 
    }
  }

}

void init2(void){ 
  int i,j, expr; 

  for (i=0; i<N; i++){ 
    expr =  i%( 3*(i/30) + 1); 
    if ( expr == 0) { 
      jmax[i] = N;
    }
    else {
      jmax[i] = 1; 
    }
    c[i] = 0.0;
  }

  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      b[i][j] = (double) (i*j+1) / (double) (N*N); 
    }
  }
 
}

struct range
{
  int lo;
  int hi;
};

#include <stdlib.h>

// #define DEBUG 1
#ifdef DEBUG
static int firstrun = 1;
#endif

// next updates the lo and hi for a thread
// and returns the range that needs to be processed
// NOT MEMORY SAFE.
struct range next(int nthreads, struct range *iterations, int myid)
{
  struct range rng = iterations[myid];
  int remaining = rng.hi - rng.lo;
  int size = (int) ceil((double) remaining / (double) nthreads);
#ifdef DEBUG
  if (firstrun && myid == 0)
    printf("[next\t] thread %d\trng.lo: %d\thi:%d\tsize: %d\n", myid, rng.lo, rng.hi, size);
#endif
  int new_lo = rng.lo + size;
  if (new_lo > rng.hi)
  {
    new_lo = rng.hi;
  }
  iterations[myid].lo = new_lo;
  rng.hi = new_lo;
#ifdef DEBUG
  if (firstrun && myid == 0)
    printf("[next1\t] thread %d\trng.lo: %d\thi:%d\tsize: %d\n", myid, rng.lo, rng.hi, size);
#endif
  return rng;
}

// runchunk runs a chunk for a specific loop, between a specific range
void runchunk(int loopid, struct range rng)
{
  switch (loopid)
  {
  case 1:
    loop1chunk(rng.lo, rng.hi);
    break;
  case 2:
    loop2chunk(rng.lo, rng.hi);
    break;
  }
}

void runloop(int loopid)
{
#ifdef DEBUG
  if (firstrun)
  {
    printf("DEBUG!\n");
  }
#endif

  struct range *iterations;
#ifdef DEBUG
#pragma omp parallel default(none) shared(loopid, iterations, firstrun)
#else
#pragma omp parallel default(none) shared(loopid, iterations)
#endif
  {
    int nthreads = omp_get_num_threads();

#ifdef DEBUG
  if (firstrun)
  {
    printf("Number of threads: %d\n", nthreads);
  }
#endif

#pragma omp single
    {
      iterations = calloc(nthreads, sizeof(struct range));
    }

    double ipt = (double)N / (double)nthreads;
    double frac = 1 / (double)nthreads;

    int myid = omp_get_thread_num();
    iterations[myid].lo = (int)ceil(myid * ipt);
    iterations[myid].hi = (int)ceil((myid + 1) * ipt);
    if (iterations[myid].hi > N)
    {
      iterations[myid].hi = N;
    }
#ifdef DEBUG
    if (firstrun)
      printf("[stage0\t] thread %d\tlo:%d\thi:%d\n", myid, iterations[myid].lo, iterations[myid].hi);
#endif

// this is needed so that `iterations` is correctly initialised!
#pragma omp barrier

    while (1)
    {
      struct range rng;

#pragma omp critical
      {
        rng = next(nthreads, iterations, myid);
      }

#ifdef DEBUG
      if (firstrun && myid == 0)
      {
        printf("[stage1\t] lo: %d\thi: %d\n", rng.lo, rng.hi);
      }
#endif

      if ((rng.hi - rng.lo) <= 0)
      {
#ifdef DEBUG
        if (firstrun && myid == 0)
        {
          printf("[stage1\t] break %d\n", rng.hi - rng.lo);
        }
#endif
        break;
      }

      runchunk(loopid, rng);
    }

    while (1)
    {
      struct range rng = { .lo = 0, .hi = 0 };
#pragma omp critical
      {
        int ml_idx = 0;
        int ml_num = iterations[0].hi - iterations[0].lo;
        for (int i = 1; i < nthreads; i++)
        {
          int num = iterations[i].hi - iterations[i].lo;
          if (num > ml_num)
          {
            ml_num = num;
            ml_idx = i;
          }
        }

        if (ml_num > 0)
        {
          rng = next(nthreads, iterations, ml_idx);
        }
      }

#ifdef DEBUG
      if (firstrun && myid == 0)
      {
        printf("[stage2\t] lo: %d\thi: %d\n", rng.lo, rng.hi);
      }
#endif

      if ((rng.hi - rng.lo) <= 0)
      {
#ifdef DEBUG
        if (firstrun && myid == 0)
        {
          printf("[stage2\t] break %d\n", rng.hi - rng.lo);
        }
#endif
        break;
      }

      runchunk(loopid, rng);
    }
  }

#ifdef DEBUG
  firstrun = 0;
#endif
}

void loop1chunk(int lo, int hi) { 
  int i,j; 
  
  for (i=lo; i<hi; i++){ 
    for (j=N-1; j>i; j--){
      a[i][j] += cos(b[i][j]);
    } 
  }

} 



void loop2chunk(int lo, int hi) {
  int i,j,k; 
  double rN2; 

  rN2 = 1.0 / (double) (N*N);  

  for (i=lo; i<hi; i++){ 
    for (j=0; j < jmax[i]; j++){
      for (k=0; k<j; k++){ 
	c[i] += (k+1) * log (b[i][j]) * rN2;
      } 
    }
  }

}

void valid1(void) { 
  int i,j; 
  double suma; 
  
  suma= 0.0; 
  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      suma += a[i][j];
    }
  }
  printf("Loop 1 check: Sum of a is %lf\n", suma);

} 


void valid2(void) { 
  int i; 
  double sumc; 
  
  sumc= 0.0; 
  for (i=0; i<N; i++){ 
    sumc += c[i];
  }
  printf("Loop 2 check: Sum of c is %f\n", sumc);
} 
 
