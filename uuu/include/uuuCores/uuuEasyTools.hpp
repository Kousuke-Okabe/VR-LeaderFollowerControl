#ifndef UUU_UUU_EASY_TOOLS_HPP
#define UUU_UUU_EASY_TOOLS_HPP

#include <uuu.hpp>

namespace uuu {

	template<typename T>using wptr=uuu::commonTools::float_ptr<T>;//弱参照の短縮用エイリアス

	namespace easy {
		namespace usings {
			using gvec2 = glm::vec2;
			using gvec3 = glm::vec3;
			using gvec4 = glm::vec4;
			using gmat4 = glm::mat4;

			template<typename T>using sptr = std::shared_ptr<T>;
			template<typename T>using uptr = std::unique_ptr<T>;

			template<typename T>using fptr = uuu::commonTools::float_ptr<T>;

			template<typename T>using opt = boost::optional<T>;
		};

		//uuu汎用機能すべての初期化をやってくれるやつ
		__int8 Init(uuu::app::windowProp prop,int loadAttribs = uuu::neoVertexOperators::positions | uuu::neoVertexOperators::norms | uuu::neoVertexOperators::indices);

		//neo3Dmeshの基本設定テンプレートを初期化する
		__int8 InitNeo3Dmesh();
		//パスとシェーダーから普通に描画可能なモデルデータ(vao)を作る カスタム性は微妙　neoVertex対応!
		class neo3Dmesh {
		//他のと共通になる可能性のあるシェーダー　モデル　vboはsharedで作る
			std::unique_ptr<uuu::neoVertexOperators::vaoOperatorNeo> vao;
			std::shared_ptr<uuu::neoVertexOperators::vboMap> vboMap;
			std::shared_ptr<uuu::modelLoaderSerializer> mod;
			//シェーダー
			std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shad;

		public:
			//基本設定テンプレート　ここを変えることで生成の手順などの設定ができる
			struct propertyTemp {
				int fileLoadFlag;//モデルファイルを読み込む際のAssimp用フラグ
				int loadAttribute;//vbo化する頂点属性のフラグvboAttachType使ってどうぞ
				std::unique_ptr<std::unordered_map<int, std::string>>autoNamingTemp;//自動名付けのテンプレート
				std::unique_ptr<std::unordered_map<int, uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage>>autoUsageTemp;//自動用法(とフォーマット)設定のテンプレート インデックスのフォーマットを変えるときはショートカットも一緒に変更してください
				
			};
			static propertyTemp temp;//このテンプレートを読み出して使う
			uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage indicesUsage;//インデックスの用法のショートカット 弱参照　モデルごと

			//シェーダーを設定
			__int8 SetShader(const std::string& source);
			__int8 SetShader(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> add);
			//引数に設定したメッシュとシェーダーをシェアする
			__int8 ShareShader(const uuu::easy::neo3Dmesh& gen);

			//パスからVaoを生成 このときにindicesのショートカットも固定
			__int8 LoadFile(const std::string& path, const std::string& mesh);
			//vboとモデルシリアライザーを引数に設定したメッシュとシェアする vaoは独自
			__int8 ShareFile(const uuu::easy::neo3Dmesh& gen);

			//エレメントを描画する
			__int8 DrawElements();

			//シェーダーにアクセスする
			std::weak_ptr<uuu::shaderProgramObjectVertexFragment> Shader();
			//モデルしリアライザーにアクセスする
			std::weak_ptr<uuu::modelLoaderSerializer>Serializer();

			neo3Dmesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> add, const std::string& path, const std::string& mesh);
			neo3Dmesh();


			//非推奨:カプセル化破壊　neo3Dmeshのプライベートに侵入するアクセサ
			struct ___accesserNeo3Dmesh{
				std::shared_ptr<uuu::modelLoaderSerializer>AccessModel(uuu::easy::neo3Dmesh& gen);
			};
		};


		//

	};
};

#endif