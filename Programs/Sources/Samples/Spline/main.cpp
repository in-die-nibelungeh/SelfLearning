#include <stdio.h>
#include <math.h>
#define MAXN 1010

#include "mcon.h"
#include "mfio.h"

void spline_cal_u(int n, double x[], double y[], double u[]);
double spline(int n, double x[], double y[], double u[],
          double xx);
void mk_graph(int n, double x[], double x1, double x2,
          double y[], double y1, double y2);
int gauss_jordan(int n, double b[]);

#define VALUE(k) (((k) - 10) * ((k) - 40) * ((k) - 80) * 1.0e-4)
mcon::Matrix<double> a;

/*=============================================================*/
/* main function                                               */
/*=============================================================*/
int main(void)
{
    double x[MAXN], y[MAXN], u[MAXN];
    //double xmin, xmax;
    const int n = 10;
    if ( false == a.Resize(n+1, n+1) )
    {
        printf("Couldn't continue ...\n");
        return 0;
    }
    //   xmin = -1.0;
    //   xmax = 1.0;

    for(int i=0; i<=n; i++)
    {
        x[i]=i;//xmin+i*(xmax-xmin)/n;
        y[i]=VALUE(i*10);//1.0/(1.0+25*x[i]*x[i]);
    }

    spline_cal_u(n,x,y,u);
    for(int i=0; i<=n; i++)
    {
        printf("u[%d]=%g\n", i, u[i]);
    }
    const int nplot = 101;
    mcon::Matrix<double> mat(2, nplot);
    mcon::Vector<double>& px = mat[0];
    mcon::Vector<double>& py = mat[1];

    for(int i=0; i<nplot; i++)
    {
        px[i]=i/10.0;//xmin+i*(xmax-xmin)/(nplot);
        py[i]=spline(n, x, y, u, px[i]);
    }

    mfio::Csv::Write("spline.csv", mat);
    // mk_graph(nplot, px, xmin, xmax, py, -0.5, 1.5);

    return 0;
}

/*=============================================================*/
/* calculation of u[i]                                         */
/*=============================================================*/
void spline_cal_u(int n, double x[], double y[], double u[])
{
   int i, j;

   for(i=0; i<=n-1; i++)
   {
     for(j=0; j<=n-1; j++){
       a[i][j]=0.0;
     }
   }
   a[1][1]=2.0*(x[2]-x[0]);
   a[1][2]=x[2]-x[1];
   u[1]=6.0*((y[2]-y[1])/(x[2]-x[1])-(y[1]-y[0])/(x[1]-x[0]));
   for(i=2; i<=n-2; i++){
     a[i][i-1]=x[i]-x[i-1];
     a[i][i]=2.0*(x[i+1]-x[i-1]);
     a[i][i+1]=x[i+1]-x[i];
     u[i]=6.0*((y[i+1]-y[i])/(x[i+1]-x[i])-
           (y[i]-y[i-1])/(x[i]-x[i-1]));
   }
   a[n-1][n-2]=x[n-1]-x[n-2];
   a[n-1][n-1]=2.0*(x[n]-x[n-2]);
   u[n-1]=6.0*((y[n]-y[n-1])/(x[n]-x[n-1])-
               (y[n-1]-y[n-2])/(x[n-1]-x[n-2]));

   for (int i = 0; i <= n-1; ++i )
   {
    printf("u[%d]=%g\n", i, u[i]);
}
   mfio::Csv::Write("equation_correct.csv", a);
   gauss_jordan(n-1, u);

   u[0]=0.0;
   u[n]=0.0;
    return;
}

/*=============================================================*/
/*    spline interpolation                                     */
/*=============================================================*/
double spline(int n, double x[], double y[], double u[], double xx)
{
  int lo, hi, k;
  double a, b, c, d;
  double h, yy;

  lo=0;
  hi=n;

  while(hi-lo>1){
    k=(hi+lo)/2;
    if(xx < x[k]){
      hi=k;
      }else{
    lo=k;
      }
  }

  a=(u[hi]-u[lo])/(6.0*(x[hi]-x[lo]));
  b=u[lo]/2.0;
  c=(y[hi]-y[lo])/(x[hi]-x[lo])
    -1.0/6.0*(x[hi]-x[lo])*(2*u[lo]+u[hi]);
  d=y[lo];

  h=xx-x[lo];
  yy=a*h*h*h+b*h*h+c*h+d;

  return(yy);

}

/*=============================================================*/
/*    simple Gauss-Jordan method                               */
/*=============================================================*/
int gauss_jordan(int n, double b[])
{

  int ipv, i, j;
  double inv_pivot, temp;

  for(ipv=1 ; ipv <= n ; ipv++){

    inv_pivot = 1.0/a[ipv][ipv];
    for(j=1 ; j <= n ; j++){
      a[ipv][j] *= inv_pivot;
    }
    b[ipv] *= inv_pivot;

    for(i=1 ; i<=n ; i++){
      if(i != ipv){
    temp = a[i][ipv];
    for(j=1 ; j<=n ; j++){
      a[i][j] -= temp*a[ipv][j];
    }
    b[i] -= temp*b[ipv];
      }
    }
  }
  return 0;
}

/*==========================================================*/
/*   make a graph                                           */
/*==========================================================*/
void mk_graph(int n, double x[], double x1, double x2,
          double y[], double y1, double y2){
  int i;
  const char* data_file = "gpdata.txt";
  FILE *out;
  FILE *gp;

  /*  == make a data file ========= */

  out = fopen(data_file, "w");

  for(i=0; i<=n; i++){
    fprintf(out, "%e\t%e\n", x[i], y[i]);
  }

  fclose(out);

  /* == flowing lines make a graph by using gnuplot == */

  gp = popen("gnuplot -persist","w");

  fprintf(gp, "reset\n");
  fprintf(gp, "set terminal postscript eps color\n");
  fprintf(gp, "set output \"graph.eps\"\n");
  fprintf(gp, "set grid\n");

  /* -------  set x axis ---------*/

  fprintf(gp, "set xlabel \"%s\"\n", "x");
  fprintf(gp, "set nologscale x\n");
  fprintf(gp, "set xrange[%e:%e]\n", x1, x2);

  /* -------  set y axis ---------*/

  fprintf(gp, "set ylabel \"%s\"\n", "y");
  fprintf(gp, "set nologscale y\n");
  fprintf(gp, "set yrange[%e:%e]\n", y1, y2);

  /* -------  plat graphs ---------*/

  fprintf(gp, "plot \"%s\" using 1:2 with line\n", data_file);

  fprintf(gp, "set terminal x11\n");
  fprintf(gp, "replot\n");

  pclose(gp);
}

