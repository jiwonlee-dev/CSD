#include<stdio.h>
#include<stdlib.h>

#define USER 1 << 10

int main(int argc, char* argv[]){

	int set[] = { 0, 1,  3, 4,  6, 9, 10, 11,12,13,15,16,17,18,19,20,40,41,42,43, 50};
	int i, j = 0, k = 0;
	int subset[USER][USER]={-1};

	printf("USER : %d\n", USER);

	for(i = 0; i< USER; i++){
		for(j = 0; j<USER; j++){
			subset[i][j] = -1;
		}
	}
	j = 0;
	int temp = subset[j][k++] = set[0];

	for(i = 1; i< sizeof(set)/sizeof(int); i++){
		if(temp+1 < set[i]){
			k = 0;
			subset[++j][k++] = set[i];
		}
		else{
			subset[j][k++] = set[i];
		}
		temp = set[i];
	}

	for(j = 0; j < USER; j++){
		if(subset[j][0] == -1 ) break;
		printf("%d set : ",j);
		for(i = 0; i< USER; i++){
			if(subset[j][i] == -1 ) break;
			printf("%d\t", subset[j][i]);
		}
		printf("\n");
	}
	return 0;

}
