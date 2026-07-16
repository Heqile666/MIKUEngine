#pragma once
#include "Direct3D12RenderBackendCommon.h"

namespace MIKU 
{
	RenderBackend* RenderBackendCreateDirect3D12(const RenderBackendDesc* desc);

	void RenderBackendDestoryDirect3D12(RenderBackend* backend);

}