#include<stdio.h>
#include<string.h>
#include<vector>
#include<math.h>
using namespace std;
class PredictResult{
	public:
	int remaining_trades;
	int lowest;
	int highest;
	void output(){
		printf("%d trades left;[%d, %d]\n",
		remaining_trades,lowest,highest);
	}
};
class Predict{
public:
char str[1010];
enum stock{BOND,CAR,CHE,BDU,ALI,TCT,BAT};
char name[7][5]={"BOND","CAR","CHE","BDU","ALI","TCT","BAT"};
vector<double> V[7];
double delta[7];
double X[1010],Y[1010];
double A[1010],phi[1010];//Asin(ix+phi)
public:
void init(int id, vector<double> v){
		delta[id]=v[0];
		V[id].clear();
		for(int j=0;j<v.size();j++)
		V[id].push_back(v[j]-delta[id]);
}
void init(){
	freopen("sample.in","r",stdin);
	while(1){
		memset(str,0,sizeof(str));
		gets(str);
		if(strlen(str)==0)break;
		int len=strlen(str);
		if(str[3]=='D'){
			stock id=BOND;
			if(str[7]=='O')id=BOND;
			if(str[7]=='A'){
				if(str[8]=='R')id=CAR;
				if(str[8]=='T')id=BAT;
			}
			if(str[7]=='H')id=CHE;
			if(str[7]=='D')id=BDU;
			if(str[7]=='L')id=ALI;
			if(str[7]=='C')id=TCT;
			int price=0;
			for(int i=(id==BOND?11:10);str[i]!=' ';i++)
			price=price*10+str[i]-'0'; 
			V[(int)id].push_back(price);
		}
	}
	for(int i=0;i<7;i++)
	init(i,V[i]);
}
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
	
	A[i]=sqrt(X[i]*X[i]+Y[i]*Y[i])/N;
	//if(i!=0)A[i]*=2;
	if(fabs(X[i])<1e-8)phi[i]=0;
	else phi[i]=atan(Y[i]/X[i]);
	//	printf("%lf ",A[i]);
	}
}
PredictResult predict(int id){
	dft(id); 
	int T=50;
	double maxv=-1e9,minv=1e9,avg=0; 
	int maxt=0,mint=0;
	int N=V[id].size();
	double err=0;
	for(int t=N;t<N+T;t++){
		double val=delta[id];
		for(int j=0;j<N;j++)
		val+=A[j]*sin(j*t+phi[j]);
		err+=(V[id][t]+delta[id]-val)*(V[id][t]+delta[id]-val);
		avg+=val;
		if(val>maxv){
			maxt=t;
			maxv=val;
		}
		if(val<minv){
			mint=t;
			minv=val;
		}
	}
	avg/=T;
	maxv+=A[0];
	err=sqrt(err/T); 
	PredictResult res;
	res.remaining_trades=max(maxt-N,mint-N);
	res.highest=(int)maxv;
	res.lowest=(int)(minv+0.999);
	return res;
}
};
int main(){
	Predict pred;
	pred.init();
	for(int i=0;i<7;i++)
	pred.predict(i).output();
	return 0;
}
