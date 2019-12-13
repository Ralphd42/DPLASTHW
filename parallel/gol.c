#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include "mpi.h"

#define debug 1
#define serial 1
#define MASTER 0


int random01();
void PrintArray( int * arr, int m, int n);
int numLiveNeighbors(int * arr, int currm,int currn, int m,int n);
int main(int argc, char*argv[])
{
    srand(time(NULL));
    int m;
    int n;
    int k;
    //m =9;
    //n =4;
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    k = atoi(argv[3]);






    // build array
    if(argc <4)
    {
        printf( "Please provide values for M N and K");   
        exit (1);

    }

    m = atoi(argv[1]);
    n = atoi(argv[2]);
    k = atoi(argv[3]);



    // build array
    int numitems = m*n;
    int * DataArr = (int*)malloc ( m * n * sizeof(int));
    int mcntr;
    int ncntr;
    
    int lifeboard [m][n];
    for(mcntr=0; mcntr < m ;mcntr++  )
    {
        for( ncntr=0;ncntr<n;ncntr++)
        {
            int rval =random01();
            lifeboard [mcntr][ncntr] = rval;//random01();
            printf(" !!@setting %d %d @!! ", rval, lifeboard [mcntr][ncntr]);
        }
        printf("\n");
    }    

    for(mcntr=0; mcntr < m ;mcntr++  )
    {
        for( ncntr=0;ncntr<n;ncntr++)
        {
            printf(" %d " ,lifeboard [mcntr][ncntr]);
        }
        printf(" \n\n ");
    }
    return 1;

    for(mcntr=0; mcntr < m ;mcntr++  )
    {
        for( ncntr=0;ncntr<n;ncntr++)
        {
            *(DataArr+mcntr*m + ncntr) = random01();
        }
    }
    //return 1;
    PrintArray(DataArr,m,n);
    int curriteration =0;
    return 1;
    for(;curriteration<k;++curriteration)
    {
        // loop all items
        for(mcntr=0; mcntr < m ;mcntr++  )
    {
        for( ncntr=0;ncntr<n;ncntr++)
        {
            int curstate = *(DataArr+mcntr*m + ncntr);
            // do all checks
            
            int lvct =numLiveNeighbors(DataArr, mcntr,mcntr,m, n);
            if (lvct<=1 || lvct>=4)
            {
                *(DataArr+mcntr*m + ncntr) =0;// dies

            }
            if( lvct==2 || lvct ==3)
            {
                *(DataArr+mcntr*m + ncntr) =1; // Cell comes back to life

            }

        }
    }
       PrintArray(DataArr,m,n);



    }




    return 0;
    
    int rc;
    int num_proc;
    int rank;
    if(! serial)
    {
        rc =MPI_Init(&argc,&argv);
        if(rc!=MPI_SUCCESS)
        {
            printf("Failed to start MPI");
            MPI_Abort(MPI_COMM_WORLD,rc);    
        }

    }
    int i=0;
    srand(time(NULL));
   



}
int random01()
{
    return ( rand() % ( 2) );
}
void PrintArray( int * arr, int m, int n)
{
    int mcntr;
    int ncntr;

    for( mcntr=0; mcntr<m;++mcntr)
    {
        for( ncntr =0; ncntr <n; ++ncntr)
        {
           // printf("%d " ,*(arr + mcntr*m + ncntr    ));
        }
        printf("\n");
    }
}
int numLiveNeighbors(int * arr, int currm,int currn, int m, int n)
{
    int livecount =0;
    int mcntr;
    mcntr = currm-1;
    if (mcntr<0)
    {
        mcntr =0;
    }
    int ncntr = currn-1;
    if( ncntr<0)
    {
        ncntr =0;
    }
    int  mend =currm+1;
    if (mend>=m)
    {
        mend = currm;
    }

    int  nend =currn+1;
    if (nend>=n)
    {
        nend = currn;
    }

    for( ;mcntr<mend; ++mcntr )
    {
        for( ;ncntr<nend; ++ncntr )
        {
            if(ncntr!=currn  && mcntr!=currm)
            {
                if(*(arr +currm* m + ncntr)==1)
                {
                    ++livecount;
                }
            }
        }
    }
    return livecount;


}

