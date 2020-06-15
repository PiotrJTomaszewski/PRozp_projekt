#include <string>

#include "packet.h"
#include "tourist.h"

class Debug {
public:
    static void dprint(Tourist &tourist, std::string text);
    static void dprintf(Tourist &tourist, std::string format, ...);
    static void dprintp(Tourist &tourist, Packet &packet, std::string text);
    static void dprintfp(Tourist &tourist, Packet &packet, std::string format, ...);
};
