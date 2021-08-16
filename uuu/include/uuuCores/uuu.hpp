#ifndef UUU_UUU_HPP
#define UUU_UUU_HPP

#define USE_EIGEN

#define _USE_MATH_DEFINES

#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <GL/glu.h>

#include <al.h>
#include <alext.h>
#include <alc.h>


#include <stdio.h>

#include <fstream>
#include <iostream>
#include <thread>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <chrono>
#include <stack>
#include <random>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#include <boost/bimap.hpp>
#include <boost/optional.hpp>
#include <boost/range/adaptor/indexed.hpp>

#ifdef USE_EIGEN
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>
#endif

//#include <btBulletCollisionCommon.h>
//#include <btBulletDynamicsCommon.h>
//#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
//#include <BulletCollision/Gimpact/btGImpactShape.h>

#define GLM_FORCE_INLNIE
#define GLM_PRECISION_MEDIUMP_INT
#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_GTX_simd_vec4
#define GLM_GTX_simd_mat4
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_GTX_norm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
//#include <glm/ext.hpp>

#include <png.h>

//daeのアニメーションで球面変形補完するか falseだと線形補間
#define ENABLE_SPHERICAL_LINEAR_INTERPOLATION true
//読み込むテクスチャ座標の最大
#define TEXCOORD_CHANNEL_MAX 8

using boneInfo = std::pair<size_t*, std::pair<std::vector<glm::vec4>*, std::vector<glm::vec4>*>>;

//不正な関数呼び出し(呼び出す順番やタイミングが違う)を表す例外
class bad_calling :public std::logic_error {

public:
	bad_calling(std::string arg) :logic_error(arg) {

	}

};
//マシンが要件を満たしていない
class no_supported_machine :public std::runtime_error {
public:
	no_supported_machine(std::string arg) :runtime_error(arg) {

	}
};
//原因不明のエラー　こわい
class unknown_error :public std::runtime_error {
public:
	unknown_error(std::string arg) :runtime_error(arg) {

	}
};
//ファイルが読み込めない
class cant_open_file:public std::runtime_error {
public:
	cant_open_file(std::string arg) :runtime_error(arg) {

	}
};

namespace uuu {
	//汎用的なツール達
	namespace commonTools {

		//unique_ptr用弱参照(所有権なしの参照)
		template<typename T>class float_ptr {

		private://!!!!!

			//ナマポや!!
			T* dat;


		public:

			float_ptr(std::unique_ptr<T>& source) {
				dat = source.get();
			}
			float_ptr(T* source) {
				dat = source;
			}
			float_ptr() {//非推奨
				dat = NULL;
			}
			~float_ptr() {
				//ぜったいにdatを開放しないこと!!!
			}
			//アクセサ これを開放するやつは鬼畜
			T* operator&() {
				return dat;
			}
			T* get() {
				return dat;
			}
			//無効値(NULL)を指定する
			void none() {
				dat = NULL;
				return;
			}
			//無効値かどうか確認する
			bool isNone() {
				return dat;
			}

			uuu::commonTools::float_ptr<T>& reset(T* source) {
				dat = source;

				return *this;
			}
			uuu::commonTools::float_ptr<T>& reset(std::unique_ptr<T>& source) {

				dat = source.get();

				return *this;

			}

		};

		//ファイルを読み込む
		std::string LoadFileAll(const std::string path);
		//eigenとglmも行列を橋渡しする
		glm::mat4 convertEmat4ToGmat4(const Eigen::Matrix4f& m);
		//glmとassimpを橋渡し
		glm::mat4 convertAmat4ToGmat4(const aiMatrix4x4& m);
		//オイラー角からクォータニオンに変換
		glm::vec4 convertEulerToQuat(const glm::vec3& m);
		//軸と角度からクォータニオン
		glm::quat GetQuatAngleAndAxis(float angle, glm::vec3 axis);
		//クォータニオンから角度を抜き出す
		float GetAngleFromQuat(glm::quat& arg);
		//クォータニオンから軸を抜き出す
		glm::vec3 GetAxisFromQuat(glm::quat& arg, glm::vec3 optionalV = glm::vec3(0, 0, 1));

		//2つのベクトルからクォータニオンを作る
		glm::quat GetQuatTwoVector(const glm::vec3 a, const glm::vec3 b);

		//ノード名からノードを取り出す
		aiNode* FindNodeFromName(std::string name, aiNode* target);

		template<class T> std::vector<std::string> split(const std::string& s, const T& separator, bool ignore_empty = 0, bool split_empty = 0) {
			struct {
				auto len(const std::string&             s) { return s.length(); }
				auto len(const std::string::value_type* p) { return p ? std::char_traits<std::string::value_type>::length(p) : 0; }
				auto len(const std::string::value_type  c) { return c == std::string::value_type() ? 0 : 1; /*return 1;*/ }
			} util;

			if (s.empty()) { /// empty string ///
				if (!split_empty || util.len(separator)) return { "" };
				return {};
			}

			auto v = std::vector<std::string>();
			auto n = static_cast<std::string::size_type>(util.len(separator));
			if (n == 0) {    /// empty separator ///
				if (!split_empty) return { s };
				for (auto&& c : s) v.emplace_back(1, c);
				return v;
			}

			auto p = std::string::size_type(0);
			while (1) {      /// split with separator ///
				auto pos = s.find(separator, p);
				if (pos == std::string::npos) {
					if (ignore_empty && p - n + 1 == s.size()) break;
					v.emplace_back(s.begin() + p, s.end());
					break;
				}
				if (!ignore_empty || p != pos)
					v.emplace_back(s.begin() + p, s.begin() + pos);
				p = pos + n;
			}
			return v;
		}

		//コンパイル時累乗計算
		template<class T>
		constexpr T spow(T base, T exp) noexcept {
			//static_assert(exp >= 0, "Exponent must not be negative");
			return exp <= 0 ? 1
				: exp == 1 ? base
				: base * spow(base, exp - 1);
		}

		//例外有りのatoi&atof　マアマア低速なので主にエラーチェックがいる入力に使ってね
		boost::optional<int> atoiEx(const std::string& arg, bool enableException = false);
		boost::optional<float> atofEx(const std::string& arg, bool enableException = false);
		//符号を取り出す
#define SIGN_OPTIONAL_ZERO_IS_PLUS 0
#define SIGN_OPTIONAL_ZERO_THROW_EXCEPTION 1
#define SIGN_OPTIONAL_ZERO_IS_ZERO 2
		template<typename T>T sign(T arg, __int8 optional = SIGN_OPTIONAL_ZERO_IS_PLUS) {
			switch (optional) {
			case SIGN_OPTIONAL_ZERO_IS_PLUS:
				return arg > 0 ? 1.0 : -1.0;
			case SIGN_OPTIONAL_ZERO_THROW_EXCEPTION:
				if (arg == 0)throw std::invalid_argument("ZERO");
				return sign(arg);
			default:
				if (arg == 0)return 0;
				return sign(arg);
			}
		}
		template<typename T>constexpr T ssign(T arg, __int8 optional = SIGN_OPTIONAL_ZERO_IS_PLUS) {
			return optional == SIGN_OPTIONAL_ZERO_IS_PLUS ? (arg > 0 ? 1.0 : -1.0) : arg == 0 ? 0.0 : sign(arg);
		}

