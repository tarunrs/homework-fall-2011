// Template for OpenMP Finite-Difference-Time-Domain Exercise
// Use "gcc -O -fopenmp" to compile
// Make changes to the Template version below to create
// parallel version


#include <omp.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#define n 1024
#define tmax 5
#define threshold (0.000000001)

double xref[n];
double yref[n];
double zref[n];
double x[n];
double y[n];
double z[n];
double a[n][n];
void compare();

main(){
  double rtclock();

  int tt,i,j,nt;
  double clkbegin, clkend;
  double t, maxdiff;

// Initialize arrays
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      a[i][j] =  sin(i)*sin(j);
    }
    xref[i] = sin(i);
    yref[i] = cos(i);
    zref[i] = sin(i)*cos(i);
  }

  clkbegin = rtclock();
        for(i=0;i<n;i++)
          for(j=0;j<n;j++)
          {
           yref[i] = yref[i] + a[i][j]*xref[j];
           zref[i] = zref[i] + a[j][i]*xref[j];
          }
  clkend = rtclock();
  t = clkend-clkbegin;
  printf ("%.1f Sequential MFLOPS;  \n",
  4*n*n/t/1000000);

  printf("Maximum threads allowed by system is: %d\n",omp_get_max_threads());

  // Loop to run test on diffreent number of threads
  for (nt=1;nt<=4;nt++)
   {
    omp_set_num_threads(nt);

// Initialize arrays
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        a[i][j] =  sin(i)*sin(j);
      }
      x[i] = sin(i);
      y[i] = cos(i);
      z[i] = sin(i)*cos(i);
    }

  clkbegin = rtclock();

//#pragma omp parallel
// Template version is intentionally made sequential
// Make suitable changes to create parallel version
    {
  //   if (omp_get_thread_num()==0)
    //  if (omp_get_num_threads() != nt)
      //  printf("Warning: Actual #threads %d differs from requested number %d\n",omp_get_num_threads(),nt);

//#pragma omp master
#pragma omp parallel private(i,j)
//#pragma omp sections
      {
#pragma omp for nowait schedule(dynamic, 8)
        for(i=0;i<n;i++)
          for(j=0;j<n;j++)
          {
           y[i] = y[i] + a[i][j]*x[j];
           z[i] = z[i] + a[j][i]*x[j];
          }/*
#pragma omp for nowait schedule(dynamic, 8)

        for(i=0;i<n;i++)
              for(j=0;j<n;j++)
            z[i] = z[i] + a[j][i]*x[j];
            */
      }
   }
    clkend = rtclock();
    t = clkend-clkbegin;
    printf ("%.1f MFLOPS with %d threads; Time = %.3f sec; ",
    4*n*n/t/1000000,nt,t);
    compare();
  }
}
double rtclock()
{
  struct timezone Tzp;
  struct timeval Tp;
  int stat;
  stat = gettimeofday (&Tp, &Tzp);
  if (stat != 0) printf("Error return from gettimeofday: %d",stat);
  return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

void compare()
{
double maxdiff,this_diff;
int numdiffs;
int i;
  numdiffs = 0;
  maxdiff = 0;
   for (i=0;i<n;i++)
    {
     this_diff = zref[i]-z[i];
     if (this_diff < 0) this_diff = -1.0*this_diff;
     if (this_diff>threshold)
      { numdiffs++;
        if (this_diff > maxdiff) maxdiff=this_diff;
      }

     this_diff = yref[i]-y[i];
     if (this_diff < 0) this_diff = -1.0*this_diff;
     if (this_diff>threshold)
      { numdiffs++;
        if (this_diff > maxdiff) maxdiff=this_diff;
      }
    }
   if (numdiffs > 0)
      printf("Error: %d Diffs found %f; Max Diff = %f\n",
               numdiffs,threshold,maxdiff);
   else
      printf("Passed correctness check\n");
}
