#ifndef _STATE_H_
#define _STATE_H_

#include <functional>
#include <string>
#include <vector>

class state {
public:
    state() : _initialized(false), _open(false) { }

    void parse(std::string &s);

    enum symbol {
        BOND, CAR, CHE, BDU, ALI, TCT, BAT, INVALID
    };
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

    std::function<void (std::string &)> send_callback;

protected:
    bool _initialized;
    bool _open;
};

#endif
