#pragma once
#include <vulkan/vulkan_core.h>

#include <GLFW/glfw3.h>

// Core components
#include "Buffer.hpp"
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
  Buffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  // Frame tracking
  size_t currentFrame = 0;

  void drawFrame();
  void createVertexBuffer();
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void recordCommandBuffers(VkPipeline vk_pipeline,
                            VkPipelineLayout pipeline_layout);
};
