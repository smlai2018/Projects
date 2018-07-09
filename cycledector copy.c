/*
http://www.oocities.org/vienna/7079/src/graph.txt

For this program you will implement a graph cycle detector. The program
will
input a directed graph and output all simple cycles (see p88), if any,
that
exist in the graph. Specifically,

1.   Your program may use no global variables.

2.   Implement a graph data structure (either adjacency matrix or
adjacency list) to hold the directed graph. The graph must be allocated and
     deallocated at runtime. Implement procedures for graph allocation,
    graph deallocation, edge addition and edge lookup.

3.   Implement procedures for detecting (and outputting) cycles in a
given graph. A cycle should be output as a directed path starting from
the smallest vertex number, e.g., '1 -> 2 -> 3 -> 5 -> 1'. Cycles
should be output in non-decreasing order by their starting vertex, where
cycles beginning with the same vertex can be output in any order. If the
graph contains no cycles, then simply output 'No cycles'.

4.   The main part of your program will read in data from a file whose
name is given as the first argument to your executable. The first line
of the file contains the number of vertices (1-100) and edges (1-1000)
in the graph. The remaining lines define the edges in the graph in the
    form 'e 5 3', which means there is a directed edge in the graph from
    vertex 5 to vertex 3. Your main program should read in the graph and
    then call your procedure to output any cycles. 

	TEST CASE: cycle.txt
	RUN COMMAND: >cyclesdector.exe cycle.txt
	
*/


/* Anan Tongprasith */
/* Compile: cc graph.c */
/**********************/
#include<stdio.h>
#include<stdlib.h>

/* This program use adjacency matrix */

/************* list of all functions **************/

/* insert a new row in the middle of array ba */
int arrayshift(int ba[100][48],int i,int nodenum);

/* detect a cycle */
int cycledetec(int **A,int startp,int nodenum);

/* add an edge, m=start vertice, n=end vertice */
void graphadd(int **A,int m,int n);

/* allocate memory for the matrix */
int **graphalloc(int nodenum);

/* free memory */
void graphfree(int **A,int nodenum);

/**************************************************/

int main(int argc,char *argv[])
{ FILE *fp;char newline[10],*tempch=" ";
  int nodenum,edgenum,i,j,**A,mybegin,myend;

/* open the input file */
	fp=fopen(argv[1],"r");
	fgets(newline,10,fp);
	
/* reading # of node */
	sscanf(newline,"%i %i",&nodenum,&edgenum);
	//printf("nodenum[%d] edgenum[%d] \n", nodenum, edgenum);
	
/* allocating the adjacency matrix */
	A=graphalloc(nodenum);
	
/* reading the input file and adding to the matrix*/
	while(fgets(newline,10,fp) != NULL)
	{ 
	   //sscanf(newline,"%s %i %i",tempch,&mybegin,&myend);
	   sscanf(newline,"%i %i",&mybegin,&myend);
	   //printf("newline[%s] mybegin[%i] myend[%i]\n", newline, mybegin, myend);
	   
	   graphadd(A,mybegin-1,myend-1);
	}
	fclose(fp);
	j=0;
	
	/*
	for(i=0;i<nodenum;i++)
	{
	  for(j=0;j<nodenum;j++)
	  	printf("%i ",A[i][j]);
	  printf("\n");
	}
	printf("\n");
	*/
	
/* detecting cycle from starting point i (1,2,3,4,5) */
	for(i=0;i<nodenum;i++)
		if(cycledetec(A,i,nodenum)==1) j+=1;
	
	//printf("test 2 \n");
	if(j==0)
		printf("No cycle\n");

/* freeing the matrix */
	graphfree(A,nodenum);
}

/* detect cycle beginning from startp */
/* how? brute force */
/* 1. find all existing paths with distance = mydist*/
/*    those paths must have really existing subpaths */
/* 2. put all those paths into "bigarray" */
/* 3. checking if those paths have redundant node? if so, throw away */
/* 4. checking if there is a path from the last node to the first node*/
/* 5. increase mydist and go back to 1. */
/****************************************/