		//toString
		std::string toString(glm::vec2 arg);
		std::string toString(glm::vec3 arg);
		std::string toString(glm::vec4 arg);
		std::string toString(glm::quat arg);
		std::string toString(glm::mat4 arg);

		//アフィン変換行列
		glm::mat4 GetTranslateMat(const glm::vec3& pvec);
		glm::mat4 GetRotateMat(const glm::vec3& eular);
		glm::mat4 GetRotateMat(const glm::vec3& axis, const float& angle);
		glm::mat4 GetRotateMat(glm::quat qrot);
		glm::mat4 GetScaleMat(const float& ratio);
		glm::mat4 GetScaleMat(const glm::vec3& ratio);


	};
	//openGLに関するツール達
	namespace glTools {
		//シェーダーのプログラムオブジェクトを作成する
		GLuint CreateProgramObject(const std::string shaderName);
		//アトリビュートの変数名とidのマップを作る
		__int8 MakeAttribMapNameAndId(std::vector<std::string> names, GLuint progObj, std::unordered_map<std::string, GLuint>& ret);
		//ユニフォームの変数名とidのマップを作る
		__int8 MakeUniformMapNameAndId(std::vector<std::string> names, GLuint progObj, std::unordered_map<std::string, GLuint>& ret);
	}
	
	//テクスチャ,レンダーバッファー共通のフォーマット
	enum renderBufferFormat {
		rgba4 = GL_RGBA4,
		rgba8 = GL_RGBA8,
		rgba12 = GL_RGBA12,
		rgba16 = GL_RGBA16,
		rgb5a1 = GL_RGB5_A1,
		rgb10a2 = GL_RGB10_A2,
		rgb565 = GL_RGB565,
		depth16 = GL_DEPTH_COMPONENT16,
		depth24 = GL_DEPTH_COMPONENT24,
		depth32 = GL_DEPTH_COMPONENT32,
		stencil8 = GL_STENCIL_INDEX8
	};
	using textureFormat=renderBufferFormat;


	//ライブラリを初期化してウィンドウを作成
	GLFWwindow* InitLibsAndCreateWindow();
	//ウィンドウメッセージをごにょごにょする 返り値はウィンドウが開いてるかのフラグ
	__int8 PollWindowMessage();

	//プロトタイプ達
	class vaoOperator;
	class shaderProgramObjectVertexFragment;
	class app;
	class cameraPersp;
	class modelLoaderSerializer;
	class textureOperator;
	class textureLoaderFromImageFile;
	class bitMapAtlas;
	class fontRenderFT;
	class keyboardInterface;
	class mouseInterface;
	namespace uup {
		class physicsManager;
		class rigidBase;
		class modelRigid;
		class sphereRigid;
	};
	namespace uud {
		class boneStructer;
	};
	namespace neoVertexOperators {
		class vboOperator;
		class vaoOperatorNeo;
	};
	class frameBufferOperator;

	//アプリケーションのシングルトン
	class app {
		friend keyboardInterface;
		friend uup::physicsManager;

		static int bind;
		//ウィンドウ達
		static std::vector<GLFWwindow*> windows;
		//ライブラリ初期化時のシステムタイム
		static std::chrono::system_clock::time_point originTime;

		app();
	public:

		//window作成時のプロパティ
		class windowProp {
		public:
			//今までのプロパティを継承するかのフラグ
			bool enableInheritanceProp;
			//ウィンドウサイズ
			int width, height;
			//ウィンドウタイトル
			std::string title;
			//モニター
			GLFWmonitor* monitor;

			//ウィンドウのサイズをユーザーが変更できるか
			boost::optional<bool> enableResize;
			//ウィンドウに枠線の装飾をするか
			boost::optional<bool> enableBorder;
			//ウィンドウが作成時にフォーカスを得るか
			boost::optional<bool> enableFocused;
			//ウィンドウを作成時に最大化するか
			boost::optional<bool> enableMaximized;
			//マルチサンプリングを使用するか&サンプル数
			boost::optional<size_t> enableMultiSampling;
			//ダブルバッファリングを有効にするか
			boost::optional<bool> disableDoubleBuffering;

			//ウィンドウ位置;
			boost::optional<glm::ivec2> windowPos;

			//バッファのスワップ待機数
			boost::optional<unsigned int> swapInterval;

			windowProp(int _width, int _height, std::string _title, bool Inheritance = false);
			windowProp(int _width, int _height, std::string _title, GLFWmonitor* _monitor, bool Inheritance = false);
		};
		//バッファちゃんタイプ
		enum bufferType {
			color=0x1,
			depth=0x2
		};
		//アルファブレンドタイプ
		enum blendType {
			srcAlpha=0,
			one,
			oneMinusSrcAlpha
		};

		//ライブラリを初期化してあらたなウィンドウを作成
		static __int8 InitUUUAndCreateWindow(const windowProp& prop);
		//ライブラリを終了しよう
		static __int8 TerminateUUU();
		//新しいウィンドウを作成
		static int CreateNewWindow(const windowProp& prop);
		//ウィンドウをバインドする
		static __int8 BindWindow(const int ind);
		//デフォルトウィンドウ(初期化時のウィンドウ)をバインドする
		static __int8 BindDefaultWindow();

		//ウィンドウのメッセージを処理する　バインドに対して作用
		static __int8 PollMesageForBind();
		//ダブルバッファリングする　つまり画面の更新　バインドに対して作用
		static __int8 UpdateForBind();

		//指定されたバッファをクリア
		static __int8 ClearBuffer(int clearTarget);

		//デプスバッファを有効化または無効化
		static __int8 SetEnableDepthBufferFlag(bool enableDepthBuffer = true);
		//カリングを有効化または無効化
		static __int8 SetEnableCullingFlag(bool enableCulling = true);
		//アルファブレンドを有効化または無効化　ブレンドモード設定可能
		static __int8 SetAlphaBlendMode(bool enableAlphaBlend = true, blendType blendModeS = blendType::one, blendType blendModeD = blendType::oneMinusSrcAlpha);
		//アンチエイリアスを有効化または無効化
		static __int8 UseMultiSampling(bool enableAntiAlias = true);

		//バインドのウィンドウを返す
		static GLFWwindow* GetBindWindow();

		//ライブラリ初期化からの経過時間を受け取る
		static size_t GetTimeFromInit();
	};

