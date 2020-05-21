#include <iostream>
//#include <rand>
#include <stdlib.h>
#include <algorithm>


using namespace std;

int* ShufflingNumberTAOCP(int depth, int rev)
{
	int* n = (int*)malloc(sizeof(int)*(1<<depth));
	int* revS = (int*)malloc(sizeof(int)*rev);
	int num = 1<<depth;
	for( int i=0; i < num; ++i )
		n[i] = i;

	for( int i=0; i < num; ++i )
	{
		int dest =  rand() % num;

		int temp = n[i];
		n[i] = n[dest];
		n[dest] = temp;
	}
	for(int i=0; i < rev; ++i )
		revS[i] = n[i];
	free(n);
	return revS;
}

int main(int argc, const char* argv[])
{
	srand(atoi(argv[3]));

	int depth = atoi(argv[1]);
	int numRev = atoi(argv[2]);
	int range = 1<<depth;

	cout << depth << " " << numRev << " ";

	int* rev = ShufflingNumberTAOCP(depth, numRev);
	sort(rev, rev+numRev);
	for(int i=0; i < numRev; ++i )
		cout << rev[i] << " ";
}

