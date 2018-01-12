#include <Arduino.h>
#include <stdexcept>

using namespace std;

class TrueRng
{
    unsigned long randomEventTimes [4] = { 0, 0 ,0 ,0};
    short collectedEvents = 0;
    String randomnessDatastore;

    public:
        TrueRng();
        void addTimestamp(unsigned long);
        bool hasRandomNumber();
        String rolloverRandomNumber();
        int getRandomBitLength();

    private:
        void calculateRandomBit();
};

TrueRng::TrueRng()
{
}

void TrueRng::addTimestamp(unsigned long millis)
{
    switch (collectedEvents)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            this->randomEventTimes[collectedEvents] = millis;
            return;

        case 4:
            this->calculateRandomBit();
        default:
            invalid_argument( "cannot reach this state" );
    }
}

void TrueRng::calculateRandomBit()
{
    // Inspiration taken from John Walkers ingenious invention on how to get real random numbers.
    // Principle: we count the time period between two pairs of "tube events" (=radioactive emission registration by the Miller tube)
    // If both periods are same, well, thats interesting but should be very seldom as we count in us.
    // Otherwise a comparison generates one bit of true random data.
    // The random data is collected for later usage.
    unsigned long t1 = randomEventTimes[1] - randomEventTimes[0];
    unsigned long t2 = randomEventTimes[3] - randomEventTimes[2];

    if (t1 == t2)
    {
        // exactly the same time period between the two pairs of measurements. this gets discarded.
        this->collectedEvents = 0;
        return;
    }

    this->randomnessDatastore += (t1 < t2) ? "0" : "1";
    this->collectedEvents = 0;
}

bool TrueRng::hasRandomNumber()
{
	return this->randomnessDatastore.length() != 0;
}

String TrueRng::rolloverRandomNumber()
{
    String randomness (this->randomnessDatastore);
    this->randomnessDatastore = "";

    return randomness;
}

int TrueRng::getRandomBitLength()
{
	return this->randomnessDatastore.length();
}