	//ゲームコントローラー用インターフェース
	class gameControllerInterface {

	public:


		//ゲーコンのボタンリストを取得する
		static __int8 GetGconButtonInput(size_t gConIndex, std::vector<unsigned char>& ret,bool checkExistGconOfIndex = true);
		//ゲーコンのジョイスティックの状態を取得する
		static __int8 GetGconJoystickInput(size_t gConIndex, std::vector<float>& ret,bool checkExistGconOfIndex = true);


	};

	//キーボード用インターフェース(?)
	class keyboardInterface {

	public:

		//あるキーの状態を取得する
		static __int8 GetKeyInput(int KeyCode);
		
	};

	//マウス用インターフェース
	class mouseInterface {

		static glm::vec<2, double>before;//直前に呼び出された時のマウスカーソルの位置

	public:

		using pos=glm::vec<2, double>;//位置を表すこのインタフェースで完結した型

		static void Init();//uuuの初期化をシてから呼び出してね

		static glm::vec<2, double>GetMousePosDifference();//バインド済みウィンドウのマウス位置の差分を取得する


	};

	//シェーダープログラムオブジェクト
	class shaderProgramObjectVertexFragment {
	public:
		friend vaoOperator;
		friend neoVertexOperators::vaoOperatorNeo;

		GLuint progObj;
		std::unordered_map<std::string, GLuint> attribNameIndexMap, uniformNameIndexMap;

	public:
		shaderProgramObjectVertexFragment();
		shaderProgramObjectVertexFragment(const std::string shaderName);
		__int8 LoadSource(const std::string shaderName);
		//アトリビュートのマップを作成する
		__int8 CreateAttribMap(const std::vector<std::string>& attribNames);
		//ユニフォームのマップを作成する
		__int8 CreateUniformMap(const std::vector<std::string>& uniformNames);
		//このシェーダープログラムを使う
		__int8 Bind();
		__int8 Unbind();

		//ユニフォームの値を設定
		__int8 SetUniformInt(std::string name, int val);
		__int8 SetUniformValue(std::string name, double value);
		__int8 SetUniformValue(std::string name, glm::dmat4 val);
		__int8 SetUniformValue(std::string name, glm::mat4 val);
		__int8 SetUniformValue(std::string name, glm::vec2 val);
		__int8 SetUniformValue(std::string name, glm::vec4 val);
		__int8 SetUniformArray(std::string name, const std::vector<glm::mat4>& val);
		__int8 SetUniformValue(std::string name, float val);
		__int8 SetUniformCameraPersp(const uuu::cameraPersp& val);
		__int8 SetUniformCameraPersp(std::string name, const uuu::cameraPersp& val);//カメラ構造体をユニフォームする　名前決めれる版 <name>Projection,<name>LookAtでユニフォームされる
		__int8 SetUniformTexUnit(std::string name, const uuu::textureOperator& val);//テクスチャユニットをユニフォームする　まあこんなもん

	};
	//vaoオペレータ(旧)
	class vaoOperator {

		//friend shaderProgramObjectVertexFragment;

		GLuint vaoId;
		std::unordered_map<GLuint, GLuint> attribVaoMap;//アトリビュートのidとvaoのマップ
		boost::optional<std::pair<GLuint,size_t>> elementBuffer;//インデックスバッファ(とそのサイズ)

	public:
		//モデルトランスフォーム
		glm::mat4 modelTransform;

		vaoOperator();

		__int8 CreateVao();
		//頂点属性のvboを作成
		template<typename T>GLuint CreateVertexAttribVbo(GLuint attribId, std::vector <T> & datas, size_t verDataSize) {
			glBindVertexArray(this->vaoId);

			GLuint newVbo;
			glGenBuffers(1, &newVbo);
			glBindBuffer(GL_ARRAY_BUFFER, newVbo);

			//アトリビュートとnewVboを紐づけ
			glEnableVertexAttribArray(attribId);

			//データ転送
			glBufferData(GL_ARRAY_BUFFER, sizeof(T)*datas.size(), datas.data(), GL_STATIC_DRAW);

			//フォーマット
			glVertexAttribPointer(attribId, verDataSize, std::is_same<T, double>() ? GL_DOUBLE : GL_FLOAT, GL_FALSE, verDataSize * sizeof(T), NULL);

			//マップに登録
			this->attribVaoMap[attribId] = newVbo;

			return newVbo;
		}
		//頂点属性のvboを作成 shaderから
		template<typename T> GLuint CreateVertexAttribVbo(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<T>& datas, size_t verDataSize) {
			auto i = shader.attribNameIndexMap.find(name);
			//もしなければおわおわり
			if (i == shader.attribNameIndexMap.end())throw std::invalid_argument((std::string)"マップに" + name + "が存在していません");

			return this->CreateVertexAttribVbo<T>(shader.attribNameIndexMap.at(name), datas, verDataSize);
		}
#ifdef USE_EIGEN 
		//Eigenのベクトルをシリアライズしてからvboを作る
		template<typename V, typename NT, size_t d> GLuint CreateVertexAttribVboForEigen(GLuint attribId, std::vector<V> & datas, std::vector<NT>* serial = NULL) {

			//シリアライズ元を作る
			std::vector<NT>* ser;
			if (serial)ser = serial;
			else ser = new  std::vector<NT>;

			ser->clear();

			//メモリの確保
			ser->resize(datas.size()*d);
			for (size_t v = 0; v < datas.size(); v++)
				for (size_t dim = 0; dim < d; dim++)
					ser->at(v*d + dim) = datas.at(v)[dim];

			return this->CreateVertexAttribVbo<NT>(attribId, *ser, d);

		}
		template<typename V, typename NT, size_t d> GLuint CreateVertexAttribVboForEigen(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<V> & datas, std::vector<NT>* serial = NULL) {

			auto i = shader.attribNameIndexMap.find(name);
			//もしなければおわおわり
			if (i == shader.attribNameIndexMap.end())throw std::invalid_argument((std::string)"マップに" + name + "が存在していません");

			return this->CreateVertexAttribVboForEigen<V, NT, d>(shader.attribNameIndexMap.at(name), datas, serial);

		}
#endif

		//デフォルトのアトリビュートでvboを作る
		enum attribNodeName {
			POSITION = 1,
			NORMAL = 1 << 2,
			COLOR = 1 << 3,
			TEX_COORD0 = 1 << 4,
			TEX_COORD1 = 1 << 5,
			TEX_COORD2 = 1 << 6,
			TEX_COORD3 = 1 << 7,
			//これ以上のテクスチャ座標はカスタム構成してね
			BONE_INFO = 1 << 8,
			INDICES = 1 << 9

		};
		GLuint CreateVertexAttribVboDefaultNameFromModelLoader(uuu::shaderProgramObjectVertexFragment& prog,uuu::modelLoaderSerializer& mod, int attribs);

