#include "trade_strategy.h"
int main(){
	trade_strategy test(1000,0,100,NULL,NULL);
	std::pair<int*, int*> result=test.trade_price();
	for (int i=0;i<15;i++)printf("%d %d\n",result.first[i],result.second[i]);
	int *a=new int[15],*b=new int[15];
	for (int i=0;i<15;i++){
		a[i]=rand()%100+1;
		b[i]=rand()%100+1;
	}
	printf("-----\n");
	trade_strategy test1(1000,0,100,a,b);
	result=test1.trade_price();
	for (int i=0;i<15;i++)printf("%d %d\n",result.first[i],result.second[i]);
}
