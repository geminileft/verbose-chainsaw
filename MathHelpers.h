#pragma once

#include "pch.h"

inline float calculateVectorLength(DirectX::XMFLOAT4 v)
{
    // | a | = sqrt((ax * ax) + (ay * ay) + (az * az))
    return (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

inline DirectX::XMFLOAT4 calculatePointRotate(DirectX::XMMATRIX rotateMatrix, DirectX::XMFLOAT4 point)
{
    DirectX::XMVECTOR vPoint = DirectX::XMLoadFloat4(&point);
    DirectX::XMVECTOR vNewPoint = XMVector4Transform(vPoint, rotateMatrix);
    DirectX::XMFLOAT4 newPoint;
    XMStoreFloat4(&newPoint, vNewPoint);
    return newPoint;
}

inline DirectX::XMVECTOR calculateUpDirection(DirectX::XMFLOAT4 eyeLocation, DirectX::XMFLOAT4 atLocation)
{
    DirectX::XMVECTOR upDirection;
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
    return upDirection;
}
