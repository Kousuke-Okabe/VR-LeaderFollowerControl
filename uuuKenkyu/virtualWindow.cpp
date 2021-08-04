#include "virtualWindow.hpp"

void _uuu::virtualWindow::SetupBuffers(uint32_t w, uint32_t h) {

	if (!fbo)fbo.reset(new uuu::frameBufferOperator());
	if (!col)col.reset(new uuu::textureOperator());
	if (!dep)dep.reset(new uuu::textureOperator());


	col->CreateManual(w, h, uuu::textureFormat::rgba16);
	dep->CreateManual(w, h, uuu::textureFormat::depth16);
	fbo->AttachTextureOperator(fbo->color0, *col);
	fbo->AttachTextureOperator(fbo->depth, *dep);

	fbo->Unbind();
}

_uuu::virtualWindow::virtualWindow():super() {

}
_uuu::virtualWindow::virtualWindow(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>shader , const std::string& path, const std::string mesh, uint32_t w, uint32_t h,glm::mat4 def, const std::string& uniform, bool skipDrawDef):super(shader,path,mesh,def,uniform,skipDrawDef) {
	this->SetupBuffers(w, h);

	this->tex = this->col.get();
}
_uuu::virtualWindow::~virtualWindow() {

}

//ƒRƒs[
_uuu::virtualWindow::virtualWindow(virtualWindow& arg):super(arg) {
	this->fbo = std::move(arg.fbo);
	this->col = std::move(arg.col);
	this->dep = std::move(arg.dep);
}
_uuu::virtualWindow& _uuu::virtualWindow::operator=(virtualWindow&& arg) {

	super::operator=(std::move(arg));

	this->fbo = std::move(arg.fbo);
	this->col = std::move(arg.col);
	this->dep = std::move(arg.dep);

	return *this;
}

void _uuu::virtualWindow::Draw(shaderSettingCall& setting, const std::string& attribName) {

	super::Draw(setting, attribName);
}
void _uuu::virtualWindow::Draw(const std::string& attribName) {

	super::Draw(attribName);
}