#pragma once

#include <vector>

using namespace std;

struct Float3
{
	float x;
	float y;
	float z;
};

struct Int3
{
	int x;
	int y;
	int z;
};

struct ObjData
{
	vector<Float3> verticesList;
	vector<Float3> normalsList;
	vector<vector<Int3>> facesList;
};