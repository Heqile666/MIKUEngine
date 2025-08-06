#pragma once
namespace MIKU {

	class RendererAPI {
	public:
		enum class API {
			None = 0, OpenGL = 1, DX12 = 2
		};
	public:
		virtual ~RendererAPI() = default;
		

	};



}