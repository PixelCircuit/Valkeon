//
// Created by a00847261 on 2024-12-31.
//

#pragma once
#include "Buffer.hpp"
#include "glm/vec3.hpp"

#include <CommandPool.hpp>
#include <Framebuffer.hpp>
#include <Pipeline.hpp>
#include <RenderPass.hpp>
#include <Swapchain.hpp>
#include <Synchronization.hpp>
#include <VulkanContext.hpp>
#include <cstdint>
#include <vector>

class ModelLoadingApplication {
  public:

  void run();
  void initWindow();
  void initVulkan();
  void mainLoop();
  void cleanup();

private:

  void loadModel();
  void createVertexBuffer();
  void createIndexBuffer();
  void drawFrame();
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void recordCommandBuffers(VkPipeline vk_pipeline,
                            VkPipelineLayout pipeline_layout);

  // Frame tracking
  size_t currentFrame = 0;

  std::vector<glm::vec3> positions;
  std::vector<uint32_t> indices;

  Buffer vertexBuffer;
  Buffer indexBuffer;

  VkFormat depthFormat;
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  GLFWwindow *window;

  VulkanContext vulkanContext;
  Swapchain swapchain;
  RenderPass renderPass;
  Framebuffer framebuffer;
  Pipeline pipeline;
  std::vector<VkCommandBuffer> commandBuffers;
  Synchronization synchronization;

  // Camera parameters with default values
  glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 5.0f);
  glm::vec3 cameraDir   = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 upVector     = glm::vec3(0.0f, 1.0f, 0.0f);
  float fov             = 45.0f; // Degrees
};
