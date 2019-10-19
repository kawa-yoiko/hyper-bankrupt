#include "state.h"

#include <iterator>
#include <sstream>

static const char *symbol_name[] = {
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
    //for (auto ss : v) puts(ss.c_str());

    if (v[0] == "HELLO") {
        puts("HELLO received");
        _initialized = true;
    } else if (v[0] == "OPEN") {
        puts("OPEN received");
        _open = true;
        std::string s = "ADD 1 BOND BUY 999 50";
        send_callback(s);
        s = "ADD 2 BOND SELL 1001 50";
        send_callback(s);
    } else if (v[0] == "CLOSE") {
        puts("CLOSE received");
        _open = false;
    } else if (v[0] == "ERROR") {
        puts("ERROR!");
    } else if (v[0] == "BOOK") {
        const char *cs = s.c_str() + 5;
        puts(cs);
        enum symbol sym = parse_symbol(cs);
        printf("BOOK received (%s)\n", symbol_name[sym]);
        puts(cs);
    } else if (v[0] == "TRADE") {
        printf("TRADE\n");
    } else if (v[0] == "ACK") {
        printf("ACK\n");
    } else if (v[0] == "REJECT") {
        puts("REJECT!");
    } else if (v[0] == "FILL") {
        puts("FILL!");
    } else if (v[0] == "OUT") {
        printf("OUT\n");
    }
}
