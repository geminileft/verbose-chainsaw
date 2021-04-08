#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include "ObjReader.h"
#include "FileUtils.h"
#include "MathHelpers.h"

using namespace App1;

using namespace DirectX;
using namespace Windows::Foundation;

float blendLinear2RGB(float v)
{
	if (v < 0.0031308f)
	{
		return v * 12.92f;
	}
	else
	{
		// TODO: investigate float to double conversion
		return (float)((1.055 * pow(v, 1 / 2.4f)) - 0.055f);
	}
}

float calculateVectorLength(Float3 v)
{
	// | a | = sqrt((ax * ax) + (ay * ay) + (az * az))
	return (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

}

Float3 calculateNormal(Float3 p1, Float3 p2, Float3 p3)
{
	// TODO : HANDLE NO NORMAL DATA
	DirectX::XMFLOAT3 u;
	DirectX::XMFLOAT3 v;

	u.x = p2.x - p1.x;
	u.y = p2.y - p1.y;
	u.z = p2.z - p1.z;

	v.x = p3.x - p1.x;
	v.y = p3.y - p1.y;
	v.z = p3.z - p1.z;

	Float3 n;
	n.x = (u.y * v.z) - (u.z * v.y);
	n.y = (u.z * v.x) - (u.x * v.z);
	n.z = (u.x * v.y) - (u.y * v.x);

	float l = calculateVectorLength(n);
	n.x = n.x / l;
	n.y = n.y / l;
	n.z = n.z / l;
	return n;
}

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources),
	m_sceneMetadata(SceneMetadata::getJsonScene(L"Main_Scene.json"))
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	if (m_sceneMetadata.getEyeLocationData().z <= 0)
	{
		m_flipFactor *= -1.0f;
	}
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		CreateSampleVertexShader(fileData);
		});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		CreateSamplePixelShader(fileData);
		});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {
		CreateNonIndexedCubeMesh();
		});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
		});
}

void App1::Sample3DSceneRenderer::CreateSampleVertexShader(const std::vector<byte>& fileData)
{
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_vertexShader
		)
	);

	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			&fileData[0],
			fileData.size(),
			&m_inputLayout
		)
	);
}

