#pragma once

#include "pch.h"
#include "ObjStructures.h"

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

inline Float3 calculateSphereInfo(Float3 outsidePoint, Float3 centerPoint)
{
    float dx = outsidePoint.x - centerPoint.x;
    float dz = outsidePoint.y - centerPoint.y;
    float dy = outsidePoint.z - centerPoint.z;

    double radial = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    double omega = atan2(dy, dx);
    double theta = atan2(sqrt((dx * dx) + (dy * dy)), dz);
    double theta2 = acos(dz / radial);
    // r = sqrt(x ^ 2 + y ^ 2 + z ^ 2)
    // o = atan2(y / x)
    // t = acos(z / r)
    Float3 sphereInfo = { radial, omega, theta };
    return sphereInfo;
}

inline Float3 convertSphericalCoordsToCartesian(Float3 sphericalCoords)
{
    Float3 val;
    val.x = sphericalCoords.x * sin(sphericalCoords.z) * cos(sphericalCoords.y);
    val.z = sphericalCoords.x * sin(sphericalCoords.z) * sin(sphericalCoords.y);
    val.y = sphericalCoords.x * cos(sphericalCoords.z);
    return val;
    // x = r * sin(t) * cos(o)
    // y = r * sin(t) * sin(o)
    // z = r * cos(t)
}
