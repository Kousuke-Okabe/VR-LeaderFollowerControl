#pragma once

#include <uuu.hpp>
#include <uuuEasyTools.hpp>
#include <uuuGame.hpp>

namespace _uuu {

	class virtualWindow:public uuu::game::texturedMesh {
		using super = uuu::game::texturedMesh;
	protected:
		void SetupBuffers(uint32_t w, uint32_t h);
	//public:
		std::unique_ptr<uuu::frameBufferOperator> fbo;
		std::unique_ptr<uuu::textureOperator> col, dep;

	public:
		//virtualWindow(uint32_t w, uint32_t h, uuu::easy::neo3Dmesh* plane);

		virtualWindow();
		virtualWindow(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, uint32_t w, uint32_t h, glm::mat4 def=glm::identity<glm::mat4>(),const std::string&uniform="tex0", bool skipDrawDef = false);
		//virtualWindow(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, uint32_t w, uint32_t h,glm::mat4 def, const std::string& uniform = "tex", bool skipDrawDef = false);

		virtual void Draw(shaderSettingCall& setting, const std::string& attribName = "modelTransform");
		virtual void Draw(const std::string& attribName = "modelTransform");

		virtual ~virtualWindow();

		//ÉRÉsÅ[
		virtualWindow(virtualWindow& arg);
		virtual virtualWindow& operator=(virtualWindow&& arg);

		uuu::frameBufferOperator* GetFbo();
	};

};