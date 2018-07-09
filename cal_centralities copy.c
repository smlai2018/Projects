#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

#define MAXLEN 512
#define col 2

struct path {
    int start;
	int dest;
	int len;
	char *visit;
    struct path *next;
};
struct path *path_parent=NULL;

struct list {
	int vertex;
	struct list * next;
};
struct list *list_parent=NULL;

void info(float**Matrix, int arrsize, int vertex);
void clo(float**Matrix, int arrsize, int vertex);
int existed(char *path, int vertex);
void btw(float**Matrix, int arrsize, int vertex);
void insPath(int start, int dest, int len, char *visit);
char *string_concat(char *str1, char *str2);
void paths(float **Matrix, int start, int dest, int arrsize, int num_neighbor, int chk_neighbor);
void ReadFile(FILE *fp, float **Matrix, int arrsize);

//---------------------------------------------------------------------------------
//   Function:   	main(void) 
//   Description: 	calculate betweenness, closeness and infomation index 
//					same result as paper or textbook but for large network, different results from Ucinet, iGraph and statnet
//
//   Programmer:  	S.M. Lai
//   
//   Date:        	07/31/2013
// 
//   Version:     	1.0
//  
//   Environment: 	
//                	Software:   Microsoft VISTA with MinGW for execution; 
//                	Compiles under MinGW C
//
//   Calls:	
//
//   Parameters: 
//
//	 Returns:	EXIT_SUCCESS
//	 Reference:
//				
// ------------------------------------------------------------------------------

int main(void)
{
	FILE *fp_r, *fp_w;
	char frname[MAXLEN], line[MAXLEN], fwname_btw[MAXLEN], fwname_clo[MAXLEN], fwname_info[MAXLEN];
	int line_cnt, size, i, j;
	float **mdat, **mdat_btw, **mdat_clo, **mdat_info;
	struct path *test;
	
	strcpy(frname, "C://Users//Gabrielle//Documents//Magic Briefcase//results//project2//pws_sym.txt");  
	strcpy(fwname_btw, "C://Users//Gabrielle//Documents//Magic Briefcase//results//project2//pws_sym_btw.txt");
	strcpy(fwname_clo, "C://Users//Gabrielle//Documents//Magic Briefcase//results//project2//pws_sym_clo.txt");
	strcpy(fwname_info, "C://Users//Gabrielle//Documents//Magic Briefcase//results//project2//pws_sym_info.txt");
	
    fp_r = fopen(frname,"r");		/* open file pointer */     
	if(fp_r==NULL){
		printf("file error!!! \n");
		exit(1);
	}
	
	line_cnt=0;
	while(fgets(line, sizeof(line), fp_r) != NULL){
		//printf("line# %d line %s \n", line_cnt, line);
		++line_cnt;
	}
	size = line_cnt;
	printf("size %d \n", size);
	fclose(fp_r);
	
	//ori. network: mdat
	mdat = (float **)malloc(size*sizeof(float *)); 
	mdat_btw = (float **)malloc(size*sizeof(float *)); 
	mdat_clo = (float **)malloc(size*sizeof(float *)); 
	mdat_info = (float **)malloc(size*sizeof(float *)); 
	if (mdat == NULL || mdat_btw ==NULL || mdat_clo ==NULL || mdat_info ==NULL)
	{
		printf("Not enough memory\n");
		return;
	}
	for (i = 0; i < size; i++)
	{
		mdat[i] = (float *)malloc(sizeof(float) * size);
		if (mdat[i] == NULL)
		{
			printf("Not enough memory\n");
			return;
		}
	}
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			mdat[i][j] = 0;
		}
	}
	
	for (i = 0; i < size; i++)
	{
		mdat_btw[i] = (float *)malloc(sizeof(float) * col);
		mdat_clo[i] = (float *)malloc(sizeof(float) * col);
		mdat_info[i] = (float *)malloc(sizeof(float) * col);
		if (mdat_btw[i] == NULL || mdat_clo[i] == NULL || mdat_info[i] == NULL)
		{
			printf("Not enough memory\n");
			return;
		}
	}

	for (i = 0; i < size; i++){
		for (j = 0; j < col; j++){
			if (j==0){ 
				mdat_btw[i][j]=i;
				mdat_clo[i][j]=i;
				mdat_info[i][j]=i;
			}
			else{
				mdat_btw[i][j]=0;
				mdat_clo[i][j]=0;
				mdat_info[i][j]=0;
			}
		}
	}
	
	fp_r = fopen(frname,"r");		/* open file pointer */     
	if(fp_r==NULL){
		printf("file error!!! \n");
		exit(1);
	}
	ReadFile(fp_r, mdat, size);

	for (i = 0; i < size; i++){
		for (j = i+1; j < size; j++){
			paths(mdat, i, j, size, -1, -1);
			list_parent=NULL;
		}
	}
	
	test = path_parent;
	while(test != NULL){
		printf("[%d->%d] visit[%s] \n", test->start, test->dest, test->visit);
		test = test->next;
	}
	printf("\n");
	
	//betweenness centrality
	for (i = 0; i < size; i++){
		printf("----------------------btw:[%d] --------------------\n", i);
		btw(mdat_btw, size, i);
	}
	/*
	for (i = 0; i < size; i++){
		mdat_btw[i][1] = mdat_btw[i][1] -(size-1);
	}
	*/
	fp_w = fopen(fwname_btw,"w"); 
	if(fp_w==NULL){
		printf("file error!!! \n");
		exit(1);
	}
	for(i=0;i<size;i++){
		fprintf(fp_w, "%8.3f\n", mdat_btw[i][1]);
	}
	fclose(fp_w);
	
	//closeness centrality
	for (i = 0; i < size; i++){
		printf("----------------------clo:[%d] --------------------\n", i);
		clo(mdat_clo, size, i);
	}
	fp_w = fopen(fwname_clo,"w"); 
	if(fp_w==NULL){
		printf("file error!!! \n");
		exit(1);
	}
	for(i=0;i<size;i++){
		fprintf(fp_w, "%8.3f\n", mdat_clo[i][1]);
	}
	fclose(fp_w);
	
	//information centrality
	for (i = 0; i < size; i++){
		printf("----------------------info:[%d] --------------------\n", i);
		info(mdat_clo, size, i);
	}
	fp_w = fopen(fwname_info,"w"); 
	if(fp_w==NULL){
		printf("file error!!! \n");
		exit(1);
	}
	for(i=0;i<size;i++){
		fprintf(fp_w, "%8.3f\n", mdat_info[i][1]);
	}
	fclose(fp_w);
	
	
	for (i = 0; i< size; i++){
		free(mdat[i]);
		free(mdat_btw[i]);
		free(mdat_clo[i]);
		free(mdat_info[i]);
	}
	free(mdat);
	free(mdat_btw);
	free(mdat_clo);
	free(mdat_info);
	
	return 0;
}

