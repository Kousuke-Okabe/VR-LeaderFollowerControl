#pragma once

#include <uuu.hpp>

namespace _uuu {

	class virtualWindow {
	protected:
		uuu::frameBufferOperator fbo;
		uuu::textureOperator col, dep;

	public:
		virtualWindow(uint32_t w, uint32_t h) {
			//col.CreateManual(w, h, uuu::textureFormat::rgba16);
			dep.CreateManual(w, h, uuu::textureFormat::depth16);
			fbo.AttachTextureOperator(fbo.color0, col);
			fbo.AttachTextureOperator(fbo.depth, dep);

			fbo.Unbind();
		}
	};

};