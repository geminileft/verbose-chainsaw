#pragma once

#include <vector>
#include <string>

using namespace std;

vector<string> splitString(const std::string& s, const std::string& delim)
{
    vector<string> v;
    v.reserve(20);
    auto start = 0U;
    auto end = s.find(delim);
    while (end != std::string::npos)
    {
		v.push_back(s.substr(start, end - start));
        // std::cout << s.substr(start, end - start) << std::endl;
        start = end + delim.length();
        end = s.find(delim, start);
    }

    v.push_back(s.substr(start, end));
    return v;
}