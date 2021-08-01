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