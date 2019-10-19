#include "trade_strategy.h"
#include <cmath>
#include <ctime>
trade_strategy::trade_strategy(double fair_price, int possession, int limit, int *buy_distribution,int *sell_distribution){
	_fair_price=fair_price;
	_possession=possession;
	_limit=limit;
	_buy_distribution=buy_distribution;
	_first_trade=(_buy_distribution==NULL);
	_sell_distribution=sell_distribution;
    //for (int i = 0; i < 15; i++) printf("%d%c", buy_distribution[i], i == 14 ? '\n' : ' ');
    //for (int i = 0; i < 15; i++) printf("%d%c", sell_distribution[i], i == 14 ? '\n' : ' ');
}
trade_strategy::~trade_strategy(){
	//delete _buy_distribution;
	//delete _sell_distribution;
}
std::pair<int*, int*> trade_strategy::trade_price(){
	int *to_buy=new int[15];
	int *to_sell=new int[15];
	if (_first_trade){
		srand(time(NULL));
		for (int i=0;i<15;i++)to_buy[i]=to_sell[i]=fmax(_limit*i*exp(-i)+rand()%5-2,0.0);
	}else{
		double delta=_possession/_limit;
		int max_to_sell=_limit+_possession,max_to_buy=_limit-_possession;
		int buy_profit=0,sell_profit=0;
		for (int i=0;i<15;i++){
			buy_profit+=_buy_distribution[i];
			sell_profit+=_sell_distribution[i];
		}
		if (buy_profit){
			for (int i=0;i<15;i++)to_buy[i]=max_to_buy*_buy_distribution[i]/buy_profit;
		}else{
			to_buy[0]=std::max(-_possession,0);
			to_buy[1]=max_to_buy-to_buy[0];
			for (int i=2;i<15;i++)to_buy[i]=0;
		}
		if (sell_profit){
			for (int i=0;i<15;i++)to_sell[i]=max_to_sell*_sell_distribution[i]/sell_profit;
		}else{
			to_sell[0]=std::max(_possession,0);
			to_sell[1]=max_to_sell-to_sell[0];
			for (int i=2;i<15;i++)to_sell[i]=0;
		}
	}
	return std::make_pair(to_sell,to_buy);
}
