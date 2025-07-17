#include "vulkanMain.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <ng-log/logging.h>

namespace Anito3D {

	VulkanMain::VulkanMain() : surface(VK_NULL_HANDLE), graphicsQueue(VK_NULL_HANDLE), renderPass(VK_NULL_HANDLE), imguiPool(VK_NULL_HANDLE), width(0), height(0), commandPool(VK_NULL_HANDLE) {}

	VulkanMain::~VulkanMain() {
		cleanup();
	}

    bool VulkanMain::init(GLFWwindow* window, uint32_t width, uint32_t height) {
        this->width = width;
        this->height = height;

        // Initialize Vulkan
        try {
            initVulkan(window);
            initImGui(window);
        }
        catch (const std::exception& e) {
            LOG(ERROR) << "Vulkan initialization failed: " << e.what();
            return false;
        }
        return true;
    }

    void VulkanMain::initVulkan(GLFWwindow* window) {
        // Check required extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        if (!glfwExtensions) {
            LOG(ERROR) << "Failed to get GLFW required extensions";
            throw std::runtime_error("GLFW extension retrieval failed");
        }
        std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        // Create Vulkan instance with vk-bootstrap
        vkb::InstanceBuilder instanceBuilder;
        auto instRet = instanceBuilder.set_app_name("Anito3D Benchmark Sandbox")
            .request_validation_layers(true)
            .use_default_debug_messenger()
            .require_api_version(1, 3, 0)
            .build();
        if (!instRet) {
            LOG(ERROR) << "Failed to create Vulkan instance: " << instRet.error().message();
            throw std::runtime_error("Vulkan instance creation failed");
        }
        vkbInstance = instRet.value();
        LOG(INFO) << "Vulkan instance created";

        // Create surface
        VkResult result = glfwCreateWindowSurface(vkbInstance.instance, window, nullptr, &surface);
        if (result != VK_SUCCESS) {
            LOG(ERROR) << "Failed to create Vulkan surface: " << result;
            throw std::runtime_error("Vulkan surface creation failed");
        }

        // Select physical device (AMD GPU)
        vkb::PhysicalDeviceSelector physDeviceSelector(vkbInstance);
        auto physDeviceRet = physDeviceSelector.set_surface(surface)
            .set_minimum_version(1, 3)
            .add_required_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
            .select();
        if (!physDeviceRet) {
            LOG(ERROR) << "Failed to select physical device: " << physDeviceRet.error().message();
            throw std::runtime_error("Physical device selection failed");
        }
        physicalDevice = physDeviceRet.value();
        LOG(INFO) << "Physical device selected: " << physicalDevice.name;

        // Create logical device
        vkb::DeviceBuilder deviceBuilder(physicalDevice);
        auto deviceRet = deviceBuilder.build();
        if (!deviceRet) {
            LOG(ERROR) << "Failed to create Vulkan device: " << deviceRet.error().message();
            throw std::runtime_error("Vulkan device creation failed");
        }
        device = deviceRet.value();
        LOG(INFO) << "Vulkan device created";

        // Get graphics queue
        auto queueRet = device.get_queue(vkb::QueueType::graphics);
        if (!queueRet) {
            LOG(ERROR) << "Failed to get graphics queue: " << queueRet.error().message();
            throw std::runtime_error("Graphics queue retrieval failed");
        }
        graphicsQueue = queueRet.value();

        // Create swapchain
        vkb::SwapchainBuilder swapchainBuilder(device);
        auto swapRet = swapchainBuilder.set_desired_extent(width, height)
            .set_desired_min_image_count(2)
            .set_desired_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            .build();
        if (!swapRet) {
            LOG(ERROR) << "Failed to create swapchain: " << swapRet.error().message();
            throw std::runtime_error("Swapchain creation failed");
        }
        swapchain = swapRet.value();
        swapchainImages = swapchain.get_images().value();
        swapchainImageViews = swapchain.get_image_views().value();
        LOG(INFO) << "Swapchain created with " << swapchainImages.size() << " images";

