#pragma once
#include <SDL.h>
#include <vulkan/vulkan_core.h>
#include "Luft/Core/Window.h"

namespace Luft
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void* GetNativeWindow() const { return m_Window; }
		VkInstance GetInstance() const { return m_VkInstance; }
		VkSurfaceKHR GetSurface() const{ return m_VkSurface; }
		VkPhysicalDevice GetPhysicalDevice() const { return m_VkPhysicalDevice; }
		VkDevice GetDevice() const { return m_VkDevice; }
		uint32_t GetQueueFamily() const { return m_VkQueueFamily; }
		VkQueue GetQueue() const { return m_VkQueue; }
		VkPipelineCache GetPipelineCache() const { return m_VkPipelineCache; }
		VkDescriptorPool GetDescriptorPool() const { return m_VkDescriptorPool; }
		VkAllocationCallbacks* GetAllocator() const { return m_VkAllocator; }

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_WindowData.Width; }
		unsigned int GetHeight() const override { return m_WindowData.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_WindowData.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		void* NativeWindow() const { return m_Window; }

	private:
		virtual void Init(const WindowProps& props);
		void VulkanSetup();
		//void ImVulkanWindowSetup(VkSurfaceKHR surface, int width, int height);
		virtual void Shutdown();

	private:
		struct WindowData
		{
			lstr Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;

			WindowData() = default;
		};
		WindowData m_WindowData;

		SDL_Window* m_Window;
		VkAllocationCallbacks* m_VkAllocator = nullptr;
		VkInstance             m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR           m_VkSurface = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT m_VkDebugReport = VK_NULL_HANDLE;
		VkPhysicalDevice       m_VkPhysicalDevice = VK_NULL_HANDLE;
		uint32_t m_VkQueueFamily = (uint32_t)-1;
		VkDevice m_VkDevice = VK_NULL_HANDLE;
		VkQueue m_VkQueue = VK_NULL_HANDLE;
		VkPipelineCache          m_VkPipelineCache = VK_NULL_HANDLE;
		VkDescriptorPool         m_VkDescriptorPool = VK_NULL_HANDLE;
	};
}