		//glmベクトル用　頂点属性vboを作成　軽いよ
		GLuint CreateVertexAttribVboForGlmDvec4(GLuint attribId, std::vector<glm::dvec4> & datas);
		GLuint CreateVertexAttribVboForGlmDvec4(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::dvec4>& datas);
		GLuint CreateVertexAttribVboForGlmVec4(GLuint attribId, std::vector<glm::vec4> & datas);
		GLuint CreateDynamicVertexAttribVboForGlmVec4(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec4>& datas);
		GLuint CreateDynamicVertexAttribVboForGlmVec4(GLuint attribId, std::vector<glm::vec4> & datas);
		GLuint UpdateVertexAttribVboForGlmVec4(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec4>& datas);
		GLuint UpdateVertexAttribVboForGlmVec4(GLuint attribId, std::vector<glm::vec4> & datas);
		GLuint CreateVertexAttribVboForGlmVec4(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec4>& datas);
		GLuint CreateVertexAttribVboForGlmVec2(GLuint attribId, std::vector<glm::vec2> & datas);
		GLuint CreateVertexAttribVboForGlmVec2(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec2>& datas);
		GLuint CreateDynamicVertexAttribVboForGlmVec2(GLuint attribId, std::vector<glm::vec2> & datas);
		GLuint CreateDynamicVertexAttribVboForGlmVec2(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec2>& datas);
		GLuint UpdateVertexAttribVboForGlmVec2(GLuint attribId, std::vector<glm::vec2> & datas);
		GLuint UpdateVertexAttribVboForGlmVec2(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec2>& datas);
		//myModelのボーン情報専用　割と便利で草生える
		__int8 CreateVertexAttribVboFromBoneInfo(const std::vector<GLuint>& attribIdIDs,const std::vector<GLuint>& attribIdWeights, boneInfo datas);
		__int8 CreateVertexAttribVboFromBoneInfo(shaderProgramObjectVertexFragment& shader,const std::vector<std::string> namesID, const std::vector<std::string> namesWeight, boneInfo datas);
		//自動名付け機能あり
		__int8 CreateVertexAttribVboFromBoneInfoAutoNaming(shaderProgramObjectVertexFragment& shader, const std::string nameGenID, const std::string nameGenWeight, boneInfo datas);

		//インデックスバッファを作る　まあ便利
		GLuint CreateIndexBuffer(std::vector<unsigned int>&indice);
		//インデックスバッファを使って描画する
		__int8 DrawVaoUsingElement();
		//vaoをバインド　アンバインドする
		__int8 Bind();
		__int8 Unbind();
	};
	//vboとvaoの新しい機能群　一つのモデルデータを複数シェーダーで使い回すときにはこちらを使用してください
	namespace neoVertexOperators {

		//vboとvaoの新しい機能群の初期化
		__int8 InitNeoVertexOperators();

		//頂点属性一つを格納できるバッファ
		class vboOperator {

		protected:
			GLuint vbo;//id本体
			size_t size;
			size_t verticesNum;//頂点数　エレメント数

		public:
			vboOperator();
			virtual ~vboOperator();

			virtual __int8 Bind();
			virtual __int8 Unbind();

			virtual size_t GetVboSize();
			virtual size_t GetVerticesNum();

			//データを送信して頂点属性一つを作成
			//バッファの特性
			enum vboUsage {
				staticDraw=GL_STATIC_DRAW,//データの書き込みを頻繁に行わずcpu側がデータを提供する
				dynamicDraw=GL_DYNAMIC_DRAW,//データの書き込みを頻繁に行いcpu側がデータを提供する
				streamDraw=GL_STREAM_DRAW,//データの書き込みは毎フレームでcpu側がデータを提供する

				staticCopy=GL_STATIC_COPY,//データの書き込みは頻繁に行わずgpu側がデータを提供する(gpuからgpuへデータをコピー)
				dynamicCopy = GL_DYNAMIC_COPY,//データの書き込みを頻繁に行いgpu側がデータを提供する(gpuからgpuへデータをコピー)
				streamCopy = GL_STREAM_COPY,//データの書き込みは毎フレームでgpu側がデータを提供する(gpuからgpuへデータをコピー)

				staticRead = GL_STATIC_READ,//データの書き込みは頻繁に行わずcpuがデータを読み取る
				dynamicRead = GL_DYNAMIC_READ,//データの書き込みを頻繁に行いcpuがデータを読み取る
				streamRead = GL_STREAM_READ//データの書き込みは毎フレームでcpuがデータを読み取る
			};
			template<typename ele>__int8 CreateAttribute(const std::vector<ele>& dats, int usage) {
				this->Bind();

				//サイズはバイト単位らしい
				glBufferData(GL_ARRAY_BUFFER, dats.size() * sizeof(ele), dats.data(), usage);

				this->size = dats.size() * sizeof(ele);
				this->verticesNum = dats.size();

				return true;
			}

		};
		//頂点属性のインデックス用に拡張したバッファ
		class vboOperatorIndices :public vboOperator {
		public:
			vboOperatorIndices() :vboOperator() {
				return;
			}
			~vboOperatorIndices(){
				return;
			}

			__int8 Bind();
			__int8 Unbind();

			template<typename ele>__int8 CreateAttribute(const std::vector<ele>& dats, int usage) {
				this->Bind();

				//サイズはバイト単位らしい
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, dats.size() * sizeof(ele), dats.data(), usage);

				this->size = dats.size() * sizeof(ele);
				this->verticesNum = dats.size();

				return true;
			}

		};

		using vboMap=std::unordered_map<int, uuu::neoVertexOperators::vboOperator*>;//vboのリストを用法でマップにしたもの　↓の関数でつくれます

		//モデルシリアライザーのデータを引き出す (loadAttributrは読み込みたい要素をORで入れてね)
		enum vboAttachType {
			positions=1<<1,
			indices = 1 << 2,
			colors = 1 << 3,
			norms = 1 << 4,
			texCoords0 = 1 << 5,
			texCoords1 = 1 << 6,
			texCoords2 = 1 << 7,
			texCoords3 = 1 << 8,//これ以上のテクスチャ座標はカスタム生成でお願いします
			boneInfos = 1 << 9,
		};
		__int8 CreateVbomapFromModelLoaderSerializer(uuu::modelLoaderSerializer& mod, std::unordered_map<int, vboOperator*>& ret,int loadAttribute);

		//vboを束ねてモデルデータを構成できる、また複数の頂点属性をシェーダーに送れるオブジェクト
		class vaoOperatorNeo {
			GLuint vao;

			static std::unordered_map<int, std::string>autoNamingTemp;//自動名付けのテンプレです

			uuu::neoVertexOperators::vboOperator* indices;

