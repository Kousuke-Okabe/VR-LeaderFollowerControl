#pragma once

#include "uuuGame.hpp"
#include "kenkyuFrame.hpp"

namespace kenkyulocal {
	class kenkyuArmMeshSet :public uuu::game::drawable {
	protected:
		//���b�V���Z�b�g�Ȃ̂ł����ς����b�V���������Ă���
		std::unordered_map<std::string, std::shared_ptr<uuu::game::mesh>> meshes;

	public:
		kenkyuArmMeshSet(std::unordered_map<std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>>* shaders,const glm::mat4& baseTransform = glm::identity<glm::mat4>());
		void Draw(const std::string& attribName = "modelTransform");
	};
};