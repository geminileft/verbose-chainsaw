#include "pch.h"
#include "SceneMetadata.h"
#include "FileUtils.h"
#include "MathHelpers.h"

float calculateVectorLength(DirectX::XMFLOAT4 v)
{
    // | a | = sqrt((ax * ax) + (ay * ay) + (az * az))
    return (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

SceneMetadata::SceneMetadata(Platform::String^ objFilename, Platform::String^ mtlFilename,
    DirectX::XMFLOAT4 eyeLocation, DirectX::XMFLOAT4 atLocation,
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
    DirectX::XMFLOAT4 atLocation = {
        (float)atLocationData->GetNumberAt(0),
        (float)atLocationData->GetNumberAt(1),
        (float)atLocationData->GetNumberAt(2),
        (float)atLocationData->GetNumberAt(3)
    };

    DirectX::XMVECTOR upDirection;
    JsonArray^ upDirectionData = data->GetNamedArray("up_direction", nullptr);
    if (upDirectionData != nullptr)
    {
        upDirection = {
            (float)upDirectionData->GetNumberAt(0),
            (float)upDirectionData->GetNumberAt(1),
            (float)upDirectionData->GetNumberAt(2),
            (float)upDirectionData->GetNumberAt(3)
        };
    }
    else
    {
        DirectX::XMVECTOR a = DirectX::XMLoadFloat4(&eyeLocation);
        DirectX::XMVECTOR b = DirectX::XMLoadFloat4(&atLocation);
        auto abSub = DirectX::XMVectorSubtract(a, b);
        DirectX::XMFLOAT4 ab;
        XMStoreFloat4(&ab, abSub);

        // calculatePointRotate
        DirectX::XMMATRIX rotateMatrix = DirectX::XMMatrixRotationX(-DirectX::XM_PIDIV2);
        auto x = calculatePointRotate(rotateMatrix, ab);

        auto l = calculateVectorLength(x);
        upDirection = {
            x.x / l,
            x.y / l,
            x.z / l,
            0
        };
    }
    DirectX::XMVECTOR lightDirection;
    JsonArray^ lightDirectionData = data->GetNamedArray("light_direction", nullptr);
    if (lightDirectionData != nullptr)
    {
        lightDirection = {
            (float)lightDirectionData->GetNumberAt(0),
            (float)lightDirectionData->GetNumberAt(1),
            (float)lightDirectionData->GetNumberAt(2),
            (float)lightDirectionData->GetNumberAt(3)
        };
    }
    else
    {
        DirectX::XMVECTOR a = DirectX::XMLoadFloat4(&eyeLocation);
        DirectX::XMVECTOR b = DirectX::XMLoadFloat4(&atLocation);
        auto abSub = DirectX::XMVectorSubtract(a, b);
        lightDirection = DirectX::XMVector4Normalize(abSub);
    }

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
    return DirectX::XMVECTOR({
        m_atLocation.x,
        m_atLocation.y,
        m_atLocation.z,
        m_atLocation.w
        });
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

DirectX::XMFLOAT4 SceneMetadata::getAtLocationData()
{
    return m_atLocation;
}

bool SceneMetadata::getCalculateNormals()
{
    return m_calculateNormals;
}
