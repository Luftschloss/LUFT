#include "ImGuiLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>
#include <misc/freetype/imgui_freetype.h>
#include "ImGuizmo.h"

#include "Luft/Core/Application.h"
#include "Luft/Core/Log.h"
#include "Luft/Core/SystemService.h"
#include <SDL_vulkan.h>


namespace Luft {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		// === Setup Dear ImGui context ===
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		float fontSize = GetIntVal(IntKey::font_size_normal).value();
		auto font = GetResVal(ResKey::font_path_puhui3);
		io.Fonts->AddFontFromFileTTF(font.value(), fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF(font.value(), fontSize);

		// === Setup Dear ImGui style ===
		// TODO:Add Window Shadow Support
		// ImGui::StyleColorsDark();
		// ImGui::StyleColorsClassic();
		// ImGui::StyleColorsLight();
		SetDarkThemeColors();
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

#ifdef LUFT_RENDERER_BACKEND_VULKAN
		SDL2Init4Vulkan();
#endif // LUFT_RENDERER_BACKEND_VULKAN

	}

	void ImGuiLayer::OnDetach()
	{
#ifdef LUFT_RENDERER_BACKEND_VULKAN
		ImGui_ImplVulkan_Shutdown();
#endif
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

#ifdef LUFT_RENDERER_BACKEND_VULKAN
		CleanupVulkanWindow();
#endif
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	bool show_demo_window = true;
	bool show_another_window = false;

	void ImGuiLayer::Begin()
	{
		ProcessSDLWindowEvents();

		// Start the Dear ImGui frame
#ifdef LUFT_RENDERER_BACKEND_VULKAN
		ImGui_ImplVulkan_NewFrame();
#endif
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		//ImGuizmo::BeginFrame();

		//============ Imgui Demo Test =================
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			auto io = ImGui::GetIO();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Rendering
		ImGui::Render();
		ImDrawData* main_draw_data = ImGui::GetDrawData();
		const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
		if (!main_is_minimized)
			FrameRender(main_draw_data);

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		
		// Present Main Platform Window
		if (!main_is_minimized)
			FramePresent();

	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	uint32_t ImGuiLayer::GetActiveWidgetID() const
	{
		return GImGui->ActiveId;
	}

	//TODO: move to window
	void ImGuiLayer::ProcessSDLWindowEvents()
	{
		auto window = static_cast<SDL_Window*>(Application::Get().GetWindow().GetNativeWindow());
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
			{
				CORE_LOG_INFO("SDL_QUIT");
				Application::Get().PushEvent(WindowCloseEvent());
			}
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
			{
				Application::Get().PushEvent(WindowCloseEvent());
				CORE_LOG_INFO("WINDOWEVENT_CLOSE");
			}
		}
	}

	void ImGuiLayer::SetupVulkanWindow(const WindowsWindow* ww, int width, int height)
	{
		auto surf = ww->GetSurface();
		auto pd = ww->GetPhysicalDevice();
		auto d = ww->GetDevice();
		auto qf = ww->GetQueueFamily();
		auto allocator = ww->GetAllocator();

		m_MainWindowData.Surface = surf;
		// Check for WSI support
		VkBool32 res;
		vkGetPhysicalDeviceSurfaceSupportKHR(pd, qf, surf, &res);
		if (res != VK_TRUE)
		{
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		m_MainWindowData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(pd, surf, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

		// Select Present Mode
#ifdef Luft_UNLIMITED_FRAME_RATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
		m_MainWindowData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(pd, surf, &present_modes[0], IM_ARRAYSIZE(present_modes));
		//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

		// Create SwapChain, RenderPass, Framebuffer, etc.
		assert(m_MinVkImageCount >= 2);
		ImGui_ImplVulkanH_CreateOrResizeWindow(ww->GetInstance(), pd, d, &m_MainWindowData, qf, allocator, width, height, m_MinVkImageCount);
	}

	void ImGuiLayer::SDL2Init4Vulkan()
	{
		auto mw = static_cast<WindowsWindow*>(&Application::Get().GetWindow());
		auto window = static_cast<SDL_Window*>(mw->GetNativeWindow());

		SetupVulkanWindow(mw, mw->GetWidth(), mw->GetHeight());

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForVulkan(window);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = mw->GetInstance();
		init_info.PhysicalDevice = mw->GetPhysicalDevice();
		init_info.Device = mw->GetDevice();
		init_info.QueueFamily = mw->GetQueueFamily();
		init_info.Queue = mw->GetQueue();
		init_info.PipelineCache = mw->GetPipelineCache();
		init_info.DescriptorPool = mw->GetDescriptorPool();
		init_info.RenderPass = m_MainWindowData.RenderPass;
		init_info.Subpass = 0;
		init_info.MinImageCount = m_MinVkImageCount;
		init_info.ImageCount = m_MainWindowData.ImageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = mw->GetAllocator();
		ImGui_ImplVulkan_Init(&init_info);
	}
	
	void ImGuiLayer::CleanupVulkanWindow()
	{
		auto mw = static_cast<WindowsWindow*>(&Application::Get().GetWindow());
		ImGui_ImplVulkanH_DestroyWindow(mw->GetInstance(), mw->GetDevice(), &m_MainWindowData, mw->GetAllocator());
	}
	
	void ImGuiLayer::FrameRender(ImDrawData* drawData)
	{
		VkResult err;
		auto mw = static_cast<WindowsWindow*>(&Application::Get().GetWindow());
		VkSemaphore image_acquired_semaphore = m_MainWindowData.FrameSemaphores[m_MainWindowData.SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = m_MainWindowData.FrameSemaphores[m_MainWindowData.SemaphoreIndex].RenderCompleteSemaphore;
		err = vkAcquireNextImageKHR(mw->GetDevice(), m_MainWindowData.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &m_MainWindowData.FrameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			m_SwapChainRebuild = true;
			return;
		}

		ImGui_ImplVulkanH_Frame* fd = &m_MainWindowData.Frames[m_MainWindowData.FrameIndex];
		{
			err = vkWaitForFences(mw->GetDevice(), 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking

			err = vkResetFences(mw->GetDevice(), 1, &fd->Fence);
		}
		{
			err = vkResetCommandPool(mw->GetDevice(), fd->CommandPool, 0);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = m_MainWindowData.RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = m_MainWindowData.Width;
			info.renderArea.extent.height = m_MainWindowData.Height;
			info.clearValueCount = 1;
			info.pClearValues = &m_MainWindowData.ClearValue;
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(drawData, fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &image_acquired_semaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &render_complete_semaphore;

			err = vkEndCommandBuffer(fd->CommandBuffer);
			err = vkQueueSubmit(mw->GetQueue(), 1, &info, fd->Fence);
		}
	}

	void ImGuiLayer::FramePresent()
	{
		if (m_SwapChainRebuild)
			return;
		auto mw = static_cast<WindowsWindow*>(&Application::Get().GetWindow());
		VkSemaphore render_complete_semaphore = m_MainWindowData.FrameSemaphores[m_MainWindowData.SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &m_MainWindowData.Swapchain;
		info.pImageIndices = &m_MainWindowData.FrameIndex;
		VkResult err = vkQueuePresentKHR(mw->GetQueue(), &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			m_SwapChainRebuild = true;
			return;
		}
		m_MainWindowData.SemaphoreIndex = (m_MainWindowData.SemaphoreIndex + 1) % m_MainWindowData.SemaphoreCount; // Now we can use the next set of semaphores
	}
}
