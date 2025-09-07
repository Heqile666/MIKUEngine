#pragma once
#include "Miku/Core/CoreTypes.h"
#include "RenderBackendTextureFormat.h"
#include "Miku/Core/CoreDefinitions.h"
#include "Miku/RenderBackend/RenderBackendHandles.h"
#include <memory>
#include <vector>
#include <Miku/RenderBackend/RenderBackendDefinitions.h>
#include <string>

namespace MIKU
{
    using PhysicalDeviceID = uint32;


   

    struct RenderBackendShaderBlob 
    {
        uint64 size;
        const uint8* data;
    };

    enum class RenderBackendResourceState
    {
        Undefined = 0,
        Present = 1,
        IndirectArgument = 2,
        VertexBuffer = 3,
        IndexBuffer = 4,
        ShaderResource = 5,
        CopySrc = 6,
        DepthStencilReadOnly = 7,
        RenderTarget = 8,
        CopyDst = 9,
        UnorderedAccess = 10,
        DepthStencil = 11,

    
    };

    enum class RenderBackendBufferCreateFlags
    {
        None = 0,
        // Update
        Upload = (1 << 0),
        Readback = (1 << 1),
        // Usage
        UnorderedAccess = (1 << 2),
        CopySrc = (1 << 3),
        CopyDst = (1 << 4),
        IndirectArguments = (1 << 5),
        ShaderResource = (1 << 6),
        VertexBuffer = (1 << 7),
        IndexBuffer = (1 << 8),
        UniformBuffer = (1 << 9),
        AccelerationStruture = (1 << 10),
        ShaderBindingTable = (1 << 11),
        // Memory access
        CreateMapped = (1 << 12),
        CpuOnly = (1 << 13),
        GpuOnly = (1 << 14),
        CpuToGpu = (1 << 15),
        GpuToCpu = (1 << 16),
    };
    MIKU_ENUM_CLASS_OPERATORS(RenderBackendBufferCreateFlags);

	struct RenderBackendSwapChainDesc 
	{
		uint32 width;
		uint32 height;
		uint64 windowHandle;
		uint32 numBuffers;
		bool vsync;
		bool fullScreen;
		RenderBackendTextureFormat format;
	
	
	};

	struct RenderBackendBufferDesc 
	{
        RenderBackendBufferDesc() = default;
        RenderBackendBufferDesc(uint32 elementSize, uint32 elementCount, RenderBackendBufferCreateFlags flags)
            : elementSize(elementSize)
            , elementCount(elementCount)
            , size(elementSize* elementCount)
            , flags(flags) {}

        FORCEINLINE bool operator==(const RenderBackendBufferDesc& rhs) const
        {
            return size == rhs.size
                && elementSize == rhs.size
                && elementCount == rhs.elementCount
                && flags == rhs.flags;
        
        }

        uint64 size;
        uint32 elementSize;
        uint32 elementCount;
        RenderBackendBufferCreateFlags flags;

        static RenderBackendBufferDesc Create(uint32 elementSize, uint32 elementCount, RenderBackendBufferCreateFlags flags) 
        {
            return RenderBackendBufferDesc(elementSize, elementCount, flags);
        }
	    
        static RenderBackendBufferDesc CreateUpload(uint64 bytes) 
        {
            auto flags = RenderBackendBufferCreateFlags::Upload | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }
	    
