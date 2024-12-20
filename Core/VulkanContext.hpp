#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanContext {
public:
  void initialize(VkSurfaceKHR surface);
  void cleanup();

  VkInstance getInstance() const { return instance; }
  VkDevice getDevice() const { return device; }
  VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
  uint32_t getGraphicsQueueFamilyIndex() const {
    return graphicsQueueFamilyIndex;
  }
  VkQueue getGraphicsQueue() const { return graphicsQueue; }
  VkQueue getPresentQueue() const { return presentQueue; }

private:
  VkInstance instance = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;

  uint32_t graphicsQueueFamilyIndex = 0;
  uint32_t presentQueueFamilyIndex = 0;
  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentQueue = VK_NULL_HANDLE;

  void createInstance();
  void pickPhysicalDevice(VkSurfaceKHR surface);
  void createLogicalDevice();
  bool checkValidationLayerSupport();
};