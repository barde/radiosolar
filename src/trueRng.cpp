#define RNG_SEED 1

class TrueRng
{
    unsigned long firstEventStart, firstEventStop, secondEventStart, secondEventStop = 0;
    unsigned long randomNumberCache = RNG_SEED;
    bool lastNonrandomBitReached = false;
    void cleanUp();

  public:
    TrueRng();
    void addTimestamp(unsigned long);
    bool hasRandomNumber();
    unsigned long rolloverRandomNumber();
};

TrueRng::TrueRng()
{
}

void TrueRng::addTimestamp(unsigned long millis)
{
    // this should never happen
    if (millis = 0)
    {
        cleanUp();
        return;
    }

    // we already have a random number for pickup
    if (this->hasRandomNumber())
    {
        return;
    }

    if (firstEventStart == 0)
    {
        firstEventStart = millis;
        return;
    }

    if (firstEventStop == 0)
    {
        firstEventStop = millis;
        return;
    }

    if (secondEventStart == 0)
    {
        secondEventStart = millis;
        return;
    }

    if (secondEventStop == 0)
    {
        secondEventStop = millis;
    }

    // buffers are full but random number not generated. we take the function as a "miss" and
    // just do the end calculation

    // shamelessly taken from John Walkers ingenious invention on how to get real random numbers.
    // Autodesk has good devs.
    // Principle: we count the time period between two pairs of "tube events" (=radioactive emission registration by the Miller tube)
    // If both periods are same, well, thats interesting but should be very seldom as we count in us.
    // Otherwise a comparison generates one bit of true random data.
    // The random data is collected for later usage.
    unsigned int t1 = firstEventStop - firstEventStart;
    unsigned int t2 = secondEventStop - secondEventStart;

    if (t1 == t2)
    {
        // exactly the same time period between the two pairs of measurements. this gets discarded.
        this->cleanUp();
        return;
    }

    bool randomBit = t1 < t2;
    randomNumberCache = (randomNumberCache << 1) & randomBit;
    this->cleanUp();
}

void TrueRng::cleanUp()
{
    this->firstEventStart = 0;
    this->firstEventStop  = 0;
    this->secondEventStart= 0;
    this->secondEventStop = 0;
}

bool TrueRng::hasRandomNumber()
{
    if(this->lastNonrandomBitReached)
    {
        return true; 
    }

    int bitCount = sizeof(unsigned int) * 8;

    // if checked position is our RNG_SEED (only 1 is useful)
    // we are sure to have a full unsigned int of randomness
    if(this->randomNumberCache << (bitCount - 1) == RNG_SEED)
    {
        this->lastNonrandomBitReached = true;
    }

    return false;
}

unsigned long TrueRng::rolloverRandomNumber()
{
    unsigned int randomNumber = this->randomNumberCache;

    this->randomNumberCache = RNG_SEED;
    this->lastNonrandomBitReached = false;

    return randomNumber;
}