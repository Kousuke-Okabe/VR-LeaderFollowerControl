#pragma once

#include "uuu.hpp"
#include "uuuGame.hpp"
#include "kenkyuFrame.hpp"

namespace kenkyulocal {
	class kenkyuArmMeshSet :public uuu::game::drawable {
	protected:
		//メッシュセットなのでいっぱいメッシュを持っている
		std::unordered_map<std::string, std::shared_ptr<uuu::game::mesh>> meshes;

		//void SetTransformForAngles(const glm::vec3& angles);//角度から変換行列を設定する
		void SetTransformForAngles(const Eigen::Matrix<double, 6, 1>& angles);
	public:
		kenkyuArmMeshSet(std::unordered_map<std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>>* shaders,const glm::mat4& baseTransform = glm::identity<glm::mat4>());
		void Draw(const std::string& attribName = "modelTransform");
		void Draw(const Eigen::Matrix<double, 6, 1>& angles,const std::string& attribName = "modelTransform");
	};
	class offsetMesh :public uuu::game::mesh {
		using super = uuu::game::mesh;
	protected:
		glm::mat4 offset;
	public:
		offsetMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, glm::mat4 offset = glm::identity<glm::mat4>());
		offsetMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, glm::mat4 def, glm::mat4 offset, bool skipDrawDef = false);
	
		virtual void SetTransform(const glm::mat4& tr);
	};
};