#include "pch.h"
#include "SceneMetadata.h"
#include "FileUtils.h"

SceneMetadata::SceneMetadata(Platform::String^ objFilename, Platform::String^ mtlFilename,
    DirectX::XMVECTOR eyeLocation, DirectX::XMVECTOR atLocation,
    DirectX::XMVECTOR upVector, DirectX::XMVECTOR lightDirection)
{
    m_objFilename = objFilename;
    m_mtlFilename = mtlFilename;
    m_eyeLocation = eyeLocation;
    m_atLocation = atLocation;
    m_upVector = upVector;
    m_lightDirection = lightDirection;
}

SceneMetadata SceneMetadata::getTestScene()
{
    SceneMetadata metadata(L"Resource\\Wavefront\\tree_palmDetailedTall.obj",
        L"Resource\\Wavefront\\tree_palmDetailedTall.mtl",
        { 0.0f, 0.0f, 5.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f });
    return metadata;
}

SceneMetadata SceneMetadata::getJsonScene(Platform::String^ jsonFilename)
{
    JsonObject^ data = FileUtils::loadJsonFile(jsonFilename);
    Platform::String^ objFilename = data->GetNamedString("obj_filename");
    Platform::String^ mtlFilename = data->GetNamedString("mtl_filename", nullptr);
    JsonArray^ eyeLocationData = data->GetNamedArray("eye_location");
    DirectX::XMVECTOR eyeLocation = {
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

    SceneMetadata metadata(objFilename, mtlFilename,
        eyeLocation, atLocation, upDirection, lightDirection);
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
    return m_eyeLocation;
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
