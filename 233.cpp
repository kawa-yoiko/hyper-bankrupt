#include<stdio.h>
#include<string.h>
#include<vector>
#include<math.h>
using namespace std;
char str[1010];
enum stock{BOND,CAR,CHE,BDU,ALI,TCT,BAT};
char name[7][5]={"BOND","CAR","CHE","BDU","ALI","TCT","BAT"};
vector<double> V[7];
void init(){
	freopen("sample.in","r",stdin);
  for(int i=0;i<7;i++)
  V[i].clear();
	while(1){
		memset(str,0,sizeof(str));
		gets(str);
		if(strlen(str)==0)break;
		int len=strlen(str);
		if(str[3]=='K'){
			stock id=BOND;
			if(str[6]=='O')id=BOND;
			if(str[6]=='A'){
				if(str[7]=='R')id=CAR;
				if(str[7]=='T')id=BAT;
			}
			if(str[6]=='H')id=CHE;
			if(str[6]=='D')id=BDU;
			if(str[6]=='L')id=ALI;
			if(str[6]=='C')id=TCT;
			int highest_buy=0,lowest_sell=0;
			for(int i=0;i<len;i++){
				if(str[i]=='Y'){
					if(str[i+2]<'0'||str[i+2]>'9')continue;
					for(int j=i+2;str[j]!=':';j++)
					highest_buy=highest_buy*10+str[j]-'0';
				}
				if(str[i]=='S'){
					if(str[i+5]<'0'||str[i+5]>'9')break;
					for(int j=i+5;str[j]!=':';j++)
					lowest_sell=lowest_sell*10+str[j]-'0';
					break;
				}
			}
			printf("%s %d at %d\n",name[id],highest_buy,lowest_sell);
			if(highest_buy>0&&lowest_sell>0)
			V[(int)id].push_back(0.5*(highest_buy+lowest_sell));
			else
			V[(int)id].push_back(max(highest_buy,lowest_sell));
		}
	}
}
double delta[7];
double X[1010],Y[1010];
double A[1010],phi[1010];//Asin(ix+phi)
void dft(int id){
	memset(X,0,sizeof(X));
	memset(Y,0,sizeof(Y));
	memset(A,0,sizeof(A));
	memset(phi,0,sizeof(phi));
	int N=V[id].size();
	const double PI=acos(-1.0);
	for(int i=0;i<N;i++)
	for(int j=0;j<N;j++){
		X[i]+=V[id][j]*cos(2*PI*i*j/N);
		Y[i]-=V[id][j]*sin(2*PI*i*j/N);
	}
	for(int i=0;i<N;i++){
		//printf("%lf %lf\n",X[i],Y[i]);
	A[i]=sqrt(X[i]*X[i]+Y[i]*Y[i]);
	if(fabs(X[i])<1e-8)phi[i]=0;
	else phi[i]=atan(Y[i]/X[i]);
	}
}
void predict(int id){
	double maxv=-1e9;
	int maxt=0;
	int N=V[id].size();
	for(int t=N;t<N+100;t++){
		double val=delta[id];
		for(int j=0;j<N;j++)
		val+=A[j]*sin(j*t+phi[j]);
		if(val>maxv){
			maxt=t;
			maxv=val;
		}
	}
	maxv+=A[0]/N;
	printf("%s %d %lf\n",name[id],maxt,maxv);
}
int main(){
	init();
	for(int i=0;i<7;i++){
		delta[i]=V[i][0];
		for(int j=V[i].size()-1;j>=0;j--){
			V[i][j]-=delta[i];
			//printf("%lf ",V[i][j]);
		}
		//printf("\n");
	}
	for(int i=0;i<7;i++){
		dft(i);
		predict(i);
	}
	return 0;
}
