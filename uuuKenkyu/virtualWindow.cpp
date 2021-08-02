#include "virtualWindow.hpp"

_uuu::virtualWindow::virtualWindow(uint32_t w, uint32_t h, uuu::easy::neo3Dmesh* plane) {
	col.CreateManual(w, h, uuu::textureFormat::rgba16);
	dep.CreateManual(w, h, uuu::textureFormat::depth16);
	fbo.AttachTextureOperator(fbo.color0, col);
	fbo.AttachTextureOperator(fbo.depth, dep);

	fbo.Unbind();

	this->plane = plane;
}