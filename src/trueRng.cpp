#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

class TrueRng
{
    unsigned long firstEventStart, firstEventStop, secondEventStart, secondEventStop = 0;
    unsigned long randomNumberCache = 0;
	short randomBitsCount = 0;

  public:
    TrueRng();
    void addTimestamp(unsigned long);
    bool hasRandomNumber();
    unsigned long rolloverRandomNumber();
    unsigned long getRandomBits();
    void cleanUp();
    short getRandomBitLength();
};

TrueRng::TrueRng()
{
	this->cleanUp();
}

void TrueRng::addTimestamp(unsigned long millis)
{
    // this should never happen
    if (millis == 0)
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
    unsigned long t1 = firstEventStop - firstEventStart;
    unsigned long t2 = secondEventStop - secondEventStart;

    if (t1 == t2)
    {
        // exactly the same time period between the two pairs of measurements. this gets discarded.
        this->cleanUp();
        return;
    }

    bool randomBit = t1 < t2;
    this->randomNumberCache = this->randomNumberCache << 1;

    // enrich the rng cache with the result of our decay difference lengths
    bitWrite(this->randomNumberCache, 0, randomBit);
    
	this->cleanUp();
	this->randomBitsCount++;
}

void TrueRng::cleanUp()
{
    this->firstEventStart = 0;
    this->firstEventStop = 0;
    this->secondEventStart = 0;
    this->secondEventStop = 0;
}

bool TrueRng::hasRandomNumber()
{
	return this->randomBitsCount == 32;
}

unsigned long TrueRng::rolloverRandomNumber()
{
    unsigned long randomNumber = this->randomNumberCache;

    return randomNumber;
}

unsigned long TrueRng::getRandomBits()
{
    return this->randomNumberCache;
}

short TrueRng::getRandomBitLength()
{
	return this->randomBitsCount;
}