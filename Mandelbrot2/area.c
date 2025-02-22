#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NPOINTS 2000
#define MAXITER 2000

struct complex
{
  double real;
  double imag;
};

int main()
{
  int numoutside = 0;
  double area, error, ztemp;
  double start, finish;
  struct complex z, c;

  /*
 *
 *
 *     Outer loops run over npoints, initialise z=c
 *
 *     Inner loop has the iteration z=z*z+c, and threshold test
 */

  start = omp_get_wtime();

  #pragma omp parallel for default(none) private(ztemp) shared(c,z) reduction(+:numoutside)
  for (int i = 0; i < NPOINTS; i++)
  {
    for (int j = 0; j < NPOINTS; j++)
    {
      c.real = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS) + 1.0e-7;
      c.imag = 1.125 * (double)(j) / (double)(NPOINTS) + 1.0e-7;
      z = c;
      for (int iter = 0; iter < MAXITER; iter++)
      {
        ztemp = (z.real * z.real) - (z.imag * z.imag) + c.real;
        z.imag = z.real * z.imag * 2 + c.imag;
        z.real = ztemp;
        if ((z.real * z.real + z.imag * z.imag) > 4.0e0)
        {
          numoutside++;
          break;
        }
      }
    }
  }

  finish = omp_get_wtime();

  /*
 *  Calculate area and error and output the results
 */

  area = 2.0 * 2.5 * 1.125 * (double)(NPOINTS * NPOINTS - numoutside) / (double)(NPOINTS * NPOINTS);
  error = area / (double)NPOINTS;

  printf("Area of Mandlebrot set = %12.8f +/- %12.8f\n", area, error);
  printf("Time = %12.8f seconds\n", finish - start);

  double min_area = 1.50927328 - 0.00075464;
  double max_area = 1.50927328 + 0.00075464;
  printf("\nExpected output: Area of Mandlebrot set =   1.50927328 +/-   0.00075464\nOriginal time =   9.11457586 seconds\n\n");
  if (area > max_area || area < min_area) {
    printf("\033[1;31mTEST FAILED\033[0m: wanted between %12.8f and %12.8f\n", min_area, max_area);
  } else {
    printf("\033[0;36mTEST SUCCESS\033[0m\n");
  }
}
