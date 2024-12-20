#include "Synchronization.hpp"
#include <stdexcept>

void Synchronization::create(VkDevice device, uint32_t maxFramesInFlight) {
  imageAvailableSemaphores.resize(maxFramesInFlight);
  renderFinishedSemaphores.resize(maxFramesInFlight);
  inFlightFences.resize(maxFramesInFlight);

  VkSemaphoreCreateInfo semaphoreInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  fenceInfo.flags =
      VK_FENCE_CREATE_SIGNALED_BIT; // Initialize fences as signaled

  for (uint32_t i = 0; i < maxFramesInFlight; i++) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) !=
            VK_SUCCESS) {
      throw std::runtime_error(
          "Failed to create synchronization objects for a frame!");
    }
  }
}

void Synchronization::cleanup(VkDevice device) {
  for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
    vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    vkDestroyFence(device, inFlightFences[i], nullptr);
  }

  imageAvailableSemaphores.clear();
  renderFinishedSemaphores.clear();
  inFlightFences.clear();
}