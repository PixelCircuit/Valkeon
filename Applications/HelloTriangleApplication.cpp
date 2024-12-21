
#include "HelloTriangleApplication.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.h>

// Constants
const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

// Function declarations
//void framebufferResizeCallback(GLFWwindow *window, int width, int height);

struct Vertex {
  float position[2];
  float color[3];
};

void HelloTriangleApplication::run() {}

void HelloTriangleApplication::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL
    glfwWindowHint(GLFW_RESIZABLE,
                   GLFW_FALSE); // Disable resizing for simplicity

    window =
        glfwCreateWindow(WIDTH, HEIGHT, "Hello Triangle", nullptr, nullptr);
    if (!window) {
      throw std::runtime_error("Failed to create GLFW window!");
    }

    // Set framebuffer resize callback if window is resizable
    //glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  }

  void HelloTriangleApplication::initVulkan() {
    // Create Vulkan Instance and Logical Device
    // First, create a temporary surface using GLFW
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(vulkanContext.getInstance(), window, nullptr,
                                &surface) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create window surface!");
    }

    vulkanContext.initialize(surface);

    // Create Swapchain
    swapchain.create(vulkanContext, surface, WIDTH, HEIGHT);

    // Create Render Pass
    renderPass.create(vulkanContext.getDevice(), swapchain.getFormat());

    // Create Framebuffers
    framebuffer.create(vulkanContext.getDevice(), renderPass.getRenderPass(),
                       swapchain.getImageViews(), swapchain.getExtent());

    // Create Pipeline
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    pipeline.createBasicPipeline(
        vulkanContext.getDevice(), renderPass.getRenderPass(),
        swapchain.getExtent(), pipelineLayout, graphicsPipeline);

    // Cleanup unused pipeline layout and pipeline if stored elsewhere
    // In this minimal example, we'll keep them local for simplicity

    // Create Command Pool
    commandPool.create(vulkanContext.getDevice(),
                       vulkanContext.getGraphicsQueueFamilyIndex());

    // Allocate Command Buffers
    commandPool.allocateCommandBuffers(vulkanContext.getDevice(),
                                       swapchain.getImageViews().size(),
                                       commandBuffers);

    // Create Synchronization Objects
    synchronization.create(vulkanContext.getDevice(), MAX_FRAMES_IN_FLIGHT);

    // Create Vertex Buffer
    createVertexBuffer();

    // Record Command Buffers
    recordCommandBuffers(graphicsPipeline, pipelineLayout);

    // Destroy temporary surface
    vkDestroySurfaceKHR(vulkanContext.getInstance(), surface, nullptr);
  }

  void HelloTriangleApplication::createVertexBuffer() {
    // Define vertices of the triangle
    Vertex vertices[] = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // Bottom vertex (red)
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},  // Top right vertex (green)
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}  // Top left vertex (blue)
    };

    VkDeviceSize bufferSize = sizeof(vertices);

    // Create Buffer (This is a simplified example. In a real application, you
    // should use proper memory management)
    VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkanContext.getDevice(), &bufferInfo, nullptr,
                       &vertexBuffer) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create vertex buffer!");
    }

    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkanContext.getDevice(), vertexBuffer,
                                  &memRequirements);

    VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits,   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(vulkanContext.getDevice(), &allocInfo, nullptr,
                         &vertexBufferMemory) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(vulkanContext.getDevice(), vertexBuffer,
                       vertexBufferMemory, 0);

    // Map memory and copy vertex data
    void *data;
    vkMapMemory(vulkanContext.getDevice(), vertexBufferMemory, 0, bufferSize, 0,
                &data);
    memcpy(data, vertices, (size_t)bufferSize);
    vkUnmapMemory(vulkanContext.getDevice(), vertexBufferMemory);
  }

  uint32_t HelloTriangleApplication::findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanContext.getPhysicalDevice(),
                                        &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
      if ((typeFilter & (1 << i)) &&
          (memProperties.memoryTypes[i].propertyFlags & properties) ==
              properties) {
        return i;
      }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
  }

  void HelloTriangleApplication::recordCommandBuffers(VkPipeline graphicsPipeline,
                            VkPipelineLayout pipelineLayout) {
    for (size_t i = 0; i < commandBuffers.size(); i++) {
      VkCommandBufferBeginInfo beginInfo{
          VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
      beginInfo.flags = 0;                  // Optional
      beginInfo.pInheritanceInfo = nullptr; // Optional

      if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
      }

      VkRenderPassBeginInfo renderPassInfo{
          VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
      renderPassInfo.renderPass = renderPass.getRenderPass();
      renderPassInfo.framebuffer = framebuffer.getFramebuffers()[i];
      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = swapchain.getExtent();

      VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColor;

      vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                           VK_SUBPASS_CONTENTS_INLINE);

      vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                        graphicsPipeline);

      VkBuffer vertexBuffers[] = {vertexBuffer};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

      vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

      vkCmdEndRenderPass(commandBuffers[i]);

      if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
      }
    }
  }

  void HelloTriangleApplication::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      drawFrame();
    }

    vkDeviceWaitIdle(vulkanContext.getDevice());
  }

  void HelloTriangleApplication::drawFrame() {
    // Wait for the current frame's fence to be signaled
    vkWaitForFences(vulkanContext.getDevice(), 1,
                    &synchronization.inFlightFence(currentFrame), VK_TRUE,
                    UINT64_MAX);

    // Reset the fence for the next frame
    vkResetFences(vulkanContext.getDevice(), 1,
                  &synchronization.inFlightFence(currentFrame));

    // Acquire the next image from the swapchain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        vulkanContext.getDevice(), swapchain.getSwapchain(), UINT64_MAX,
        synchronization.acquireSemaphore(currentFrame), VK_NULL_HANDLE,
        &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      // Handle swapchain recreation if window is resized (not handled in this
      // minimal example)
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("Failed to acquire swap chain image!");
    }

    // Submit the command buffer
    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};

    VkSemaphore waitSemaphores[] = {
        synchronization.acquireSemaphore(currentFrame)};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {
        synchronization.renderSemaphore(currentFrame)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanContext.getGraphicsQueue(), 1, &submitInfo,
                      synchronization.inFlightFence(currentFrame)) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to submit draw command buffer!");
    }

    // Present the image
    VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain.getSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(vulkanContext.getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      // Handle swapchain recreation if window is resized (not handled in this
      // minimal example)
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void HelloTriangleApplication::cleanup() {
    // Cleanup synchronization objects
    synchronization.cleanup(vulkanContext.getDevice());

    // Cleanup vertex buffer
    vkDestroyBuffer(vulkanContext.getDevice(), vertexBuffer, nullptr);
    vkFreeMemory(vulkanContext.getDevice(), vertexBufferMemory, nullptr);

    // Cleanup command pool
    commandPool.cleanup(vulkanContext.getDevice());

    // Cleanup pipeline
    // Note: Pipeline and pipeline layout were created in initVulkan and not
    // stored, so in a real application, you should store and destroy them
    // properly here.

    // Cleanup framebuffers
    framebuffer.cleanup(vulkanContext.getDevice());

    // Cleanup render pass
    renderPass.cleanup(vulkanContext.getDevice());

    // Cleanup swapchain
    swapchain.cleanup(vulkanContext.getDevice());

    // Cleanup Vulkan context
    vulkanContext.cleanup();

    // Destroy window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
  }