void info(float**Matrix, int arrsize, int vertex){
	int i, iflag=0;
	struct path *ptr;
	float fsum;
	
	fsum=0;
	for (i = 0; i< arrsize; i++){
		if(vertex != i){
			printf("[clo] [%d->%d] \n", vertex, i);
			ptr = path_parent;
			while(ptr != NULL && iflag==0){
				if (vertex < i){
					if(ptr->start == vertex && ptr->dest == i){
						//printf("[%d->%d] len[%d] \n", ptr->start, ptr->dest, ptr->len);
						if ( ((ptr->len)-1) > 0 ){
							fsum = fsum + (1/((ptr->len)-1))*1000;
							iflag=1;
						}
					}
				}
				else{
					if(ptr->start == i && ptr->dest == vertex){
						//printf("[%d->%d] len[%d] \n", ptr->start, ptr->dest, ptr->len);
						if ( ((ptr->len)-1) > 0 ){
							fsum = fsum + (1/((ptr->len)-1))*1000;
							iflag=1;
						}
					}
				}
				ptr = ptr->next;
			}
			printf("final sum[%8.3f] \n", fsum);
			Matrix[vertex][1] = arrsize * fsum;
				
			iflag=0;
		}	//if(vertex != i)
	}
}

void clo(float**Matrix, int arrsize, int vertex){
	int i, iflag=0;
	struct path *ptr;
	float fsum;
	
	fsum=0;
	for (i = 0; i< arrsize; i++){
		if(vertex != i){
			printf("[clo] [%d->%d] \n", vertex, i);
			ptr = path_parent;
			while(ptr != NULL && iflag==0){
				if (vertex < i){
					if(ptr->start == vertex && ptr->dest == i){
						//printf("[%d->%d] len[%d] \n", ptr->start, ptr->dest, ptr->len);
						fsum = fsum + ((ptr->len)-1);
						iflag=1;
					}
				}
				else{
					if(ptr->start == i && ptr->dest == vertex){
						//printf("[%d->%d] len[%d] \n", ptr->start, ptr->dest, ptr->len);
						fsum = fsum + ((ptr->len)-1);
						iflag=1;
					}
				}
				ptr = ptr->next;
			}
			printf("final sum[%8.3f] \n", fsum);
			if(fsum != 0)
				Matrix[vertex][1] = 1/ fsum;
				
			iflag=0;
		}	//if(vertex != i)
	}
}

