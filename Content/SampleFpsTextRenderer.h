#pragma once

#include <string>
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "GameMessageSystem.h"

namespace App1
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class SampleFpsTextRenderer
	{
	public:
		SampleFpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void SetMessageSystem(GameMessageSystem* messageSystem);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Resources related to text rendering.
		std::wstring                                    m_text;
		DWRITE_TEXT_METRICS	                            m_textMetrics;
		Microsoft::WRL::ComPtr<IDWriteTextLayout3>      m_textLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat2>      m_textFormat;

		std::wstring                                    m_inputControlText;
		DWRITE_TEXT_METRICS	                            m_inputControlTextMetrics;
		Microsoft::WRL::ComPtr<IDWriteTextLayout3>      m_inputControlTextLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat2>      m_inputControlTextFormat;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;

		GameMessageSystem* m_messageSystem;
		long m_subscriptionId;
		bool m_isObjectSelected = false;
	};
}