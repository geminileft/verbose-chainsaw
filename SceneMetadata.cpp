#include "pch.h"
#include "SceneMetadata.h"

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
    SceneMetadata metadata(L"Resource\\Wavefront\\tree_fat_fall.obj",
        L"Resource\\Wavefront\\tree_fat_fall.mtl",
        { 0.0f, 0.0f, 5.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f });
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
