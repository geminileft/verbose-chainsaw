#pragma once
class SceneMetadata
{
private:
	Platform::String^ m_objFilename;
	Platform::String^ m_mtlFilename;

	DirectX::XMFLOAT4 m_eyeLocationData;
	DirectX::XMFLOAT4 m_atLocation;
	DirectX::XMVECTOR m_upVector;
	DirectX::XMVECTOR m_lightDirection;

	bool m_calculateNormals;

public:
	SceneMetadata(Platform::String^ objFilename, Platform::String^ mtlFilename,
		DirectX::XMFLOAT4 eyeLocation, DirectX::XMFLOAT4 atLocation,
		DirectX::XMVECTOR upVector, DirectX::XMVECTOR lightDirection, bool calculateNormals);

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
	DirectX::XMFLOAT4 getAtLocationData();

	bool getCalculateNormals();
};

