#include "trueRng.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>

using namespace std;

void testRandomNumberGeneration_AllOnes()
{
	TrueRng trueRng;
	srand(std::time(nullptr));

	short amountOfRandomEvents = 4 * 32;

	unsigned long lastTime = 1;

	bool isLowNumber = true;
	short counttwo = 0;

	for (int i = 0; i < amountOfRandomEvents; i++)
	{
		if (isLowNumber)
		{
			lastTime += 1;
		}
		else
		{
			lastTime += 100;
		}	
		
		trueRng.addTimestamp(lastTime);

		if (counttwo == 1)
		{
			isLowNumber = !isLowNumber;
			counttwo = 0;
		}
		else
		{
			counttwo = 1;
		}
	}

	unsigned long randomBitsLength = trueRng.getRandomBitLength();
	unsigned long randomBits = trueRng.getRandomBits();
	assert(trueRng.hasRandomNumber());
	cout << trueRng.rolloverRandomNumber();
}

void testRandomNumberGeneration_AllZeroes()
{
	TrueRng trueRng;
	srand(std::time(nullptr));

	short amountOfRandomEvents = 4 * 32;

	unsigned long lastTime = 1;

	bool isLowNumber = false;
	short counttwo = 0;

	for (int i = 0; i < amountOfRandomEvents; i++)
	{
		if (isLowNumber)
		{
			lastTime += 1;
		}
		else
		{
			lastTime += 100;
		}

		trueRng.addTimestamp(lastTime);

		if (counttwo == 1)
		{
			isLowNumber = !isLowNumber;
			counttwo = 0;
		}
		else
		{
			counttwo = 1;
		}
	}

	unsigned long randomBitsLength = trueRng.getRandomBitLength();
	unsigned long randomBits = trueRng.getRandomBits();
	assert(trueRng.hasRandomNumber());
	cout << trueRng.rolloverRandomNumber();
}

void testRandomNumberGeneration()
{
	TrueRng trueRng;
	srand(std::time(nullptr));

	short amountOfRandomEvents = 4 * 32;

	unsigned long lastRandomTime = rand() % 10000000 + 1;

	for (int i = 0; i < amountOfRandomEvents; i++)
	{
		lastRandomTime = rand() % 100 + 1 + lastRandomTime;

		trueRng.addTimestamp(lastRandomTime);
	}

	unsigned long randomBitsLength = trueRng.getRandomBitLength();
	unsigned long randomBits = trueRng.getRandomBits();
	assert(trueRng.hasRandomNumber());
	cout << trueRng.rolloverRandomNumber();
}

int main()
{
	testRandomNumberGeneration_AllOnes();
	testRandomNumberGeneration_AllZeroes();
	testRandomNumberGeneration();


	return 0;
}