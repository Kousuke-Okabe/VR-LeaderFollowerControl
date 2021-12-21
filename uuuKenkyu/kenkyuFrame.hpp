#pragma once

#include <uuu.hpp>
#include <uuuEasyTools.hpp>
#include <uuuGame.hpp>
#include <openvr.h> 

#include <vrToolkits.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include "xbeeForWin.hpp"

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <mutex>
#include <algorithm>

#include <fstream>

#include <limits>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "armJointSolver.hpp"
#include "kenkyuArmMeshes.hpp"

#include "TextureManager.h"


#include <opencv2/opencv.hpp>

//#include "armOpeTransfer.hpp"

#ifdef _DEBUG
#pragma comment (lib, "LinearMath_Debug.lib")
#pragma comment (lib, "BulletCollision_Debug.lib")
#pragma comment (lib, "BulletDynamics_Debug.lib")

#else
#pragma comment (lib, "LinearMath.lib")
#pragma comment (lib, "BulletCollision.lib")
#pragma comment (lib, "BulletDynamics.lib")

#endif

namespace kenkyulocal {

	//前方宣言s
	class armTransferSlip;

	template<typename T>class mutexed {
	private:
		mutexed(const mutexed&);
		mutexed& operator=(const mutexed&);
	protected:
		std::mutex mt;
		std::unique_ptr<T> obj;
	public:
		void reset(T* ptr) {
			obj.reset(ptr);
		}
		void release() {
			obj.release();
		}

		mutexed() {}
		mutexed(T* ptr) {
			reset(ptr);
		}

		void Access(std::function<void(T&)> func) {
			std::lock_guard<std::mutex>lock(mt);

			func(*obj.get());
		}
		void Access(std::function<void(const T&)> func) const {
			std::lock_guard<std::mutex>lock(mt);

			func(*obj.get());
		}
	public:
		T getCopy() {
			T ret;
			Access([&](T& o) {ret = o; });

			return ret;
		}
		void setValue(T ob) {
			Access([&](T& o) {o = ob; });
		}

	};

	//viewportつけれるfboオペレータ
	class fboOperatorWithViewport:public uuu::frameBufferOperator {
		using super = uuu::frameBufferOperator;
	protected:
		size_t u, v;
		size_t defu, defv;
		bool returnAutoSize;//デフォルトサイズ==0で自動復帰
	public:
		fboOperatorWithViewport(size_t u, size_t v, size_t defu, size_t defv);
		fboOperatorWithViewport(size_t u, size_t v);

		virtual __int8 Bind();
		virtual __int8 Unbind()const;
	};

	//ビューポートサイズを指定の値にしてデストラクタで復帰する
	class viewportSetterAndAutoReverter {
	protected:
		GLint retvp[4];
	public:
		viewportSetterAndAutoReverter(GLint vp[4]);
		~viewportSetterAndAutoReverter();
	};

	//to_string フォーマット指定できる版
	std::string to_stringf(double _Val, const char* format = "%.3f");

	//卒研のフレームシングルトン
	class kenkyu {
	public:
		const static int version;//今のバージョン

		using Vector7 = Eigen::Matrix<double, 7, 1>;
		using Vector6 = Eigen::Matrix<double, 6, 1>;

		struct posAndQuat {
			glm::vec3 pos;
			glm::quat quat;

			glm::mat4 toMat() const;
			static posAndQuat Make(const Vector7& gen);
		};
	private:
		kenkyu();
	protected:

		//いろいろなプロパティ
		struct _properties {

			double cameraNear;//最短表示距離
			double cameraFar;//最長表示距離
			double fov;//視野
			double pd;//瞳孔間距離

			std::string assetpath;//アセットまでのパス

			unsigned int swapInterval;//同期を待つ回数

			double windowSizeRatio;//VRの解像度とシステムウィンドウとの大きさの比率

			std::string serialPort;//シリアルポート名

			std::string logpath;//ログファイルのパス
			
			double baseVelocity;//[s^-1] アームを動かすベースの速さの逆数
			std::array<double, 6> armVelocitycoefficients;//各モーターにかける速さ比係数
			std::array<std::pair<double, double>, 6>armAngleArea;//各モーターの値域

			std::string glslVersion;
			bool autoSkip;

			bool enableVrSystem;//VRシステムを有効化する
			bool enableSerialSystem;//シリアル通信を有効化する
			bool enableDebugMode;//デバッグモードを有効化する
			bool enableMovie;//動画送信を有効化する

			glm::mat4 vrRotYAxis;//VR関係行列を補正する
		};
		static _properties properties;
		//サブシステムの起動状態を保存
		struct _systemBootFlags {
			bool vr;
			bool serial;

			_systemBootFlags();
		};
		static _systemBootFlags systemBootFlags;