int existed(char *path, int vertex){
	int iflag;
	char *token;
	
	//http://csm00.csu.edu.tw/0391/c/function3.pdf
	iflag=0;
	token = strtok(path, ",");
	while(token != NULL){
		if(atoi(token)==vertex)	iflag=1;
		token = strtok(NULL, ",");
	}
	
	return iflag;
}

void btw(float**Matrix, int arrsize, int vertex){
	struct path *tmp;
	int i, j, iflag, length;
	char *path;
	float ftot, fcnt, fbtw;
	
	ftot=0;
	fcnt=0;
	fbtw=0;
	iflag=0;
	for (i = 0; i < arrsize; i++){
		for (j = i+1; j < arrsize; j++){
			printf("[%d -> %d] Vertex[%d] \n", i, j, vertex);
			tmp = path_parent;
			while(tmp != NULL){
				if(tmp->start ==i && tmp->dest == j){ 
					ftot++;
					length = (strlen(tmp->visit)+1);
					path = (char*)malloc(sizeof(char) * length);  
					strcpy(path, tmp->visit);  
					printf("[btw] path[%s]\n", path);
					iflag=existed(path, vertex);
					if(iflag==1)	fcnt++;
				}
				tmp = tmp->next;
			}	//while(tmp != NULL)
			if(ftot!=0){
				fbtw=fbtw+(fcnt/ftot);
				printf("fcnt[%5.2f] / ftot[%5.2f] = %5.2f \n", fcnt, ftot, (fcnt/ftot));
			} 
			
			ftot=0;
			fcnt=0;
			free(path);
		}
	}
	//printf("final = %5.2f \n", fbtw/(((arrsize-1)*arrsize)/2));
	//Matrix[vertex][1] = fbtw/(((arrsize-1)*arrsize)/2);
	Matrix[vertex][1] = fbtw;
}

void insPath(int start, int dest, int len, char *visit){
	int i, j, ifound, ishortpath;
	struct path *current=NULL, *tmp=NULL, *prev=NULL, *insnode=NULL, *delnode=NULL;
	
	if(path_parent==NULL){
		current = (struct path *)malloc(sizeof(struct path));
		if (current == NULL) exit(EXIT_FAILURE);
		current->start = start;
		current->dest = dest;
		current->len = len;
		current->visit =visit;
		current->next = NULL;
		path_parent = current;
		return;
	}
	else{
		ifound=0;
		ishortpath=9999;
		prev = path_parent;
		tmp = path_parent;
		while(tmp != NULL && ifound==0){
			if(tmp->start == start && tmp->dest ==dest){
				ishortpath = tmp->len;
				ifound=1;
			}
			else{
				prev = tmp;
				tmp = tmp->next;
			}
		}
		if(ifound==0 || ishortpath == len){	//not found and len == shortpath
			current = (struct path *)malloc(sizeof(struct path));
			if (current == NULL) exit(EXIT_FAILURE);
			current->start = start;
			current->dest = dest;
			current->len = len;
			current->visit = visit;
			current->next = NULL;
			
			insnode = path_parent; ;
			while(insnode->next != NULL){
				insnode = insnode->next;
			}
			insnode->next = current;
		}	// not found and len == shortpath
		else if(ifound==1 && ishortpath > len){
			tmp = path_parent;
			if(tmp->next == NULL){
				current = (struct path *)malloc(sizeof(struct path));
				if (current == NULL) exit(EXIT_FAILURE);
				current->start = start;
				current->dest = dest;
				current->len = len;
				current->visit = visit;
				current->next = NULL;
				path_parent = current;
			}
			else{
				//del longer node first
				prev = path_parent;
				tmp = path_parent;
				while(tmp != NULL){
					if (tmp->start == start && tmp->dest == dest){
						if(tmp == path_parent){
							path_parent = path_parent->next;
						}
						delnode = tmp;
						if(tmp->next == NULL){
							tmp = NULL;
							prev->next = NULL;
						}
						else{ 
							tmp = tmp->next;
							prev->next = tmp;
						}
						free(delnode);
					}	//tmp->start == start && tmp->dest == dest
					else{
						prev = tmp;
						tmp = tmp->next;
					}
				}
				
				//insert short node
				current = (struct path *)malloc(sizeof(struct path));
				if (current == NULL) exit(EXIT_FAILURE);
				current->start = start;
				current->dest = dest;
				current->len = len;
				current->visit = visit;
				current->next = NULL;
			
				insnode = path_parent; ;
				while(insnode->next != NULL){
					insnode = insnode->next;
				}
				insnode->next = current;
				
			}
		}	//ifound==1 && ishortpath > len
	}	//ELSE path_parent != NULL
}

