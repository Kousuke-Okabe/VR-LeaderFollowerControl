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

	//�O���錾s
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

	//viewport�����fbo�I�y���[�^
	class fboOperatorWithViewport:public uuu::frameBufferOperator {
		using super = uuu::frameBufferOperator;
	protected:
		size_t u, v;
		size_t defu, defv;
		bool returnAutoSize;//�f�t�H���g�T�C�Y==0�Ŏ������A
	public:
		fboOperatorWithViewport(size_t u, size_t v, size_t defu, size_t defv);
		fboOperatorWithViewport(size_t u, size_t v);

		virtual __int8 Bind();
		virtual __int8 Unbind()const;
	};

	//�r���[�|�[�g�T�C�Y���w��̒l�ɂ��ăf�X�g���N�^�ŕ��A����
	class viewportSetterAndAutoReverter {
	protected:
		GLint retvp[4];
	public:
		viewportSetterAndAutoReverter(GLint vp[4]);
		~viewportSetterAndAutoReverter();
	};

	//to_string �t�H�[�}�b�g�w��ł����
	std::string to_stringf(double _Val, const char* format = "%.3f");

	//�����̃t���[���V���O���g��
	class kenkyu {
	public:
		const static int version;//���̃o�[�W����

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

		//���낢��ȃv���p�e�B
		struct _properties {

			double cameraNear;//�ŒZ�\������
			double cameraFar;//�Œ��\������
			double fov;//����
			double pd;//���E�ԋ���

			std::string assetpath;//�A�Z�b�g�܂ł̃p�X

			unsigned int swapInterval;//������҂�

			double windowSizeRatio;//VR�̉𑜓x�ƃV�X�e���E�B���h�E�Ƃ̑傫���̔䗦

			std::string serialPort;//�V���A���|�[�g��

			std::string logpath;//���O�t�@�C���̃p�X
			
			double baseVelocity;//[s^-1] �A�[���𓮂����x�[�X�̑����̋t��
			std::array<double, 6> armVelocitycoefficients;//�e���[�^�[�ɂ����鑬����W��
			std::array<std::pair<double, double>, 6>armAngleArea;//�e���[�^�[�̒l��

			std::string glslVersion;
			bool autoSkip;

			bool enableVrSystem;//VR�V�X�e����L��������
			bool enableSerialSystem;//�V���A���ʐM��L��������
			bool enableDebugMode;//�f�o�b�O���[�h��L��������
			bool enableMovie;//���摗�M��L��������

			glm::mat4 vrRotYAxis;//VR�֌W�s���␳����
		};
		static _properties properties;
		//�T�u�V�X�e���̋N����Ԃ�ۑ�
		struct _systemBootFlags {
			bool vr;
			bool serial;

			_systemBootFlags();
		};
		static _systemBootFlags systemBootFlags;

		static uuu::vrMgr kenkyuVr;
		//static std::vector<uuu::easy::neo3Dmesh> meshs;
		static std::unordered_map<std::string, std::unique_ptr<uuu::game::drawable>> gmeshs;//���C���V�[���p�̃��b�V��
		static std::unordered_map<std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>> shaders;
		static std::unordered_map<std::string, std::unique_ptr<uuu::textureOperator>> textures;

		static std::unordered_map<std::string, std::unique_ptr<uuu::game::drawable>> meshesInMonitor;

		static uuu::cameraPersp mainCamera, eyeR, eyeL;
		static std::unique_ptr<uuu::frameBufferOperator> fboR, fboL;//VR�p�̃t���[��

		static std::unique_ptr<uuu::textureOperator> colR, colL;//VR�p�̃J���[�o�b�t�@

		static std::chrono::system_clock::time_point origin;//�J�n����

		static std::unique_ptr<armTransferSlip> armTransfer;//�A�[���ւ̓]�����s��

		static glm::vec3 hmdPos;//�w�b�h�Z�b�g�̈ʒu

		
		static posAndQuat beforePosR, beforeposL;//����V�X�e���Ńt���[���Ԃ̃R���g���[���̕ω����Ƃ�
		static posAndQuat reference;//���݂̃A�[�����̖ڕW�p��

		static std::unique_ptr<boost::thread> solverThread;//�\���o�[�𓮂����X���b�h
		static bool N_killSover;//�\���o�[���E���t���O

		//static kenkyuArm arm;
		static std::unique_ptr<armJointSolver::armInverseKineticsSolverForKenkyu<double,6,7>> armSolver;
		static std::mutex mutexRefPoint;//�A�[���ڕW�l�̐�L

		struct _actionWarehouse {
			int rhandtype;//0:hand 1:pointer 2:goo
			int lhandtype;

			//�O���b�p�[�̓x�� +-144�x(�b��)
			int rHandingAngle, lHandingAngle;

			_actionWarehouse();
		};
		static mutexed<_actionWarehouse> actionWarehouse;

		static boost::optional<std::ofstream> logStream;
		static std::unique_ptr<boost::thread> logThread;

		static std::pair<unsigned int, unsigned int> windowBounds;//�E�B���h�E�̃T�C�Y(VR�g�p�Ɩ��g�p�̍����z��)

		static std::mutex solverSpanMiliSecShareMutex;
		static double solverSpanMillSecShare;//�\���o�[���[�v�̃X�p�������C���ƃ\���o�[�X���b�g�ŋ��L����

		//�\���o�[�̓����
		class _solverState {
		public:
			enum rawFormat{
				notYet,//�܂���͂��Ă��Ȃ�
				solved,//�w��񐔓��ŋ��e��ɓ�����
				closer,//���e��ɂ͓����Ă��Ȃ����ߐڂ��Ă���
				impossible,//���U�������͓��B�s�\
			};
			_solverState();
			operator std::string();
			void SetRawFormat(const rawFormat& w);//�X���b�h�Z�[�u
			rawFormat GetRawFormat();//�X���b�h�Z�[�u

			//�X�V��ʒm������
			void SetUpdate(unsigned int up);//�A�b�v�f�[�g�ɒl��ݒ�
			void DecrementCount();//�A�b�v�f�[�g���f�N�������g
			unsigned int GetUpdate();

		protected:
			rawFormat raw;
			std::mutex rawMutex;

			unsigned int updated;//(�t���O�����Ƃ���Ă��猻�݂܂ł�)solve���ꂽ�Ȃ�!=0
			std::mutex updatedMutex;
		};
		static _solverState solverState;

		//���b�V���̎Q�ƒu����@���Ȃ炸���L�����ق��ɓn������
		struct _specialMeshs {
			uuu::game::virtualWindow* inMonitor;
		};
		static _specialMeshs specialMeshs;

		//static std::unordered_map<std::string,uuu::textureOperator*> texturesRequiringBindAndUniform;//�o�C���h�ƃ��j�t�H�[����v������e�N�X�`�� VR�֌W�ȊO���ׂĂ̎Q��

		//���t�@�����X�̌��݂̏��L��
		enum _managerForReferencePos { NONE, VR, DEBUG, ESP };
		static _managerForReferencePos nowManagerForReference;

		//����ȃ��t�@�����X�̓������Ǘ�����
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

		//�����p��
		static const Vector6 initialAngles;
		static const Vector7 initialMotion;
		//�ҋ@�p��
		static const Vector6 foldArmAngles;
		static const Vector7 foldArmMotion;
		//�[���̎��̈ʒu
		static const Vector7 zeroMotion;

		//hmdMat��ϊ�����
		static glm::mat4 TransVrMatToGmat4(const vr::HmdMatrix34_t& gen);

		//GUI��`�悷��
		static void DrawGui();

		//VR��Ԃ̃t���[����`�悷��
		static void DrawVrFrame(const uuu::cameraPersp& eye);
		//����̃t���[���o�b�t�@��VR��Ԃ���������
		static void DrawVrFrame(uuu::frameBufferOperator& fbo, const uuu::cameraPersp& eye);
		//VR�̃f�B�X�v���C�Ƀf�[�^�𑗂�
		static void TransVrHmd();
		//HMD�̗��ڂ������_�����O���f�[�^�]�����s��
		static void DrawHmdFrame();

		//�R���g���[����C�x���g����������
		static void CallbackVrEvents(vr::VREvent_t event);

		//�ėp�C�x���g�̏����@�f�o�C�X�̒ǉ��⃍�[���̕ύX�ȂǃV�X�e���I callbackvrevents����Ăяo����
		static void VrGeneralEvents(vr::VREvent_t event);
		//�V�[���C�x���g�̏����@���ނƂ�����w���Ƃ��@callbackvrevents����Ăяo����
		static void VrSceneEvents(vr::VREvent_t event);
		//�g���b�L���O�����@�R���g���[���Ƃ̎p�����z���o��
		static void VrTrackingEvents(vr::VREvent_t event);

		//GUI�C�x���g�̏���
		static void GuiEvents();
		//�f�o�b�N�̃C�x���g
		static void DebugEvent();
		//�t���[���ʐM�C�x���g
		static void MovieEvent();
		class _movieBufferCraft {
			//�������ʂ��W�ς��ăt���[���f�[�^���쐬����
		public:
			std::deque<uint8_t> stack;//�f�Ѓf�[�^���W�߂�
		public:
			void operator()(const std::vector<uint8_t>& buf);
			_movieBufferCraft();
			bool findBegin(const std::vector<uint8_t>& buf, std::vector<uint8_t>::const_iterator& ret);
		};
		static _movieBufferCraft movieBufferCraft;
		static cv::Mat movieFrameMat;//�t���[��

		//kenkyu�̃v���p�e�B�[���v���p�e�B�V�[�g����ǂݏo��
		static void GetProperty(const std::string& path);
		//kenkyu�̃v���p�e�B�������o�ɓK�p����
		static void ApplyPropertiesAndSystemBootFlagsToMember();

		//���낢��ȏ����l���쐬
		static void InitAnyMembers();

		//�^���w������
		static void SolveAngles();

		//�A�[���p�Ɋp�x�̒l��𐧌����ĕԂ�
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

		//ImVec2�𑫂����킹��
		static ImVec2 AddImVec2s(const ImVec2& a, const ImVec2& b);

		//�p�x�𐳋K������ 0~2pi�܂�
		template<typename T>static T CorrectAngle(const T& a) {
			//0����2pi�܂�
			//2pi�ȏよ���Ⴂ���
			auto disroop = fmod(a, 2 * M_PI);

			if (disroop < 0.0)disroop += 2 * M_PI;

			return disroop;
		}
		//�p�x�𒆉��悹�ɂ��� -pi~pi�܂�
		template<typename T>static T CorrectAngleCentered(const T& a) {
			auto junk = CorrectAngle(a);
			//-PI����PI�܂�//PI~2PI�܂ł�2PI�������
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


		//�p�x�̕��ς��Ƃ�
		template<typename T, size_t dim>static T AveAngleVec(const Eigen::Matrix<T, dim, 1>& m) {
			T sum = 0;

			for (size_t i = 0; i < dim; i++)
				sum += fabs(m(i));

			return sum / 7.0;
		}

		//���W�A������T�[�{�p�p�x�����
		static int ToHutabaDegreeFromRadians(const double& rad);
		template<typename T, size_t dim>static Eigen::Matrix<int, dim, 1> ToHutabaDegreeFromRadiansVec(const Eigen::Matrix<T, dim, 1>& m) {
			Eigen::Matrix<int, dim, 1> ret;
			for (size_t i = 0; i < dim; i++)
				ret(i) = ToHutabaDegreeFromRadians(m(i));

			return ret;
		}

		//�p�x��1/10����
		template<typename T, size_t dim>static Eigen::Matrix<T, dim, 1> ToDegreeFrom10TimesDegree(const Eigen::Matrix<T, dim, 1>& m) {
			Eigen::Matrix<T, dim, 1> ret;

			for (size_t i = 0; i < dim; i++)
				ret(i) = m(i) / (T)10.0;

			return ret;
		}

		//�폜�ς�!!! �p�x�𐥐�����(���W�A��)
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

		//�Ăяo���̃X�p�����v������(�t���[�����[�g���v�Z�ł���)
		static std::chrono::milliseconds GetSpan();

		//�A�[���̏��^���w
		static Vector7 fjikkenWithGenMatrixVersion(const Vector6& q, const Eigen::Quaterniond& gen);//�s��Ł@���������킩��₷���悤��
		static Vector7 fjikkenWithGen(const Vector6& q, const Eigen::Quaterniond& gen);

		//���߂��V���A���ő���
		class MgrSendPosquadx{
			MgrSendPosquadx();
		public:
			static mutexed<posAndQuat> open;//����� ���C���X���b�h�����珑��������
		protected:

			static mutexed<bool> N_kill;//�L���X�C�b�`
			static posAndQuat close;//�����@�������炵���g���Ȃ�
			static void Sub();
			static std::unique_ptr<std::thread> subthread;
		public:
			static void Boot(const posAndQuat& init);
			static void Terminate();
		};

	public:

		static bool continueLoop;//���[�v�𑱂���t���O�@���C�����[�v�ŊĎ�����

		//uuu�{�̂�uuuVR���N�����E�B���h�E���J��
		static void BootUuuSetForKekyu();

		//�g�p����O���t�B�b�N������������ �܂�VR�̂��߂̃��\�[�X�������s��
		static void InitGraphics();

		//�����p�I�u�W�F�N�g��`�悷��
		static void Draw();
		//�����A�v���p�̃C�x���g�𓊂���
		static void Event();

		//�A�v�����I������
		static void Terminate();

		//�񓯊��I�Ƀ��M���O����V�X�e��
		class printer {
			printer();
		protected:
			static std::unique_ptr<std::thread> writeThread;
			static mutexed<std::deque<std::string>> closedBuffer;//�����ɂ����������߂�
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
			static mutexed<std::deque<std::string>> closedBuffer;//�����ɂ����������߂�
			static mutexed<bool> N_kill;
			static std::unique_ptr<std::ofstream> file;
		public:


			static void Sub();

			static void Boot();

			static void Terminate();

			static void Queue(const std::string& str);
		};

		//���M���O����
		enum logState {
			logInfo,
			logWarning,
			logError,
			logNote,
			logDebug,
			logSaved//�t�@�C���ɕۑ�
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

		//���[�^�[�̉�]�p���o���@�Ȃ��Ƃ��͗�O
		static Vector6 GetMoterAngles();

		//kenkyu�̃v���C�x�[�g�𑀍�ł��������
		static void Lab();

	};

	
};