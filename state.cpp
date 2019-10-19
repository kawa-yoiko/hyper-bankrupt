#include "state.h"

#include <sstream>

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
    } else if (v[0] == "CLOSE") {
        puts("CLOSE received");
        _open = false;
    } else if (v[0] == "ERROR") {
        puts("ERROR!");
    } else if (v[0] == "BOOK") {
        puts("BOOK received");
        const char *cs = s.c_str() + 5;
        enum symbol sym = parse_symbol(cs);
        printf("%d\n", (int)sym);
    } else if (v[0] == "TRADE") {
    } else if (v[0] == "ACK") {
    } else if (v[0] == "REJECT") {
        puts("REJECT!");
    } else if (v[0] == "FILL") {
        puts("FILL!");
    } else if (v[0] == "OUT") {
    }
}
