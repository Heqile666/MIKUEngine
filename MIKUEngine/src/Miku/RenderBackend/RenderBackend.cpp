#include "mikupch.h"
#include <Miku/RenderBackend/RenderBackendInterface.h>
#include "Direct3D12RenderBackend/Direct3D12RenderBackend.h"



namespace MIKU 
{
	const uint64 RenderBackendBufferSubresourceRange::WholeSize = ~0ULL;
	const RenderBackendBufferSubresourceRange RenderBackendBufferSubresourceRange::Whole = RenderBackendBufferSubresourceRange(0, RenderBackendBufferSubresourceRange::WholeSize);

	const uint32 RenderBackendTextureSubresourceRange::RemainingMipLevels = ~0U;
	const uint32 RenderBackendTextureSubresourceRange::RemainingArrayLayers = ~0U;
	const RenderBackendTextureSubresourceRange RenderBackendTextureSubresourceRange::All = RenderBackendTextureSubresourceRange(0, RenderBackendTextureSubresourceRange::RemainingMipLevels, 0, RenderBackendTextureSubresourceRange::RemainingArrayLayers);


	const RenderBackendTextureClearValue RenderBackendTextureClearValue::None = RenderBackendTextureClearValue();
	const RenderBackendTextureClearValue RenderBackendTextureClearValue::Black = RenderBackendTextureClearValue(0.0f, 0.0f, 0.0f, 1.0f);
	const RenderBackendTextureClearValue RenderBackendTextureClearValue::White = RenderBackendTextureClearValue(1.0f, 1.0f, 1.0f, 1.0f);
	const RenderBackendTextureClearValue RenderBackendTextureClearValue::DepthOne = RenderBackendTextureClearValue(1.0f, 0);
	const RenderBackendTextureClearValue RenderBackendTextureClearValue::DepthZero = RenderBackendTextureClearValue(0.0f, 0);

	const RenderBackendColorBlendAttachmentState RenderBackendColorBlendAttachmentState::Additive =
	{
			true,                                  // blendEnable
			RenderBackendBlendFactor::One,         // srcColorBlendFactor
			RenderBackendBlendFactor::One,         // dstColorBlendFactor
			RenderBackendBlendOp::Add,             // colorBlendOp
			RenderBackendBlendFactor::Zero,        // srcAlphaBlendFactor
			RenderBackendBlendFactor::One,         // dstAlphaBlendFactor
			RenderBackendBlendOp::Add,             // alphaBlendOp
			RenderBackendColorComponentFlags::RGBA // writeMask
	};
	const RenderBackendColorBlendAttachmentState RenderBackendColorBlendAttachmentState::AdditiveRGB =
	{
		true,                                  // blendEnable
		RenderBackendBlendFactor::One,         // srcColorBlendFactor
		RenderBackendBlendFactor::One,         // dstColorBlendFactor
		RenderBackendBlendOp::Add,             // colorBlendOp
		RenderBackendBlendFactor::One,         // srcAlphaBlendFactor (默认)
		RenderBackendBlendFactor::Zero,        // dstAlphaBlendFactor (默认)
		RenderBackendBlendOp::Add,             // alphaBlendOp       (默认)
		RenderBackendColorComponentFlags::RGB  // writeMask
	};

	RenderBackend* RenderBackendCreateInstance(const RenderBackendDesc* desc) 
	{
		RenderBackend* backend = nullptr;
		if (desc->type == RenderBackendType::Vulkan)
		{
			//backend = RenderBackendCreateVulkan(desc);
		}
		else if(desc->type == RenderBackendType::D3D12)
		{
			backend = RenderBackendCreateDirect3D12(desc);
		}
		else 
		{
			MIKU_CORE_ERROR("UnKnown RenderBackendType!");
		}

		return backend;
	}
	
	void RenderBackendDestoryInstance(RenderBackend* backend) 
	{
	
	}
}