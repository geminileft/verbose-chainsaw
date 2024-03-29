﻿#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "GameMessageSystem.h"
#include <vector>
#include "ObjStructures.h"
#include "SceneMetadata.h"

using namespace std;

namespace App1
{
	// This sample renderer instantiates a basic rendering pipeline.
	using namespace concurrency;

	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }
		void SetMessageSystem(GameMessageSystem* messageSystem);


	private:
		void Rotate(float radians);
		void CreateCubeMesh();
		void CreateNonIndexedCubeMesh();
		static vector<VertexPositionColor> CreateMeshFromObjData(ObjData data, std::map<std::string, MaterialData> matData, bool calculateNormals);
		void CreateSampleVertexShader(const std::vector<byte>& fileData);
		void CreateSamplePixelShader(const std::vector<byte>& fileData);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_cubeVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;
		size_t  m_vertexCount;
		double m_totalRotation = 0;
		bool m_isObjectSelected = false;
		DirectX::XMFLOAT3 m_objectRotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 m_cameraRotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		GameMessageSystem* m_messageSystem;
		long m_subscriptionId;
		SceneMetadata m_sceneMetadata;
		float m_flipFactor = 1.0f;
		float m_isLightVectorLocked = false;
	};
}

