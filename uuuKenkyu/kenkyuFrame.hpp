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

	//�����Ŏg���r
	class kenkyuArm : public newarm::armSolver<double, 6, 7> {
	public:

		//6���R�x�A�[��
		using Vector6d = Eigen::Matrix<double, 6, 1>;
		using Vector7d = Eigen::Matrix<double, 7, 1>;
		static Vector7d fjikken(const Vector6d& q);
	
		kenkyuArm(const Vector6d& defQ);

	};


	//�����̃t���[���V���O���g��
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

		//���낢��ȃv���p�e�B
		struct _properties {
			double cameraNear;//�ŒZ�\������
			double cameraFar;//�Œ��\������
			double fov;//����
			double pd;//���E�ԋ���

			std::string assetpath;//�A�Z�b�g�܂ł̃p�X
			double windowSizeRatio;//VR�̉𑜓x�ƃV�X�e���E�B���h�E�Ƃ̑傫���̔䗦

			std::string serialPort;//�V���A���|�[�g��

			std::string logpath;//���O�t�@�C���̃p�X
			
			double baseVelocity;//[s^-1] �A�[���𓮂����x�[�X�̑����̋t��
			std::array<double, 6> armVelocitycoefficients;//�e���[�^�[�ɂ����鑬����W��
			std::array<std::pair<double, double>, 6>armAngleArea;//�e���[�^�[�̒l��

			std::string glslVersion;
			bool autoSkip;
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
		static std::unordered_map<std::string, uuu::game::mesh> gmeshs;
		static std::unordered_map<std::string, std::shared_ptr<uuu::shaderProgramObjectVertexFragment>> shaders;

		static uuu::cameraPersp mainCamera, eyeR, eyeL;
		static std::unique_ptr<uuu::frameBufferOperator> fboR, fboL;//VR�p�̃t���[��

		static std::unique_ptr<uuu::textureOperator> colR, colL;//VR�p�̃J���[�o�b�t�@

		static std::chrono::system_clock::time_point origin;//�J�n����

		static std::unique_ptr<umeArmTransfer> armMgr;//�A�[���ւ̓]�����s��

		static glm::vec3 hmdPos;//�w�b�h�Z�b�g�̈ʒu

		
		static posAndQuat beforePosR, beforeposL;//����V�X�e���Ńt���[���Ԃ̃R���g���[���̕ω����Ƃ�
		static posAndQuat reference;//���݂̃A�[�����̖ڕW�p��

		//static std::list<boost::thread> serialWriteThreads;//���܏������ݒ��̃V���A��
		static std::array<std::unique_ptr<boost::thread>,6> serialWriteThreads;//�������݂��邽�߂̃X���b�h
		static std::unique_ptr<boost::thread> solverThread;//�\���o�[�𓮂����X���b�h
		static bool N_killSover;//�\���o�[���E���t���O

		static kenkyuArm arm;
		static std::mutex mutexRefPoint;//�A�[���ڕW�l�̐�L

		struct _actionWarehouse {
			int rhandtype;//0:hand 1:pointer 2:goo
			int lhandtype;
		};
		static _actionWarehouse actionWarehouse;

		static boost::optional<std::ofstream> logStream;
		static std::unique_ptr<boost::thread> logThread;

		static std::pair<unsigned int, unsigned int> windowBounds;//�E�B���h�E�̃T�C�Y(VR�g�p�Ɩ��g�p�̍����z��)

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
			void Set(const rawFormat& w);//�X���b�h�Z�[�u
			rawFormat Get();//�X���b�h�Z�[�u

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

		static std::unique_ptr<_uuu::virtualWindow> inMonitor;//3D��Ԃɓ������ꂽ���j�^�[

		static std::unordered_map<std::string,uuu::textureOperator*> texturesRequiringBindAndUniform;//�o�C���h�ƃ��j�t�H�[����v������e�N�X�`�� VR�֌W�ȊO���ׂĂ̎Q��

		//hmdMat��ϊ�����
		static glm::mat4 TransVrMatToGmat4(const vr::HmdMatrix34_t& gen);

		//GUI��`�悷��
		static void DrawGui();


		//VR��Ԃ̃t���[����`�悷��
		static void DrawVrFrame(const uuu::cameraPersp& eye);
		//VR�̃t���[����VR��Ԃ���������
		static void DrawVrFrame(uuu::frameBufferOperator& fbo, const uuu::cameraPersp& eye);
		//VR�̃f�B�X�v���C�Ƀf�[�^�𑗂�
		static void TransVrHmd();

		//�R���g���[����C�x���g����������
		static void CallbackVrEvents(vr::VREvent_t event);

		//�ėp�C�x���g�̏����@�f�o�C�X�̒ǉ��⃍�[���̕ύX�ȂǃV�X�e���I callbackvrevents����Ăяo����
		static void GeneralEvents(vr::VREvent_t event);
		//�V�[���C�x���g�̏����@���ނƂ�����w���Ƃ��@callbackvrevents����Ăяo����
		static void SceneEvents(vr::VREvent_t event);
		//�g���b�L���O�����@�R���g���[���Ƃ̎p�����z���o��
		static void TrackingEvents(vr::VREvent_t event);

		//GUI�C�x���g�̏���
		static void GuiEvents();

		//kenkyu�̃v���p�e�B�[���v���p�e�B�V�[�g����ǂݏo��
		static void GetProperty(const std::string& path);

		//���낢��ȏ����l���쐬
		static void InitAnyMembers();

		//�^���w������
		static void _deleted_SolveAngles();//�ڕW�l�܂ŋߕt���Ă���]������ver
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

	};

	
};