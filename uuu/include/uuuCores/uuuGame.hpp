#pragma once

#ifndef UUU_UUU_GAME_TOOLS_HPP
#define UUU_UUU_GAME_TOOLS_HPP

#include <uuu.hpp>
#include <functional>
#include <uuuEasyTools.hpp>

namespace uuu {
	//eazyに依存しつつある

	namespace game {
		class mesh {

		protected:
			uuu::easy::neo3Dmesh inner;
			glm::mat4 transform;

			
		public:
			bool skipDraw;//描画をスキップする

			using shaderSettingCall = std::function<void(std::weak_ptr<uuu::shaderProgramObjectVertexFragment>)>;

			mesh();
			mesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh);
			mesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh,glm::mat4 def,bool skipDrawDef=false);

			void SetTransform(const glm::mat4& tr);

			glm::mat4& GetTransform();
			uuu::easy::neo3Dmesh& GetMesh();

			void Draw(shaderSettingCall&setting,const std::string& attribName="modelTransform");
			void Draw(const std::string& attribName = "modelTransform");
		};


	};
};

#endif