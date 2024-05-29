#include <SDL_vulkan.h>
#include "WindowsWindow.h"
#include "Luft/Core/Log.h"
#include "Version.h"
#include "Luft/Core/larray.h"


namespace Luft
{
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
	}

	bool WindowsWindow::IsVSync() const
	{
		return false;
	}

	int SDLEventWatcher(void* data, SDL_Event* event)
	{
		SDL_Window* window = (SDL_Window*)(data);
		if (event->window.windowID == SDL_GetWindowID(window))
		{
			switch (event->type)
			{
			case SDL_QUIT:

				break;
			case SDL_WINDOWEVENT:
				if (event->window.event == SDL_WINDOWEVENT_CLOSE)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_RESTORED)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_RESIZED)
				{
				}
				else if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_MINIMIZED)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_MOVED)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_HIT_TEST)
				{

				}
				break;
			}
		}
		return 0;
	}

	static void check_vkresult(VkResult err)
	{
		if (err == 0)
			return;
		CORE_LOG_ERROR("[vulkan] Error: VkResult = {0}", (int)err);
		if (err < 0)
			abort();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
		{
			CORE_LOG_ERROR("SDL_Init(): {0}", SDL_GetError());
			return;
		}

		// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

		// Get Display 0 info
		SDL_DisplayMode sdm0;
		SDL_GetCurrentDisplayMode(0, &sdm0);
		auto display0Width = sdm0.w;
		auto display0Height = sdm0.h;
		auto WindowDefaultWidth = display0Width * 0.8;
		auto WindowDefaultHeight = display0Height * 0.8;
		auto WindowMinWidth = display0Width * 0.55;
		auto WindowMinHeight = display0Height * 0.55;

		// Create window with Vulkan graphics context
		// TODO :User define toobar need add flag :SDL_WINDOW_BORDERLESS
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		m_Window = SDL_CreateWindow(PRODUCT_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowDefaultWidth, WindowDefaultHeight, window_flags);
		if (m_Window == NULL)
		{
			CORE_LOG_ERROR("SDL_CreateWindow(): %s", SDL_GetError());
			return;
		}
		SDL_SetWindowResizable(m_Window, SDL_TRUE);
		SDL_SetWindowMinimumSize(m_Window, WindowMinWidth, WindowMinHeight);
		// TODO :User define toobar
		//SDL_SetWindowHitTest(window, HitTestCallback, NULL);
		//SDL_AddEventWatch(SDLEventWatcher, this->m_Window);

		//Vulkan Setup
		VulkanSetup();

		// Create Window Surface
		if (SDL_Vulkan_CreateSurface(m_Window, m_VkInstance, &m_VkSurface) == 0)
		{
			CORE_LOG_ERROR("Failed to create Vulkan surface.");
			return;
		}
	}