void App1::Sample3DSceneRenderer::CreateSamplePixelShader(const std::vector<byte>& fileData)
{
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_pixelShader
		)
	);

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&m_constantBuffer
		)
	);
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	/*
	static const XMVECTOR eye = m_sceneMetadata.getEyeLocation();
	static const XMVECTOR at = m_sceneMetadata.getAtLocation();
	static const XMVECTOR up = m_sceneMetadata.getUpVector();

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
	*/

	const XMVECTOR lightDirection = m_sceneMetadata.getLightDirection();
	XMStoreFloat4(&m_constantBufferData.lightDirection, lightDirection);

}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		const float degreeIncrement = XMConvertToRadians(45.0f) / 60.0f;
		auto messages = m_messageSystem->GetSubscriptionMessages(m_subscriptionId);
		while (!messages.empty()) {
			auto nextMessage = messages.front();
			if (nextMessage.mType == GameMessageType::ActionStop) {
				m_degreesPerSecond = 0;
			}
			else if (nextMessage.mType == GameMessageType::DirectionRight) {
				if (m_isObjectSelected)
				{
					m_objectRotation.y = (float)fmod(m_objectRotation.y + degreeIncrement, XM_2PI);
					m_degreesPerSecond = 45;
				}
			}
			else if (nextMessage.mType == GameMessageType::DirectionLeft) {
				if (m_isObjectSelected)
				{
					m_objectRotation.y = (float)fmod(m_objectRotation.y - degreeIncrement, XM_2PI);
					m_degreesPerSecond = -45;
				}
			}
			else if (nextMessage.mType == GameMessageType::DirectionUp) {
				if (m_isObjectSelected)
				{
					m_degreesPerSecond = -45;
					m_objectRotation.x = (float)fmod(m_objectRotation.x - degreeIncrement, XM_2PI);
				}
				else
				{
					DirectX::XMFLOAT4 atLocation = m_sceneMetadata.getAtLocationData();
					XMFLOAT4 oldEyeLocationData = m_sceneMetadata.getEyeLocationData();
					Float3 atSphere = calculateSphereInfo({ oldEyeLocationData.x, oldEyeLocationData.y, oldEyeLocationData.z },
						{ atLocation.x, atLocation.y, atLocation.z });
					atSphere.x += -.1f;
					auto abcd = convertSphericalCoordsToCartesian(atSphere);
					m_sceneMetadata.setEyeLocationData({ abcd.x, abcd.y, abcd.z, 0.0f });

				}
			}
			else if (nextMessage.mType == GameMessageType::DirectionDown) {
				if (m_isObjectSelected)
				{
					m_degreesPerSecond = -45;
					m_objectRotation.x = (float)fmod(m_objectRotation.x + degreeIncrement, XM_2PI);
				}
				else
				{
					DirectX::XMFLOAT4 atLocation = m_sceneMetadata.getAtLocationData();
					XMFLOAT4 oldEyeLocationData = m_sceneMetadata.getEyeLocationData();
					Float3 atSphere = calculateSphereInfo({ oldEyeLocationData.x, oldEyeLocationData.y, oldEyeLocationData.z },
						{ atLocation.x, atLocation.y, atLocation.z });
					atSphere.x += .1f;
					auto abcd = convertSphericalCoordsToCartesian(atSphere);
					m_sceneMetadata.setEyeLocationData({ abcd.x, abcd.y, abcd.z, 0.0f });
				}
			}
			else if (nextMessage.mType == GameMessageType::GameSwitchInputControl)
			{
				m_isObjectSelected = !m_isObjectSelected;
			}
			else if (nextMessage.mType == GameMessageType::ControlsStrafeLeft)
			{
				if (!m_isObjectSelected)
				{
					DirectX::XMFLOAT4 atLocation = m_sceneMetadata.getAtLocationData();
					XMFLOAT4 oldEyeLocationData = m_sceneMetadata.getEyeLocationData();
					Float3 atSphere = calculateSphereInfo({ oldEyeLocationData.x, oldEyeLocationData.y, oldEyeLocationData.z },
						{ atLocation.x, atLocation.y, atLocation.z });
					atSphere.y += .1f;
					auto abcd = convertSphericalCoordsToCartesian(atSphere);
					if ((oldEyeLocationData.z * abcd.z) < 0)
					{
						m_flipFactor *= -1.0f;
					}
					m_sceneMetadata.setEyeLocationData({ abcd.x, abcd.y, abcd.z, 0.0f });
				}
			}
			else if (nextMessage.mType == GameMessageType::ControlsStrafeRight)
			{
				if (!m_isObjectSelected)
				{
					DirectX::XMFLOAT4 atLocation = m_sceneMetadata.getAtLocationData();
					XMFLOAT4 oldEyeLocationData = m_sceneMetadata.getEyeLocationData();
					Float3 atSphere = calculateSphereInfo({ oldEyeLocationData.x, oldEyeLocationData.y, oldEyeLocationData.z },
						{ atLocation.x, atLocation.y, atLocation.z });
					atSphere.y += -.1f;
					auto abcd = convertSphericalCoordsToCartesian(atSphere);
					if ((oldEyeLocationData.z * abcd.z) < 0)
					{
						m_flipFactor *= -1.0f;
					}
					m_sceneMetadata.setEyeLocationData({ abcd.x, abcd.y, abcd.z, 0.0f });
				}
			}
			else if (nextMessage.mType == GameMessageType::ControlsCircleUp)
			{
				if (!m_isObjectSelected)
				{
					DirectX::XMFLOAT4 atLocation = m_sceneMetadata.getAtLocationData();
					XMFLOAT4 oldEyeLocationData = m_sceneMetadata.getEyeLocationData();
					Float3 atSphere = calculateSphereInfo({ oldEyeLocationData.x, oldEyeLocationData.y, oldEyeLocationData.z },
						{ atLocation.x, atLocation.y, atLocation.z });
					atSphere.z += (atSphere.y >= 0 ? -.1f : .1f) * m_flipFactor;
					auto abcd = convertSphericalCoordsToCartesian(atSphere);
					m_sceneMetadata.setEyeLocationData({abcd.x, abcd.y, abcd.z, 0.0f});
					if ((oldEyeLocationData.z * abcd.z) < 0)
					{
						m_sceneMetadata.reverseUpVector();
					}
				}
			}
			else if (nextMessage.mType == GameMessageType::ControlsCircleDown)
			{
				if (!m_isObjectSelected)
				{
					DirectX::XMFLOAT4 atLocation = m_sceneMetadata.getAtLocationData();
					XMFLOAT4 oldEyeLocationData = m_sceneMetadata.getEyeLocationData();
					Float3 atSphere = calculateSphereInfo({ oldEyeLocationData.x, oldEyeLocationData.y, oldEyeLocationData.z },
						{ atLocation.x, atLocation.y, atLocation.z });
					atSphere.z += (atSphere.y >= 0 ? .1f : -.1f) * m_flipFactor;
					auto abcd = convertSphericalCoordsToCartesian(atSphere);
					m_sceneMetadata.setEyeLocationData({ abcd.x, abcd.y, abcd.z, 0.0f });
					if ((oldEyeLocationData.z * abcd.z) < 0)
					{
						m_sceneMetadata.reverseUpVector();
					}
				}
			}
			else if (nextMessage.mType == GameMessageType::GameLightDirUpdate)
			{
				m_isLightVectorLocked = !m_isLightVectorLocked;
			}
			messages.pop();
		}

		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		m_totalRotation += ( radiansPerSecond / 60.0);
		float radians = static_cast<float>(fmod(m_totalRotation, XM_2PI));

		Rotate(radians);

		if (!m_isLightVectorLocked)
		{
			m_sceneMetadata.updateLightDirection();
			const XMVECTOR lightDirection = m_sceneMetadata.getLightDirection();
			XMStoreFloat4(&m_constantBufferData.lightDirection, lightDirection);
		}
	}
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	auto modelMatrix = XMMatrixTranspose(XMMatrixRotationRollPitchYaw(
		m_objectRotation.x, m_objectRotation.y, m_objectRotation.z));
	XMStoreFloat4x4(&m_constantBufferData.model, modelMatrix);
	XMStoreFloat4x4(&m_constantBufferData.normal, XMMatrixTranspose(XMMatrixInverse(nullptr, modelMatrix)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

void Sample3DSceneRenderer::SetMessageSystem(GameMessageSystem* messageSystem)
{
	m_messageSystem = messageSystem;
	std::set<GameMessageType> messageFilters;
	messageFilters.insert(GameMessageType::ActionStop);
	messageFilters.insert(GameMessageType::DirectionLeft);
	messageFilters.insert(GameMessageType::DirectionRight);
	messageFilters.insert(GameMessageType::DirectionUp);
	messageFilters.insert(GameMessageType::DirectionDown);
	messageFilters.insert(GameMessageType::GameSwitchInputControl);
	messageFilters.insert(GameMessageType::GameLightDirUpdate);
	messageFilters.insert(GameMessageType::ControlsStrafeLeft);
	messageFilters.insert(GameMessageType::ControlsStrafeRight);
	messageFilters.insert(GameMessageType::ControlsCircleUp);
	messageFilters.insert(GameMessageType::ControlsCircleDown);
	m_subscriptionId = m_messageSystem->CreateSubscription(messageFilters);
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	XMVECTOR eye = m_sceneMetadata.getEyeLocation();
	XMVECTOR at = m_sceneMetadata.getAtLocation();
	XMVECTOR up = m_sceneMetadata.getUpVector();

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
		);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;

	context->IASetVertexBuffers(
		0,
		1,
		m_cubeVertexBuffer.GetAddressOf(),
		&stride,
		&offset
		);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
		);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		1,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
		);

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	context->PSSetConstantBuffers1(
		1,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
		);

	// Draw the objects.
	context->Draw(
		(UINT)m_vertexCount,
		0
	);
}

