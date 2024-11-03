#include<cstdlib>
#include<iostream>
#include<string.h>
using namespace std;
int main(int argc,char* argv[]){
	char command[200200]="dig";
        int validator=0;
        if(argv[1][0]=='@'){
      	  for(int i=0;argv[2][i]!='\0';i++){
	   	 if (argv[2][i]=='.'){
	    		 validator=1;
		 	 break;	 
		 }
	  }
	  if(!validator){
	  if(argv[3][0]=='N') validator=1; 
	  else strcpy(argv[3],"NS");
	  }
	}else{
		for(int i=0;argv[1][i]!='\0';i++){
			if(argv[1][i]=='.'){
				validator=1;
				break;
			}
		}
		if(!validator){
		if(argv[2][0]=='N') validator=1;
		else strcpy(argv[2],"NS");
		}
        }	
	if(!validator){
		strcat(command," +noall +answer");
	}
	for(int i=1;i<argc;i++){
		strcat(command," ");
		strcat(command,argv[i]);
	};
	if(!validator){
		strcat(command," | grep -v ';;' | awk '{print $5}' | while read ns;do dig +noall +answer A $ns;done");
	}
	cout<<command<<endl;
	int j=system(command);
	return 0;
}
