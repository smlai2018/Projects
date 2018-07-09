#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

#define col 1001    
#define MAXLEN 512
#define sum_col 4

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) ((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )

void ReadFile(FILE *fp, float **Matrix, int arrsize);
void net2sym_v2(float **Matrix, int arrsize, char nettype);
void net2sym(float **Matrix, int arrsize);
void sym2ratio(float **Matrix_ratio, float **Matrix, int arrsize);
void Matrix_Mult(float **tiMdat, float **multMdat, float **m1, int size, int steps);
void Matrix_Add(float **tiMdat, float **multMdat, int size);
void Matrix_Normalize(float **tiMdat, int size, int steps);
int quickSort(float array[], int left, int right);

//---------------------------------------------------------------------------------
//   Function:   main(void) 
//   Description: 
//                calculation of Ti and To  
//
//   Programmer:  S.M. Lai
//   
//   Date:        10/31/2011
// 
//   Version:     1.0
//  
//   Environment: PC
//                Software:   Microsoft VISTA with MinGW for execution; 
//                Compiles under MinGW C
//
//      Calls:	
//      
//
//      Parameters: 
//
//	Returns:	EXIT_SUCCESS
// ------------------------------------------------------------------------------

int main(void)
{
	FILE *fp_r, *fp_w_ti, *fp_w_to, *fp_w;
	char frname[MAXLEN], line[MAXLEN], fwname_ti[MAXLEN], fwname_to[MAXLEN], fwname[MAXLEN], filedir[MAXLEN], snum[3];
	float **mdat, **mdat_ratio, **mdat_mult, **mdat_ti;
	int line_cnt, i, j, size, k, t, steps, count_steps;
	float ti, threshold, previous_to, to, deltaThreshold;
	float **tos, **sum, **to_profile;
	float *thresholds;
	float qMid, qOne, qThree, pos;
	float fsum, fmax_to, ftu;
	char cnet_dw, c_TOtype;	//degree nw; weighted nw
	
	struct list {
		int  index;
		int *fieldset;
		struct list *next;
	};
	struct list *head=NULL;
	struct list *current=NULL;
	struct list *prev=NULL;
	struct list *ptr=NULL;
	
	printf("input nets -- d: degree net, w: weighted net==> \n");
	scanf("%c", &cnet_dw);
	/*
	在scanf();中的%c之前留一個空白就可以跳過此換行字元.
	使用fflush(stdio)函數來清除緩衝區的資料.
	c_io.ppt
	*/
	
	fflush(stdin);
	printf("input OLD or NEW TO -- o: old TO, n: new TO==> \n");
	scanf(" %c", &c_TOtype);
	printf("%c net, %c TO\n", cnet_dw, c_TOtype);
	//cnet_dw='d';
	//c_TOtype='n';

	fflush(stdin);
	printf("steps ==> \n");
	scanf(" %d", &steps);

	strcpy(frname, "../../results/PWS_inoutflow.txt");  
	strcpy(filedir, "../../results/project2_hierarchical clustering/pws/");
    	strcpy(fwname, "");

    	strcat(fwname, filedir);
    	strcat(fwname, "TO_profile.txt");
	
        fp_r = fopen(frname,"r");		/* open file pointer */     
	if(fp_r==NULL){
		printf("file error-Line 110 !!! \n");
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

	to_profile = (float **)malloc((size+1) * sizeof(float *)); 
	if (to_profile == NULL )
	{
		printf("Not enough memory\n");
		return 0;
	}
	for (i = 0; i <= size; i++)
	{
		to_profile[i] = (float *)malloc(sizeof(float) * steps);
		if (to_profile[i] == NULL)
		{
			printf("Not enough memory\n");
			return 0;
		}
	}

	for (i = 0; i <= size; i++){
		for (j = 0; j < steps; j++){
			if (i==0) to_profile[i][j]= j+1;
			else
				to_profile[i][j] = 0 ;
		}
	}

	count_steps=1;
	while(count_steps <= steps){

        	strcpy(fwname_ti, "");
        	strcpy(fwname_to, "");
		
		strcat(fwname_ti, filedir);
        	strcat(fwname_to, filedir);

		strcat(fwname_ti, "pws_sym_ti_step");
		strcat(fwname_to, "pws_sym_to_step");
		sprintf(snum, "%d", count_steps);
        	printf(" steps: %s \n", snum);
		strcat(fwname_ti, snum);
		strcat(fwname_to, snum);
        	strcat(fwname_ti, ".txt");
        	strcat(fwname_to, ".txt");
        	//printf("ti file %s\n", fwname_ti);
        	//printf("to file %s\n", fwname_to);
		//**************** don't use DL file **********************
		/*
		strcpy(frname, "C://Users//Gabrielle//Google 雲端硬碟//Magic Briefcase//results//PWS_inoutflow.txt");  
		strcpy(fwname_ti, "C://Users//Gabrielle//Google 雲端硬碟//Magic Briefcase//results//project2_hierarchical clustering//pws//xpws_sym_ti_stepx.txt");
		strcpy(fwname_to, "C://Users//Gabrielle//Google 雲端硬碟//Magic Briefcase//results//project2_hierarchical clustering//pws/pws_sym_OLDto_step5_t 0.0208.txt");
	
		strcpy(frname, "../../results/PWS_inoutflow.txt");  
		strcpy(fwname_ti, "../../results/project2_hierarchical clustering/pws/pws_sym_ti_stepx.txt");
		strcpy(fwname_to, "../../results/project2_hierarchical clustering/pws/pws_sym_to_step5x.txt");
		*/
	
		//ori. network: mdat
		mdat = (float **)malloc(size*sizeof(float *)); 
		mdat_ratio = (float **)malloc(size*sizeof(float *));
		mdat_mult = (float **)malloc(size*sizeof(float *));
		mdat_ti = (float **)malloc(size*sizeof(float *));
		if (mdat == NULL || mdat_ratio == NULL || mdat_mult == NULL || mdat_ti == NULL)
		{
			printf("Not enough memory\n");
			return 0;
		}
		for (i = 0; i < size; i++)
		{
			mdat[i] = (float *)malloc(sizeof(float) * size);
			mdat_ratio[i] = (float *)malloc(sizeof(float) * size);
			mdat_mult[i] = (float *)malloc(sizeof(float) * size);
			mdat_ti[i] = (float *)malloc(sizeof(float) * size);
			if (mdat[i] == NULL || mdat_ratio[i] == NULL || mdat_mult[i] == NULL || mdat_ti[i] == NULL)
			{
				printf("Not enough memory\n");
				return 0;
			}
		}
		for (i = 0; i < size; i++){
			for (j = 0; j < size; j++){
				mdat[i][j] = 0;
				mdat_ratio[i][j] = 0;
				mdat_mult[i][j] = 0;
				mdat_ti[i][j] = 0;
			}
		}
		
		thresholds = (float *)malloc(sizeof(float)*col);
		if (thresholds == NULL) exit(EXIT_FAILURE);
		for (i = 0; i < col; i++){
			thresholds[i]=0;
		}
	
		tos = (float **)malloc((size+1) * sizeof(float *)); 
		if (tos == NULL )
		{
			printf("Not enough memory\n");
			return 0;
		}
		for (i = 0; i <= size; i++)
		{
			tos[i] = (float *)malloc(sizeof(float) * col);
			if (tos[i] == NULL)
			{
				printf("Not enough memory\n");
				return 0;
			}
		}
		for (i = 0; i <= size; i++){
			for (j = 0; j < col; j++){
				tos[i][j] = 0 ;
			}
		}
	
		sum = (float **)malloc(size*sizeof(float *));	//[][1]: sum of Ti	[][2]:sum of To_ref  [][3]:sum of TF=Ti + (1-TO_ref)
		if (sum == NULL)
		{
			printf("Not enough memory\n");
			return 0;
		}
		for (i = 0; i < size; i++)
		{
			sum[i] = (float *)malloc(sizeof(float) * sum_col);
			if (sum[i] == NULL)
			{
				printf("Not enough memory\n");
				return 0;
			}
		}
		for (i = 0; i < size; i++){
			for (j = 0; j < sum_col; j++){
				if (j==0) sum[i][j] = i;
				else	sum[i][j]=0;
			}
		}
	
		fp_r = fopen(frname,"r");		/* open file pointer */     
		if(fp_r==NULL){
			printf("file error Line 211!!! \n");
			exit(1);
		}
		ReadFile(fp_r, mdat, size);
		net2sym_v2(mdat, size, cnet_dw);
		sym2ratio(mdat_ratio, mdat, size);
	
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				mdat_ti[i][j]=mdat_ratio[i][j];
			}
		}
	
		Matrix_Mult(mdat_ti, mdat_mult, mdat_ratio, size, count_steps);
	
		/*
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				printf("[%8.3f] ", mdat_ti[i][j]);
			}
			printf("\n");
		}
		printf("----------------- end --------------------- \n");
		*/
	
		thresholds[0] = 0;
		thresholds[col-1] = 1;
		deltaThreshold = (thresholds[col-1] - thresholds[0]) / (col-1) ;
		for (i = 1; i < col-1; i++){
			thresholds[i] = i * deltaThreshold;
		}
	
		for (i = 0; i <= size; i++){
			for (j = 0; j < col; j++){
				if(i==0){tos[i][j] = thresholds[j];}
				else tos[i][j] = 0 ;
			}
		}
	
		t=0;
		while(t<col){
			threshold =thresholds[t];
			/////printf(" \n threshold %f \n", threshold);
			for(i=0;i<size;i++){
				current = (struct list *)malloc(sizeof(struct list));
				if (current == NULL) exit(EXIT_FAILURE);
				current->next = NULL;
				current->index = i;
				current->fieldset =(int*) malloc(sizeof(int) * size);
				for (k = 0; k < size; k++) {
					current->fieldset[k] = 0;
				}

				for(j=0;j<size;j++){
					if ((mdat_ti[i][j])>= threshold){
						current->fieldset[j]=1;
					}
				}  
		
				if (head == NULL) head = current;
				else prev->next = current;
				prev = current;	
		
			}	//for i
	
			/*
			printf(" ====== TOs matrix ====== \n");
			current = head;
			while(current != NULL) {
				//printf("index: %d \n", current->index);
				printf("[species %d] ", (current->index)+1);
				for (k = 0; k < size; k++) {
					printf("%d ", current->fieldset[k]);
				}
				printf("\n");
				current = current->next;
			}		
			*/
		
			current = head;
			while(current != NULL){
				ptr = head;
				to=0;
				previous_to=0;
				while(ptr != NULL){
					if (ptr->index != current->index){
						for (i = 0; i < size; i++) {
							if (c_TOtype == 'n'){
								if ((current->fieldset[i] == 1 && ptr->fieldset[i] == 1) ||
									(current->fieldset[i] == 0 && ptr->fieldset[i] == 0) ){
									to = to + 1;
								}
							}  //if (c_TOtype == 'n')
							else{
								if (current->fieldset[i] == 1 && ptr->fieldset[i] == 1){
									to = to + 1;
									/*
									if (current->index == 0 && ptr->index ==1){
										printf("to: %f \n", to);
									}
									*/
								}
							}
						
						}
						//printf("curr[%d] ptr[%d] sum[%f] \n", current->index, ptr->index, to);
						/////printf("sp[%d] VS. sp[%d] overlap(0 or 1)[%f] \n", (current->index)+1, (ptr->index)+1, to-previous_to);
						previous_to = to;
					}
					ptr = ptr->next;
				}
				//printf("====> i[%d] to[%f] \n", current->index, to);
				/////printf("====> species(%d)'s to[%f] \n", (current->index)+1, to);
				tos[(current->index)+1][t] = to;
				current = current->next;
			}

		
			// free linked list
			current = head;
			head = NULL;
			while(current != NULL) {
				prev = current;
				current = current->next;
				free(prev->fieldset);
				free(prev);
			}
		
			t=t+1;
		} // while t<col
	
		/*
		printf(" ===== TOs ===== \n");
		for(i=0;i<=size;i++){
			for(j=0;j<col;j++){
				printf("%8.4f ", tos[i][j]);
			}
			printf("\n");
		}
		*/
	
		fsum=0;
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				fsum = fsum + mdat_ti[i][j];
			}
			sum[i][1]=fsum;
			fsum=0;
		}
	
		//TO_ref
		fsum=0;
		for(i=1;i<=size;i++){
			for(j=0;j<col;j++){
				fsum = fsum + tos[i][j];
			}
			sum[i-1][2]=fsum;
			to_profile[i][count_steps-1] = fsum;
			fsum=0;
		}
		fmax_to=0;
		for(i=0;i<size;i++){
			if (fmax_to < sum[i][2]){
				fmax_to = sum[i][2];
			}
		}
		fsum=0;
		for(i=0;i<size;i++){
			sum[i][2] =(sum[i][2] / fmax_to);
		}
	
		//TF
		ftu=0;
		fsum=0;
		for(i=0;i<size;i++){
			ftu = 1- sum[i][2];
			sum[i][3] = ftu + sum[i][1];
		}
	
	
		fp_w_ti = fopen(fwname_ti,"w"); 
		if(fp_w_ti==NULL){
			printf("file error-Line 395 !!! \n");
			exit(1);
		}
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				fprintf(fp_w_ti, "%8.3f ", mdat_ti[i][j]);
			}
			//fprintf(fp_w_to, "\n", "");
			fprintf(fp_w_ti, "%s", "\n");
		}
	
	
		fp_w_to = fopen(fwname_to,"w"); 
		if(fp_w_to==NULL){
			printf("file error-line 413!!! \n");
			exit(1);
		}
		for(i=0;i<=size;i++){
			for(j=0;j<col;j++){
				fprintf(fp_w_to, "%8.4f ", tos[i][j]);
			}
			fprintf(fp_w_to, "%s", "\n");
		}
	
		for (i = 0; i<=size; i++)
			free(tos[i]);
		
		for (i = 0; i< size; i++){
			free(mdat[i]);
			free(mdat_ratio[i]);
			free(mdat_mult[i]);
			free(mdat_ti[i]);
			free(sum[i]);
		}
	
		free(mdat);
		free(mdat_ratio);  
		free(mdat_mult);
		free(mdat_ti);
		free(thresholds);
		free(tos);
		free(sum);
	
		fclose(fp_w_ti);
		fclose(fp_w_to);

		count_steps = count_steps+1;
	}  // while(count_steps < steps)
	

	fp_w = fopen(fwname,"w"); 
	if(fp_w==NULL){
		printf("file error-line 483!!! \n");
		exit(1);
	}
	for(i=0;i<=size;i++){
		for(j=0;j<steps; j++){
			fprintf(fp_w, "%9.1f ", to_profile[i][j]);
		}
		fprintf(fp_w, "%s", "\n");
	}


	for (i = 0; i<=size; i++)
		free(to_profile[i]);
	free(to_profile);
	fclose(fp_w);

	return 0;
}

