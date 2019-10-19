#ifndef TRADE_STRATEGY_H
#define TRADE_STRATEGY_H
#include <algorithm>
class trade_strategy{
public:
	double _fair_price;
	int _possession;
	int _limit;
    int* _buy_distribution;
    int* _sell_distribution;
    bool _first_trade;
	trade_strategy(double fair_price, int possession, int limit, int *buy_distribution, int *sell_distribution);
    ~trade_strategy();
	std::pair<int*, int*> trade_price();
};
#endif
