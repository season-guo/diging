#include<iostream>
#include<arpa/nameser.h>
#include<resolv.h>
#include<string.h>
using namespace std;
enum{
	IN=1,
};
enum{
	A=1,
	NS=2,
};
int main(){
	res_init();
	char  domain[]="google.com";
        unsigned char buf[4096];
	int len=res_query(domain,IN,A,buf,sizeof(buf));
	if(len<0){ 
		cerr<<"query fail"<<endl;
		return 1;
	};
	ns_msg msg;
	if(ns_initparse(buf,len,&msg)<0){
		cerr<<"parseinit fail"<<endl;
		return 1;
	};
	ns_rr rr;
	char name[256],rdate[256];
	for(int i=0;i<ns_msg_count(msg,ns_s_an);i++){
		if(ns_parserr(&msg,ns_s_an,i,&rr)<0){
				cerr<<"parserr fail"<<endl;
				return 1;
		}
		ns_sprintrr(&msg,&rr,rdate,sizeof(rdate));
		cout<<domain<"->"<<rdate<<endl;
	}
	return 0;
}
