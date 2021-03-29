#include "pch.h"
#include "SceneMetadata.h"
#include "FileUtils.h"

SceneMetadata::SceneMetadata(Platform::String^ objFilename, Platform::String^ mtlFilename,
    DirectX::XMFLOAT4 eyeLocation, DirectX::XMVECTOR atLocation,
    DirectX::XMVECTOR upVector, DirectX::XMVECTOR lightDirection, bool calculateNormals)
{
    m_objFilename = objFilename;
    m_mtlFilename = mtlFilename;
    m_eyeLocationData = eyeLocation;
    // m_eyeLocation = eyeLocation;
    m_atLocation = atLocation;
    m_upVector = upVector;
    m_lightDirection = lightDirection;
    m_calculateNormals = calculateNormals;
}

SceneMetadata SceneMetadata::getTestScene()
{
    SceneMetadata metadata(L"Resource\\Wavefront\\tree_palmDetailedTall.obj",
        L"Resource\\Wavefront\\tree_palmDetailedTall.mtl",
        { 0.0f, 0.0f, 5.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f },
        false);
    return metadata;
}

SceneMetadata SceneMetadata::getJsonScene(Platform::String^ jsonFilename)
{
    JsonObject^ data = FileUtils::loadJsonFile(jsonFilename);
    Platform::String^ objFilename = data->GetNamedString("obj_filename");
    Platform::String^ mtlFilename = data->GetNamedString("mtl_filename", nullptr);
    JsonArray^ eyeLocationData = data->GetNamedArray("eye_location");
    DirectX::XMFLOAT4 eyeLocation = {
        (float)eyeLocationData->GetNumberAt(0),
        (float)eyeLocationData->GetNumberAt(1),
        (float)eyeLocationData->GetNumberAt(2),
        (float)eyeLocationData->GetNumberAt(3)
    };
    JsonArray^ atLocationData = data->GetNamedArray("at_location");
    DirectX::XMVECTOR atLocation = {
        (float)atLocationData->GetNumberAt(0),
        (float)atLocationData->GetNumberAt(1),
        (float)atLocationData->GetNumberAt(2),
        (float)atLocationData->GetNumberAt(3)
    };
    JsonArray^ upDirectionData = data->GetNamedArray("up_direction");
    DirectX::XMVECTOR upDirection = {
        (float)upDirectionData->GetNumberAt(0),
        (float)upDirectionData->GetNumberAt(1),
        (float)upDirectionData->GetNumberAt(2),
        (float)upDirectionData->GetNumberAt(3)
    };
    JsonArray^ lightDirectionData = data->GetNamedArray("light_direction");
    DirectX::XMVECTOR lightDirection = {
        (float)lightDirectionData->GetNumberAt(0),
        (float)lightDirectionData->GetNumberAt(1),
        (float)lightDirectionData->GetNumberAt(2),
        (float)lightDirectionData->GetNumberAt(3)
    };

    bool calculateNormals = data->GetNamedBoolean("calculate_normals", false);
    SceneMetadata metadata(objFilename, mtlFilename,
        eyeLocation, atLocation, upDirection, lightDirection, calculateNormals);
    return metadata;
}

Platform::String^ SceneMetadata::getObjFilename()
{
    return m_objFilename;
}

Platform::String^ SceneMetadata::getMtlFilename()
{
    return m_mtlFilename;
}

DirectX::XMVECTOR SceneMetadata::getEyeLocation()
{

    auto tmp = DirectX::XMVECTOR({
        m_eyeLocationData.x,
        m_eyeLocationData.y,
        m_eyeLocationData.z,
        m_eyeLocationData.w
        });
    auto test = false;
    return tmp;
}

DirectX::XMVECTOR SceneMetadata::getAtLocation()
{
    return m_atLocation;
}

DirectX::XMVECTOR SceneMetadata::getUpVector()
{
    return m_upVector;
}

DirectX::XMVECTOR SceneMetadata::getLightDirection()
{
    return m_lightDirection;
}

void SceneMetadata::setEyeLocationData(DirectX::XMFLOAT4 eyeLocationData)
{
    m_eyeLocationData = eyeLocationData;
}

DirectX::XMFLOAT4 SceneMetadata::getEyeLocationData()
{
    return m_eyeLocationData;
}

bool SceneMetadata::getCalculateNormals()
{
    return m_calculateNormals;
}