		public:

			//vboのアタッチの仕方を詳しく設定する
			struct vboAttachUsage {
				GLuint type;//要素型
				size_t shift;//1要素あたりのバイト数
				GLboolean enableNormalize;//自動正規化するか
			};

			vaoOperatorNeo();
			~vaoOperatorNeo();

			__int8 Bind();
			__int8 Unbind();

			//vboのマップをシェーダーと一緒に渡してvaoを作ります　オートネーミングとManualネーミングがあります
			static __int8 __init_auto_naming_temp();

			//指定のアトリビュートIDにvboをアタッチ
			__int8 AttachVboByID(GLuint atId, uuu::neoVertexOperators::vboOperator& tar,const vboAttachUsage& usage);
			//指定のアトリビュートIDにvboをアタッチ　名前を文字列で指定する版
			__int8 AttachVboByString(uuu::shaderProgramObjectVertexFragment& shad, std::string name, uuu::neoVertexOperators::vboOperator& tar, const vboAttachUsage usage);
			//エレメント(インデックスを指定する)
			__int8 SetIndexBuffer(uuu::neoVertexOperators::vboOperator* in);

			//自動名付けテンプレートを使ってvboマップをアタッチする　まあええで もちもちカスタム名付けテンプレートもつかえるんや
			__int8 AttachVboMap(uuu::shaderProgramObjectVertexFragment& shad, vboMap& tar, std::unordered_map<int, std::string>* namingTemp = NULL, std::unordered_map<int, uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage>* usageTemp = NULL);


			//描画する
			__int8 DrawElements(uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage usage);
		};

		//vboのアタッチ参照情報(usage)のテンプレ
		class vboUsageTemp {

			friend vaoOperatorNeo;

			static uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage pos;//4次元位置
			static uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage norm;//四次元法線
			static uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage ind;//インデックス
			static uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage texCoord;//テクスチャ座標

			static std::unordered_map<int, uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage>usageMap;//vboの用法とそのアタッチ情報のマップ

		public:
			vboUsageTemp() = delete;
			~vboUsageTemp() = delete;
			vboUsageTemp(const vboUsageTemp& arg) = delete;

			static __int8 Init();

