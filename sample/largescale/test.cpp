#include <iostream>
#include <fstream>
using namespace std;

int main()
{
	//FILE *fp1,*fp2;
	//fp1=fopen("input.txt","r");
	//fp2=fopen("output2.txt","w");
	
	ifstream input("input.txt");
	ofstream output("output222.txt");
	long long int x=1;
	while(x>0)
	{
		/*
		if(input.eof())
		{
			break;
		}
		*/
		input>>x;
		
		//fscanf(fp1,"%d",&x);
		//fprintf(fp2,"%d ",x%3137);
		output<<x%3173<<" ";
	}
	
	return 0;
}