int cycledetec(int **A,int startp,int nodenum)
{ int maxdist=nodenum-startp,cycle=0,mydist,i,subdist,bigarray[100][48];
  int index,j,k,l;
  int itmp, jtmp;

	/* checking for distance = 1, for examples, 4 -> 4, 3 -> 3 */ 
	if(A[startp][startp]==1)		/* mydist=0 */
	{  printf("%i -> %i\n",startp+1,startp+1);
	   cycle=1;
	}

	/* checking for distance > 1 */
	/****** outer loop check from distance = 2 to maximum distance
	(when startp=1) ********/

	for(mydist=1;mydist<maxdist;mydist++) 
	{	
		for(i=0;i<100;i++)	/* assume possible route < 100 */
		{  for(index=0;index<nodenum;index++)
				bigarray[i][index]=-1;
	   	   bigarray[i][0]=startp;
		}		/* this loop initialize the bigarray */
		index=0;
		
		
		
		
		/******* now start to fill in the bigarray **********/
		/******* each row in the big array is an existing route *********/
		/******* each possible route is already checked for existance of*****/
		/******* subpath before being put in the bigarray ******/
		/******* the bigarray are cleared when start looking the next distance */

		/* this subloop keep checking until reaching the wanted distance */	
		for(subdist=1;subdist<=mydist;subdist++)  
		{
		   index=0;

		/* node in the middle should be numbered less than starter */
		   for(i=startp+1;i<nodenum;i++)
		   {
			if(bigarray[index][subdist-1]>=0&&A[bigarray[index][subdist-1]][i]!=0) /****** checking path from last point to current point *****/
			{	bigarray[index][subdist]=i;
				arrayshift(bigarray,index,nodenum);
				index+=1;
				bigarray[index][subdist-1]=bigarray[index-1][subdist-1];
			}
		   }
		}	
		/* After reaching the wanted distance, checking path from the
			last nodes in all existing path back to the starting point */
		
		/*
		printf("\n check big array \n");
		for(itmp=0;itmp<100;itmp++){
			for(jtmp=0;jtmp<20;jtmp++){
				printf("%d ", bigarray[itmp][jtmp]);
			}
			printf("\n");
		}
		printf("----------------------------------------\n");
		*/
		
		for(i=0;i<index;i++)
		{	l=0;

			/* this loop check for repeated node */
			/* ex. 1->3->3->4 , throw it away*/
			for(j=0;j<=mydist;j++)
			{   for(k=0;k<=mydist;k++)
			    {	if(j!=k && bigarray[i][j]==bigarray[i][k])
					l+=1;
			    }
			}

			/* no repeated node? check for path from */
			/* the last node to startp */

			if(l==0&&A[bigarray[i][mydist]][startp]==1)
			{  for(j=0;j<=mydist;j++)
			   	printf("%i -> ",bigarray[i][j]+1);
			   printf("%i\n",startp+1);
			   cycle=1;	/* found a cycle */
			}
		}
	} 
	return cycle;
}
/* making rooms */
int arrayshift(int ba[100][48],int index,int nodenum)
{ int i,j;
	for(i=98;i>index;i--)
	{  for(j=0;j<nodenum;j++)
		ba[i+1][j]=ba[i][j];
	}
}
void graphadd(int **A,int m,int n)
{	A[m][n]=1;
}
int **graphalloc(int nodenum)
{ int **A,i,j;
	A=malloc(nodenum*8);
	for(i=0;i<nodenum;i++)
	{ A[i]=malloc(nodenum*4);
	  for(j=0;j<nodenum;j++)
		A[i][j]=0;
	}
	return A;
}
void graphfree(int **A,int nodenum)
{ int i;
	for(i=0;i<nodenum;i++)
	{ free(A[i]);
	}
	free(A);
}
