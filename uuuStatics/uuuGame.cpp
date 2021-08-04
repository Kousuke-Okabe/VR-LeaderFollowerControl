#include "uuuGame.hpp"
using namespace std;

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

void uuu::game::mesh::SetTransform(const glm::mat4& tr) {
	this->transform = tr;
}

glm::mat4& uuu::game::mesh::GetTransform() {
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