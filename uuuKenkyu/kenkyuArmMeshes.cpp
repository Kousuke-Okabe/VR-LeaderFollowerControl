#include "kenkyuArmMeshes.hpp"

kenkyulocal::kenkyuArmMeshSet::kenkyuArmMeshSet(std::unordered_map < std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>>* shaders,const glm::mat4& baseTransform) {
	this->SetTransform(baseTransform);

	//ここからセットアップしていく
	this->meshes["base"].reset(new uuu::game::mesh((*shaders)["red"], kenkyu::assets("arm.dae"), "base-mesh", this->transform));
	this->meshes["arm0"].reset(new uuu::game::mesh((*shaders)["norm"], kenkyu::assets("arm.dae"), "link0-mesh", this->transform));
	this->meshes["arm1"].reset(new uuu::game::mesh((*shaders)["red"], kenkyu::assets("arm.dae"), "link1-mesh", this->transform));
	this->meshes["arm2"].reset(new uuu::game::mesh((*shaders)["norm"], kenkyu::assets("arm.dae"), "link2-mesh", this->transform*glm::translate(glm::vec3(0, +0.8, 0))*glm::rotate((float)M_PI / 2, glm::vec3(0, 0, 1))*glm::translate(glm::vec3(0,-0.8,0))));
}

void kenkyulocal::kenkyuArmMeshSet::Draw(const std::string& attribName) {
	
	auto quat = kenkyu::GetMoterAngles();
	this->SetTransformForAngles(glm::vec3(quat[0], quat[1], quat[2]));
	
	for (const auto& i : this->meshes)
		i.second.get()->Draw(attribName);
}

void kenkyulocal::kenkyuArmMeshSet::SetTransformForAngles(const glm::vec3& angles) {
	//それぞれのボーン情報　base-b0-link0-b1-link1-b2-link2
	const glm::mat4 b0 = this->transform;
	const glm::mat4 b1 = glm::translate(glm::vec3(0, -0.2, 0));
	const glm::mat4 b2 = glm::translate(glm::vec3(0, -0.6, 0));

	//ローカル変形
	const glm::mat4 l0 = glm::rotate(angles[0], glm::vec3(0, 1, 0));
	const glm::mat4 l1 = glm::rotate(angles[1], glm::vec3(0, 0, 1));
	const glm::mat4 l2 = glm::rotate(angles[2], glm::vec3(0, 0, 1));

	//グローバル変形
	glm::mat4 g0 = b0 * l0;
	glm::mat4 g1 = g0 * b1 * l1;
	glm::mat4 g2 = g1 * b2 * l2;

	this->meshes["arm0"].get()->SetTransform(g0);
	this->meshes["arm1"].get()->SetTransform(g1);
	this->meshes["arm2"].get()->SetTransform(g2);
}
