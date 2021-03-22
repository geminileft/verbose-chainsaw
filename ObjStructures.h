#pragma once

#include <vector>
#include <string>
#include <map>

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
	map<int, string> mtlSwitch;
};

struct MaterialData
{
	float ns;
	Float3 ka;
	Float3 kd;
	Float3 ks;
	float ni;
	float d;
	float illum;
};