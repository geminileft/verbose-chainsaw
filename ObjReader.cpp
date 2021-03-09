#include "pch.h"
#include "ObjReader.h"
#include "FileUtils.h"
#include <sstream>
#include <iostream>
#include "ObjStructures.h"
#include "Helpers.h"

using namespace std;

ObjReader::ObjReader()
{
	m_basicReaderWriter = ref new BasicReaderWriter();
}

ObjData ObjReader::readObject(Platform::String^ objFilePath)
{
	auto myReader = ref new BasicReaderWriter();
	Platform::Array<byte>^ bytecode = myReader->ReadData(objFilePath);
	std::string s((const char*)&(bytecode[0]), bytecode->Length);
	std::istringstream stream(s);
	std::string line;
	int tokenIdx = 0;

	ObjData objData;

	int VERTEX_Y_INDEX = 2;
	int VERTEX_Z_INDEX = 3;

	float VERTEX_Y_MULTIPLIER = 1;
	while (std::getline(stream, line)) {
		auto fixedLine = line;
		// strip trailing carriage return
		auto lastChar = line[line.size() - 1];
		if (line[line.size() - 1] == '\r')
		{
			fixedLine = line.substr(0, line.size() - 1);
		}
		tokenIdx = 0;
		vector<string> splitVals = splitString(fixedLine, " ");

		if (splitVals[0] =="v")
		{
			Float3 vertex;
			vertex.x = stof(splitVals[1]);
			vertex.y = stof(splitVals[VERTEX_Y_INDEX]) * VERTEX_Y_MULTIPLIER;
			vertex.z = stof(splitVals[VERTEX_Z_INDEX]);
			objData.verticesList.push_back(vertex);
		}
		else if (splitVals[0] == "vn")
		{
			Float3 normal;
			normal.x = stof(splitVals[1]);
			normal.y = stof(splitVals[2]);
			normal.z = -stof(splitVals[3]);
			objData.normalsList.push_back(normal);
		}
		else if (splitVals[0] == "f")
		{
			if (splitVals.size() <= 5)
			{
				vector<Int3> faceVerticesData;
				for (int i = 1; i < splitVals.size(); ++i)
				{

					auto group = splitString(splitVals[i], "/");
					if (group.size() == 3)
					{
						Int3 faceVertexIndices;
						faceVertexIndices.x = stoi(group[0]);
						faceVertexIndices.y = stoi(group[1]);
						faceVertexIndices.z = stoi(group[2]);
						faceVerticesData.push_back(faceVertexIndices);
					}
				}
				objData.facesList.push_back(faceVerticesData);
			}
			else
			{
				cout << "We have something different...";
			}
		}
	}
	return objData;
}