void Sample3DSceneRenderer::CreateCubeMesh()
{

	// Load mesh vertices. Each vertex has a position and a color.
	static const VertexPositionColor cubeVertices[] =
	{
		{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
		{XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
		{XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
	};

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
		)
	);

	// Load mesh indices. Each trio of indices represents
	// a triangle to be rendered on the screen.
	// For example: 0,2,1 means that the vertices with indexes
	// 0, 2 and 1 from the vertex buffer compose the 
	// first triangle of this mesh.
	static const unsigned short cubeIndices[] =
	{
		0,2,1, // -x
		1,2,3,

		4,5,6, // +x
		5,7,6,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	};

	m_indexCount = ARRAYSIZE(cubeIndices);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_indexBuffer
		)
	);

}

void App1::Sample3DSceneRenderer::CreateNonIndexedCubeMesh()
{
	Platform::String^ filename = m_sceneMetadata.getObjFilename();
	Platform::String^ mtlFilename = m_sceneMetadata.getMtlFilename();
	ObjReader objReader;
	ObjData objData = objReader.readObject(filename);
	std::map<std::string, MaterialData> allMaterial;
	if (mtlFilename != nullptr)
	{
		allMaterial = objReader.readMaterial(mtlFilename);
	}
	vector<VertexPositionColor> meshData = CreateMeshFromObjData(objData, allMaterial, m_sceneMetadata.getCalculateNormals());
	VertexPositionColor* meshArr = new VertexPositionColor[meshData.size()];
	for (int i = 0; i < meshData.size(); ++i)
	{
		meshArr[i] = meshData[i];
	}

	// position, color, normal
	static const VertexPositionColor cubeVertices3[] =
	{
	{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)}
	, {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)}
	, {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)}
	, {XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)}
	
	// current
	, {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}
	, {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}
	, {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}
	, {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}

	// varies
	, {XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}

	, {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}
	, {XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}
	, {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}
	, {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}
	, {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}
	, {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}

	// varies
	, {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)}
	, {XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)}

	, {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}
	, {XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}
	, {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}
	, {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}
	, {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}
	, {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}
	};

	// TODO: IS THIS THE RIGHT CAST
	auto mySize = (UINT) (sizeof(meshArr[0]) * meshData.size());
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = meshArr;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(mySize, D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_cubeVertexBuffer
		)
	);
	m_vertexCount = meshData.size();
}

