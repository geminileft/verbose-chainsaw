#pragma once
class SceneMetadata
{
private:
	Platform::String^ m_objFilename;
	Platform::String^ m_mtlFilename;
	DirectX::XMVECTOR m_eyeLocation;
	DirectX::XMVECTOR m_atLocation;
	DirectX::XMVECTOR m_upVector;
	DirectX::XMVECTOR m_lightDirection;

public:
	SceneMetadata(Platform::String^ objFilename, Platform::String^ mtlFilename,
		DirectX::XMVECTOR eyeLocation, DirectX::XMVECTOR atLocation,
		DirectX::XMVECTOR upVector, DirectX::XMVECTOR lightDirection);

	static SceneMetadata getTestScene();
	Platform::String^ getObjFilename();
	Platform::String^ getMtlFilename();
	DirectX::XMVECTOR getEyeLocation();
	DirectX::XMVECTOR getAtLocation();
	DirectX::XMVECTOR getUpVector();
	DirectX::XMVECTOR getLightDirection();
};

