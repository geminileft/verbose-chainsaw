#include "pch.h"
#include "FileUtils.h"
#include "Common/DirectXHelper.h"
#include "BasicReaderWriter.h"

using namespace Windows::Data::Json;
using namespace DirectX;

JsonObject^ FileUtils::loadJsonFile(Platform::String^ filename)
{
    Platform::String^ newString = loadTextFile(filename);
    return JsonObject::Parse(newString);
}

Platform::String^ FileUtils::loadTextFile(Platform::String^ filename)
{
    auto fn = filename->Data();

    auto myReader = ref new BasicReaderWriter();
    Platform::Array<byte>^ bytecode = myReader->ReadData(filename);
    std::string s((const char*)&(bytecode[0]), bytecode->Length);
    std::wstring w_str = std::wstring(s.begin(), s.end());
    const wchar_t* w_chars = w_str.c_str();
    auto newString = ref new Platform::String(w_chars);

    return newString;
}
