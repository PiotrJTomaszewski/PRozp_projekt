#include <string>

#include "tourist.h"

class Debug {
public:
    static void dprint(Tourist &tourist, std::string text);
    static void dprintf(Tourist &tourist, std::string format, ...);
};
