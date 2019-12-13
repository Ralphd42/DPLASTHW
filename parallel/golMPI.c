#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include "mpi.h"

#define debug 1
#define MASTER 0

struct slicevars
{
    int k;
    int m;
    int n;
    int slicecnt;
}slicevars;



int random01();
void PrintArray( int * arr, int m, int n);
int numLiveNeighbors(int * arr, int currm,int currn, int m,int n);
int main(int argc, char*argv[])
{
    int rc;
    int num_proc;
    int rank;
    int m,n;
    int k;
    int slicecnt;


    rc =MPI_Init(&argc,&argv);
    if(rc!=MPI_SUCCESS)
    {
            printf("Failed to start MPI");
            MPI_Abort(MPI_COMM_WORLD,rc);    
    }
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank    );
    if(rank ==MASTER)
    {
        /*
            master will handle slicing the array and splitting between slaves
            1) do we have a valid number of processors
        
            MPI_Send (&buf,count,datatype,dest,tag,comm)
        */
       if(argc <4)
        {
            printf( "Please provide values for M N and K");   
            MPI_Finalize();
            exit (1);

        }
        m = atoi(argv[1]);
        n = atoi(argv[2]);
        k = atoi(argv[3]);
        slicecnt = m/num_proc;


       if( m%num_proc!=0      )
       {
           printf("Please choose number values so that m%np =0");
           MPI_Finalize();
           exit(0);
        }
        
        //build arrays
        int lifArr [m][n];
        int ncntr;
        srand(time(NULL));
        for(mcntr=0; mcntr < m ;mcntr++  )
        {
            for( ncntr=0;ncntr<n;ncntr++)
            {
                lifArr [m][n] = random01();
            }
        }  
        struct slicevars sv;

        sv.k       =k       ;
        sv.m       =m       ;
        sv.n       =n       ;
        sv.slicecnt=slicecnt;
        // send this to all threads
        int rnkcnt;
        for(rnkcnt=0;rnccnt< num_proc;++rnkcnt)
        {
             
            MPI_Send(&sv, sizeof(sv), MPI_BYTE, rnkcnt, 0, MPI_COMM_WORLD);
        }
        // chop up the array and send it
        int procSlice [slicecnt][n];
        for(rnkcnt=0;rnkcnt< num_proc;++rnkcnt)
        {
            int slcntr;
            for(slcntr=0;slcntr<slicecnt;slcntr++)
            {
                for( ncntr=0;ncntr<n;ncntr++)
                {
                    procSlice [slcntr][ncntr] = lifArr[slcntr + (rnkcnt * slicecnt)      ][ncntr];
                }
            }
            int sliceSize = n* slicecnt;
            MPI_Send(&procSlice, sliceSize,MPI_INT,  rnkcnt,1,MPI_COMM_WORLD);
        }
    }
    struct slicevars sv ;
    MPI_Recv(&sv, sizeof(sv), MPI_BYTE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
    int rnkSlice[sv.slicecnt][sv.n];  
    int sliceSize = sv.n * sv.slicecnt;
    MPI_Recv(&rnkSlice, sliceSize , MPI_INT, 0, 1, MPI_COMM_WORLD,&status);
    // run the game
    int lifecyclecnt =0;
    
    //

    // need arrays to hold items from other ranks

    int sendDown[n]; // send bottom to rank belo
    int sendUp[n]; // send top to rank above
    int getDown[n]; //get item from below rank
    int getUp[n];  //get item from above rank
    for ( lifecyclecnt=0;lifecyclecnt<sv.k;++lifecyclecnt)
    {
        //send bottom row down
        // except for last
        int ncntr ;
        if(rank!= num_proc-1)
        {
            for (ncntr =0; ncntr<n;n++)
            {
                sendDown[ncntr] = rnkSlice[sv.slicecnt-1][ncntr];
            }
            MPI_Send(&sendDown, sliceSize,MPI_INT,  (rank+1),1,MPI_COMM_WORLD);
            MPI_Recv(&getDown, n, MPI_INT,rank+1,MPI_COMM_WORLD,&status);
        }
        //send top row up
        // all except first
        if(rank!=MASTER)
        {
            for (ncntr =0; ncntr<n;n++)
            {
                sendUp[ncntr] = rnkSlice[0][ncntr];
            }
            MPI_Send(&sendUp, sliceSize,MPI_INT,  (rank-1),1,MPI_COMM_WORLD);
            MPI_Recv(&getUp, n, MPI_INT,rank-1,MPI_COMM_WORLD,&status);
        }
        int sliceRowcntr;
        int sliceColCntr;
        for(sliceRowcntr=0; sliceRowcntr<sv.slicecnt;++sliceRowcntr         )
        {
            for(sliceColCntr=0;sliceColCntr<n;++sliceColCntr)
            {
                int livecnt =0;
                //the fun part find out how many alive cells surround the current cell
                //special case has no top row
                
                //most of the cases
                if( 
                    sliceRowcntr>0 && 
                    sliceColCntr>0 && 
                    sliceRowcntr<(  sv.slicecnt-1   ) &&
                    sliceColCntr<(sv.n-1)
                )
                {
                    int r;
                    int c;
                    for(r=sliceRowcntr; r<=(sliceRowcntr+1); ++r     )
                    {
                        for(c=sliceColCntr; c<=(sliceColCntr+1); ++c     )
                        {
                            if( r!= sliceRowcntr && 
                                c != sliceColCntr &&  
                                rnkSlice[sliceRowcntr][sliceColCntr]     
                            )
                            {
                                ++livecnt;
                            }
                        }
                    }
                }else if (sliceRowcntr==0)
                { //bottom
                    // need to use bottom then others


                }
                
                
                
                if( rank==MASTER){
                    
                }
                // special case has no bottom
                if(rank ==num_proc-1)
                {}

                for(      )









            }
        }
        

        // get top row all excpt first














    }

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