int quickSort(float array[], int left, int right) {
	int pivot, i, j;
	float pivot_val, temp;
	
	// 指標交界結束排序
	if(right <= left) { return 1; }  
	
    pivot_val = array[left];
	j=left;
	
	// 尋找比樞紐小的數  
	for(i=left+1; i <= right ; i++){
		// 跳過等於或大於的數 
		if (array[i] >= pivot_val) {continue;}
        j++;
		
		// 交換 array[i] , array[j]  
       temp = array[i];  
       array[i] = array[j];  
       array[j]= temp;
    } 
	
	// 將樞紐位址移到中間
	pivot = j;
	// 交換 array[left] , array[pivot]
	temp = array[left]; 
	array[left] = array[pivot];
	array[pivot] = temp;  
   
	quickSort(array, left, pivot-1);   // 對左邊進行遞迴 
    quickSort(array, pivot+1, right);  // 對右邊進行遞迴 
	
	return 1;
} 

void Matrix_Normalize(float **tiMdat, int size, int steps)
{
	int i, j;
	float **tmp;
 
	tmp = (float **)malloc(size*sizeof(float *)); 
	if (tmp == NULL )
	{
		printf("Not enough memory\n");
		return;
	}
	for (i = 0; i < size; i++)
	{
		tmp[i] = (float *)malloc(sizeof(float) * size);
		if (tmp[i] == NULL)
		{
			printf("Not enough memory\n");
			return;
		}
	}
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			tmp[i][j] = 0;
				
	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			tmp[i][j]=tiMdat[i][j];	
			
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++) {
			tiMdat[i][j] = tmp[i][j] / steps;
		}
	}
	
	for (i = 0; i< size; i++)
		free(tmp[i]);

	free(tmp);	
}

