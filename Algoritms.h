#pragma once

bool colatz(int& n) {
	if (n % 2 == 0) {
		n /= 2;
		return true;
	}
	else {
		(n *= 3)++;
		return false;
	}
}

bool isPrime(int n) {
	for (int i = 2; i <= n / 2; ++i)
	{
		if (n % i == 0)
		{
			return false;
		}
	}
	return true;
}

int nextPrimegap(int prime) {
	int i = 0;
	bool foundPrime = false;
	while (!foundPrime && i < 1000) {
		i++;
		bool newp = isPrime(prime + i);
		if (newp) {
			return i;
		}
	}
}