		static uuu::vrMgr kenkyuVr;
		//static std::vector<uuu::easy::neo3Dmesh> meshs;
		static std::unordered_map<std::string, std::unique_ptr<uuu::game::drawable>> gmeshs;//メインシーン用のメッシュ
		static std::unordered_map<std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>> shaders;
		static std::unordered_map<std::string, std::unique_ptr<uuu::textureOperator>> textures;

		static std::unordered_map<std::string, std::unique_ptr<uuu::game::drawable>> meshesInMonitor;

		static uuu::cameraPersp mainCamera, eyeR, eyeL;
		static std::unique_ptr<uuu::frameBufferOperator> fboR, fboL;//VR用のフレーム

		static std::unique_ptr<uuu::textureOperator> colR, colL;//VR用のカラーバッファ

		static std::chrono::system_clock::time_point origin;//開始時間

		static std::unique_ptr<armTransferSlip> armTransfer;//アームへの転送を行う

		static glm::vec3 hmdPos;//ヘッドセットの位置

		
		static posAndQuat beforePosR, beforeposL;//操作システムでフレーム間のコントローラの変化をとる
		static posAndQuat reference;//現在のアーム手先の目標姿勢

		static std::unique_ptr<boost::thread> solverThread;//ソルバーを動かすスレッド
		static bool N_killSover;//ソルバーを殺すフラグ

		//static kenkyuArm arm;
		static std::unique_ptr<armJointSolver::armInverseKineticsSolverForKenkyu<double,6,7>> armSolver;
		static std::mutex mutexRefPoint;//アーム目標値の占有

		struct _actionWarehouse {
			int rhandtype;//0:hand 1:pointer 2:goo
			int lhandtype;

			//グリッパーの度数 +-144度(暫定)
			int rHandingAngle, lHandingAngle;

			_actionWarehouse();
		};
		static mutexed<_actionWarehouse> actionWarehouse;

		static boost::optional<std::ofstream> logStream;
		static std::unique_ptr<boost::thread> logThread;

		static std::pair<unsigned int, unsigned int> windowBounds;//ウィンドウのサイズ(VR使用と未使用の差を吸収)

		static std::mutex solverSpanMiliSecShareMutex;
		static double solverSpanMillSecShare;//ソルバーループのスパンをメインとソルバースレットで共有する

		//ソルバーの動作状況
		class _solverState {
		public:
			enum rawFormat{
				notYet,//まだ解析していない
				solved,//指定回数内で許容域に入った
				closer,//許容域には入っていないが近接している
				impossible,//発振もしくは到達不可能
			};
			_solverState();
			operator std::string();
			void SetRawFormat(const rawFormat& w);//スレッドセーブ
			rawFormat GetRawFormat();//スレッドセーブ

			//更新を通知するやつ
			void SetUpdate(unsigned int up);//アップデートに値を設定
			void DecrementCount();//アップデートをデクリメント
			unsigned int GetUpdate();

		protected:
			rawFormat raw;
			std::mutex rawMutex;

			unsigned int updated;//(フラグが落とされてから現在までに)solveされたなら!=0
			std::mutex updatedMutex;
		};
		static _solverState solverState;

		//メッシュの参照置き場　かならず所有権をほかに渡すこと
		struct _specialMeshs {
			uuu::game::virtualWindow* inMonitor;
		};
		static _specialMeshs specialMeshs;

		//static std::unordered_map<std::string,uuu::textureOperator*> texturesRequiringBindAndUniform;//バインドとユニフォームを要求するテクスチャ VR関係以外すべての参照

		//リファレンスの現在の所有者
		enum _managerForReferencePos { NONE, VR, DEBUG, ESP };
		static _managerForReferencePos nowManagerForReference;

		//特殊なリファレンスの動きを管理する
		class espReferenceController {
		protected:
			static posAndQuat raim, rbase;
			static double nowcount;
			static double countdist;
			static _managerForReferencePos beforeManager;
		public:
			static void SetReferenceAim(const posAndQuat& aim, const posAndQuat& base, _managerForReferencePos bef, const double& dist = 1.0 / 60.0);
			static void EventEspReference();
		};

		//初期姿勢
		static const Vector6 initialAngles;
		static const Vector7 initialMotion;
		//待機姿勢
		static const Vector6 foldArmAngles;
		static const Vector7 foldArmMotion;
		//ゼロの時の位置
		static const Vector7 zeroMotion;

		//hmdMatを変換する
		static glm::mat4 TransVrMatToGmat4(const vr::HmdMatrix34_t& gen);

		//GUIを描画する
		static void DrawGui();

