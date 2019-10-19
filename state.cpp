#include "state.h"
#include <cmath>

using symbol = state::symbol;

#include <iterator>
#include <sstream>

const char *state::symbol_name[] = {
    "BOND", "CAR", "CHE", "BDU", "ALI", "TCT", "BAT", "INVALID"
};

inline std::vector<std::string> split(const std::string &s)
{
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    return std::vector<std::string>(begin, end);
}


inline symbol state::parse_symbol(const_cstr &s) {
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

static inline int parse_int(state::const_cstr &s) {
    int ret = 0;
    while (*s >= '0' && *s <= '9') ret = ret * 10 + *(s++) - '0';
    return ret;
}

static inline std::pair<int, int> parse_pair(state::const_cstr s) {
    int price = parse_int(s);
    s++;    // Skip ':'
    int qty = parse_int(s);
    return {price, qty};
}




int state::add_order(symbol sym, bool is_buy, int price, int qty)
{
    char s[1024];
    int id = _id++;
    sprintf(s, "ADD %d %s %s %d %d", id, symbol_name[sym],
        is_buy ? "BUY" : "SELL", price, qty);
    send_callback(s);
    _orders.push_back(order {id, sym, is_buy, price, qty});
    return id;
}

void state::cancel_order(int id)
{
    char s[16];
    sprintf(s, "CANCEL %d", id);
    send_callback(s);
}


void state::updFairPrice()
{
    std::fill(fair, fair+COUNT, -1);
    fair[BOND] = 1000;
    std::vector<symbol> stock = {CAR, BDU, ALI, TCT};
    for (auto s: stock)
        if (!_book[s][0].empty() && !_book[s][0].empty())
            fair[s] = 0.5 * (_book[s][0][0].first + _book[s][1][0].first);
    fair[CHE] = fair[CAR];
    fair[BAT] = 0.3*fair[BOND] + 0.2*fair[BDU] + 0.3*fair[ALI] + 0.2*fair[TCT];
    if (fair[BDU]==-1) fair[BAT] = -1;
    if (fair[ALI]==-1) fair[ALI] = -1;
    if (fair[TCT]==-1) fair[TCT] = -1;
}


void state::updTradeNaive(symbol sym)
{
    const int BUY = 1, SELL = 0;
    static int prevbuy[COUNT] = {-1,-1,-1,-1,-1,-1,-1,-1};
    static int prevsell[COUNT] = {-1,-1,-1,-1,-1,-1,-1,-1};
    static int prevfair[COUNT] = {-1,-1,-1,-1,-1,-1,-1,-1};

    if (int(round(fair[sym])) != prevfair[sym] && int(round(fair[sym])) != -1)
    {
        if (prevbuy[sym] != -1) cancel_order(prevbuy[sym]);
        if (prevsell[sym] != -1) cancel_order(prevsell[sym]);
        prevfair[sym] = int(round(fair[sym]));
        prevbuy[sym] = add_order(sym, BUY, prevfair[sym]-2, 5);
        prevsell[sym] = add_order(sym, SELL, prevfair[sym]+2, 5);
    }
}




void state::handle(std::string &s)
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
    } else if (v[0] == "OPEN") {
        puts("OPEN received");
        _open = true;
        add_order(BOND, true, 999, std::min(100, 100 - _pos[BOND]));
        add_order(BOND, false, 1001, std::min(100, 100 + _pos[BOND]));
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
        //if (sym != BOND && sym != BAT && sym != CHE) return;
        //printf("BOOK received (%s)\n", symbol_name[sym]);
        //puts(cs);
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
        updTradeNaive(CHE);
        updTradeNaive(BAT);

        //printf("BOOK received (%s)\n", symbol_name[sym]);
        //puts(cs);

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
        if (o.sym == BOND) add_order(BOND, o.is_buy, o.price, qty);
    } else if (v[0] == "OUT") {
        printf("OUT %s\n", v[1].c_str());
    }
}