void Matrix_Add(float **tiMdat, float **multMdat, int size)
{
	int i, j;
	char *s;
   	
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++) {
			tiMdat[i][j] = tiMdat[i][j] + multMdat[i][j];
		}
	}
}

void Matrix_Mult(float **tiMdat, float **multMdat, float **m1, int size, int steps)
{
	int i, j , k, t;
	float **tmp;
	char *s;
   
	i = 0; j = 0; k = 0; 
	//
	tmp = (float **)malloc(size*sizeof(float *)); 
	if (tmp == NULL )
	{
		printf("Not enough memory\n");
		return;
	}
	for (i = 0; i < size; i++)
	{
		tmp[i] = (float *)malloc(sizeof(float) * size);
		if (tmp[i] == NULL)
		{
			printf("Not enough memory\n");
			return;
		}
	}
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			tmp[i][j] = 0;
				
	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			tmp[i][j]=m1[i][j];		   
				   
	if (steps >1){
		for (t = 2; t<= steps; t++){
			for(i = 0; i < size; i++){ 
				for( j = 0; j < size; j++){
					multMdat[i][j]=0;
					for( k = 0; k < size; k++){ 
						multMdat[i][j] +=  tmp[i][k] * m1[k][j];
					}
				}
			}
			for(i=0;i<size;i++){
				for(j=0;j<size;j++){
					tmp[i][j]=multMdat[i][j];
				}
			}
			
			Matrix_Add(tiMdat, multMdat, size);
		}
		Matrix_Normalize(tiMdat, size, steps);
	} else {
		for(i=0;i<size;i++)
			for(j=0;j<size;j++)
				tiMdat[i][j]=m1[i][j];
	}
	
	for (i = 0; i< size; i++)
		free(tmp[i]);

	free(tmp);			   
}