#ifdef LUFT_USE_VULKAN_DEBUG_REPORT
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
	{
		(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
		//CORE_LOG_ERROR("[vulkan] Debug report from ObjectType: %i\nMessage: %s\n", objectType, pMessage);
		return VK_FALSE;
	}
#endif // APP_USE_VULKAN_DEBUG_REPORT

	void WindowsWindow::VulkanSetup()
	{
		VkResult err;
		unsigned int extensions_count = 0;
		larray<const char*> extensions;
		SDL_Vulkan_GetInstanceExtensions(m_Window, &extensions_count, NULL);
		extensions.resize(extensions_count);
		SDL_Vulkan_GetInstanceExtensions(m_Window, &extensions_count, extensions.data());
		// Create Vulkan Instance
		{
			VkInstanceCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

			// Enumerate available extensions
			unsigned int properties_count = 0;
			larray<VkExtensionProperties> properties;
			vkEnumerateInstanceExtensionProperties(NULL, &properties_count, NULL);
			properties.resize(properties_count);
			err = vkEnumerateInstanceExtensionProperties(NULL, &properties_count, properties.data());
			check_vkresult(err);
			// Enable required extensions
			for (auto& p : properties)
			{
				if (std::strcmp(p.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0) {
					extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
				}
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
				else if (std::strcmp(p.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0) {
					extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
					create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
				}

#endif // VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
			}

			// Enabling validation layers
#ifdef LUFT_USE_VULKAN_DEBUG_REPORT
			const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
			create_info.enabledLayerCount = 1;
			create_info.ppEnabledLayerNames = layers;
			extensions.push_back("VK_EXT_debug_report");
#endif // LUFT_USE_VULKAN_DEBUG_REPORT

			// Create Vulkan Instance
			create_info.enabledExtensionCount = extensions.size();
			create_info.ppEnabledExtensionNames = extensions.data();
			err = vkCreateInstance(&create_info, m_VkAllocator, &m_VkInstance);
			check_vkresult(err);

#ifdef LUFT_USE_VULKAN_DEBUG_REPORT
			auto f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugReportCallbackEXT");
			assert(f_vkCreateDebugReportCallbackEXT != NULL);
			VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
			debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debug_report_ci.pfnCallback = debug_report;
			debug_report_ci.pUserData = NULL;
			err = f_vkCreateDebugReportCallbackEXT(m_VkInstance, &debug_report_ci, m_VkAllocator, &m_VkDebugReport);
			check_vkresult(err);
#endif // LUFT_USE_VULKAN_DEBUG_REPORT

		}

		// Select Physical Device (GPU)
		{
			uint32_t gpu_count;
			VkResult err = vkEnumeratePhysicalDevices(m_VkInstance, &gpu_count, NULL);
			check_vkresult(err);
			assert(gpu_count > 0);

			larray<VkPhysicalDevice> gpus;
			gpus.resize(gpu_count);
			err = vkEnumeratePhysicalDevices(m_VkInstance, &gpu_count, gpus.data());
			check_vkresult(err);

			bool find_gpu = false;
			// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
			// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
			// dedicated GPUs) is out of scope of this sample.
			for (VkPhysicalDevice& device : gpus)
			{
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(device, &properties);
				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					find_gpu = true;
					m_VkPhysicalDevice = device;
					break;
				}
			}

			// Use first GPU (Integrated) is a Discrete one is not available.
			if (!find_gpu && gpu_count > 0)
			{
				find_gpu = true;
				m_VkPhysicalDevice = gpus[0];
			}
			if (!find_gpu)
				m_VkPhysicalDevice = VK_NULL_HANDLE;
		}

		// Select graphics queue family
		{
			uint32_t count;
			larray<VkQueueFamilyProperties> queues;
			vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &count, NULL);
			queues.resize(count);
			vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &count, queues.data());
			for (uint32_t i = 0; i < count; i++)
			{
				if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					m_VkQueueFamily = i;
					break;
				}
			}
			assert(m_VkQueueFamily != (uint32_t)-1);
		}

		// Create Logical Device (with 1 queue)
		{
			larray<const char*> device_extensions;
			device_extensions.push_back("VK_KHR_swapchain");

			// Enumerable physical device extension
			unsigned properties_count;
			larray<VkExtensionProperties> properties;
			vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, NULL, &properties_count, NULL);
			properties.resize(properties_count);
			vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, NULL, &properties_count, properties.data());

#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
			for (auto& p : properties)
			{
				if (std::strcmp(p.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0)
					device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
			}
#endif
			const float queue_priority[] = { 1.0f };
			VkDeviceQueueCreateInfo queue_info[1] = {};
			queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info[0].queueFamilyIndex = m_VkQueueFamily;
			queue_info[0].queueCount = 1;
			queue_info[0].pQueuePriorities = queue_priority;
			VkDeviceCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
			create_info.pQueueCreateInfos = queue_info;
			create_info.enabledExtensionCount = (uint32_t)device_extensions.size();
			create_info.ppEnabledExtensionNames = device_extensions.data();
			err = vkCreateDevice(m_VkPhysicalDevice, &create_info, m_VkAllocator, &m_VkDevice);
			check_vkresult(err);
			vkGetDeviceQueue(m_VkDevice, m_VkQueueFamily, 0, &m_VkQueue);
		}

		// Create Descriptor Pool
		// The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
		// If you wish to load e.g. additional textures you may need to alter pools sizes.
		{
			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
			};
			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1;
			pool_info.poolSizeCount = (uint32_t)ARRAYSIZE(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;
			err = vkCreateDescriptorPool(m_VkDevice, &pool_info, m_VkAllocator, &m_VkDescriptorPool);
			check_vkresult(err);
		}
	}

	void WindowsWindow::Shutdown()
	{
		vkDestroyDescriptorPool(m_VkDevice, m_VkDescriptorPool, m_VkAllocator);

#ifdef LUFT_USE_VULKAN_DEBUG_REPORT
		// Remove the debug report callback
		auto f_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugReportCallbackEXT");
		f_vkDestroyDebugReportCallbackEXT(m_VkInstance, m_VkDebugReport, m_VkAllocator);
#endif // APP_USE_VULKAN_DEBUG_REPORT

		vkDestroyDevice(m_VkDevice, m_VkAllocator);
		vkDestroyInstance(m_VkInstance, m_VkAllocator);
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
	}
}