void paths(float **Matrix, int start, int dest, int arrsize, int num_neighbor, int chk_neighbor){
	struct list *current=NULL, *tmp=NULL, *head_neighbor=NULL, *neighbor=NULL, *prev=NULL, *weble=NULL;
	int ihead, ipathloop, i, ineighbor, ilen;
	char* visitnode;
	char snum[3];
	
	//record start vertex
	current = (struct list *)malloc(sizeof(struct list));
	if (current == NULL) exit(EXIT_FAILURE);
	current->vertex = start;
	current->next = NULL;
	if (list_parent == NULL) list_parent = current;
	else{
		tmp = list_parent ;
		while(tmp->next != NULL){
			tmp = tmp->next;
		}
		tmp->next = current;
	}

	//output all elements of previous, and return
	//printf("start[%d] dest[%d] \n", start, dest);
	if (start == dest){
		ilen=0;
		visitnode = malloc(1);
		visitnode[0] = '\0';
		
		tmp = list_parent;
		ihead = list_parent->vertex;
		while(tmp != NULL){
			//printf("%d -> ", tmp->vertex);
			visitnode=string_concat(visitnode, itoa(tmp->vertex, snum, 10));
			visitnode=string_concat(visitnode, ",");
			ilen=ilen+1;
			tmp = tmp->next;
		}
		visitnode[strlen(visitnode)-1] = '\0'; 
		//printf("\n");
		
		//printf("start-dest[%d %d] len[%d] visits[%s] \n", ihead, dest, ilen, visitnode);
		insPath(ihead, dest, ilen, visitnode);
		
		return;
	}
	else if(start != dest && num_neighbor == chk_neighbor++ && num_neighbor >=0){
		
		return;
	}
	
	//record neighbors of current i, trace its neighbor recursive later on
	ineighbor=0;
	for (i = 0; i < arrsize; i++){
		if (Matrix[start][i]>0){
			ipathloop=0;
			tmp = list_parent;
			while(tmp != NULL){
				if(tmp->vertex == i) ipathloop=1;
				tmp = tmp->next;
			}
			if(ipathloop==0){
				neighbor = (struct list *)malloc(sizeof(struct list));
				neighbor->vertex = i;
				neighbor->next = NULL;
				if(head_neighbor==NULL)	head_neighbor = neighbor;
				else {
					tmp = head_neighbor;
					while(tmp->next != NULL){
						tmp = tmp->next;
					}
					tmp->next = neighbor;
				}
				ineighbor = ineighbor +1;
			}
		}	//if (Matrix[start][i]>0)
	}
	
	/*
	printf("%d's neighbors:[ ", start);
	weble = head_neighbor;
	while(weble != NULL){
		printf("%d- ", weble->vertex);
		weble = weble->next;
	}
	printf("] \n");
	*/
	
	for (i = 0; i<ineighbor; i++){
		tmp = head_neighbor;
		if (head_neighbor->next != NULL) head_neighbor = head_neighbor->next;
		tmp->next=NULL;
		//printf("start[%d] recursive[%d] [%d %d] \n", start, tmp->vertex, ineighbor, i);
		paths(Matrix, tmp->vertex, dest, arrsize, ineighbor, i);
		// free neighbor node
		free(tmp);
		
		//free nodes which has been printed
		tmp = list_parent;
		prev = list_parent;
		while(list_parent !=NULL && tmp->next!=NULL){
			prev = tmp;
			tmp = tmp->next;
		}
		prev->next = NULL;
		free(tmp);
	}
}

/* string_concat [字串相加] 
* 將 str1 與 str2 相加，並返回新的字串 
* dynamic allocate string size:http://jax-work-archive.blogspot.tw/2009/04/c_7407.html
* */  
char *string_concat(char *str1, char *str2) {  
	// 計算所需的陣列長度  
	int length=strlen(str1)+strlen(str2)+1;  
     
	// 產生新的陣列空間  
	char *result = (char*)malloc(sizeof(char) * length);  
     
	// 複製第一個字串至新的陣列空間  
	strcpy(result, str1);  
	// 串接第二個字串至新的陣列空間  
	strcat(result, str2); 
	
   return result;  
}

/*讀檔案內的矩陣到陣列*/
void ReadFile(FILE *fp, float **Matrix, int arrsize)
{
	int i, j;
	
	for(i=0;i<arrsize;i++){
		for(j=0;j<arrsize;j++){  
			fscanf(fp,"%f",&Matrix[i][j]);
		}
	}
    fclose(fp);
}
