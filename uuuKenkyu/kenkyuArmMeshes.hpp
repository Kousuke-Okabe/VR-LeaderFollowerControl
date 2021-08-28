#pragma once

#include "uuu.hpp"
#include "uuuGame.hpp"
#include "kenkyuFrame.hpp"

namespace kenkyulocal {
	class kenkyuArmMeshSet :public uuu::game::drawable {
	protected:
		//���b�V���Z�b�g�Ȃ̂ł����ς����b�V���������Ă���
		std::unordered_map<std::string, std::shared_ptr<uuu::game::mesh>> meshes;

		void SetTransformForAngles(const glm::vec3& angles);//�p�x����ϊ��s���ݒ肷��
	public:
		kenkyuArmMeshSet(std::unordered_map<std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>>* shaders,const glm::mat4& baseTransform = glm::identity<glm::mat4>());
		void Draw(const std::string& attribName = "modelTransform");
	};

	//�I�t�Z�b�g�s������������b�V��
	class offsetMesh :public uuu::game::mesh {
		using super = uuu::game::mesh;
	protected:
		glm::mat4 offset;
	public:
		offsetMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, glm::mat4 offset = glm::identity<glm::mat4>());
		offsetMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, glm::mat4 def, glm::mat4 offset, bool skipDrawDef = false);
	
		virtual void SetTransform(const glm::mat4& tr);
		virtual glm::mat4& GetTransform();
	};
};