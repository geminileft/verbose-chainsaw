#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleFpsTextRenderer.h"

#include "GameInputManager.h"
#include "GameMessageSystem.h"

// Renders Direct2D and 3D content on the screen.
namespace App1
{
	class App1Main : public DX::IDeviceNotify
	{
	public:
		App1Main(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~App1Main();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// std::unique_ptr<
		std::unique_ptr<GameInputManager> m_inputManager;
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;
		std::unique_ptr<GameMessageSystem> m_messageSystem;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}