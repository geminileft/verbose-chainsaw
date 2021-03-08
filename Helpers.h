#pragma once

#include <vector>
#include <string>

using namespace std;

vector<string> splitString(string input, const char * separators) {
	vector<string> splitVals;
	char* next_token1 = NULL;
	char* token1 = NULL;
	token1 = strtok_s(&input[0], separators, &next_token1);
	while ((token1 != NULL))
	{
		splitVals.push_back(token1);
		token1 = strtok_s(NULL, separators, &next_token1);

	}
	return splitVals;
}