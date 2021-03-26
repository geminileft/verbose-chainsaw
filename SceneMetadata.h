#pragma once
class SceneMetadata
{
private:
	Platform::String^ m_objFilename;
	Platform::String^ m_mtlFilename;
	DirectX::XMVECTOR m_eyeLocation;
	DirectX::XMVECTOR m_lightDirection;

public:
	SceneMetadata(Platform::String^ objFilename, Platform::String^ mtlFilename,
		DirectX::XMVECTOR eyeLocation, DirectX::XMVECTOR lightDirection);

	static SceneMetadata getTestScene();
	Platform::String^ getObjFilename();
	Platform::String^ getMtlFilename();
	DirectX::XMVECTOR getEyeLocation();
	DirectX::XMVECTOR getLightDirection();
};

