#pragma once

#define RENDER_BACKEND_REMAINING_MIP_LEVELS (~0U)//usigned int


enum
{
    RenderBackendMaxNumGPUs = 16,
    RenderBackendMaxNumDevices = 64,
    RenderBackendMaxNumSwapChainBuffers = 16,
    RenderBackendMaxNumSimultaneousColorRenderTargets = 8,
    RenderBackendMaxNumViewports = 16,
    RenderBackendMaxNumShaderStages = 10,
    RenderBackendMaxNumTextureMipLevels = 16,
    RenderBackendMaxNumTimingQueryRegions = 128,
};