		//VR空間のフレームを描画する
		static void DrawVrFrame(const uuu::cameraPersp& eye);
		//特定のフレームバッファにVR空間を書き込む
		static void DrawVrFrame(uuu::frameBufferOperator& fbo, const uuu::cameraPersp& eye);
		//VRのディスプレイにデータを送る
		static void TransVrHmd();
		//HMDの両目をレンダリングしデータ転送を行う
		static void DrawHmdFrame();

		//コントローラやイベントを処理する
		static void CallbackVrEvents(vr::VREvent_t event);

		//汎用イベントの処理　デバイスの追加やロールの変更などシステム的 callbackvreventsから呼び出して
		static void VrGeneralEvents(vr::VREvent_t event);
		//シーンイベントの処理　つかむとか動作指示とか　callbackvreventsから呼び出して
		static void VrSceneEvents(vr::VREvent_t event);
		//トラッキング処理　コントローラとの姿勢を吸い出す
		static void VrTrackingEvents(vr::VREvent_t event);

		//GUIイベントの処理
		static void GuiEvents();
		//デバックのイベント
		static void DebugEvent();
		//フレーム通信イベント
		static void MovieEvent();
		class _movieBufferCraft {
			//微小結果を集積してフレームデータを作成する
		public:
			std::deque<uint8_t> stack;//断片データを集める
		public:
			void operator()(const std::vector<uint8_t>& buf);
			_movieBufferCraft();
			bool findBegin(const std::vector<uint8_t>& buf, std::vector<uint8_t>::const_iterator& ret);
		};
		static _movieBufferCraft movieBufferCraft;
		static cv::Mat movieFrameMat;//フレーム

		//kenkyuのプロパティーをプロパティシートから読み出す
		static void GetProperty(const std::string& path);
		//kenkyuのプロパティをメンバに適用する
		static void ApplyPropertiesAndSystemBootFlagsToMember();

		//いろいろな初期値を作成
		static void InitAnyMembers();

		//運動学を解く
		static void SolveAngles();

		//アーム用に角度の値域を制限して返す
		template<typename T>static T CorrectAngleAreaForUmeArmFromProperties(const T& rad, size_t id) {
			//if (rad >= 0.0)return std::min<T>(rad, M_PI * (150 / 180.0));
			//else return std::max<T>(rad, -M_PI * (150.0 / 180.0));


			if (rad >= 0.0)return std::min<T>(rad, M_PI * (properties.armAngleArea.at(id).first / 180.0));
			else return std::max<T>(rad, M_PI * (properties.armAngleArea.at(id).second / 180.0));
		}
		template<typename T, size_t dim>static Eigen::Matrix<T, dim, 1> CorrectAngleVecAreaForHutaba(const Eigen::Matrix<T, dim, 1>& vec) {
			Eigen::Matrix<T, dim, 1> ret;

			for (size_t i = 0; i < dim; i++)
				ret(i) = kenkyu::CorrectAngleAreaForUmeArmFromProperties(vec(i), i);

			return ret;
		}

		//ImVec2を足し合わせる
		static ImVec2 AddImVec2s(const ImVec2& a, const ImVec2& b);

		//角度を正規化する 0~2piまで
		template<typename T>static T CorrectAngle(const T& a) {
			//0から2piまで
			//2pi以上ｈじゃいらん
			auto disroop = fmod(a, 2 * M_PI);

			if (disroop < 0.0)disroop += 2 * M_PI;

			return disroop;
		}
		//角度を中央よせにする -pi~piまで
		template<typename T>static T CorrectAngleCentered(const T& a) {
			auto junk = CorrectAngle(a);
			//-PIからPIまで//PI~2PIまでを2PIから引く
			if (junk > M_PI)junk -= 2.0 * M_PI;

			return junk;

		}

		template<typename T, size_t dim>static Eigen::Matrix<T, dim, 1> CorrectAngleVec(const Eigen::Matrix<T, dim, 1>& m) {
			Eigen::Matrix<T, dim, 1> ret;

			for (size_t i = 0; i < dim; i++)
				ret(i) = CorrectAngle<T>(m(i));

			return ret;
		}
		template<typename T, size_t dim>static Eigen::Matrix<T, dim, 1> CorrectAngleCenteredVec(const Eigen::Matrix<T, dim, 1>& m) {
			Eigen::Matrix<T, dim, 1> ret;

			for (size_t i = 0; i < dim; i++)
				ret(i) = CorrectAngleCentered<T>(m(i));

			return ret;
		}


		//角度の平均をとる
		template<typename T, size_t dim>static T AveAngleVec(const Eigen::Matrix<T, dim, 1>& m) {
			T sum = 0;

			for (size_t i = 0; i < dim; i++)
				sum += fabs(m(i));

			return sum / 7.0;
		}

