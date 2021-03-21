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
			vertex.y = stof(splitVals[2]);
			vertex.z = stof(splitVals[3]);
			objData.verticesList.push_back(vertex);
		}
		else if (splitVals[0] == "vn")
		{
			Float3 normal;
			normal.x = stof(splitVals[1]);
			normal.y = stof(splitVals[2]);
			normal.z = stof(splitVals[3]);
			objData.normalsList.push_back(normal);
		}
		else if (splitVals[0] == "f")
		{
			vector<Int3> faceVerticesData;
			for (int i = 1; i < splitVals.size(); ++i)
			{
				auto group = splitString(splitVals[i], "/");
				if (group.size() == 3)
				{
					Int3 faceVertexIndices;
					faceVertexIndices.x = stoi(group[0]);
					faceVertexIndices.y = group[1].length() == 0 ? -1 : stoi(group[1]);
					faceVertexIndices.z = stoi(group[2]);
					faceVerticesData.push_back(faceVertexIndices);
				}
				else {
					auto unknownFaceCount = 1;
				}
			}
			objData.facesList.push_back(faceVerticesData);
		}
	}
	return objData;
}
