#include <iostream>
#include <vector>

using namespace std;

typedef enum {EMPTY, ALL, S, D} SDType;

int gDepth=2;

int isBit(int element, int index)
{
	int x = 1;
	x <<= (gDepth-index);
	x &= element;

	return (x!=0);
}

int nextPrefix(int thePrefix, int theIndex, int theBit)
{
	int x = -2 + theBit;
	x <<= (gDepth-theIndex);

	return  (thePrefix & x);
}

int isAdditionalSD(int x, int y)
{
	if((x==ALL && (y==EMPTY || y==D)) ||
		(y==ALL && (x==EMPTY || x==D)))
		return 1;
	else	return 0;

	return 0;
}

int resolveSDType(int x, int y)
{
	switch(x) {
		case EMPTY: 
			switch(y) {
				case EMPTY: return EMPTY;
				case ALL  : return S;
				case S    : 
				case D    : return D;
			}
		case ALL:
			switch(y) {
				case ALL  : return ALL;
				default   : return S;
			}
		
		case S:
			switch(y) {
				case ALL  : return S;
#ifdef COMBI
				case S    : return S;
#endif
				default   : return D;
			}
		case D:
			switch(y) {
				case ALL  : return S;
				default   : return D;
			}
	}

	return ALL;
}

// sd returns the type
int sd(int& theNumSD, int theIndex, int thePrefix, vector<int>& theRevSet)
{
	if(theIndex==gDepth) {
		theNumSD = 0;
		if(theRevSet.size()>0) {
			return EMPTY;
		} else	{
			return ALL;
		}
	}

	if(theRevSet.size()==0) {
		theNumSD = 0;
		return ALL;
	}

	// S0 <--, S1 <--
	vector<int> S0, S1;
	for(int rev : theRevSet) {
		if(isBit(rev,theIndex+1)==0)
			S0.push_back(rev);
		else
			S1.push_back(rev);
	}

	int SD0Num, SD1Num;
	int S0Type = sd(SD0Num, theIndex+1, nextPrefix(thePrefix,theIndex+1,0), S0);
	int S1Type = sd(SD1Num, theIndex+1, nextPrefix(thePrefix,theIndex+1,1), S1);
 
	// type resolution
	theNumSD = SD0Num+SD1Num;
	if(isAdditionalSD(S0Type,S1Type)) {
		theNumSD++;
	}

	return resolveSDType(S0Type,S1Type);

} 

int main()
{
	vector<int> revSet;

	int numRev;
	cin >> gDepth >> numRev;

	int i;
	for(i=0; i< numRev; ++i) {
		int rev;
		cin >> rev;
		revSet.push_back(rev);
	}

	int numSD;
	int sdType = sd(numSD, 0, 0, revSet);

//	cout << "# of sd=" << numSD << " sdtype = " << sdType << endl;
	cout << numSD << " ";

	return 0;
}

