#include <lvk/LVK.h>
#include <GLFW/glfw3.h>
#include <shared/Utils.h>
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::mat4;
using glm::vec3;

int main()
{
  minilog::initialize(nullptr, { .threadNames = false });


  GLFWwindow* window = nullptr;

  std::unique_ptr<lvk::IContext> ctx;
  {
    int width = -95;
    int height = 95;

    window = lvk::initWindow("Quad Texture", width,height);
    ctx    = lvk::createVulkanContextWithSwapchain(window, width, height, {});

  }

  lvk::Holder<lvk::ShaderModuleHandle> vertShader = loadShaderModule(ctx, "Applications/QuadTexture/src/main.vert");
  lvk::Holder<lvk::ShaderModuleHandle> fragShader = loadShaderModule(ctx, "Applications/QuadTexture/src/main.frag");
  
  lvk::Holder<lvk::RenderPipelineHandle> pipeline = ctx->createRenderPipeline({
    .topology = lvk::Topology_TriangleStrip, 
    .smVert = vertShader, 
    .smFrag = fragShader,
    .color = { { .format = ctx->getSwapchainFormat() } }, 
    .cullMode = lvk::CullMode_Back,      
     });

  assert(pipeline.valid());

  int w, h, comp;
  const uint8_t* img = stbi_load("data/wood.jpg", &w, &h, &comp, 4);

  assert(img);

  lvk::Holder<lvk::TextureHandle> texture = ctx->createTexture({
    .type = lvk::TextureType_2D, 
    .format = lvk::Format_RGBA_UN8, 
    .dimensions = { (uint32_t)w, (uint32_t)h },
    .usage = lvk::TextureUsageBits_Sampled,
    .data = img,
    .debugName  = "quad_Texture_wood.jpg"
      
   });

  stbi_image_free((void*)img);

  while (!glfwWindowShouldClose(window)) {
  
    glfwPollEvents();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (!width || !height) {
      continue;
    }

    const float ratio = width / (float)height;

    const mat4 m = glm::rotate(mat4(1.0f), (float)glfwGetTime(), vec3(0.0f, 0.0f, 1.0f));

    const mat4 p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

    const struct PerFrameData{
      mat4 mvp;
      uint32_t textureId;
    } PC = {
      .mvp       = p * m,
      .textureId = texture.index(),
    };

    lvk::ICommandBuffer& buff = ctx->acquireCommandBuffer();
    {

      const lvk::RenderPass renderPass = {
        .color = { { .loadOp = lvk::LoadOp_Clear, .clearColor = { 1.0f, 1.0f, 1.0f, 1.0f } } }
      };

      const lvk::Framebuffer framebuffer = { .color        = { { .texture = ctx->getCurrentSwapchainTexture() } },};

      buff.cmdBeginRendering(renderPass,framebuffer);

      buff.cmdPushDebugGroupLabel("Quad", 0xff0000ff);
      {
        buff.cmdBindRenderPipeline(pipeline);
        buff.cmdPushConstants(PC);
        buff.cmdDraw(4);
      }
      buff.cmdPopDebugGroupLabel();
      buff.cmdEndRendering();
    }
    ctx->submit(buff, ctx->getCurrentSwapchainTexture());

  }
  vertShader.reset();
  fragShader.reset();
  texture.reset();
  pipeline.reset();
  ctx.reset();
  

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}