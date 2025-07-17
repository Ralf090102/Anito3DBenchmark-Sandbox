#pragma once

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <functional>

#include "imGuiMain.hpp"
#include "anitoImGuiStyle.hpp"

namespace Anito3D {

	class VulkanMain {
	public:
		VulkanMain();
		~VulkanMain();

		bool init(GLFWwindow* window, uint32_t width, uint32_t height);

		// Run the main menu loop, returns selected renderer (0 = none, 1 = BGFX, etc.)
		int runMainMenu(GLFWwindow* window);

		void cleanup();

	private:
        // Vulkan resources
        vkb::Instance vkbInstance;
        VkSurfaceKHR surface;
        vkb::PhysicalDevice physicalDevice;
        vkb::Device device;
        VkQueue graphicsQueue;
        vkb::Swapchain swapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkRenderPass renderPass;
        std::vector<VkFramebuffer> framebuffers;
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        // ImGui resources
        VkDescriptorPool imguiPool;

        // Window dimensions
        uint32_t width, height;

        void initVulkan(GLFWwindow* window);
        void initImGui(GLFWwindow* window);
        void createRenderPass();
        void createFramebuffers();
        void createCommandBuffers();
        void createSyncObjects();
	};

}