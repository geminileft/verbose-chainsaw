#pragma once
class SceneMetadata
{
private:
	Platform::String^ m_objFilename;
	Platform::String^ m_mtlFilename;

	DirectX::XMFLOAT4 m_eyeLocationData;
	DirectX::XMVECTOR m_atLocation;
	DirectX::XMVECTOR m_upVector;
	DirectX::XMVECTOR m_lightDirection;

public:
	SceneMetadata(Platform::String^ objFilename, Platform::String^ mtlFilename,
		DirectX::XMFLOAT4 eyeLocation, DirectX::XMVECTOR atLocation,
		DirectX::XMVECTOR upVector, DirectX::XMVECTOR lightDirection);

	static SceneMetadata getTestScene();
	static SceneMetadata getJsonScene(Platform::String^ jsonFilename);
	Platform::String^ getObjFilename();
	Platform::String^ getMtlFilename();
	DirectX::XMVECTOR getEyeLocation();
	DirectX::XMVECTOR getAtLocation();
	DirectX::XMVECTOR getUpVector();
	DirectX::XMVECTOR getLightDirection();

	void setEyeLocationData(DirectX::XMFLOAT4 eyeLocationData);
	DirectX::XMFLOAT4 getEyeLocationData();
};

