class TrueRng
{
    unsigned long firstEventStart, firstEventStop, secondEventStart, secondEventStop;
    unsigned long randomNumberCache;
    bool lastNonrandomBitReached;

  public:
    TrueRng();
    void addTimestamp(unsigned long);
    bool hasRandomNumber();
    unsigned long rolloverRandomNumber();
    unsigned long getRandomBits();    
    short getRandomBitLength();
    void cleanUp();    
};