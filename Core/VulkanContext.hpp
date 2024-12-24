#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include "GLFW/glfw3.h"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanContext {
public:
  void createInstance(const std::vector<const char *> &extensions);
  void createSurface(VkInstance instance, GLFWwindow *window,
                     VkSurfaceKHR *surface);
  void pickPhysicalDevice(VkSurfaceKHR surface);
  void createLogicalDevice();
  void cleanup();

  VkInstance getInstance() const { return instance; }
  VkDevice getDevice() const { return device; }
  VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
  VkQueue getGraphicsQueue() const { return graphicsQueue; }
  VkQueue getPresentQueue() const { return presentQueue; }
  uint32_t getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }
  uint32_t getPresentQueueFamilyIndex() const { return presentQueueFamilyIndex; }
  std::vector<const char *> getRequiredExtensions();

private:
  VkInstance instance = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentQueue = VK_NULL_HANDLE;
  uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
  uint32_t presentQueueFamilyIndex = UINT32_MAX;

  bool enableValidationLayers = true;
  std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  bool checkValidationLayerSupport();

};