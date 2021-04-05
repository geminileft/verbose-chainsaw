#pragma once

#include "pch.h"

inline DirectX::XMFLOAT4 calculatePointRotate(DirectX::XMMATRIX rotateMatrix, DirectX::XMFLOAT4 point)
{
    DirectX::XMVECTOR vPoint = DirectX::XMLoadFloat4(&point);
    DirectX::XMVECTOR vNewPoint = XMVector4Transform(vPoint, rotateMatrix);
    DirectX::XMFLOAT4 newPoint;
    XMStoreFloat4(&newPoint, vNewPoint);
    return newPoint;
}