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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "newArm.hpp"
#include "virtualWindow.hpp"

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
#define assets(name) properties.assetpath+#name

	//研究で使う腕
	class kenkyuArm : public newarm::armSolver<double, 6, 7> {
	public:

		//6自由度アーム
		using Vector6d = Eigen::Matrix<double, 6, 1>;
		using Vector7d = Eigen::Matrix<double, 7, 1>;
		static Vector7d fjikken(const Vector6d& q);
	
		kenkyuArm(const Vector6d& defQ);

	};


	//卒研のフレームシングルトン
	class kenkyu {
	public:
		struct posAndQuat {
			glm::vec3 pos;
			glm::quat quat;

			glm::mat4 toMat();
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
			double windowSizeRatio;//VRの解像度とシステムウィンドウとの大きさの比率

			std::string serialPort;//シリアルポート名

			std::string logpath;//ログファイルのパス
			
			double baseVelocity;//[s^-1] アームを動かすベースの速さの逆数
			std::array<double, 6> armVelocitycoefficients;//各モーターにかける速さ比係数
			std::array<std::pair<double, double>, 6>armAngleArea;//各モーターの値域

			std::string glslVersion;
			bool autoSkip;
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
		static std::unordered_map<std::string, uuu::game::mesh> gmeshs;
		static std::unordered_map<std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>> shaders;

		static uuu::cameraPersp mainCamera, eyeR, eyeL;
		static std::unique_ptr<uuu::frameBufferOperator> fboR, fboL;//VR用のフレーム

		static std::unique_ptr<uuu::textureOperator> colR, colL;//VR用のカラーバッファ

		static std::chrono::system_clock::time_point origin;//開始時間

		static std::unique_ptr<umeArmTransfer> armMgr;//アームへの転送を行う

		static glm::vec3 hmdPos;//ヘッドセットの位置

		
		static posAndQuat beforePosR, beforeposL;//操作システムでフレーム間のコントローラの変化をとる
		static posAndQuat reference;//現在のアーム手先の目標姿勢

		//static std::list<boost::thread> serialWriteThreads;//いま書き込み中のシリアル
		static std::array<std::unique_ptr<boost::thread>,6> serialWriteThreads;//書き込みするためのスレッド
		static std::unique_ptr<boost::thread> solverThread;//ソルバーを動かすスレッド
		static bool N_killSover;//ソルバーを殺すフラグ

		static kenkyuArm arm;
		static std::mutex mutexRefPoint;//アーム目標値の占有

		struct _actionWarehouse {
			int rhandtype;//0:hand 1:pointer 2:goo
			int lhandtype;
		};
		static _actionWarehouse actionWarehouse;

		static boost::optional<std::ofstream> logStream;
		static std::unique_ptr<boost::thread> logThread;

		static std::pair<unsigned int, unsigned int> windowBounds;//ウィンドウのサイズ(VR使用と未使用の差を吸収)

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
			void Set(const rawFormat& w);//スレッドセーブ
			rawFormat Get();//スレッドセーブ

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

		static std::unique_ptr<_uuu::virtualWindow> inMonitor;//3D空間に内蔵されたモニター

		static std::unordered_map<std::string,uuu::textureOperator*> texturesRequiringBindAndUniform;//バインドとユニフォームを要求するテクスチャ VR関係以外すべての参照

		//hmdMatを変換する
		static glm::mat4 TransVrMatToGmat4(const vr::HmdMatrix34_t& gen);

		//GUIを描画する
		static void DrawGui();


		//VR空間のフレームを描画する
		static void DrawVrFrame(const uuu::cameraPersp& eye);
		//VRのフレームにVR空間を書き込む
		static void DrawVrFrame(uuu::frameBufferOperator& fbo, const uuu::cameraPersp& eye);
		//VRのディスプレイにデータを送る
		static void TransVrHmd();

		//コントローラやイベントを処理する
		static void CallbackVrEvents(vr::VREvent_t event);

		//汎用イベントの処理　デバイスの追加やロールの変更などシステム的 callbackvreventsから呼び出して
		static void GeneralEvents(vr::VREvent_t event);
		//シーンイベントの処理　つかむとか動作指示とか　callbackvreventsから呼び出して
		static void SceneEvents(vr::VREvent_t event);
		//トラッキング処理　コントローラとの姿勢を吸い出す
		static void TrackingEvents(vr::VREvent_t event);

		//GUIイベントの処理
		static void GuiEvents();

		//kenkyuのプロパティーをプロパティシートから読み出す
		static void GetProperty(const std::string& path);

		//いろいろな初期値を作成
		static void InitAnyMembers();

		//運動学を解く
		static void _deleted_SolveAngles();//目標値まで近付けてから転送するver
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

	};

	
};