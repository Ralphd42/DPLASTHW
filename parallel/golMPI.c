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
    struct slicevars sv ;
    int **msterData;
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
        printf( "Num PROC %d\n\n     ", num_proc);
        if( m%num_proc!=0      )
        {
           printf("Please choose number values so that m mod np =0");
           MPI_Finalize();
           exit(0);
        }
        printf( "- 1 - \n");
        //build arrays
        int lifArr [m][n];
        int ncntr, mcntr;
        srand(time(NULL));
        printf( "- 2 - \n");
        for(mcntr=0; mcntr < m ;mcntr++  )
        {
            for( ncntr=0;ncntr<n;ncntr++)
            {
                lifArr [m][n] = random01();
            }
        }  
        //struct slicevars sv;
        sv.k       =k       ;
        sv.m       =m       ;
        sv.n       =n       ;
        sv.slicecnt=slicecnt;
        // send this to all threads
        int rnkcnt;
        printf( "- 3 - \n");
        for(rnkcnt=1;rnkcnt< num_proc;++rnkcnt)
        {
            MPI_Send(&sv, sizeof(sv), MPI_BYTE, rnkcnt, 0, MPI_COMM_WORLD);
            printf( "- 4A - \n");
        }
        // chop up the array and send it
        int procSlice [slicecnt][n];
        msterData = (int**)  malloc(sizeof( int *) * slicecnt + sizeof(int) *n * slicecnt);
        
        
        
        printf( "- 5A - \n");
        for(rnkcnt=0;rnkcnt< num_proc;++rnkcnt)
        {
            int slcntr;
            for(slcntr=0;slcntr<slicecnt;slcntr++)
            {
                for( ncntr=0;ncntr<n;ncntr++)
                {
                    if( rnkcnt!=0)
                    {
                        procSlice [slcntr][ncntr] = lifArr[slcntr + (rnkcnt * slicecnt) ][ncntr];
                    }else
                    {
                        msterData[slcntr][ncntr] = lifArr[slcntr + (rnkcnt * slicecnt) ][ncntr];
                    }
                    
                }

            }
            int sliceSize = n* slicecnt;
            printf( "- MPI_Send(&procSlice, sliceSize,MPI_INT,  rnkcnt,1,MPI_COMM_WORLD); - \n");
            if (rnkcnt>0){
                MPI_Send(&procSlice, sliceSize,MPI_INT,  rnkcnt,1,MPI_COMM_WORLD);
            }
        }
    }// end master initializing
    printf("rnk %d ---after mster", rank);
    
    if(rank!=MASTER)
    {
        printf("rnk %d -A--after mster", rank);
        MPI_Recv(&sv, sizeof(sv), MPI_BYTE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    int rnkSlice[sv.slicecnt][sv.n];  
    int sliceSize = sv.n * sv.slicecnt;
    if(rank!=MASTER){
        printf("rnk %d -B--after mster", rank);
        MPI_Recv(&rnkSlice, sliceSize , MPI_INT, 0, 1, MPI_COMM_WORLD,&status);
    }else
    {   
        printf("rnk %d -B--after mster", rank);
        int ii,jj;
        for(ii=0; ii<sv.slicecnt;ii++ )
        {
            for(jj=0; jj<sv.n;jj++ )
            {
                rnkSlice[ii][jj]  =msterData[ii][jj];  

            }
        }

    }
    
    // run the game
    int lifecyclecnt =0;
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
            MPI_Recv(&getDown, n, MPI_INT,rank+1,1,MPI_COMM_WORLD,&status);
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
            MPI_Recv(&getUp, n, MPI_INT,rank-1,1,MPI_COMM_WORLD,&status);
        }
        int sliceRowcntr;
        int sliceColCntr;
        for(sliceRowcntr=0; sliceRowcntr<sv.slicecnt;++sliceRowcntr         )
        {
            
            for(sliceColCntr=0;sliceColCntr<n;++sliceColCntr)
            {
                int livecnt =0;
                if( 
                    sliceRowcntr>0 && 
                    //sliceColCntr>0 && 
                    sliceRowcntr<(  sv.slicecnt-1   ) 
                    //sliceColCntr<(sv.n-1)
                )
                {
                    int c =sliceColCntr-1;
                    if (c<0)
                    {
                        c=0;
                    }
                    int cend =sliceColCntr+1;
                    if( cend>sv.n)
                    {
                        cend =sv.n;
                    }
                    int r;
                    for(r=(sliceRowcntr-1); r<=(sliceRowcntr+1); ++r     )
                    {
                        for(c=sliceColCntr; c<=cend; ++c     )
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
                }
                // determine life of cell
                {
                    if (livecnt<=1 || livecnt>=4)
                    {
                         rnkSlice[sliceRowcntr][sliceColCntr]  =0;// dies
                    }
                    if( livecnt==2 || livecnt ==3)
                    {
                        rnkSlice[sliceRowcntr][sliceColCntr] =1; // Cell comes back to life
                    }
                }
            }
        }
        if(debug)
        {
            if (rank==MASTER) 
	        {
            // display master then rest then get rest
                int r,c =0;
                for(;r<sv.slicecnt;++r)
                {
                    for(;c<sv.n;++c)
                    {
                        printf("%d" ,rnkSlice[r][c]);// output here
                    }
                    printf("\n");
                }
                int irnkcnt=1;
                for(;irnkcnt<num_proc;++irnkcnt )
                {
                    int recvSlice[sv.slicecnt][sv.n];  
                    MPI_Recv(&recvSlice,sv.n * sv.slicecnt,MPI_INT, irnkcnt,1,MPI_COMM_WORLD,&status);
                    // display the rest
                    for(;r<sv.slicecnt;++r)
                {
                    for(;c<sv.n;++c)
                    {
                        printf("%d" ,recvSlice[r][c]);// output here
                    }
                    printf("\n");
                }
                }

            }else
            {
                MPI_Send(&rnkSlice, sv.n * sv.slicecnt,MPI_INT,  MASTER,1,MPI_COMM_WORLD);
            }
        }
    }
}

int random01()
{
    return ( rand() % ( 2) );
}