void sym2ratio(float **Matrix_ratio, float **Matrix, int arrsize)
{
	float *degree;
	int i, j, sum;
	
	degree = (float *)malloc(arrsize*sizeof(float));
	for (i = 0; i < arrsize; i++)
		degree[i] = 0;
		
	for (i = 0; i < arrsize; i++){
		sum=0;
		for (j = 0; j < arrsize; j++){
			if (Matrix[j][i]>=1) {
				sum = sum + 1 ; 
			}
		}
		degree[i]=sum;
	}
	
	for (i = 0; i < arrsize; i++){
		for (j = 0; j < arrsize; j++){
			if (Matrix[j][i] >= 1){
				Matrix_ratio[j][i] = 1 / degree[i];	
			}
		}
	}
	
	free(degree);
}

void net2sym_v2(float **Matrix, int arrsize, char nettype)
{
	int i, j;
	float sum;

	struct sumofcells {
		int  from;
		int  to;
		float sum;
		struct sumofcells *next;
	};
	struct sumofcells *head=NULL;
	struct sumofcells *current=NULL;
	struct sumofcells *prev=NULL;

	sum=0;
	for(i=0;i<arrsize;i++){
		for(j=0;j<arrsize;j++){  
			if (Matrix[i][j]>0 && Matrix[j][i]>0){
				if (nettype == 'd')
					sum = 2;
				else
					sum = Matrix[i][j] + Matrix[j][i];
				
				current = (struct sumofcells *)malloc(sizeof(struct sumofcells));
				if (current == NULL) exit(EXIT_FAILURE);
				current->next = NULL;
				current->from = i;
				current->to = j;
				current->sum = sum ;
			
				if (head == NULL) head = current;
				else prev->next = current;
				prev = current;	
				
			}
			sum = 0;
		}
    }
	
	for(i=0;i<arrsize;i++){
		for(j=0;j<arrsize;j++){  
			if (Matrix[i][j]>0){
					Matrix[j][i]=Matrix[i][j];
			}
		}
    }
	
	current = head;
	while(current != NULL){
		Matrix[current->from][current->to] = current->sum; 
		Matrix[current->to][current->from] = current->sum; 
		current = current->next;
	}

	// free linked list
	current = head;
	head = NULL;
	while(current != NULL) {
		prev = current;
		current = current->next;
		free(prev);
	}
	
}

void net2sym(float **Matrix, int arrsize)
{
	int i, j;
	
	for(i=0;i<arrsize;i++){
		for(j=0;j<arrsize;j++){  
			if (Matrix[i][j]>0){
					Matrix[i][j]=1;
					Matrix[j][i]=1;
			}
		}
    }
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
