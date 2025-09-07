#include "mikupch.h"
#include <Miku/RenderBackend/RenderBackendInterface.h>



namespace MIKU 
{
	RenderBackend* GRenderBackend = nullptr;

	const RenderBackendTextureClearValue RenderBackendTextureClearValue::None = RenderBackendTextureClearValue();
	const RenderBackendTextureClearValue RenderBackendTextureClearValue::Black = RenderBackendTextureClearValue(0.0f, 0.0f, 0.0f, 1.0f);
	const RenderBackendTextureClearValue RenderBackendTextureClearValue::White = RenderBackendTextureClearValue(1.0f, 1.0f, 1.0f, 1.0f);
	const RenderBackendTextureClearValue RenderBackendTextureClearValue::DepthOne = RenderBackendTextureClearValue(1.0f, 0);
	const RenderBackendTextureClearValue RenderBackendTextureClearValue::DepthZero = RenderBackendTextureClearValue(0.0f, 0);
}