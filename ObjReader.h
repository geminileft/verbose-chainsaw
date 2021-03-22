#pragma once

#include "BasicReaderWriter.h"
#include "ObjStructures.h"
#include <map>

class ObjReader
{
public:
	ObjReader();
	ObjData readObject(Platform::String^ objFilePath);
	std::map<std::string, MaterialData> readMaterial(Platform::String^ mtlFilePath);

private:
	BasicReaderWriter^ m_basicReaderWriter;
};

