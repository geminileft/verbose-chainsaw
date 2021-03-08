#pragma once

using namespace Windows::Data::Json;

class FileUtils {

public:
	static JsonObject^ loadJsonFile(Platform::String^ filename);
	static Platform::String^ loadTextFile(Platform::String^ filename);
};
