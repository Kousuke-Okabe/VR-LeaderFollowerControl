#include "uuuGame.hpp"
using namespace std;

uuu::game::drawable::~drawable() {

}

uuu::game::mesh::mesh() {
	this->transform = glm::identity<glm::mat4>();

	this->skipDraw = false;
}
uuu::game::mesh::mesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh):inner(shader, path, mesh) {
	this->transform = glm::identity<glm::mat4>();

	this->skipDraw = false;
}
uuu::game::mesh::mesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, glm::mat4 def,bool skipDrawDef) : mesh(shader, path, mesh) {
	this->SetTransform(def);

	this->skipDraw = skipDrawDef;
}

void uuu::game::drawable::SetTransform(const glm::mat4& tr) {
	this->transform = tr;
}

glm::mat4& uuu::game::drawable::GetTransform() {
	return this->transform;
}

uuu::easy::neo3Dmesh& uuu::game::mesh::GetMesh() {
	return this->inner;
}

void uuu::game::mesh::Draw(shaderSettingCall&setting,const std::string& attribName) {
	setting(this->inner.Shader());
	
	this->Draw(attribName);
	return;
}
void uuu::game::mesh::Draw(const std::string& attribName) {

	if (skipDraw)return;

	this->inner.Shader().lock()->SetUniformValue(attribName, this->transform);
	this->inner.DrawElements();

	return;
}

uuu::game::mesh::~mesh() {

}

uuu::game::mesh::mesh(mesh& arg) {
	this->operator=(std::move(arg));
}
uuu::game::mesh& uuu::game::mesh::operator=(uuu::game::mesh&& arg) {
	inner = std::move(arg.inner);
	transform = arg.transform;

	return *this;
}

uuu::game::texturedMesh::texturedMesh():super() {

}
uuu::game::texturedMesh::texturedMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, textureOperator* tex, const std::string& uniformName):super(shader,path,mesh) {
	this->tex = tex;
	this->uniformName = uniformName;
}
uuu::game::texturedMesh::texturedMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, textureOperator* tex, glm::mat4 def, const std::string& uniformName, bool skipDrawDef) : super(shader, path, mesh,def,skipDrawDef) {
	this->tex = tex;
	this->uniformName = uniformName;
}

uuu::game::texturedMesh::~texturedMesh() {

}
uuu::game::texturedMesh::texturedMesh(uuu::game::texturedMesh& arg) :super(arg) {
	this->tex = arg.tex;
	this->uniformName = arg.uniformName;
}
uuu::game::texturedMesh& uuu::game::texturedMesh::operator=(uuu::game::texturedMesh&& arg) {
	this->tex = arg.tex;
	this->uniformName = arg.uniformName;

	super::operator=(std::move(arg));

	return *this;
}

void uuu::game::texturedMesh::Draw(shaderSettingCall& setting, const std::string& attribName) {

	this->tex->Bind();
	this->GetMesh().Shader().lock()->SetUniformTexUnit(this->uniformName, *this->tex);

	super::Draw(setting, attribName);
}
void uuu::game::texturedMesh::Draw(const std::string& attribName) {

	this->tex->Bind();
	this->GetMesh().Shader().lock()->SetUniformTexUnit(this->uniformName, *this->tex);

	super::Draw(attribName);
}

uuu::game::texturedMesh::texturedMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, glm::mat4 def, const std::string& uniformName, bool skipDrawDef) : super(shader, path, mesh, def, skipDrawDef) {
	this->uniformName = uniformName;
}




void uuu::game::virtualWindow::SetupBuffers(uint32_t w, uint32_t h) {

	if (!fbo)fbo.reset(new uuu::frameBufferOperator());
	if (!col)col.reset(new uuu::textureOperator());
	if (!dep)dep.reset(new uuu::textureOperator());


	col->CreateManual(w, h, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	dep->CreateManual(w, h, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
	fbo->AttachTextureOperator(fbo->color0, *col);
	fbo->AttachTextureOperator(fbo->depth, *dep);

	fbo->Unbind();
}

uuu::game::virtualWindow::virtualWindow() :super() {

}
uuu::game::virtualWindow::virtualWindow(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>shader, const std::string& path, const std::string mesh, uint32_t w, uint32_t h,drawContents contents, glm::mat4 def, const std::string& uniform, bool skipDrawDef) : super(shader, path, mesh, def, uniform, skipDrawDef),contents(contents) {
	this->SetupBuffers(w, h);

	this->tex = this->col.get();
}
uuu::game::virtualWindow::~virtualWindow() {
	int debug = 0;
}

//ƒRƒs[
uuu::game::virtualWindow::virtualWindow(virtualWindow& arg) :super(arg) {
	this->fbo = std::move(arg.fbo);
	this->col = std::move(arg.col);
	this->dep = std::move(arg.dep);
}
uuu::game::virtualWindow& uuu::game::virtualWindow::operator=(virtualWindow&& arg) {

	super::operator=(std::move(arg));

	this->fbo = std::move(arg.fbo);
	this->col = std::move(arg.col);
	this->dep = std::move(arg.dep);

	return *this;
}

void uuu::game::virtualWindow::Draw(shaderSettingCall& setting, const std::string& attribName) {

	super::Draw(setting, attribName);
}
void uuu::game::virtualWindow::Draw(const std::string& attribName) {

	super::Draw(attribName);
}

uuu::frameBufferOperator* uuu::game::virtualWindow::GetFbo() {
	return this->fbo.get();
}

void uuu::game::virtualWindow::DrawEvent() {
	this->GetFbo()->Bind();
	this->contents();
	this->GetFbo()->Unbind();
}