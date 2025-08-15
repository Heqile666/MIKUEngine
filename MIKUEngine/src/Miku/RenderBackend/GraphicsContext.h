#pragma once

namespace MIKU {
	
	//An interface for rendering context
	class GraphicsContext {
	public:
		virtual void Init() = 0;
		virtual void SwapBuffer() = 0;
	private:



	};


}