		//ラジアンからサーボ用角度を作る
		static int ToHutabaDegreeFromRadians(const double& rad);
		template<typename T, size_t dim>static Eigen::Matrix<int, dim, 1> ToHutabaDegreeFromRadiansVec(const Eigen::Matrix<T, dim, 1>& m) {
			Eigen::Matrix<int, dim, 1> ret;
			for (size_t i = 0; i < dim; i++)
				ret(i) = ToHutabaDegreeFromRadians(m(i));

			return ret;
		}

		//角度を1/10する
		template<typename T, size_t dim>static Eigen::Matrix<T, dim, 1> ToDegreeFrom10TimesDegree(const Eigen::Matrix<T, dim, 1>& m) {
			Eigen::Matrix<T, dim, 1> ret;

			for (size_t i = 0; i < dim; i++)
				ret(i) = m(i) / (T)10.0;

			return ret;
		}

		//削除済み!!! 角度を是正する(ラジアン)
		template<typename T>static T _deleted_CorrectAngleAreaForHutaba(const T& rad) {
			if (rad >= 0.0)return std::min<T>(rad, M_PI * (150 / 180.0));
			else return std::max<T>(rad, -M_PI * (150.0 / 180.0));
		}
		template<typename T, size_t dim>static Eigen::Matrix<T, dim, 1> _deleted_CorrectAngleVecAreaForHutaba(const Eigen::Matrix<T, dim, 1>& vec) {
			Eigen::Matrix<T, dim, 1> ret;

			for (size_t i = 0; i < dim; i++)
				ret(i) = kenkyu::CorrectAngleAreaForUmeArmFromProperties(vec(i), i);

			return ret;
		}

		//呼び出しのスパンを計測する(フレームレートを計算できる)
		static std::chrono::milliseconds GetSpan();

		//アームの順運動学
		static Vector7 fjikkenWithGenMatrixVersion(const Vector6& q, const Eigen::Quaterniond& gen);//行列版　方程式がわかりやすいように
		static Vector7 fjikkenWithGen(const Vector6& q, const Eigen::Quaterniond& gen);

		//命令をシリアルで送る
		class MgrSendPosquadx{
			MgrSendPosquadx();
		public:
			static mutexed<posAndQuat> open;//解放側 メインスレッド側から書き換える
		protected:

			static mutexed<bool> N_kill;//キルスイッチ
			static posAndQuat close;//閉鎖側　ここからしか使えない
			static void Sub();
			static std::unique_ptr<std::thread> subthread;
		public:
			static void Boot(const posAndQuat& init);
			static void Terminate();
		};

	public:

		static bool continueLoop;//ループを続けるフラグ　メインループで監視して

		//uuu本体とuuuVRを起動しウィンドウを開く
		static void BootUuuSetForKekyu();

		//使用するグラフィックを初期化する またVRのためのリソース生成も行う
		static void InitGraphics();

		//研究用オブジェクトを描画する
		static void Draw();
		//研究アプリ用のイベントを投げる
		static void Event();

		//アプリを終了する
		static void Terminate();

		//非同期的にロギングするシステム
		class printer {
			printer();
		protected:
			static std::unique_ptr<std::thread> writeThread;
			static mutexed<std::deque<std::string>> closedBuffer;//ここにたすくをためる
			static mutexed<bool> N_kill;
		public:


			static void Sub();

			static void Boot();

			static void Terminate();

			static void Queue(const std::string& str);

		};
		class filelogger{
			filelogger();
		protected:
			static std::unique_ptr<std::thread> writeThread;
			static mutexed<std::deque<std::string>> closedBuffer;//ここにたすくをためる
			static mutexed<bool> N_kill;
			static std::unique_ptr<std::ofstream> file;
		public:


			static void Sub();

			static void Boot();

			static void Terminate();

			static void Queue(const std::string& str);
		};

		//ロギングする
		enum logState {
			logInfo,
			logWarning,
			logError,
			logNote,
			logDebug,
			logSaved//ファイルに保存
		};
		static void log(const std::string& str, logState st = logState::logInfo);
		static void log(const std::string name, glm::vec3 v3, logState st = logState::logInfo);
		static void log(const std::string name, glm::quat v4, logState st = logState::logInfo);
		template<typename T, size_t dim>static void log(const std::string name, const Eigen::Matrix<T, dim, 1>& vec, logState st,const std::string split=", ") {
			std::string str = name + "\t";

			for (const auto& i : vec)
				str += std::to_string(i) + split;

			kenkyu::log(str, st);
		}

		static bool GetYorN();

		static void SaveALog(std::string);
		
		static std::string assets(const std::string& details);

		//モーターの回転角を出す　ないときは例外
		static Vector6 GetMoterAngles();

		//kenkyuのプライベートを操作できる実験室
		static void Lab();

	};

	
};