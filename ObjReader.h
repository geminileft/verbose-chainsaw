#pragma once

#include "BasicReaderWriter.h"
#include "ObjStructures.h"

class ObjReader
{
public:
	ObjReader();
	ObjData readObject(Platform::String^ objFilePath);

private:
	BasicReaderWriter^ m_basicReaderWriter;
};

