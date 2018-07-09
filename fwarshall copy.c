//Finding a Least Cost Path   http://www.cs.rochester.edu/~nelson/courses/csc_173/graphs/apsp.html
// for directed or undirected graph
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
 
#define MAXLEN 512

/* Define Infinite as a large enough value. This value will be used
  for vertices not connected to each other */
#define INF 99999

void ReadFile(FILE *fp, int **Matrix, int arrsize);
void floydWarshell (int **g, int **p, int size);
// A function to print the solution matrix
void printSolution(int **dist, int size);
void printPath (int u, int v, int **p);

// driver program to test above function
int main(void)
{
	FILE *fp_r;
	char frname[MAXLEN], line[MAXLEN];
	int size, line_cnt, i, j;
	int **graph, **path;

	strcpy(frname, "C://Users//Gabrielle//Google ¶³ºÝµwºÐ//Magic Briefcase//results//project2//PWS_inoutflow.txt");  
 	
/*
	strcpy(frname, "../../results/project2/PWS_inoutflow.txt");  
	strcpy(fwname, "../../results/project2/v2_hc/pws_allpairs_shortestdist.txt");
*/    	
	
    fp_r = fopen(frname,"r");		/* open file pointer */     
	if(fp_r==NULL){
		printf("file error in line 105!!! \n");
		exit(1);
	}
	
	line_cnt=0;
	while(fgets(line, sizeof(line), fp_r) != NULL){
		//printf("line# %d line %s \n", line_cnt, line);
		++line_cnt;
	}
	size = line_cnt;
	//printf("size %d \n", size);
	fclose(fp_r);
	
	//ori. network: graph
	graph = (int **)malloc(size*sizeof(int *));
	path = (int **)malloc(size*sizeof(int *));
	if (graph == NULL || path == NULL){
		printf("Not enough memory\n");
		return 0;
	}	

	for (i = 0; i < size; i++)
	{
		graph[i] = (int *)malloc(sizeof(int) * size);
		path[i] = (int *)malloc(sizeof(int) * size);
		if (graph[i] == NULL || path[i] == NULL)
		{
			printf("Not enough memory\n");
			return 0;
		}
	}
	
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			graph[i][j] = 0;
			path[i][j] = 0;
		}
	}

   	fp_r = fopen(frname, "r");
	if(fp_r==NULL){
		printf("file error in line 143 \n");
		return 0;
	}
	ReadFile(fp_r, graph, size);
	
	//transform directed graph into undirected graph to calculate shortest path
	//if disable this loop then the algorithm will calculate shortest path for directed graph
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			if(graph[i][j]>0){
				graph[j][i] = 1;
			}
		}
	}
	
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			if (graph[i][j]==0 && i != j){
				graph[i][j] = INF;
			}
		}
	}
	
	// Print the solution
	floydWarshell(graph, path, size);
	
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			if (graph[i][j]==1 && i!=j)	printf(" %d -> %d dist = [%d] \n", i+1, j+1, graph[i][j]);
			else if (i != j){
				printf(" %d -> %d pass nodes [", i+1, j+1);
				printPath(i,j, path);
				printf(" ] \n");
			}
		}
	}
	
	
	for (i = 0; i< size; i++){
		free(graph[i]);
		free(path[i]);
	}
	free(graph);	
	free(path);	
	
	return 0;
}

void ReadFile(FILE *fp, int **Matrix, int arrsize)
{
	int i, j;
	
	for(i=0;i<arrsize;i++){
		for(j=0;j<arrsize;j++){  
			fscanf(fp,"%i",&Matrix[i][j]);
		}
	}
    fclose(fp);
}

// Solves the all-pairs shortest path problem using Floyd Warshall algorithm
void floydWarshell (int **g, int **p, int size)
{
    /* dist[][] will be the output matrix that will finally have the shortest 
      distances between every pair of vertices */
    int **dist, i, j, k;
 
	dist = (int **)malloc(size*sizeof(int *));
	if (dist ==NULL){
		printf("Not enough memory\n");
		exit(1);
	}	

	for (i = 0; i < size; i++)
	{
		dist[i] = (int *)malloc(sizeof(int) * size);
		if (dist[i] == NULL)
		{
			printf("Not enough memory\n");
			exit(1);
		}
	}
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			dist[i][j] = 0;
		
    /* Initialize the solution matrix same as input graph matrix. Or 
       we can say the initial values of shortest distances are based
       on shortest paths considering no intermediate vertex. */
    for (i = 0; i < size; i++){
        for (j = 0; j < size; j++){
            dist[i][j] = g[i][j];
			p[i][j] = -1;
		}
		dist[i][i] = 0;
	}	
 
    /* Add all vertices one by one to the set of intermediate vertices.
      ---> Before start of a iteration, we have shortest distances between all
      pairs of vertices such that the shortest distances consider only the
      vertices in set {0, 1, 2, .. k-1} as intermediate vertices.
      ----> After the end of a iteration, vertex no. k is added to the set of
      intermediate vertices and the set becomes {0, 1, 2, .. k} */
    for (k = 0; k < size; k++)
    {
        // Pick all vertices as source one by one
        for (i = 0; i < size; i++)
        {
            // Pick all vertices as destination for the
            // above picked source
            for (j = 0; j < size; j++)
            {
                // If vertex k is on the shortest path from
                // i to j, then update the value of dist[i][j]
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
					p[i][j] = k;
				}
            }
        }
    }

    // Print the shortest distance matrix
    printSolution(dist, size);
	 
	for (i = 0; i< size; i++){
		free(dist[i]);
	}
	free(dist);	
}

/* A utility function to print solution */
void printSolution(int **dist, int size)
{
	int i,j;
	
   //printf ("Following matrix shows the shortest distances"
   //         " between every pair of vertices \n");
   
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            if (dist[i][j] == INF)
                printf("%7s", "INF");
            else
                printf ("%7d", dist[i][j]);
        }
        printf("\n");
    }
}

void printPath (int u, int v, int **p)
{
	int k;
  
	k = p[u][v];
	if (k == -1) return;
	printPath(u,k, p);
	printf("%d ", k+1);
	printPath(k,v, p);
  
} /* Path */
