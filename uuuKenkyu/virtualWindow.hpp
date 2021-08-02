#pragma once

#include <uuu.hpp>
#include <uuuEasyTools.hpp>

namespace _uuu {

	class virtualWindow {
	public:
		uuu::frameBufferOperator fbo;
		uuu::textureOperator col, dep;
		uuu::easy::neo3Dmesh* plane;

	public:
		virtualWindow(uint32_t w, uint32_t h, uuu::easy::neo3Dmesh* plane);

		
	};

};