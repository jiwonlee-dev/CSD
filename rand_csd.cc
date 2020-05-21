#include <iostream>
//#include <rand>
#include<stdio.h>
#include <stdlib.h>


using namespace std;


int* getBit(int i, int depth) {
	int* bits;
	bits = (int*)malloc(sizeof(int)*depth);
	int bits_index = 0;
	int j = 0x01;
	while ( i > 0 ) { 
		//printf("dep : %d, bits : %d, ij : %d\n",depth, bits_index, (i & j));
		bits[bits_index++] = (i & j );
		i = ( i >> 1); 
	}   
	return bits;
}


int main(int argc, const char* argv[])
{
	srand(atoi(argv[3]));

	int depth = atoi(argv[1]);
	int numStar = atoi(argv[2]);
	int range = 1<<depth;
	int cnt = 0;
	int *c;
	int *d;

	c = (int*)malloc(sizeof(int)*(depth+1));
	d = (int*)malloc(sizeof(int)*(depth+1));


	cout << depth << " ";

	int i;
	int dStar = 0;
	for(i=0; i<depth; i++) {
		c[i] = rand()%2;
	}
	for(i=0; i<numStar; i++) {
		c[rand()%depth] = 2;
	}
	/*
	for(i=0; i<depth; i++) {
		cout << c[i] << " ";
	}
	*/
	

	for(i=0; i<depth; i++) {
		if(c[i] != 2)
			d[i] = c[i];
		else d[i] = rand()%3;
		if(d[i] == 2) dStar++;
	}
	
	/*
	cout << "\n";
	for(i=0; i<depth; i++) {
		cout << d[i] << " ";
	}
	cout << "\n";
	*/

	int* cset = (int*)malloc(sizeof(int)*(1 << numStar));
	int* dset = (int*)malloc(sizeof(int)*(1 << dStar));

	int* rset = (int*)malloc(sizeof(int)*(((1 << dStar)) + ((1 << depth) - (1 << numStar))));
	bool cflag = true;
	bool dflag = true;
	int rnum = 0;
	//cout << ((1 << dStar)) + (1 << depth) - (1 << numStar) << " ";
	for(i = 0; i < (1 << depth); i++){
		int* dum = getBit(i, depth);
		for(int j = 0; j < depth; j++){
			if(!(c[j] == 2 || dum[j] == c[j])){
				cflag = false;
				//cout << "c in " << j << " dum : " << dum[j] << " c : " << c[j] << "\n";
				break;
			}
		}
		if(cflag){
			for(int j = 0; j < depth; j++){
				if((d[j] != 2 && dum[j] != d[j])){
					dflag = false;
					//cout << "d in " << j << " dum : " << dum[j] << " d : " << d[j] << "\n";
					break;
				}
			}
		}
		if(cflag && dflag) {
			rset[rnum++] = i;
			//cout << i << " ";
		}
		else if(!cflag) {
			rset[rnum++] = i;
			//cout << i << " ";
		}
		else{
			//cout << i << " ";
		}

		cflag = true;
		dflag = true;
	}

	cout << rnum << " ";
	for(i = 0 ;i<rnum; i++){
		cout << rset[i] << " ";
	}

	
	return 0;

}
