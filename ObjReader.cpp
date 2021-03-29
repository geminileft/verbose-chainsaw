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
	int faceIdx = 0;

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
				if (group.size() == 1)
				{
					Int3 faceVertexIndices;
					faceVertexIndices.x = stoi(group[0]);
					faceVertexIndices.y = -1;
					faceVertexIndices.z = -1;
					faceVerticesData.push_back(faceVertexIndices);
				}
				else if (group.size() == 3)
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
			++faceIdx;
		}
		else if (splitVals[0] == "usemtl")
		{
			std::string mtlName = splitVals[1];
			objData.mtlSwitch[faceIdx] = mtlName;
		}
	}
	return objData;
}

std::map<std::string, MaterialData> ObjReader::readMaterial(Platform::String^ mtlFilePath)
{
	std::map<std::string, MaterialData> allData;
	auto myReader = ref new BasicReaderWriter();
	Platform::Array<byte>^ bytecode = myReader->ReadData(mtlFilePath);
	std::string s((const char*)&(bytecode[0]), bytecode->Length);
	std::istringstream stream(s);
	std::string line;
	int tokenIdx = 0;

	MaterialData matData;
	std::string matName;

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
		if (splitVals[0] == "newmtl")
		{
			if (!matName.empty())
			{
				allData[matName] = matData;
			}
			matName = splitVals[1];
		}
		else if (splitVals[0] == "Ns")
		{
			matData.ns = stof(splitVals[1]);
		}
		else if (splitVals[0] == "Ka")
		{
			matData.ka.x = stof(splitVals[1]);
			matData.ka.y = stof(splitVals[2]);
			matData.ka.z = stof(splitVals[3]);
		}
		else if (splitVals[0] == "Kd")
		{
			matData.kd.x = stof(splitVals[1]);
			matData.kd.y = stof(splitVals[2]);
			matData.kd.z = stof(splitVals[3]);
		}
		else if (splitVals[0] == "Ks")
		{
			matData.ks.x = stof(splitVals[1]);
			matData.ks.y = stof(splitVals[2]);
			matData.ks.z = stof(splitVals[3]);
		}
		else if (splitVals[0] == "Ni")
		{
			matData.ni = stof(splitVals[1]);
		}
		else if (splitVals[0] == "d")
		{
			matData.d = stof(splitVals[1]);
		}
		else if (splitVals[0] == "illum")
		{
			matData.illum = stof(splitVals[1]);
		}
	}
	allData[matName] = matData;
	return allData;
}
