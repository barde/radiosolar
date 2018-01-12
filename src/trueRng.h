#include <Arduino.h>

class TrueRng
{
    unsigned long firstEventStart, firstEventStop, secondEventStart, secondEventStop;
    unsigned long randomNumberCache;
    bool lastNonrandomBitReached;

  public:
    TrueRng();
    void addTimestamp(unsigned long);
    bool hasRandomNumber();
    String rolloverRandomNumber();
    int getRandomBitLength();
};