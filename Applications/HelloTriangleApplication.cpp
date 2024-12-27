#include "HelloTriangleApplication.hpp"

#include "Buffer.hpp"

#include "Types.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>

// Implementation of HelloTriangleApplication methods
void HelloTriangleApplication::run() {
  initWindow();
  initVulkan();
  mainLoop();
  cleanup();
}

void HelloTriangleApplication::initWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW!");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(800, 600, "Hello Triangle", nullptr, nullptr);
  if (!window) {
    throw std::runtime_error("Failed to create GLFW window!");
  }
}

void HelloTriangleApplication::initVulkan() {
  // Step 1: Get required extensions from GLFW
  std::vector<const char *> extensions = vulkanContext.getRequiredExtensions();

  // Step 2: Create Vulkan instance with the required extensions
  try {
    vulkanContext.createInstance(extensions);
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to create Vulkan instance: ") +
                             e.what());
  }

  // Step 3: Create window surface
  VkSurfaceKHR surface;
  try {
    vulkanContext.createSurface(vulkanContext.getInstance(), window, &surface);
    std::cout << "Window Surface Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to create window surface: ") +
                             e.what());
  }

  // Step 4: Pick physical device based on the surface
  try {
    vulkanContext.pickPhysicalDevice(surface);
    std::cout << "Physical Device Selected Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to pick physical device: ") +
                             e.what());
  }

  // Step 5: Create logical device
  try {
    vulkanContext.createLogicalDevice();
    std::cout << "Logical Device Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to create logical device: ") +
                             e.what());
  }

  // Step 6: Create Swapchain (Assuming you have a Swapchain class)
  try {
    swapchain.create(vulkanContext, surface, 800, 600);
    std::cout << "Swapchain Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to create swapchain: ") +
                             e.what());
  }

  // Step 7: Create Render Pass
  try {
    renderPass.create(vulkanContext.getDevice(), swapchain.getFormat());
    std::cout << "Render Pass Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to create render pass: ") +
                             e.what());
  }

  // Step 8: Create Framebuffers
  try {
    framebuffer.create(vulkanContext.getDevice(), renderPass.getRenderPass(),
                       swapchain.getImageViews(), swapchain.getExtent());
    std::cout << "Framebuffers Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to create framebuffers: ") +
                             e.what());
  }

  // Step 9: Create Graphics Pipeline
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  try {
    pipeline.createBasicPipeline(
        vulkanContext.getDevice(), renderPass.getRenderPass(),
        swapchain.getExtent(), pipelineLayout, graphicsPipeline);
    std::cout << "Graphics Pipeline Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(
        std::string("Failed to create graphics pipeline: ") + e.what());
  }

  // Step 10: Create Command Pool
  try {
    commandPool.create(vulkanContext.getDevice(),
                       vulkanContext.getGraphicsQueueFamilyIndex());
    std::cout << "Command Pool Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to create command pool: ") +
                             e.what());
  }

  // Step 11: Allocate Command Buffers
  try {
    commandPool.allocateCommandBuffers(vulkanContext.getDevice(),
                                       swapchain.getImageViews().size(),
                                       commandBuffers);
    std::cout << "Command Buffers Allocated Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(
        std::string("Failed to allocate command buffers: ") + e.what());
  }

  // Step 12: Create Synchronization Objects
  try {
    synchronization.create(vulkanContext.getDevice(),
                           2); // Assuming MAX_FRAMES_IN_FLIGHT = 2
    std::cout << "Synchronization Objects Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(
        std::string("Failed to create synchronization objects: ") + e.what());
  }

  // Step 13: Create Vertex Buffer
  try {
    createVertexBuffer();
    std::cout << "Vertex Buffer Created Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to create vertex buffer: ") +
                             e.what());
  }

  // Step 14: Record Command Buffers
  try {
    for (size_t i = 0; i < commandBuffers.size(); i++) {
      VkCommandBufferBeginInfo beginInfo{
          VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
      beginInfo.flags = 0;
      beginInfo.pInheritanceInfo = nullptr;

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

      // Bind vertex buffer
      VkBuffer vertexBuffers[] = {vertexBuffer.getBuffer()};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

      vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

      vkCmdEndRenderPass(commandBuffers[i]);

      if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
      }
    }
    std::cout << "Command Buffers Recorded Successfully." << std::endl;
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(std::string("Failed to record command buffers: ") +
                             e.what());
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
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

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
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
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

  currentFrame = (currentFrame + 1) % 2; // Assuming MAX_FRAMES_IN_FLIGHT = 2
}

void HelloTriangleApplication::createVertexBuffer() {
  // Define vertices of the triangle
  // Vertex data
  const std::vector<Vertex> vertices = {
      {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // Bottom vertex (Red)
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},  // Top right vertex (Green)
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, // Top left vertex (Blue)
  };

  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  // Create a staging buffer (host-visible)
  Buffer stagingBuffer;
  stagingBuffer.create(vulkanContext, bufferSize,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Map memory and copy vertex data
  void *data;
  vkMapMemory(vulkanContext.getDevice(), stagingBuffer.getMemory(), 0,
              bufferSize, 0, &data);
  memcpy(data, vertices.data(), (size_t)bufferSize);
  vkUnmapMemory(vulkanContext.getDevice(), stagingBuffer.getMemory());

  // Create the actual vertex buffer (device local)
  vertexBuffer.create(vulkanContext, bufferSize,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  // Copy data from staging buffer to vertex buffer
  copyBuffer(stagingBuffer.getBuffer(), vertexBuffer.getBuffer(), bufferSize);

  // Cleanup staging buffer
  stagingBuffer.cleanup(vulkanContext.getDevice());
}

void HelloTriangleApplication::copyBuffer(VkBuffer srcBuffer,
                                          VkBuffer dstBuffer,
                                          VkDeviceSize size) {
  // Allocate a temporary command buffer
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool.getCommandPool();
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  if (vkAllocateCommandBuffers(vulkanContext.getDevice(), &allocInfo,
                               &commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error(
        "Failed to allocate command buffer for buffer copy!");
  }

  // Begin recording
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error(
        "Failed to begin recording buffer copy command buffer!");
  }

  // Copy buffer
  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  // End recording
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record buffer copy command buffer!");
  }

  // Submit the command buffer
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  // No semaphores to wait on or signal in this case
  if (vkQueueSubmit(vulkanContext.getGraphicsQueue(), 1, &submitInfo,
                    VK_NULL_HANDLE) != VK_SUCCESS) {
    throw std::runtime_error("Failed to submit buffer copy command buffer!");
  }

  // Wait for the queue to finish
  vkQueueWaitIdle(vulkanContext.getGraphicsQueue());

  // Free the command buffer
  vkFreeCommandBuffers(vulkanContext.getDevice(), commandPool.getCommandPool(),
                       1, &commandBuffer);
}
void HelloTriangleApplication::cleanup() {
  synchronization.cleanup(vulkanContext.getDevice());
  commandPool.cleanup(vulkanContext.getDevice());
  framebuffer.cleanup(vulkanContext.getDevice());
  renderPass.cleanup(vulkanContext.getDevice());
  swapchain.cleanup(vulkanContext.getDevice());
  vulkanContext.cleanup();
  glfwDestroyWindow(window);
  glfwTerminate();
}
