#include <stdio.h>

#include <vector>
#include <math.h>

#define NOTPRIME 0;
#define PRIME 1;

std::vector <int> primeNumbers = { 2 };

bool isPrime(int number) 
{
	if (std::find(primeNumbers.begin(), primeNumbers.end(), number) != primeNumbers.end()) //number already in vector primeNumbers
		return PRIME;

	int upperLimit = floor( sqrt(number) );
	for (int i = 0; ; i++) //iterate trough primeNumbers
	{
		int divider = primeNumbers[i];
		if (upperLimit < divider) //all numbers verified
		{
			return PRIME;
		}
		else if (number % divider == 0) //divider found
		{
			return NOTPRIME;
		}
		else if (primeNumbers.size() - 1 == i) //need to take next prime dividor, but there is none in primeNumbers vector
		{
			int nextPrimeNumber;
			for (nextPrimeNumber = primeNumbers.back() + 1; !isPrime(nextPrimeNumber); nextPrimeNumber++) //find next prime number
			{
				continue;
			}
			primeNumbers.push_back(nextPrimeNumber);
			divider = nextPrimeNumber;
		}
	}
}

std::vector <int> findPrimeNumbers(int lowerLimit, int upperLimit)
{
	std::vector <int> result;

	for (int testedNumber = lowerLimit; testedNumber <= upperLimit; testedNumber++)
	{
		if (isPrime(testedNumber))
			result.push_back(testedNumber);
	}

	return result;
}

int main() {
	std::vector <int> tmp = findPrimeNumbers(2, 100);
	for (int i = 0; i < tmp.size(); i++)
		printf("%d ", tmp[i]);
}