#include <iostream>
#include <vector>

using namespace std;

int main()
{
	vector<int> revSet;

	int numRev;
	int gDepth;
	cin >> gDepth >> numRev;

	int i;
	int temp=-2;
	int numSD=0;
	int revSD = 0;
	for(i=0; i< numRev; ++i) {
		int rev;
		cin >> rev;
		if(temp < rev-1) numSD++;
		/*
		else{
			cout << temp << " "<< rev <<"\n";
			revSD++;
		}
		*/
		temp = rev;
		//revSet.push_back(rev);
	}


	

//	cout << "# of sd=" << numSD << " sdtype = " << sdType << endl;
	cout << numSD << " ";
	//cout << revSD << " ";

	return 0;
}

