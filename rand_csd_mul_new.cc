#include <iostream>
//#include <rand>
#include<stdio.h>
#include <stdlib.h>
#include<list>
#include<unordered_set>


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
	int numSet = atoi(argv[2]);
	int range = 1<<depth;
	int cnt = 0;
	int **c;
	int **d;

	int* cStar;
	int* dStar;

	int i;

	cStar = (int*)malloc(sizeof(int)*numSet);
	dStar = (int*)malloc(sizeof(int)*numSet);

	c = (int**)malloc(sizeof(int)*(numSet));
	d = (int**)malloc(sizeof(int)*(numSet));

	for(i=0;i<numSet;i++){
		c[i] = (int*)malloc(sizeof(int)*(depth));
		d[i] = (int*)malloc(sizeof(int)*(depth));

		cStar[i] = 0;
		dStar[i] = 0;
	}


	cout << depth << " ";

	int j;
	for(j=0;j<numSet;j++){
		for(i=0; i<depth; i++) {
			c[j][i] = rand()%2;
		}
		for(i=0; i< 10;i++){
			c[j][rand()%depth] = 2;
		}
		cStar[j] = 10;
	}

	for(j=0; j<numSet; j++){
		for(i=0; i<depth; i++) {
			if(c[j][i] != 2)
				d[j][i] = c[j][i];
			else if(dStar[j] < 5) d[j][i] = rand()%3;
			if(d[j][i] == 2) dStar[j]++;
		}
		if(dStar[j] != 5){
			for(i=0;i<depth; i++){
				if(c[j][i] == 2 && d[j][i] != 2){
					d[j][i] = 2;
					dStar[j] ++;
					if(dStar[j] == 5) break;
				}
			}
		}
	}

	
	/*
	cout << "\n";

	for(j=0; j<numSet; j++){
		for(i=0; i<depth; i++) {
			cout << c[j][i] << " ";
		}
		cout << "\n";
		for(i=0; i<depth; i++) {
			cout << d[j][i] << " ";
		}
		cout << "\n";
		cout << "\n";
	}
	cout << "\n";
*/

	list<int> revokeSet;
	list<int> set;

	int k;

	int rcnt=0;

	for(k=0; k<numSet; k++){
		int* cset = (int*)malloc(sizeof(int)*(1 << cStar[k]));
		int* dset = (int*)malloc(sizeof(int)*(1 << dStar[k]));

		int* rset = (int*)malloc(sizeof(int)*(((1 << dStar[k])) + ((1 << depth) - (1 << cStar[k]))));
		bool cflag = true;
		bool dflag = true;
		int rnum = 0;
		//cout << ((1 << dStar)) + (1 << depth) - (1 << cStar) << " ";
		for(i = 0; i < (1 << depth); i++){
			int* dum = getBit(i, depth);
			for(j = 0; j < depth; j++){
				if(!(c[k][j] == 2 || dum[j] == c[k][j])){
					cflag = false;
					//cout << "c in " << j << " dum : " << dum[j] << " c : " << c[j] << "\n";
					break;
				}
			}
			if(cflag){
				for(j = 0; j < depth; j++){
					if((d[k][j] != 2 && dum[j] != d[k][j])){
						dflag = false;
						//cout << "d in " << j << " dum : " << dum[j] << " d : " << d[j] << "\n";
						break;
					}
				}
			}
			if(cflag && dflag) {
				//revokeSet.push_back(i);
				//rset[rnum++] = i;
				//cout << i << " ";
			}
			else if(!cflag) {
				//revokeSet.push_back(i);
				//rset[rnum++] = i;
				//cout << i << " ";
			}
			   else{
					set.push_back(i);
			   //cout << i << " ";
			   }
			 

			cflag = true;
			dflag = true;
		}
	}


	//revokeSet.sort();
	set.sort();
	
	unordered_set<int> s;
	set.remove_if([&](int n){
			return (s.find(n) == s.end())?(s.insert(n), false) : true;
			});
	int temp;
	if(!set.empty()){
		temp = set.front(); 
		set.pop_front();
	}
	else{
		cout << 1<<depth << " ";
		for(i = 0; i<1<<depth; i++){
			cout << i << " ";
		}
		return 0;
	}

	for(i=0; i < 1<<depth;i++){
		if(i != temp){
			revokeSet.push_back(i);
		}
		else{
			if(!set.empty()){
				temp = set.front();
				set.pop_front();
			}
		}
	}
	cout << revokeSet.size() << " ";
	while(!revokeSet.empty()){
		cout<< revokeSet.front() << " ";
		revokeSet.pop_front();
	}
	return 0;

}
