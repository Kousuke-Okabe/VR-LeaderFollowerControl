#include "kenkyuArmMeshes.hpp"

kenkyulocal::kenkyuArmMeshSet::kenkyuArmMeshSet(std::unordered_map < std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>>* shaders,const glm::mat4& baseTransform) {
	this->SetTransform(baseTransform);

	//ここからセットアップしていく
	this->meshes["base"].reset(new uuu::game::mesh((*shaders)["red"], kenkyu::assets("arm.dae"), "base-mesh", this->transform));
	this->meshes["arm0"].reset(new uuu::game::mesh((*shaders)["norm"], kenkyu::assets("arm.dae"), "link0-mesh", this->transform));
	this->meshes["arm1"].reset(new uuu::game::mesh((*shaders)["red"], kenkyu::assets("arm.dae"), "link1-mesh", this->transform));
	this->meshes["monkey"].reset(new uuu::game::mesh((*shaders)["rainbow"], kenkyu::assets("arm.dae"), "monkey-mesh", this->transform));
	//this->meshes["arm2"].reset(new uuu::game::mesh((*shaders)["norm"],("C:/local/uuu21-master/assets/arm.dae"), "link2-mesh", this->transform*glm::translate(glm::vec3(0, +0.8, 0))*glm::rotate((float)M_PI / 2, glm::vec3(0, 0, 1))*glm::translate(glm::vec3(0,-0.8,0))));
}

void kenkyulocal::kenkyuArmMeshSet::Draw(const std::string& attribName) {
	
	auto quat = kenkyu::GetMoterAngles();

	this->Draw(quat, attribName);
}
void kenkyulocal::kenkyuArmMeshSet::Draw(const Eigen::Matrix<double, 6, 1>& angles, const std::string& attribName) {

	this->SetTransformForAngles(angles);

	for (const auto& i : this->meshes)
		i.second.get()->Draw(attribName);
}

void kenkyulocal::kenkyuArmMeshSet::SetTransformForAngles(const Eigen::Matrix<double,6,1>& angles) {
	//それぞれのボーン情報　base-b0-link0-b1-link1-b2-link2
	const glm::mat4 b0 = this->transform;
	const glm::mat4 b1 = glm::translate(glm::vec3(0, -0.28, 0));
	const glm::mat4 b2 = glm::translate(glm::vec3(0, -0.35, 0));

	//ローカル変形
	const glm::mat4 l0 = glm::rotate((float)angles[0], glm::vec3(0, 1, 0));
	const glm::mat4 l1 = glm::rotate((float)angles[1], glm::vec3(0, 0, 1));
	const glm::mat4 l2 = glm::rotate((float)angles[2], glm::vec3(0, 0, -1));
	const glm::mat4 l3 = glm::rotate((float)angles[3], glm::vec3(0, -1, 0));
	const glm::mat4 l4 = glm::rotate((float)angles[4], glm::vec3(1, 0, 0));
	const glm::mat4 l5 = glm::rotate((float)angles[5], glm::vec3(0, 1, 0));

	//グローバル変形
	glm::mat4 g0 = b0 * l0 * l1;
	glm::mat4 g1 = g0 * b1 * l2;
	glm::mat4 g2 = g1 * b2 * l3 * l4 * l5;

	this->meshes["arm0"].get()->SetTransform(g0);
	this->meshes["arm1"].get()->SetTransform(g1);
	this->meshes["monkey"].get()->SetTransform(g2);
}


kenkyulocal::offsetMesh::offsetMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, glm::mat4 offset):uuu::game::mesh(shader,path,mesh) {
	this->offset = offset;

	this->SetTransform(glm::identity<glm::mat4>());
}
kenkyulocal::offsetMesh::offsetMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, glm::mat4 def, glm::mat4 offset, bool skipDrawDef) :uuu::game::mesh(shader, path, mesh, def, skipDrawDef) {
	this->offset = offset;

	this->SetTransform(def);
}
void kenkyulocal::offsetMesh::SetTransform(const glm::mat4& tr) {
	super::SetTransform(offset * tr);
}