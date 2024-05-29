#pragma once

#include "Luft/Core/Layer.h"
#include <backends/imgui_impl_vulkan.h>
#ifdef LUFT_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif


namespace Luft {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		
		void SetDarkThemeColors();

		uint32_t GetActiveWidgetID() const;
	private:
		void ProcessSDLWindowEvents();
		void SetupVulkanWindow(const WindowsWindow* ww, int width, int height);
		void SDL2Init4Vulkan();
		void CleanupVulkanWindow();
		void FrameRender(ImDrawData* drawData);
		void FramePresent();

		ImGui_ImplVulkanH_Window m_MainWindowData;
		
		const uint32_t m_MinVkImageCount = 2;
		
		bool m_SwapChainRebuild = false;
		bool m_BlockEvents = true;
	};

}