vector<VertexPositionColor> App1::Sample3DSceneRenderer::CreateMeshFromObjData(ObjData data, std::map<std::string, MaterialData> matData, bool calculateNormals)
{
	vector<VertexPositionColor> vertices;

	// vector<vector<Int3>> facesList;
	int faceIdx = 0;
	XMFLOAT3 triColor = XMFLOAT3(0.7f, 0.7f, 0.7f);
	// XMFLOAT3 triColor = XMFLOAT3(0.277363f, 0.446672f, 0.072673f);
	for (vector<Int3> face : data.facesList)
	{
		auto idxMat = data.mtlSwitch.find(faceIdx);
		if (idxMat != data.mtlSwitch.end())
		{
			auto matVal = idxMat->second;
			if (matData.find(matVal) != matData.end())
			{
				auto useMatData = matData[matVal];
				triColor = XMFLOAT3(blendLinear2RGB(useMatData.kd.x),
					blendLinear2RGB(useMatData.kd.y),
					blendLinear2RGB(useMatData.kd.z));
			}
		}
		Int3 vIndices0 = face[0];
		Float3 position0 = data.verticesList[vIndices0.x - 1];
		// TODO : HANDLE NO NORMAL DATA
		// p1, p2, p3, if the vector U = p2 - p1 and the vector V = p3 - p1
		// Nx = UyVz - UzVy
		// Ny = UzVx - UxVz
		// Nz = UxVy - UyVx

		Float3 normal0 = { 0.0f, 0.0f, 0.0f };
		if (vIndices0.z > 0)
		{
			normal0 = data.normalsList[vIndices0.z - 1];
		}
		for (int i = 1; i < face.size() - 1; ++i)
		{
			Int3 vIndices1 = face[i];
			Float3 position1 = data.verticesList[vIndices1.x - 1];
			Float3 normal1 = { 0.0f, 0.0f, 0.0f };
			if (vIndices1.z > 0)
			{
				normal1 = data.normalsList[vIndices1.z - 1];
			}
			Int3 vIndices2 = face[i + 1];
			Float3 position2 = data.verticesList[vIndices2.x - 1];
			Float3 normal2 = { 0.0f, 0.0f, 0.0f };
			if (vIndices2.z > 0)
			{
				normal2 = data.normalsList[vIndices2.z - 1];
			}
			if (calculateNormals)
			{
				Float3 normal = calculateNormal(position0, position1, position2);
				normal0 = normal;
				normal1 = normal;
				normal2 = normal;
			}
			vertices.push_back({
				XMFLOAT3(position0.x, position0.y, position0.z),
				triColor,
				XMFLOAT3(normal0.x, normal0.y, normal0.z)
				});
			vertices.push_back({
				XMFLOAT3(position2.x, position2.y, position2.z),
				triColor,
				XMFLOAT3(normal2.x, normal2.y, normal2.z)
				});
			vertices.push_back({
				XMFLOAT3(position1.x, position1.y, position1.z),
				triColor,
				XMFLOAT3(normal1.x, normal1.y, normal1.z)
				});
		}
		++faceIdx;
	}
	return vertices;
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}