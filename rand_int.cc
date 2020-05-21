#include <iostream>
//#include <rand>
#include <stdlib.h>

#include<algorithm>


using namespace std;

int main(int argc, const char* argv[])
{
	srand(atoi(argv[3]));

	int depth = atoi(argv[1]);
	int numRev = atoi(argv[2]);
	int range = 1<<depth;

	cout << depth << " " << numRev << " ";

	int i;

	int* rset = (int*)malloc(sizeof(int)*(numRev));
	for(i=0; i<numRev; i++) {
		rset[i] = rand()%range;
	}
	sort(rset, rset+numRev);
	for(i=0; i<numRev; i++) {
		cout << rset[i] << " ";
	}

}

