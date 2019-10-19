#include "state.h"
#include "trade_strategy.h"

#include <iterator>
#include <sstream>

const char *state::symbol_name[] = {
    "BOND", "CAR", "CHE", "BDU", "ALI", "TCT", "BAT", "INVALID"
};

static inline std::vector<std::string> split(const std::string &s)
{
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    return std::vector<std::string>(begin, end);
}

void state::parse(std::string &s)
{
    auto v = split(s);

    if (v[0] == "HELLO") {
        puts("HELLO received");
        _initialized = true;
        for (int i = 1; i < v.size(); i++) {
            const char *p = v[i].c_str();
            symbol sym = parse_symbol(p);
            p++;    // Skip ':'
            sscanf(p, "%d", &_pos[(int)sym]);
        }
        for (int i = 1; i <= 4; i++) {
            add_order(BOND, true, 1000 - i, (i >= 3 ? 10 : 30));
            add_order(BOND, false, 1000 + i, (i >= 3 ? 10 : 30));
        }
        _filled_since_last_recal = 0;
    } else if (v[0] == "OPEN") {
        puts("OPEN received");
        _open = true;
    } else if (v[0] == "CLOSE") {
        puts("CLOSE received");
        _open = false;
    } else if (v[0] == "ERROR") {
        puts("!!!!!!");
        puts(s.c_str());
        puts("!!!!!!");
    } else if (v[0] == "BOOK") {
        const char *cs = s.c_str() + 5;
        enum symbol sym = parse_symbol(cs);
        if (sym != BOND) return;
        printf("BOOK received (%s)\n", symbol_name[sym]);
        puts(cs);
        int i = 3;  // BOOK <SYM> BUY
        std::vector<std::pair<int, int>> book_entry[2];
        for (; v[i][0] != 'S'; i++) {
            book_entry[1].push_back(parse_pair(v[i].c_str()));
        }
        i++;    // Skip 'SELL'
        for (; i < v.size(); i++) {
            book_entry[0].push_back(parse_pair(v[i].c_str()));
        }
        _book[(int)sym][0] = book_entry[0];
        _book[(int)sym][1] = book_entry[1];
        updFairPrice();
        //for (auto p : book_entry[0]) printf("%d %d\n", p.first, p.second);
    } else if (v[0] == "TRADE") {
        //printf("TRADE\n");
    } else if (v[0] == "ACK") {
        printf("ACK %s\n", v[1].c_str());
    } else if (v[0] == "REJECT") {
        puts("------");
        printf("REJECT %s %s\n", v[1].c_str(), v[2].c_str());
        puts("------");
    } else if (v[0] == "FILL") {
        int id = std::stoi(v[1]);
        int price = std::stoi(v[4]);
        int qty = std::stoi(v[5]);
        printf("FILL price = %d qty = %d\n", price, qty);
        const char *cs = v[2].c_str();
        const auto &o = _orders[id];
        int sym = (int)o.sym;

        _fills[sym][o.is_buy][price] += qty;

        if ((_filled_since_last_recal += qty) >= 50) {
            _filled_since_last_recal = 0;
            auto dist = cal_dist(BOND);

            trade_strategy strategy(1000, _pos[BOND], 100, dist.second, dist.first);
            auto result = strategy.trade_price();
            for (int i = 0; i < 15; i++) printf("-- SELL %d %d\n", i, result.first[i]);
            for (int i = 0; i < 15; i++) printf("-- BUY %d %d\n", i, result.second[i]);

            delete[] dist.first;
            delete[] dist.second;
            delete[] result.first;
            delete[] result.second;

            _fills[sym][0].clear();
            _fills[sym][1].clear();
        }
    } else if (v[0] == "OUT") {
        printf("OUT %s\n", v[1].c_str());
    }
}

std::pair<int *, int *> state::cal_dist(symbol sym)
{
    double fair_price = 1000;
    int *buy = new int[15]();
    int *sell = new int[15]();
    // Sell, deal price >= fair price
    for (auto p : _fills[sym][0]) {
        int idx = (int)(p.first - fair_price + 0.5);
        idx = std::max(0, std::min(14, idx));
        sell[idx] += p.second;
        printf("!! %d %d\n", p.first, idx);
    }
    // Buy, deal price <= fair price
    for (auto p : _fills[sym][1]) {
        int idx = (int)(-p.first + fair_price + 0.5);
        idx = std::max(0, std::min(14, idx));
        buy[idx] += p.second;
        printf("!! %d %d\n", p.first, idx);
    }
    return {sell, buy};
}
