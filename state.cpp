#include "state.h"

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
    } else if (v[0] == "OPEN") {
        puts("OPEN received");
        _open = true;
        for (int i = 1; i <= 4; i++) {
            add_order(BOND, true, 1000 - i, (i >= 3 ? 10 : 30));
            add_order(BOND, false, 1000 + i, (i >= 3 ? 10 : 30));
        }
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
        if (sym == BOND) {
            printf("BOOK received (%s)\n", symbol_name[sym]);
            puts(cs);
        }
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
        add_order(BOND, o.is_buy, o.price, qty);
    } else if (v[0] == "OUT") {
        printf("OUT %s\n", v[1].c_str());
    }
}
