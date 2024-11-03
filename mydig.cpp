#include<cstdlib>
#include<iostream>
#include<string.h>
using namespace std;
int main(int argc,char* argv[]){
	char command[]="dig";
	for(int i=1;i<argc;i++){
		strcat(command," ");
		strcat(command,argv[i]);
	};
	cout<<command<<endl;
	int j=system(command);
	return 0;
}