        createRenderPass();
        createFramebuffers();
        createCommandBuffers();
        createSyncObjects();
    }


    void VulkanMain::initImGui(GLFWwindow* window) {
        // Create descriptor pool for ImGui
        VkDescriptorPoolSize poolSizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000;
        poolInfo.poolSizeCount = std::size(poolSizes);
        poolInfo.pPoolSizes = poolSizes;
        VkResult result = vkCreateDescriptorPool(device.device, &poolInfo, nullptr, &imguiPool);
        if (result != VK_SUCCESS) {
            LOG(ERROR) << "Failed to create ImGui descriptor pool: " << result;
            throw std::runtime_error("ImGui descriptor pool creation failed");
        }

        // Initialize ImGui
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
        if (!ImGui_ImplGlfw_InitForVulkan(window, true)) {
            LOG(ERROR) << "Failed to initialize ImGui GLFW backend";
            throw std::runtime_error("ImGui GLFW initialization failed");
        }

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = vkbInstance.instance;
        initInfo.PhysicalDevice = physicalDevice.physical_device;
        initInfo.Device = device.device;
        initInfo.QueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();
        initInfo.Queue = graphicsQueue;
        initInfo.PipelineCache = VK_NULL_HANDLE;
        initInfo.DescriptorPool = imguiPool;
        initInfo.Subpass = 0;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = swapchain.image_count;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        if (!ImGui_ImplVulkan_Init(&initInfo, renderPass)) {
            LOG(ERROR) << "Failed to initialize ImGui Vulkan backend";
            throw std::runtime_error("ImGui Vulkan initialization failed");
        }

        // Upload ImGui fonts
        VkCommandBuffer commandBuffer = commandBuffers[0];
        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if (result != VK_SUCCESS) {
            LOG(ERROR) << "Failed to begin command buffer for ImGui fonts: " << result;
            throw std::runtime_error("ImGui font command buffer failed");
        }
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        vkEndCommandBuffer(commandBuffer);
        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        LOG(INFO) << "ImGui initialized successfully";
    }

    void VulkanMain::createRenderPass() {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = swapchain.image_format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(device.device, &renderPassInfo, nullptr, &renderPass);
        if (result != VK_SUCCESS) {
            LOG(ERROR) << "Failed to create render pass: " << result;
            throw std::runtime_error("Render pass creation failed");
        }
    }

    void VulkanMain::createFramebuffers() {
        framebuffers.resize(swapchainImageViews.size());
        for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
            VkFramebufferCreateInfo fbInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            fbInfo.renderPass = renderPass;
            fbInfo.attachmentCount = 1;
            fbInfo.pAttachments = &swapchainImageViews[i];
            fbInfo.width = width;
            fbInfo.height = height;
            fbInfo.layers = 1;
            VkResult result = vkCreateFramebuffer(device.device, &fbInfo, nullptr, &framebuffers[i]);
            if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to create framebuffer " << i << ": " << result;
                throw std::runtime_error("Framebuffer creation failed");
            }
        }
    }

    void VulkanMain::createCommandBuffers() {
        VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolInfo.queueFamilyIndex = device.get_queue_index(vkb::QueueType::graphics).value();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Enable command buffer reset
        VkResult result = vkCreateCommandPool(device.device, &poolInfo, nullptr, &commandPool);
        if (result != VK_SUCCESS) {
            LOG(ERROR) << "Failed to create command pool: " << result;
            throw std::runtime_error("Command pool creation failed");
        }
        LOG(INFO) << "Command pool created with VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT";

        VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(framebuffers.size());
        commandBuffers.resize(framebuffers.size());
        result = vkAllocateCommandBuffers(device.device, &allocInfo, commandBuffers.data());
        if (result != VK_SUCCESS) {
            LOG(ERROR) << "Failed to allocate command buffers: " << result;
            throw std::runtime_error("Command buffer allocation failed");
        }
        LOG(INFO) << "Allocated " << commandBuffers.size() << " command buffers";
    }

    void VulkanMain::createSyncObjects() {
        imageAvailableSemaphores.resize(2);
        renderFinishedSemaphores.resize(2);
        inFlightFences.resize(2);

        VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < 2; ++i) {
            VkResult result = vkCreateSemaphore(device.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
            if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to create image available semaphore " << i << ": " << result;
                throw std::runtime_error("Semaphore creation failed");
            }
            result = vkCreateSemaphore(device.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
            if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to create render finished semaphore " << i << ": " << result;
                throw std::runtime_error("Semaphore creation failed");
            }
            result = vkCreateFence(device.device, &fenceInfo, nullptr, &inFlightFences[i]);
            if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to create fence " << i << ": " << result;
                throw std::runtime_error("Fence creation failed");
            }
        }
    }

    int VulkanMain::runMainMenu(GLFWwindow* window) {
        if (!window) {
            LOG(ERROR) << "VulkanMain::runMainMenu: Null window provided";
            return 0;
        }

        int selectedRenderer = 0; // 0 = none, 1 = BGFX, 2 = Ogre3D, 3 = Diligent
        uint32_t currentFrame = 0;

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Main menu UI
            ImGui::Begin("Anito3D Benchmark Menu");
            ImGui::Text("Select Renderer:");
            if (ImGui::Button("BGFX")) selectedRenderer = 1;
            ImGui::Button("Ogre3D (Coming Soon)");
            ImGui::Button("Diligent Engine (Coming Soon)");
            ImGui::End();

            // Render
            vkWaitForFences(device.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
            vkResetFences(device.device, 1, &inFlightFences[currentFrame]);

            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(device.device, swapchain.swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
                LOG(WARNING) << "Swapchain out of date, skipping frame";
                continue;
            }
            else if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to acquire swapchain image: " << result;
                break;
            }

            vkResetCommandBuffer(commandBuffers[imageIndex], 0);
            VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
            result = vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo);
            if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to begin command buffer: " << result;
                break;
            }

            VkRenderPassBeginInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = framebuffers[imageIndex];
            renderPassInfo.renderArea.extent = { width, height };
            VkClearValue clearColor = { {{0.3f, 0.3f, 0.3f, 1.0f}} };
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;
            vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[imageIndex]);

            vkCmdEndRenderPass(commandBuffers[imageIndex]);
            result = vkEndCommandBuffer(commandBuffers[imageIndex]);
            if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to end command buffer: " << result;
                break;
            }

            VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];
            result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
            if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to submit draw command buffer: " << result;
                break;
            }

            VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &swapchain.swapchain;
            presentInfo.pImageIndices = &imageIndex;
            result = vkQueuePresentKHR(graphicsQueue, &presentInfo);
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
                LOG(WARNING) << "Swapchain out of date, skipping frame";
                continue;
            }
            else if (result != VK_SUCCESS) {
                LOG(ERROR) << "Failed to present swapchain image: " << result;
                break;
            }

            currentFrame = (currentFrame + 1) % 2;

            if (selectedRenderer != 0) break;
        }

        return selectedRenderer;
    }

    void VulkanMain::cleanup() {
        if (device.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(device.device);

            for (auto semaphore : imageAvailableSemaphores) if (semaphore) vkDestroySemaphore(device.device, semaphore, nullptr);
            for (auto semaphore : renderFinishedSemaphores) if (semaphore) vkDestroySemaphore(device.device, semaphore, nullptr);
            for (auto fence : inFlightFences) if (fence) vkDestroyFence(device.device, fence, nullptr);
            if (commandPool) vkDestroyCommandPool(device.device, commandPool, nullptr);
            for (auto framebuffer : framebuffers) if (framebuffer) vkDestroyFramebuffer(device.device, framebuffer, nullptr);
            if (renderPass) vkDestroyRenderPass(device.device, renderPass, nullptr);
            if (imguiPool) {
                ImGui_ImplVulkan_Shutdown();
                vkDestroyDescriptorPool(device.device, imguiPool, nullptr);
            }
            for (auto imageView : swapchainImageViews) if (imageView) vkDestroyImageView(device.device, imageView, nullptr);
            if (swapchain.swapchain) vkb::destroy_swapchain(swapchain);
            if (device.device) vkb::destroy_device(device);
            if (surface) vkb::destroy_surface(vkbInstance.instance, surface);
            if (vkbInstance.instance) vkb::destroy_instance(vkbInstance);

            device.device = VK_NULL_HANDLE;
            surface = VK_NULL_HANDLE;
            renderPass = VK_NULL_HANDLE;
            imguiPool = VK_NULL_HANDLE;
        }
        LOG(INFO) << "VulkanMain cleaned up";
    }
}