			//アクセサ
			static uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage GetPos();
			static uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage GetNorm();
			static uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage GetInd();
			static uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage GetTexCoord();

		};

	};

	//透視投影カメラ
	class cameraPersp {
	public:
		glm::mat4 persp;
		glm::mat4 lookAt;
	public:
		cameraPersp();

		//透視投影行列を設定する　直接指定と算出
		__int8 SetPerspMatrix(glm::mat4& p);
		__int8 SetPersp(float near, float far, float fov, float aspect);

		//視点行列を設定する
		__int8 SetLookAtMatrix(glm::mat4& p);
		virtual __int8 SetLookAt(glm::vec3 eye, glm::vec3 tar, glm::vec3 up);
		//視点行列をキャラチックに☆
		virtual __int8 SetLookAtChar(glm::vec3 eyepos, glm::vec3 tarWay, glm::vec3 up) {
			return this->SetLookAt(eyepos, eyepos + tarWay, up);
		}
	};

	//モデルローダー&シリアライザー
	//モデルを読み込ませて頂点位置やインデックスなどを最適化して抜き出せる
	class modelLoaderSerializer {

		friend uud::boneStructer;

		Assimp::Importer* loader;
		const aiScene const* scene;
		aiMesh* mesh;//大事なのはこれ

		std::vector<glm::vec4> verticesList;//頂点リスト
		std::vector<glm::vec4> normalsList;//法線リスト
		std::vector<unsigned int> indicesList;//インデックスリスト

		std::vector<glm::vec4> boneIdsList;//ボーンid
		std::vector<glm::vec4> weightsList;//ウェイトのリスト
		std::vector<std::vector<glm::vec2>*> texCoordsList;//頂点座標達
		size_t boneInfosStride;//位置頂点に割り振るvbo数

	public:
		modelLoaderSerializer();
		modelLoaderSerializer(const std::string path,const std::string meshName, unsigned int flag = aiProcessPreset_TargetRealtime_MaxQuality);
		modelLoaderSerializer(const std::string path, size_t index, unsigned int flag = aiProcessPreset_TargetRealtime_MaxQuality);
		~modelLoaderSerializer();

		//モデルをファイルから読み込む
		__int8 LoadModelFromFile(std::string path, size_t index, unsigned int flag = aiProcessPreset_TargetRealtime_MaxQuality);
		__int8 LoadModelFromScene(const aiScene const* scene, size_t index);
		__int8 LoadModelFromFile(const std::string path, const std::string meshName, unsigned int flag = aiProcessPreset_TargetRealtime_MaxQuality);

		//頂点リストを抜き出す(引数をfalseにすると頂点リストの上書き防止チェックを飛ばします)
		std::vector<glm::vec4>* GetVerticesList(bool checkMeshUpdate = true);
		//法線リストを抜き出す(引数をfalseにすると頂点リストの上書き防止チェックを飛ばします)
		std::vector<glm::vec4>* GetNormalsList(bool checkMeshUpdate = true);
		//インデックスリストを抜き出す(引数をfalseにすると頂点リストの上書き防止チェックを飛ばします)
		std::vector<unsigned int>* GetIndicesList(bool checkMeshUpdate = true);
		//ボーン情報リスト(ボーンidとウェイト)を抜き出す　割と重い(引数をfalseにすると頂点リストの上書き防止チェックを飛ばします)
		boneInfo GetBoneInfosList(bool checkMeshUpdate = true);
		//テクスチャ座標リストの取得
		std::vector<glm::vec2>* GetTexCoordsList(size_t channel,bool checkMeshUpdate = true);

	};

	//レンダリング結果を保存しておくバッファ
	class renderBufferOperator {

		friend frameBufferOperator;

		GLuint rbo;

	public:
		renderBufferOperator();
		__int8 Bind() const;
		__int8 Unbind() const;

		//データ構成を設定する enum以外もいろいろあるので状況に応じて選択しよう
		__int8 SetStrage(int rboFormat, GLuint width, GLuint height);

	};
	//フレームバッファーオペレータ　
	class frameBufferOperator {

		friend renderBufferOperator;
		friend uuu::textureOperator;

		GLuint fbo;

	public:
		frameBufferOperator();
		__int8 Bind() const;
		__int8 Unbind() const;

		enum attachTargetType {
			color0 = GL_COLOR_ATTACHMENT0,
			color1 = GL_COLOR_ATTACHMENT1,
			color2 = GL_COLOR_ATTACHMENT2,
			color3 = GL_COLOR_ATTACHMENT3,
			color4 = GL_COLOR_ATTACHMENT4,
			color5 = GL_COLOR_ATTACHMENT5,
			color6 = GL_COLOR_ATTACHMENT6,
			color7 = GL_COLOR_ATTACHMENT7,
			color8 = GL_COLOR_ATTACHMENT8,
			color9 = GL_COLOR_ATTACHMENT9,
			color10 = GL_COLOR_ATTACHMENT10,
			color11 = GL_COLOR_ATTACHMENT11,
			color12 = GL_COLOR_ATTACHMENT12,
			color13 = GL_COLOR_ATTACHMENT13,
			color14 = GL_COLOR_ATTACHMENT14,
			color15 = GL_COLOR_ATTACHMENT15,
			depth = GL_DEPTH_ATTACHMENT,
			stencil = GL_STENCIL_ATTACHMENT
		};

		//☆実装位置
		GLuint GetFboId() {
			return fbo;
		}

		//レンダーバッファオペレータをfboに組み込む　アタッチ(コール時にこのfboがバインドされます)
		__int8 AttachRenderBufferOperator(const attachTargetType target, const renderBufferOperator& obj);
		//テクスチャをfboに組み込む(コール時にこのfboがバインドされます)
		__int8 AttachTextureOperator(int target, const textureOperator& obj);
	};
	//☆フレームバッファオペレータをなくしてしまった人用　いまバインド中のfboを外すマニュアル操作
	__int8 BindDefaultFrameBuffer();

	//テクスチャオペレータ
	class textureOperator {
		friend uuu::textureLoaderFromImageFile;
		friend uuu::fontRenderFT;
		friend uuu::frameBufferOperator;
		friend uuu::shaderProgramObjectVertexFragment;

	protected:
		GLuint texture;
		GLuint sampler;
		GLuint texUnit;
		bool enable = false;//こいつらが有効か
		
		//使用済みテクスチャユニットたち
		static std::vector<GLuint> reservedTexUnits;

		//テクスチャを生成する　内部用
		__int8 __CreateTexture(GLuint texUnit, GLuint width, GLuint height, GLuint format, GLuint informat,GLuint type, void* data = nullptr);
	public:
		//マニュアル操作でテクスチャを生成する　ふつうは画像データを読み込む
		__int8 CreateManual(GLuint texUnit, GLuint width, GLuint height,GLuint format, GLuint informat, GLuint type,void* data=nullptr);
		//texUnit自動生成する版
		__int8 CreateManual(GLuint width, GLuint height, GLuint format, GLuint informat, GLuint type,void* data=nullptr);
		//予約済テクスチャユニットを開放
		__int8 ReleaseTexture();
		//予約済みではないテクスチャユニットを探す
		static GLuint FindFreeTexUnit();

		__int8 Bind() const;
		__int8 Unbind() const;

		GLuint GetTexUnit()const;
		GLuint GetTexId()const {
			return texture;
		}

		~textureOperator();
	};
	//テクスチャローダー
	class textureLoaderFromImageFile {
		friend uuu::bitMapAtlas;
	protected:
		//継承先でテクスチャをいじる

	public:
		//pngファイルを読み込んでテクスチャオペレータを作る
		static __int8 CreateTextureFromPNG(const std::string path, textureOperator& ret,size_t texUnit);
		static __int8 CreateTextureFromPNG(const std::string path, textureOperator& ret);
		//フォントファイルと文字のベクタからフォントのアトラステクスチャを生成 文字列のレンダリングも可能
		static __int8 CreateFontTextureAtlasFromFontFile(const std::string path, textureOperator& ret);

	};

	//ビットマップフォントアトラスのオペレータ 文字をレンダリングできる
	class bitMapAtlas {

	protected:
		//フリータイプのライブラリさん 全部共有
		static boost::optional<FT_Library*> library;
		//フェイスの数
		static size_t faceCounter;

		//フォントを読み込むフェイス
		boost::optional<FT_Face*> fontFace;

		//アトラステクスチャ本体&サンプラー
		boost::optional<GLuint*>atlas;
		size_t unit;//ユニット
		boost::optional<GLuint*>sampler;

		//アトラスに追加する予定の文字達
		std::vector<char16_t> charas;
		//アトラスに追加した文字たちのレイアウト(オフセットx,y,サイズx,y)
		std::unordered_map<char16_t, glm::vec4> charasLayout;

	protected:
		~bitMapAtlas();
		//フリータイプ本体を初期化
		__int8 InitFreeTypeLib();
		//フェイスを初期化　フォントファイルのパスくれ
		__int8 CreateFontFaceFromFilePath(const std::string path, int index = 0);
		//アトラスを作成　つくるだけ
		__int8 _CreateAtlasTexture(size_t u);
		//文字を新たにバッファに追加する
		__int8 AddCharasToBuffer(const std::u16string ini);
		__int8 AddCharasToBuffer(const char16_t from, char16_t to);
		//バッファに保存された文字たちをアトラスに追加する
		__int8 _AddBufferedCharasToAtlas();
		//ユニットを取り出す
		int GetUnitIndex();


		//バインド&アンバインド
		__int8 bind();
		__int8 unbind();

	};

	//フリータイプ用のフォントレンダー ビットマップアトラスの関数をまとめただけなのは秘密
	class fontRenderFT:private uuu::bitMapAtlas,uuu::textureOperator {

	public:

		//コンストラクタ
		fontRenderFT();
		//コンストラクタ　フォントのロードと作成もそのままやっちゃう
		fontRenderFT(const std::string path, int index = 0);

		//フォントをロード　作成
		__int8 CreateFontFace(const std::string path, int index = 0);

		//アトラスに文字を追加
		__int8 AddCharasToBuffer(const std::u16string ini);
		__int8 AddCharasToBuffer(const char16_t from, char16_t to);

		//アトラスに文字を追加してレンダリング
		__int8 AddCharasToBufferAndRendering(const std::u16string ini,size_t texUnit);
		__int8 AddCharasToBufferAndRendering(const char16_t from, char16_t to,size_t texUnit);

		//レンダリング
		//__int8 Rendering(textureOperator& ret,size_t texUnit);
		__int8 Rendering(size_t texUnit);

		//バインド
		__int8 Bind();
		__int8 Unbind();

	};

	//スケルタルアニメーション関係 UmeUmeDanceライブラリ
	namespace uud {
		class boneStructer {

			const aiScene const* m_pScene;
			aiMesh* m_pMesh;
			//aiAnimation* m_pAnimation;

			glm::mat4 m_rootInverse;

			std::vector<glm::mat4> boneOffsets;
			std::vector<glm::mat4> finalTransform;

			std::unordered_map<std::string, size_t> boneIndexMap;

		public:
			boneStructer(const aiScene const* scene, aiMesh* mesh);
			boneStructer(const modelLoaderSerializer& loaader);

			aiAnimation* GetAnimationPtr(size_t index);

			size_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
			size_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
			size_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

			void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
			void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
			void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

			const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);

			void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform, aiAnimation* anim);

			void BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms, aiAnimation* anim);
		};
	};
	//オーディオ関係 UmeUmeAudioライブラリ(OPENAL使用)
	namespace uua {

		//ヘッダーを読み込んで波形データの直前で止めておく(頭出し)
		int ReadHeaderWav(FILE* fp, int *channel, int* bit, int *size, int* freq);

		//音声を操る程度の能力
		class wavOperator {
			//デバイスとコンテキスト　共通
			static ALCdevice *device;
			static ALCcontext *context;

			//バッファセッティング
			size_t bufferQueueMaxNum;//バッファを用意する数と
			size_t bufferSize;//こっちが個々のサイズ

			//PlaySingleをマルチスレッド化したときのスレ
			std::thread* pPlayTh;

			//ソース　この音源の元締め
			ALuint source;
			//ソースの開始位置
			long sourceBegin;

			//音声再生時に処理済みバッファを確認する頻度　ミリ秒ごとに指定できる
			size_t processedBufferCheckDelay;

			//wavヘッダデータ
			int wavChannel, wavBit, wavSize, wavFreq;//チャンネル数(ステレオ　モノラル),ビット数,音源の総サイズ,サンプリング周波数
		public:
			//音源データのストリーム(あとこっちで作ったんか否か)
			FILE* fp; bool CreateFPMyself;

			//音源データを転送するための器
			unsigned char* data;

		public:
			//playSingleを強制停止するにはこれをfalseにしてね
			bool stopPlaySingle;
		protected:

			//ヘッダーを読み込んで波形データの直前で止めておく(頭出し)
			int __DELETED__ReadHeaderWav(FILE* fp, int *channel, int* bit, int *size, int* freq) {
				short res16;
				int res32;
				int dataSize, chunkSize;
				short channelCnt, bitParSample, blockSize;
				int samplingRate, byteParSec;

				int dataPos;
				int flag = 0;

				fread(&res32, 4, 1, fp);
				if (res32 != 0x46464952) {	//"RIFF"
					return 1;	//error 1
				}

				//データサイズ = ファイルサイズ - 8 byte の取得
				fread(&dataSize, 4, 1, fp);

				//WAVEヘッダーの読み
				fread(&res32, 4, 1, fp);
				if (res32 != 0x45564157) {	//"WAVE"
					return 2;	//error 2
				}

				while (flag != 3) {
					//チャンクの読み
					fread(&res32, 4, 1, fp);
					fread(&chunkSize, 4, 1, fp);

					switch (res32) {
					case 0x20746d66:	//"fmt "
						//format 読み込み
						//PCM種類の取得
						fread(&res16, 2, 1, fp);
						if (res16 != 1) {
							//非対応フォーマット
							return 4;
						}
						//モノラル(1)orステレオ(2)
						fread(&channelCnt, 2, 1, fp);
						if (res16 > 2) {
							//チャンネル数間違い
							return 5;
						}
						//サンプリングレート
						fread(&samplingRate, 4, 1, fp);
						//データ速度(byte/sec)=サンプリングレート*ブロックサイズ
						fread(&byteParSec, 4, 1, fp);
						//ブロックサイズ(byte/sample)=チャンネル数*サンプルあたりのバイト数
						fread(&blockSize, 2, 1, fp);
						//サンプルあたりのbit数(bit/sample)：8 or 16
						fread(&bitParSample, 2, 1, fp);

						*channel = (int)channelCnt;
						*bit = (int)bitParSample;
						*freq = samplingRate;

						flag += 1;

						break;
					case 0x61746164:	//"data"
					case 0x64617461:

						*size = chunkSize;

						dataPos = ftell(fp);

						flag += 2;
						break;
					default://それ以外のチャンクならサイズ分読み飛ばす
						fseek(fp, chunkSize, SEEK_CUR);
					}

				}

				//頭出し("fmt "が"data"より後にあった場合のみ動く)
				if (dataPos != ftell(fp)) {
					fseek(fp, dataPos, SEEK_SET);
				}

				return 0;
			}

			//キューの数を取得して最大数に満たなければ追加する ちなみに返り値はファイルの残量があるんかないんか
			__int8 PushNewQueue();
			//キューされたバッファを再生　閾値以上再生されたらこの関数を抜けて戻ってくる
			__int8 PlayQueueBuffer(size_t thou);

			//シングルスレッドで再生
			__int8 PlaySingle(size_t thou, size_t loadNumMax = 1);

		public:

			

			wavOperator();
			wavOperator(const std::string path);
			~wavOperator();
			__int8 InitAudioAndMakeContextCurrent();
			__int8 SetBufferProp(size_t _bufferQueueMaxNum, size_t _bufferSize);
			//wavファイルを読み込む
			__int8 LoadFile(FILE* _f,bool lop=false);
			__int8 LoadFile(const std::string path,bool lop=false);
			//これを呼び出すとロードか再生か判断して実行してくれる
			__int8 Update(size_t thou, size_t loadNumMax = 1);
			//別スレッドで再生してくれるいいやつ
			__int8 PlayMultiThread(size_t thou, size_t loadNumMax = 1);
			//スレッドが終わったらジョインする　終わればtrue 終わってなかったらfalse返す まず再生されてなかったらtrueにするかシないかはフラグで決めれれるよ
			__int8 JoinPlayThread(bool noThreadReturn = false);
			//巻き戻し
			__int8 Rewind();
			__int8 Stop();


			//処理済みバッファのチェックの頻度を設定
			__int8 SetProcessedBufferCheckDelay(size_t ii);
		};
		//neo 具体的にはストリーミングできなくなった
		class neoWavOperator {

		protected:
			static ALCdevice* device;//サウンドデバイス
			static ALCcontext* context;//サウンドのコンテキスト

			ALuint source, buffer;

			//wavヘッダデータ
			int wavChannel, wavBit, wavSize, wavFreq;//チャンネル数(ステレオ　モノラル),ビット数,音源の総サイズ,サンプリング周波数
			std::shared_ptr<char> dataBuffer;//データが入ってるとこ

		public:
			static void Init();
			static void Terminate();
			//初期化する
			neoWavOperator();
			~neoWavOperator();
			//ファイルを読み込む
			void LoadFile(const std::string path);
			//ファイルを破棄する
			void DeleteFileData();

			//再生
			void Play();
			//巻き戻し
			void Rewind();
			//一時停止
			void Pause();
			//停止
			void Stop();
			//ループ
			void Loop(bool enable);

			//再生中かな
			bool IsPlaying();

		};
	};
	namespace _DELETED_uup
	{
		//力学関係(bullet使用)
		//namespace uup {

		//	//あたったオブジェクトをすべて回収する
		//	class  getAllContactObjectCallBack:public btDynamicsWorld::ContactResultCallback {

		//	public:

		//		//ダブリがあるかもね
		//		std::vector<const btCollisionObject*> result;

		//		getAllContactObjectCallBack();

		//		btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);
		//	};
		//	//特定のオブジェクトと衝突しているかをチェックしてくれる
		//	class  contactOneObjectCallBack :public btDynamicsWorld::ContactResultCallback {

		//	public:

		//		//
		//		btCollisionObject* target;
		//		bool resultFrag;//あたってたらtrue

		//		//引数のオブジェとコンタクティング!!
		//		contactOneObjectCallBack(btCollisionObject* oneObj);

		//		btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);
		//	};
		//	

		//	//全体的な設定を行う
		//	class physicsManager {
		//		friend uuu::uup::rigidBase;
		//		friend uuu::uup::modelRigid;
		//	public:
		//		static std::vector<btDynamicsWorld*> worlds;
		//		static boost::optional<size_t>  binded;
		//		static std::unordered_map<btCollisionObject*, uuu::uup::rigidBase*> collisionObjMap;

		//		physicsManager();
		//	public:
		//		//新しい力学ワールドを作成　ちなみにバインドもしてくれる
		//		struct worldProp {
		//			//重力加速度
		//			glm::vec3 gravity;
		//			//gimpactを有効にするか
		//			bool enableGimpact;

		//			worldProp(glm::vec3 _gravity,bool _enableGimpact=true) {

		//				gravity = _gravity;
		//				enableGimpact = _enableGimpact;

		//			}
		//		};
		//		static __int8 CreateNewWorld(worldProp prop);
		//		
		//		//バインド中のワールドをシミュレーションしてくれる
		//		static __int8 SimulationBinded(size_t sub = 0,float clock=0);



		//	};
		//	//剛体のベースクラス
		//	class rigidBase {
		//	protected:
		//	public:
		//		btRigidBody* rigidObj;

		//	public:
		//		struct rigidProp {

		//			btScalar mass;//質量
		//			btVector3 inertia;//慣性モーメント
		//			btQuaternion qrot;//姿勢
		//			btVector3 position;//位置

		//			boost::optional<btScalar> friction;//摩擦係数
		//			boost::optional<btScalar> restitution;//反発係数

		//			rigidProp(btScalar _mass, btVector3 _inertia, btQuaternion _qrot, btVector3 _position) {
		//				mass = _mass;
		//				inertia = _inertia;
		//				qrot = _qrot;
		//				position = _position;
		//			}
		//			rigidProp(btScalar _mass, btVector3 _pos):rigidProp(_mass,btVector3(0,0,0),btQuaternion(0,0,0,1),_pos) {
		//				return;
		//			}
		//		};
		//		rigidBase();

		//		//剛体の変換を受け取る
		//		glm::mat4 GetTransform();
		//		//この剛体をバインドされてるワールドに追加する
		//		__int8 AddThisFromBindedWorld();

		//		//重心位置を取得する
		//		glm::vec3 GetMassPosition();
		//		//重心位置を設定する
		//		__int8 SetMassPosition(const glm::vec3 &pos);

		//		//線形速度を取得する
		//		glm::vec3 GetLinearVelocity();
		//		//線形速度を設定する
		//		__int8 SetLinearVelocity(const glm::vec3 &vel);

		//		//剛体に力を加える
		//		__int8 ApplyForce(glm::vec3 force, glm::vec3 pos = glm::zero<glm::vec3>());


		//		//当たり判定その1 自分と引数のオブジェクトがあたっているかチェックしてくれる(2引数目を立てると凹包との当たり判定も考慮)
		//		virtual __int8 CheckHitOtherObject(uuu::uup::rigidBase* tar);
		//		virtual __int8 GetHitAllObjects(std::vector<uuu::uup::rigidBase*>& ret);
		//	};
		//	//球形剛体のクラス
		//	class sphereRigid :public rigidBase {
		//	public:


		//		struct rigidProp :public uuu::uup::rigidBase::rigidProp {
		//	
		//			btScalar radius;

		//			rigidProp(btScalar _mass, btVector3 _inertia, btQuaternion _qrot, btVector3 _position, btScalar _radius) :uuu::uup::rigidBase::rigidProp(_mass, _inertia, _qrot, _position) {

		//				radius = _radius;
		//			}
		//			rigidProp(btScalar _mass, btVector3 _pos,btScalar _rad) :rigidProp(_mass, btVector3(0, 0, 0), btQuaternion(0, 0, 0, 1), _pos,_rad) {
		//				return;
		//			}
		//		};

		//		__int8 CreateRigid(uuu::uup::sphereRigid::rigidProp prop, bool addBinded);

		//		~sphereRigid();

		//	};
		//	//四角剛体のクラス
		//	class boxRigid:public rigidBase {
		//	public:
		//		struct rigidProp :public uuu::uup::rigidBase::rigidProp {

		//			btVector3 lengths;
		//			rigidProp(btScalar _mass, btVector3 _inertia, btQuaternion _qrot, btVector3 _position, btVector3 _lengths) :uuu::uup::rigidBase::rigidProp(_mass, _inertia, _qrot, _position) {

		//				lengths = _lengths;
		//			}
		//			rigidProp(btScalar _mass, btVector3 _pos, btVector3 _lengths) :rigidProp(_mass, btVector3(0, 0, 0), btQuaternion(0, 0, 0, 1), _pos, _lengths) {
		//				return;
		//			}
		//		};

		//		__int8 CreateRigid(uuu::uup::boxRigid::rigidProp prop, bool addBinded);

		//		~boxRigid();

		//	};
		//	//モデルを読み込んでくれる超絶スーパーデラックス
		//	class modelRigid :public rigidBase {

		//		__int8 __ExportIndices(std::vector<int>& ret, std::vector<unsigned int>& gen);
		//		__int8 __ExportVertices(std::vector<btScalar>& ret, std::vector<glm::vec4>& gen);

		//		std::vector<int> exportedIndices;
		//		std::vector<btScalar> exportedVertices;

		//		btGImpactMeshShape *shape;

		//	public:
		//		struct rigidProp :public rigidBase::rigidProp {
		//			rigidProp(btScalar _mass, btVector3 _inertia, btQuaternion _qrot, btVector3 _position) :uuu::uup::rigidBase::rigidProp(_mass, _inertia, _qrot, _position){}
		//			rigidProp(btScalar _mass, btVector3 _pos):rigidProp(_mass, btVector3(0, 0, 0), btQuaternion(0, 0, 0, 1), _pos){}
		//		};
		//		//モデルを読み込んで剛体作る　最後のフラグでワールドに追加するか選択可能
		//		__int8 CreateFromModel(uuu::modelLoaderSerializer& mod, modelRigid::rigidProp prop, bool addBinded);

		//		~modelRigid();

		//	};

		//};
	}

};

#endif