#pragma once

#ifndef UUU_UUU_GAME_TOOLS_HPP
#define UUU_UUU_GAME_TOOLS_HPP

#include <uuu.hpp>
#include <functional>
#include <uuuEasyTools.hpp>

namespace uuu {
	//eazyに依存しつつある

	namespace game {

		//描画可能なクラスの基底
		class drawable {
		protected:
			glm::mat4 transform;
		public:
			bool skipDraw;//描画をスキップする

			virtual ~drawable();
			virtual void Draw(const std::string& attribName = "modelTransform") = 0;
		
			virtual void SetTransform(const glm::mat4& tr);
			glm::mat4& GetTransform();
		};

		class mesh :public drawable {

		protected:
			uuu::easy::neo3Dmesh inner;

		public:

			using shaderSettingCall = std::function<void(std::weak_ptr<uuu::shaderProgramObjectVertexFragment>)>;

			mesh();
			mesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh);
			mesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, glm::mat4 def, bool skipDrawDef = false);

			uuu::easy::neo3Dmesh& GetMesh();

			virtual void Draw(shaderSettingCall& setting, const std::string& attribName = "modelTransform");
			virtual void Draw(const std::string& attribName = "modelTransform");

			virtual ~mesh();

			//コピー
			mesh(mesh& arg);
			virtual mesh& operator=(mesh&& arg);
		};

		class texturedMesh :public mesh {
		protected:
			using super = mesh;

			textureOperator* tex;
			std::string uniformName;

			//virtualWindowが使用するコンストラクタ
			texturedMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shader, const std::string& path, const std::string mesh, glm::mat4 def, const std::string& uniformName, bool skipDrawDef);
		public:
			texturedMesh();
			texturedMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, textureOperator* tex, const std::string& uniformName = "tex0");
			texturedMesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, textureOperator* tex, glm::mat4 def, const std::string& uniformName = "tex0", bool skipDrawDef = false);
		
			virtual void Draw(shaderSettingCall& setting, const std::string& attribName = "modelTransform");
			virtual void Draw(const std::string& attribName = "modelTransform");

			virtual ~texturedMesh();

			//コピー
			texturedMesh(uuu::game::texturedMesh& arg);
			virtual texturedMesh& operator=(texturedMesh&& arg);
		};

		class virtualWindow :public uuu::game::texturedMesh {
			using super = uuu::game::texturedMesh;
		protected:
			void SetupBuffers(uint32_t w, uint32_t h);
			//public:
			std::unique_ptr<uuu::frameBufferOperator> fbo;
			std::unique_ptr<uuu::textureOperator> col, dep;

			using drawContents = std::function<void()>;
			drawContents contents;//描画内容
			
		public:
			//virtualWindow(uint32_t w, uint32_t h, uuu::easy::neo3Dmesh* plane);

			virtualWindow();
			virtualWindow(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, uint32_t w, uint32_t h,drawContents contents, glm::mat4 def = glm::identity<glm::mat4>(), const std::string& uniform = "tex0", bool skipDrawDef = false);
			//virtualWindow(std::shared_ptr<uuu::shaderProgramObjectVertexFragment>, const std::string& path, const std::string mesh, uint32_t w, uint32_t h,glm::mat4 def, const std::string& uniform = "tex", bool skipDrawDef = false);

			virtual void Draw(shaderSettingCall& setting, const std::string& attribName = "modelTransform");
			virtual void Draw(const std::string& attribName = "modelTransform");

			virtual ~virtualWindow();

			//コピー
			virtualWindow(virtualWindow& arg);
			virtual virtualWindow& operator=(virtualWindow&& arg);

			uuu::frameBufferOperator* GetFbo();

			//描画時に呼び出されるイベント
			void DrawEvent();
		};
	};
};

#endif