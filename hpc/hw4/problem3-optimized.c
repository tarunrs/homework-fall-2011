// Template for OpenMP Finite-Difference-Time-Domain Exercise
// Use "gcc -O -fopenmp -lm" to compile
// Make changes to the Template version below to create parallel version


#include <omp.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include<math.h>
#define nx 1024
#define ny 1024
#define tmax 5
#define coeff1 0.5
#define coeff2 0.7
#define threshold (0.000000001)

double exref[nx][ny+1];
double eyref[nx+1][ny];
double hzref[nx][ny];
double ex[nx][ny+1];
double ey[nx+1][ny];
double hz[nx][ny];
void compare();

main(){
  double rtclock();

  int tt,i,j,nt;
  double clkbegin, clkend;
  double t, maxdiff;

// Initialize arrays
  for (i=0; i<nx; i++) {
    for (j=0; j<=ny; j++) {
      exref[i][j] = ex[i][j] = sin(i)*sin(j);
    }
  }
  for (i=0; i<=nx; i++) {
    for (j=0; j<ny; j++) {
      eyref[i][j] = ey[i][j] = cos(i)*cos(j);
    }
  }
  for (i=0; i<nx; i++) {
    for (j=0; j<ny; j++) {
      hzref[i][j] = hz[i][j] = sin(i)*cos(j);
    }
  }

  clkbegin = rtclock();
  for (tt=0; tt<tmax; tt++){
    for (j=0; j<ny; j++)
      eyref[0][j] = tt;

     for (j=0; j<ny; j++) 
      for (i=1; i<nx; i++) 
       eyref[i][j] = eyref[i][j] - coeff1*(hzref[i][j]-hzref[i-1][j]);

     for (j=1; j<ny; j++) 
      for (i=0; i<nx; i++) 
       exref[i][j] = exref[i][j] - coeff1*(hzref[i][j]-hzref[i][j-1]);

     for (j=0; j<ny; j++) 
      for (i=0; i<nx; i++) 
       hzref[i][j] =  hzref[i][j] -
         coeff2*(exref[i][j+1]-exref[i][j]+eyref[i+1][j]-eyref[i][j]);
  }
  clkend = rtclock();
  t = clkend-clkbegin;
  printf ("%.1f Sequential MFLOPS;  \n",
  10*nx*ny*tmax/t/1000000);

  printf("Maximum threads allowed by system is: %d\n",omp_get_max_threads());

  // Loop to run test on diffreent number of threads
  for (nt=1;nt<=3;nt++)
   {
    omp_set_num_threads(nt);

    // Initialize arrays
    for (i=0; i<nx; i++) {
      for (j=0; j<=ny; j++) {
        ex[i][j] = ex[i][j] = sin(i)*sin(j);
      }
    }
    for (i=0; i<=nx; i++) {
      for (j=0; j<ny; j++) {
        ey[i][j] = ey[i][j] = cos(i)*cos(j);
      }
    }
    for (i=0; i<nx; i++) {
      for (j=0; j<ny; j++) {
        hz[i][j] = hz[i][j] = sin(i)*cos(j);
      }
    }
    clkbegin = rtclock();

//#pragma omp parallel
 //Template version is intentionally made sequential
 //Make suitable changes to create parallel version
    {
 //    if (omp_get_thread_num()==0)
//      if (omp_get_num_threads() != nt)
 //       printf("Warning: Actual #threads %d differs from requested number %d\n",omp_get_num_threads(),nt);

//#pragma omp master

      for (tt=0; tt<tmax; tt++)
      {
#pragma omp parallel for
       for (j=0; j<ny; j++)
        ey[0][j] = tt;

       for (j=0; j<ny; j++)
#pragma omp parallel for schedule (dynamic, 16)

        for (i=1; i<nx; i++)

         ey[i][j] = ey[i][j] - coeff1*(hz[i][j]-hz[i-1][j]);

       for (j=1; j<ny; j++)
           #pragma omp parallel for
       for (i=0; i<nx; i++)

         ex[i][j] = ex[i][j] - coeff1*(hz[i][j]-hz[i][j-1]);

       for (j=0; j<ny; j++)
           #pragma omp parallel for
        for (i=0; i<nx; i++) 

         hz[i][j] =  hz[i][j] -
           coeff2*(ex[i][j+1]-ex[i][j]+ey[i+1][j]-ey[i][j]);
      }
    }

    clkend = rtclock();
    t = clkend-clkbegin;
    printf ("%.1f MFLOPS with %d threads; Time = %.3f sec; ",
    10*nx*ny*tmax/t/1000000,nt,t);
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
int i,j;
  numdiffs = 0;
  maxdiff = 0;
  for (i=0;i<nx;i++)
   for (j=0;j<ny;j++)
    {
     this_diff = hzref[i][j]-hz[i][j];
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
