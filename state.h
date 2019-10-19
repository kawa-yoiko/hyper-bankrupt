#ifndef _STATE_H_
#define _STATE_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

class state {
public:
    state() : _initialized(false), _open(false), _id(0) { }

    void parse(std::string &s);

    enum symbol {
        BOND = 0, CAR, CHE, BDU, ALI, TCT, BAT, INVALID, COUNT
    };
    static const char *symbol_name[COUNT];

    typedef const char *const_cstr;
    static inline enum symbol parse_symbol(const_cstr &s) {
        switch (s[0]) {
        case 'B':
            switch (s[1]) {
                case 'O': s += 4; return BOND;
                case 'D': s += 3; return BDU;
                case 'A': s += 3; return BAT;
                default: return INVALID;
            }
        case 'C':
            switch (s[1]) {
                case 'A': s += 3; return CAR;
                case 'H': s += 3; return CHE;
                default: return INVALID;
            }
        case 'A':
            s += 3; return ALI;
        case 'T':
            s += 3; return TCT;
        default: return INVALID;
        }
    }

    static inline int parse_int(const_cstr &s) {
        int ret = 0;
        while (*s >= '0' && *s <= '9') ret = ret * 10 + *(s++) - '0';
        return ret;
    }

    static inline std::pair<int, int> parse_pair(const_cstr s) {
        int price = parse_int(s);
        s++;    // Skip ':'
        int qty = parse_int(s);
        return {price, qty};
    }

    std::function<void (const char *)> send_callback;

    int add_order(symbol sym, bool is_buy, int price, int qty)
    {
        char s[1024];
        int id = _id++;
        sprintf(s, "ADD %d %s %s %d %d", id, symbol_name[sym],
            is_buy ? "BUY" : "SELL", price, qty);
        send_callback(s);
        _orders.push_back(order {id, sym, is_buy, price, qty});
        return id;
    }

    double fair[233];

    void updFairPrice()
    {
        fair[BOND] = 1000;
        std::vector<symbol> stock = {CAR, BDU, ALI, TCT};
        for (auto s: stock)
            if (!_book[s][0].empty() && !_book[s][0].empty())
                fair[s] = 0.5 * (_book[s][0][0].first + _book[s][1][0].first);
        fair[CHE] = fair[CAR];
        fair[BAT] = 0.3*fair[BOND] + 0.2*fair[BDU] + 0.3*fair[ALI] + 0.2*fair[TCT];
    }

protected:
    bool _initialized;
    bool _open;

    int _id;

    struct order {
        int id;
        symbol sym;
        bool is_buy;
        int price;
        int qty;
    };
    std::vector<order> _orders;

    int _pos[COUNT];
    // [0] = sell, [1] = buy
    std::vector<std::pair<int, int>> _book[COUNT][2];
};

#endif
