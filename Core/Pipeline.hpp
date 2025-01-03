#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Pipeline {

public:
  void createBasicPipeline(VkDevice device, VkRenderPass renderPass,
                           VkExtent2D extent, VkPipelineLayout &pipelineLayout,
                           VkPipeline &pipeline);
  void createPipeline(VkDevice device, VkRenderPass renderPass,
                           VkExtent2D extent, VkPipelineLayout &pipelineLayout,
                           VkPipeline &pipeline, VkShaderModule &vertShaderModule,VkShaderModule &fragShaderModule);

  VkShaderModule createShaderModule(VkDevice device,
                                   const std::vector<char> &code);

  VkShaderModule createShaderModule(VkDevice device,
                                    const std::string &filePath);

private:

};