        static RenderBackendBufferDesc CreateUpload(uint64 bytes, RenderBackendBufferCreateFlags flags)
        {
            flags |= RenderBackendBufferCreateFlags::Upload | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateReadback(uint64 bytes)
        {
            auto flags = RenderBackendBufferCreateFlags::Readback | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateIndirectArguments(uint64 bytes)
        {
            auto flags = RenderBackendBufferCreateFlags::IndirectArguments | RenderBackendBufferCreateFlags::UnorderedAccess | RenderBackendBufferCreateFlags::ShaderResource;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateIndirectArguments(uint32 elementSize, uint32 elementCount)
        {
            auto flags = RenderBackendBufferCreateFlags::IndirectArguments | RenderBackendBufferCreateFlags::UnorderedAccess | RenderBackendBufferCreateFlags::ShaderResource;
            return RenderBackendBufferDesc(elementSize, elementCount, flags);
        }

        static RenderBackendBufferDesc CreateByteAddress(uint64 bytes, bool dynamic = false)
        {
            auto flags = RenderBackendBufferCreateFlags::UnorderedAccess | RenderBackendBufferCreateFlags::ShaderResource;
            if (dynamic)
            {
                flags |= RenderBackendBufferCreateFlags::CpuToGpu;
            }
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateStructured(uint32 elementSize, uint32 elementCount)
        {
            auto flags = RenderBackendBufferCreateFlags::UnorderedAccess | RenderBackendBufferCreateFlags::ShaderResource;
            return RenderBackendBufferDesc(elementSize, elementCount, flags);
        }

        static RenderBackendBufferDesc CreateShaderBindingTable(uint64 bytes)
        {
            auto flags = RenderBackendBufferCreateFlags::ShaderBindingTable | RenderBackendBufferCreateFlags::CpuOnly | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateShaderBindingTable(uint64 handleSize, uint32 handleCount)
        {
            auto flags = RenderBackendBufferCreateFlags::ShaderBindingTable | RenderBackendBufferCreateFlags::CpuOnly | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)((handleSize * handleCount) >> 2), flags);
        }
	};

    enum class RenderBackendTextureType 
    {
        Texture1D,
        Texture2D,
        Texture3D,
        TextureCube,
        Count
    };

    enum class RenderBackendTextureCreateFlags 
    {
        None = 0,
        RenderTarget = (1 << 0),
        DepthStencil = (1 << 1),
        ShaderResource = (1 << 2),
        UnorderedAccess = (1 << 3),
        Present = (1 << 4),
        SRGB = (1 << 5),
        NoTilling = (1 << 6),
        Dynamic = (1 << 7),
        Readback = (1 << 8),
        Sparse = (1 << 9)
    };
    MIKU_ENUM_CLASS_OPERATORS(RenderBackendTextureCreateFlags)

    enum class RenderBackendTextureAddressMode 
    {
        Wrap = 0, 
        Mirror = 1,
        Clamp = 2, 
        Border = 3, 

    };

    struct RenderBackendTextureClearValue
    {
        enum class ClearOp
        {
            None,
            Color,
            DepthStencil
        };

        ClearOp clearOp = ClearOp::None;

        union
        {
            struct ClearColorValue
            {
                union
                {
                    float float32[4];
                    int32 int32[4];
                    uint32 uint32[4];
                };

            } colorValue;

            struct ClearDepthStencilValue
            {
                float depth;
                uint32 stencil;
            } depthStencilValue;
        };

        static const RenderBackendTextureClearValue None;
        static const RenderBackendTextureClearValue Black;
        static const RenderBackendTextureClearValue White;
        static const RenderBackendTextureClearValue DepthOne;
        static const RenderBackendTextureClearValue DepthZero;

        RenderBackendTextureClearValue()
            :clearOp(ClearOp::None)
        {
            std::memset(&colorValue, 0, sizeof(colorValue));
            std::memset(&depthStencilValue, 0, sizeof(depthStencilValue));

        }

        RenderBackendTextureClearValue(float r, float g, float b, float a) 
            :clearOp(ClearOp::Color)
        {
            colorValue.float32[0] = r;
            colorValue.float32[1] = g;
            colorValue.float32[2] = b;
            colorValue.float32[3] = a;
        }

        RenderBackendTextureClearValue(uint32 r, uint32 g, uint32 b, uint32 a)
            : clearOp(ClearOp::Color)
        {
            colorValue.uint32[0] = r;
            colorValue.uint32[1] = g;
            colorValue.uint32[2] = b;
            colorValue.uint32[3] = a;
        }

        RenderBackendTextureClearValue(float depth, uint32 stencil)
            : clearOp(ClearOp::DepthStencil)
        {
            depthStencilValue.depth = depth;
            depthStencilValue.stencil = stencil;
        }

        bool operator==(const RenderBackendTextureClearValue& rhs) const 
        {
            if ((clearOp == ClearOp::None) && (rhs.clearOp == ClearOp::None)) 
            {
                return true;
            }
            else if ((clearOp == ClearOp::Color) && (rhs.clearOp == ClearOp::Color))
            {
                return colorValue.float32[0] == rhs.colorValue.float32[0]
                    && colorValue.float32[1] == rhs.colorValue.float32[1]
                    && colorValue.float32[2] == rhs.colorValue.float32[2]
                    && colorValue.float32[3] == rhs.colorValue.float32[3]
                    && colorValue.uint32[0] == rhs.colorValue.uint32[0]
                    && colorValue.uint32[1] == rhs.colorValue.uint32[1]
                    && colorValue.uint32[2] == rhs.colorValue.uint32[2]
                    && colorValue.uint32[3] == rhs.colorValue.uint32[3];
            }
            else if ((clearOp == ClearOp::DepthStencil) && (rhs.clearOp == ClearOp::DepthStencil))
            {
                return depthStencilValue.depth == rhs.depthStencilValue.depth
                    && depthStencilValue.stencil == rhs.depthStencilValue.stencil;
            }
            return false;
        }
    
    };

    enum class RenderBackendTextureFilter 
    {
        MinMagMipPoint,
        MinMagPointMipLinear,
        MinPointMagLinearMipPoint,
        MinPointMagMipLinear,
        MinLinearMagMipPoint,
        MinLinearMagPointMipLinear,
        MinMagLinearMipPoint,
        MinMagMipLinear,
        Anisotropic,
        ComparisonMinMagMipPoint,
        ComparisonMinMagPointMipLinear,
        ComparisonMinPointMagLinearMipPoint,
        ComparisonMinPointMagMipLinear,
        ComparisonMinLinearMagMipPoint,
        ComparisonMinLinearMagPointMipLinear,
        ComparisonMinMagLinearMipPoint,
        ComparisonMinMagMipLinear,
        ComparisonAnisotropic,
        MinimumMinMagMipPoint,
        MinimumMinMagPointMipLinear,
        MinimumMinPointMagLinearMipPoint,
        MinimumMinPointMagMipLinear,
        MinimumMinLinearMagMipPoint,
        MinimumMinLinearMagPointMipLinear,
        MinimumMinMagLinearMipPoint,
        MinimumMinMagMipLinear,
        MinimumAnisotropic,
        MaximumMinMagMipPoint,
        MaximumMinMagPointMipLinear,
        MaximumMinPointMagLinearMipPoint,
        MaximumMinPointMagMipLinear,
        MaximumMinLinearMagMipPoint,
        MaximumMinLinearMagPointMipLinear,
        MaximumMinMagLinearMipPoint,
        MaximumMinMagMipLinear,
        MaximumAnisotropic,
    };

    struct RenderBackendTextureSubresourceRange 
    {
        RenderBackendTextureSubresourceRange()
            :firstLevel(0), mipLevels(0xFFFFFFFF), firstLayer(0), arrayLayers(0xFFFFFFFF) {}
        RenderBackendTextureSubresourceRange(uint32 firstLevel, uint32 mipLevels, uint32 firstLayer, uint32 arrayLayers)
            : firstLevel(firstLevel), mipLevels(mipLevels), firstLayer(firstLayer), arrayLayers(arrayLayers) {}

        uint32 firstLevel;
        uint32 mipLevels;
        uint32 firstLayer;
        uint32 arrayLayers;

        FORCEINLINE bool IsAll() const 
        {
            return firstLayer  == 0
                && mipLevels   == 0xFFFFFFFF
                && firstLayer  == 0
                && arrayLayers == 0xFFFFFFFF;
        }

        FORCEINLINE bool operator==(const RenderBackendTextureSubresourceRange& rhs) const 
        {
            return firstLevel == rhs.firstLevel
                && mipLevels == rhs.mipLevels
                && firstLayer == rhs.firstLayer
                && arrayLayers == rhs.arrayLayers;
        }
        static const RenderBackendTextureSubresourceRange All;
    };

    struct RenderBackendTextureSubresourceLayers 
    {
        uint32 mipLevel;
        uint32 firstLayers;
        uint32 arrayLayers;
    };

    enum class RenderBackendPipelineType 
    {
        Graphics,
        Compute,
        RayTracing
    };
    enum class RenderBackendCompareOp
    {
        Never = 0,
        Less = 1,
        Equal = 2,
        LessOrEqual = 3,
        Greater = 4,
        NotEqual = 5,
        GreaterOrEqual = 6,
        Always = 7,
    };

    
    struct RenderBackendTextureDesc 
    {
        uint32 width;
        uint32 height;
        uint32 depth;
        uint32 mipLevels;
        uint32 arrayLayers;
        uint32 samples;
        RenderBackendTextureType type;
        RenderBackendTextureFormat format;
        RenderBackendTextureCreateFlags flags;
        RenderBackendTextureClearValue clearValue;
        RenderBackendResourceState initialState;

        RenderBackendTextureDesc()
            : width(1)
            , height(1)
            , depth(1)
            , mipLevels(1)
            , arrayLayers(1)
            , samples(1)
            , type(RenderBackendTextureType::Texture2D)
            , format(RenderBackendTextureFormat::Unknown)
            , flags(RenderBackendTextureCreateFlags::None)
            , clearValue(RenderBackendTextureClearValue::None)
            , initialState(RenderBackendResourceState::Undefined)
        {

        }

        RenderBackendTextureDesc(
            uint32 width,
            uint32 height,
            uint32 depth,
            uint32 mipLevels,
            uint32 arraySize,
            uint32 samples,
            RenderBackendTextureType type,
            RenderBackendTextureFormat format,
            RenderBackendTextureCreateFlags flags,
            RenderBackendTextureClearValue clearValue,
            RenderBackendResourceState initialState)
            : width(width)
            , height(height)
            , depth(depth)
            , mipLevels(mipLevels)
            , arrayLayers(arraySize)
            , samples(samples)
            , type(type)
            , format(format)
            , flags(flags)
            , clearValue(clearValue)
            , initialState(initialState) {}

        bool operator==(const RenderBackendTextureDesc& rhs) const 
        {
            return width == rhs.width
                && height == rhs.height
                && depth == rhs.depth
                && mipLevels == rhs.mipLevels
                && arrayLayers == rhs.arrayLayers
                && samples == rhs.samples
                && type == rhs.type
                && format == rhs.format
                && flags == rhs.flags
                && clearValue == rhs.clearValue
                && initialState == rhs.initialState;

        
        }

        static RenderBackendTextureDesc CreateTexture2D(
            uint32 width,
            uint32 height,
            uint32 mipLevels,
            RenderBackendTextureFormat format,
            RenderBackendTextureClearValue clearValue = RenderBackendTextureClearValue::None,
            RenderBackendResourceState initalState = RenderBackendResourceState::Undefined

        ) 
        {
            RenderBackendTextureCreateFlags flags = RenderBackendTextureCreateFlags::ShaderResource | RenderBackendTextureCreateFlags::RenderTarget;
            return RenderBackendTextureDesc(width, height, 1, mipLevels, 1, 1, RenderBackendTextureType::Texture2D, format, flags, clearValue, initalState);
        }
    
        static RenderBackendTextureDesc Create2D(
            uint32 width,
            uint32 height,
            RenderBackendTextureFormat format,
            RenderBackendTextureCreateFlags flags,
            RenderBackendTextureClearValue clearValue = RenderBackendTextureClearValue::None,
            uint32 mipLevels = 1,
            uint32 samples = 1,
            RenderBackendResourceState initalState = RenderBackendResourceState::Undefined)
        {
            return RenderBackendTextureDesc(width, height, 1, mipLevels, 1, 1, RenderBackendTextureType::Texture2D, format, flags, clearValue, initalState);
        }
        static RenderBackendTextureDesc Create2DArray(
            uint32 width,
            uint32 height,
            RenderBackendTextureFormat format,
            RenderBackendTextureCreateFlags flags,
            uint32 arraySize,
            RenderBackendTextureClearValue clearValue = RenderBackendTextureClearValue::None,
            uint32 mipLevels = 1,
            uint32 samples = 1,
            RenderBackendResourceState initalState = RenderBackendResourceState::Undefined)
        {
            return RenderBackendTextureDesc(width, height, 1, mipLevels, arraySize, samples, RenderBackendTextureType::Texture2D, format, flags, clearValue, initalState);
        }

        static RenderBackendTextureDesc Create3D(
            uint32 width,
            uint32 height,
            uint32 depth,
            RenderBackendTextureFormat format,
            RenderBackendTextureCreateFlags flags,
            uint32 mipLevels = 1,
            uint32 samples = 1,
            RenderBackendTextureClearValue clearValue = RenderBackendTextureClearValue::None,
            RenderBackendResourceState initalState = RenderBackendResourceState::Undefined)
        {
            return RenderBackendTextureDesc(width, height, depth, mipLevels, 1, samples, RenderBackendTextureType::Texture3D, format, flags, clearValue, initalState);
        }

        static RenderBackendTextureDesc CreateCube(
            uint32 sizeInPixels,
            RenderBackendTextureFormat format,
            RenderBackendTextureCreateFlags flags,
            uint32 mipLevels = 1,
            uint32 samples = 1,
            RenderBackendTextureClearValue clearValue = RenderBackendTextureClearValue::None,
            RenderBackendResourceState initalState = RenderBackendResourceState::Undefined)
        {
            return RenderBackendTextureDesc(sizeInPixels, sizeInPixels, 1, mipLevels, 6, samples, RenderBackendTextureType::TextureCube, format, flags, clearValue, initalState);
        }

        static RenderBackendTextureDesc CreateCubeArray(
            uint32 sizeInPixels,
            RenderBackendTextureFormat format,
            RenderBackendTextureCreateFlags flags,
            uint32 arraySize,
            RenderBackendTextureClearValue clearValue = RenderBackendTextureClearValue::None,
            uint32 mipLevels = 1,
            uint32 samples = 1,
            RenderBackendResourceState initalState = RenderBackendResourceState::Undefined)
        {
            return RenderBackendTextureDesc(sizeInPixels, sizeInPixels, 1, mipLevels, 6 * arraySize, samples, RenderBackendTextureType::TextureCube, format, flags, clearValue, initalState);
        }
    };

    struct RenderBackendTextureSRVDesc 
    {

        RenderBackendTextureSRVDesc()
            : texture()
            , baseMipLevel(0)
            , numMipLevels(RENDER_BACKEND_REMAINING_MIP_LEVELS)
            , baseArrayLayer(0)
            , numArrayLayers(RENDER_BACKEND_REMAINING_MIP_LEVELS) {}

        RenderBackendTextureSRVDesc(RenderBackendTextureHandle texture, uint32 baseMipLevel, uint32 numMipLevels, uint32 baseArrayLayer, uint32 numArrayLayers)
            : texture(texture)
            , baseMipLevel(baseMipLevel)
            , numMipLevels(numMipLevels)
            , baseArrayLayer(baseArrayLayer)
            , numArrayLayers(numArrayLayers) {}

        bool IsValid() const
        {
            return texture.IsValid();
        }

        RenderBackendTextureHandle texture;
        uint32 baseMipLevel;
        uint32 numMipLevels;
        uint32 baseArrayLayer;
        uint32 numArrayLayers;

    };

    struct RenderBackendTextureUAVDesc 
    {
        RenderBackendTextureHandle texture = RenderBackendTextureHandle::Null;
        uint32 mipLevel = 0;

        RenderBackendTextureUAVDesc() = default;
        RenderBackendTextureUAVDesc(RenderBackendTextureHandle texture, uint32 mipLevel)
            : texture(texture), mipLevel(mipLevel) {}

        static RenderBackendTextureUAVDesc Create(RenderBackendTextureHandle texture, uint32 mipLevel = 0)
        {
            return RenderBackendTextureUAVDesc(texture, mipLevel);
        }
    
        bool IsValid() const
        {
            return texture.IsValid();
        }
    };


    struct RenderBackendTextureUploadDataDesc 
    {
        uint64 totalSize;

        struct SubresourceData 
        {
            uint32 level;
            uint32 layer;
            const void* buffer;
            uint32 width;
            uint32 height;
            uint64 size;
            uint64 row_pitch_bytes;    
        };

        std::vector<SubresourceData> data;

        RenderBackendTextureUploadDataDesc()
            : totalSize(0)
            , data({})
        {

        }
        
        void AddSubresourceData(uint32 level, uint32 layer, const void* buffer, uint32 width, uint32 height, uint64 size, uint64 row_pitch_bytes)
        {
            SubresourceData subresourceData = {
               level,
               layer,
               buffer,
               width,
               height,
               size,
               row_pitch_bytes,
            };

            data.push_back(subresourceData);
            totalSize += size;
        }
    
    };

    struct RenderBackendSamplerDesc 
    {
        RenderBackendTextureFilter filter;

        RenderBackendTextureAddressMode addressModeU;
        RenderBackendTextureAddressMode addressModeV;
        RenderBackendTextureAddressMode addressModeW;
        
        float mipLodBias;
        float minLod;
        float maxLod;
        uint32 maxAnisotropy;
        RenderBackendCompareOp compareOp;

        RenderBackendSamplerDesc(
            RenderBackendTextureFilter filter,
            RenderBackendTextureAddressMode addressModeU,
            RenderBackendTextureAddressMode addressModeV,
            RenderBackendTextureAddressMode addressModeW,
            float mipLodBias,
            float minLod,
            float maxLod,
            uint32 maxAnisotropy,
            RenderBackendCompareOp compareOp)
            : filter(filter)
            , addressModeU(addressModeU)
            , addressModeV(addressModeV)
            , addressModeW(addressModeW)
            , mipLodBias(mipLodBias)
            , minLod(minLod)
            , maxLod(maxLod)
            , maxAnisotropy(maxAnisotropy)
            , compareOp(compareOp) {}

        static RenderBackendSamplerDesc CreateLinearWarp(float mipLodBias, float minLod, float maxLod, uint32 maxAnisotropy)
        {
            return RenderBackendSamplerDesc(
                RenderBackendTextureFilter::MinMagMipLinear,
                RenderBackendTextureAddressMode::Wrap,
                RenderBackendTextureAddressMode::Wrap,
                RenderBackendTextureAddressMode::Wrap,
                mipLodBias,
                minLod,
                maxLod,
                maxAnisotropy,
                RenderBackendCompareOp::Never);
        }

        static RenderBackendSamplerDesc CreateLinearClamp(float mipLodBias, float minLod, float maxLod, uint32 maxAnisotropy)
        {
            return RenderBackendSamplerDesc(
                RenderBackendTextureFilter::MinMagMipLinear,
                RenderBackendTextureAddressMode::Clamp,
                RenderBackendTextureAddressMode::Clamp,
                RenderBackendTextureAddressMode::Clamp,
                mipLodBias,
                minLod,
                maxLod,
                maxAnisotropy,
                RenderBackendCompareOp::Never);
        }

        static RenderBackendSamplerDesc CreateLinearBorder(float mipLodBias, float minLod, float maxLod, uint32 maxAnisotropy)
        {
            return RenderBackendSamplerDesc(
                RenderBackendTextureFilter::MinMagMipLinear,
                RenderBackendTextureAddressMode::Border,
                RenderBackendTextureAddressMode::Border,
                RenderBackendTextureAddressMode::Border,
                mipLodBias,
                minLod,
                maxLod,
                maxAnisotropy,
                RenderBackendCompareOp::Never);
        }

        static RenderBackendSamplerDesc CreatePointWarp(float mipLodBias, float minLod, float maxLod, uint32 maxAnisotropy)
        {
            return RenderBackendSamplerDesc(
                RenderBackendTextureFilter::MinMagMipPoint,
                RenderBackendTextureAddressMode::Wrap,
                RenderBackendTextureAddressMode::Wrap,
                RenderBackendTextureAddressMode::Wrap,
                mipLodBias,
                minLod,
                maxLod,
                maxAnisotropy,
                RenderBackendCompareOp::Never);
        }

        static RenderBackendSamplerDesc CreatePointClamp(float mipLodBias, float minLod, float maxLod, uint32 maxAnisotropy)
        {
            return RenderBackendSamplerDesc(
                RenderBackendTextureFilter::MinMagMipPoint,
                RenderBackendTextureAddressMode::Clamp,
                RenderBackendTextureAddressMode::Clamp,
                RenderBackendTextureAddressMode::Clamp,
                mipLodBias,
                minLod,
                maxLod,
                maxAnisotropy,
                RenderBackendCompareOp::Never);
        }

        static RenderBackendSamplerDesc CreatePointBorder(float mipLodBias, float minLod, float maxLod, uint32 maxAnisotropy)
        {
            return RenderBackendSamplerDesc(
                RenderBackendTextureFilter::MinMagMipPoint,
                RenderBackendTextureAddressMode::Border,
                RenderBackendTextureAddressMode::Border,
                RenderBackendTextureAddressMode::Border,
                mipLodBias,
                minLod,
                maxLod,
                maxAnisotropy,
                RenderBackendCompareOp::Never);
        }

        static RenderBackendSamplerDesc CreateComparisonLinearClamp(float mipLodBias, float minLod, float maxLod, uint32 maxAnisotropy, RenderBackendCompareOp compareOp)
        {
            return RenderBackendSamplerDesc(
                RenderBackendTextureFilter::ComparisonMinMagMipLinear,
                RenderBackendTextureAddressMode::Clamp,
                RenderBackendTextureAddressMode::Clamp,
                RenderBackendTextureAddressMode::Clamp,
                mipLodBias,
                minLod,
                maxLod,
                maxAnisotropy,
                compareOp);
        }
    };

    enum class RenderBackendShaderStage : uint8
    {
    
        Vertex = 0,
        Pixel = 1,
        Compute = 2,
        Task = 8,
        Mesh = 9,
        RayGen = 3,
        Miss = 6,
        AnyHit = 4,
        ClosestHit = 5,
        Intersection = 7,
        Count = 10,
    };

    enum RenderBackendShaderStageFlags
    {
        None = 0,
        Vertex = (1 << (int)RenderBackendShaderStage::Vertex),
        Pixel = (1 << (int)RenderBackendShaderStage::Pixel),
        Compute = (1 << (int)RenderBackendShaderStage::Compute),
        RayGen = (1 << (int)RenderBackendShaderStage::RayGen),
        AnyHit = (1 << (int)RenderBackendShaderStage::AnyHit),
        ClosestHit = (1 << (int)RenderBackendShaderStage::ClosestHit),
        Miss = (1 << (int)RenderBackendShaderStage::Miss),
        Intersection = (1 << (int)RenderBackendShaderStage::Intersection),
        Task = (1 << (int)RenderBackendShaderStage::Task),
        Mesh = (1 << (int)RenderBackendShaderStage::Mesh),
        All = 0x7FFFFFFF,

    };

    enum class RenderBackendIndexType
    {
        UINT32,
        UINT16,
        UINT8,
    };


    struct RenderBackendShaderDesc
    {
        std::string entryPoints[RenderBackendMaxNumShaderStages] = {};
        RenderBackendShaderBlob stages[RenderBackendMaxNumShaderStages] = {};
    };

    enum class RenderBackendQueryType
    {
        Occlusion,
        Timestamp,
    };

    struct RenderBackendTimingQueryHeapDesc
    {
        uint32 maxRegions;
        RenderBackendTimingQueryHeapDesc() = default;
        RenderBackendTimingQueryHeapDesc(uint32 maxRegions) : maxRegions(maxRegions) {}
    };

    struct RenderBackendOcclusionQueryHeapDesc
    {
        uint32 maxQueries;
        RenderBackendOcclusionQueryHeapDesc() = default;
        RenderBackendOcclusionQueryHeapDesc(uint32 maxQueries) : maxQueries(maxQueries) {}
    };

    struct RenderBackendGraphicsPipelineState 
    {
        RenderBackendRasterizationState 
    
    };

    struct RenderBackendViewport 
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
        RenderBackendViewport() = default;
        RenderBackendViewport(float width, float height)
            : x(0.0f), y(0.0f), width(width), height(height), minDepth(0.0f), maxDepth(1.0f) {}
        RenderBackendViewport(float x, float y, float width, float height)
            : x(x), y(y), width(width), height(height), minDepth(0.0f), maxDepth(1.0f) {}
        RenderBackendViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
            : x(x), y(y), width(width), height(height), minDepth(minDepth), maxDepth(maxDepth) {}

    
    };

    
    
    struct RenderBackendScissor
    {
        int32 left;
        int32 top;
        uint32 width;
        uint32 height;
        RenderBackendScissor() = default;
        RenderBackendScissor(uint32 width, uint32 height)
            : left(0), top(0), width(width), height(height) {}
        RenderBackendScissor(int32 left, int32 top, uint32 width, uint32 height)
            : left(left), top(top), width(width), height(height) {}
    };

    struct RenderBackendStatistics
    {
        uint64 nonIndexedDraws;
        uint64 indexedDraws;
        uint64 nonIndexedIndirectDraws;
        uint64 indexedIndirectDraws;
        uint64 computeDispatches;
        uint64 computeIndirectDispatches;
        uint64 traceRayDispatches;
        uint64 vertices;
        uint64 pipelines;
        uint64 transitions;
        uint64 renderPasses;
        void Add(const RenderBackendStatistics& other)
        {
            nonIndexedDraws += other.nonIndexedDraws;
            indexedDraws += other.indexedDraws;
            nonIndexedIndirectDraws += other.nonIndexedIndirectDraws;
            indexedIndirectDraws += other.indexedIndirectDraws;
            computeDispatches += other.computeDispatches;
            computeIndirectDispatches += other.computeIndirectDispatches;
            traceRayDispatches += other.traceRayDispatches;
            vertices += other.vertices;
            pipelines += other.pipelines;
            transitions += other.transitions;
            renderPasses += other.renderPasses;
        }
    };

    struct RenderBackendBarrier 
    {
        enum class Type 
        {
            Memory,
            Texture,
            Buffer,
        };
        Type type;
        RenderBackendResourceState stateBefore;
        RenderBackendResourceState stateAfter;

        union 
        {
            struct 
            {
                RenderBackendTextureHandle texture;
                RenderBackendTextureSubresourceRange textureRange;
            };
        
        };
    };

    struct RenderBackendRenderPassInfo 
    {
        struct ColorRenderTarget 
        {
            RenderBackendTextureHandle texture;
            uint32 mipLevel;
            uint32 arrayLayer;
            RenderBackendRenderTargetLoadOp depthLoadOp;
            RenderBackendRenderTargetStoreOp storeOp;

        };

        struct DepthStencilRenderTarget 
        {
            RenderBackendTextureHandle texture;
            uint32 mipLevel;
            uint32 arrayLayer;
            RenderBackendRenderTargetLoadOp depthLoadOp;
            RenderBackendRenderTargetStoreOp depthStoreOp;
            RenderBackendRenderTargetLoadOp stencilLoadOp;
            RenderBackendRenderTargetStoreOp stencilStoreOp;
        };

        ColorRenderTarget colorRenderTarget[RenderBackendMaxNumSimultaneousColorRenderTargets];
        DepthStencilRenderTarget depthStencilRenderTarget;
        
    
    };


    enum class RenderBackendRenderTargetLoadOp
    {
        DontCare,
        Load,
        Clear,
        Count
    };

    enum class RenderBackendRenderTargetStoreOp 
    {
        DontCare,
        Store,
        Count

    };

    enum class RenderBackendClearDepthStencilOp
    {
        ClearDepth,
        ClearStencil,
        ClearDepthStencil
    };

}