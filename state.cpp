#ifndef _STATE_H_
#define _STATE_H_

#include <functional>
#include <string>
#include <vector>

class state {
public:
    state() : _initialized(false), _open(false), _id(0) {
        limit[BOND] = 100;
        limit[CAR] = 10;
        limit[CHE] = 10;
        limit[BDU] = 100;
        limit[ALI] = 100;
        limit[TCT] = 100;
        limit[BAT] = 100;
    }
    void handle(std::string &s);

    enum symbol {
        BOND, CAR, CHE, BDU, ALI, TCT, BAT, INVALID, COUNT
    };
    static const char *symbol_name[COUNT];
    int limit[COUNT];
    int _pos[COUNT] = {0};

    typedef const char *const_cstr;
    static inline symbol parse_symbol(const_cstr &s);

    std::function<void (const char *)> send_callback;

    int add_order(symbol sym, bool is_buy, int price, int qty);
    int add_convert(symbol sym, bool is_buy, int qty);
    void cancel_order(int id);

    double fair[233];
    void updFairPrice();
    void updTradeNaive(symbol sym);

protected:
    bool _initialized;
    bool _open;

    int _id;

    struct order {
        int id;
        symbol sym;
        bool is_convert;
        bool is_buy;
        int price;
        int qty;
    };
    std::vector<std::pair<int, int>> _book[COUNT][2];
    std::vector<order> _orders;
};

#endif
