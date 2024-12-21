#pragma once
#include <vulkan/vulkan_core.h>

#include <GLFW/glfw3.h>

// Core components
#include "CommandPool.hpp"
#include "Framebuffer.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"
#include "Synchronization.hpp"
#include "VulkanContext.hpp"

#include "Utils.hpp"

#include <vector>

class HelloTriangleApplication {
public:
  HelloTriangleApplication() = default;
  void run();
  void initWindow();

  void initVulkan();
  void mainLoop();
  void cleanup();

private:
  GLFWwindow *window;

  VulkanContext vulkanContext;
  Swapchain swapchain;
  RenderPass renderPass;
  Framebuffer framebuffer;
  Pipeline pipeline;
  CommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;
  Synchronization synchronization;

  // Vertex Buffer
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  // Frame tracking
  size_t currentFrame = 0;

  void drawFrame();
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  void createVertexBuffer();
  void recordCommandBuffers(VkPipeline vk_pipeline, VkPipelineLayout pipeline_layout);
};
