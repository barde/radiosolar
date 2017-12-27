#include <string>

class TrueRng
{
    unsigned long firstEventStart, firstEventStop, secondEventStart, secondEventStop;
    unsigned long randomNumberCache;
    bool lastNonrandomBitReached;

  public:
    TrueRng();
    void addTimestamp(unsigned long);
    bool hasRandomNumber();
    std::string rolloverRandomNumber();
    int getRandomBitLength();
};