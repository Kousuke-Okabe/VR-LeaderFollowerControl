#include "kenkyuArmMeshes.hpp"

kenkyulocal::kenkyuArmMeshSet::kenkyuArmMeshSet(std::unordered_map < std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>>* shaders,const glm::mat4& baseTransform) {
	this->SetTransform(baseTransform);

	//ここからセットアップしていく
	this->meshes["base"].reset(new uuu::game::mesh((*shaders)["red"], kenkyu::assets("arm.dae"), "base-mesh", this->transform));
	this->meshes["arm0"].reset(new uuu::game::mesh((*shaders)["norm"], kenkyu::assets("arm.dae"), "link0-mesh", this->transform));
	this->meshes["arm1"].reset(new uuu::game::mesh((*shaders)["red"], kenkyu::assets("arm.dae"), "link1-mesh", this->transform));
	this->meshes["arm2"].reset(new uuu::game::mesh((*shaders)["norm"], kenkyu::assets("arm.dae"), "link2-mesh", this->transform));
}

void kenkyulocal::kenkyuArmMeshSet::Draw(const std::string& attribName) {
	for (const auto& i : this->meshes)
		i.second.get()->Draw(attribName);
}