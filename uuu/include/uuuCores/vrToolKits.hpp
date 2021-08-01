#pragma once

#include <uuu.hpp>
#include <openvr.h>
#include <boost/bind.hpp>
#include <functional>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace uuu {

	class vr_exception:public std::runtime_error {
	public:
		vr_exception(std::string arg) :runtime_error(arg) {

		}
	};

	class vrMgr {
	protected:
		glm::mat4 convertHmat4ToGmat4Rot(vr::HmdMatrix34_t arg) {

			glm::mat4 ret;

			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 4; j++)
					ret[i][j] = arg.m[i][j];
			
			return ret;
		}
	public:
		vr::IVRSystem* hmd;
		uint32_t ww, wh;
	public:
		using vrEvent=std::function<void(vr::VREvent_t)>;

		vrMgr();
		~vrMgr();
		bool InitVr();

		void Event(const vrEvent& ev);

		vr::TrackedDevicePose_t GetHmdPos();

		//hmd‚Ìó‘Ô‚ðƒJƒƒ‰‚ÉÝ’è‚·‚é
		void __SetCameraLookAtFromHmd(uuu::cameraPersp& cam) {
			glm::mat4 sex = convertHmat4ToGmat4Rot(this->GetHmdPos().mDeviceToAbsoluteTracking);
			glm::vec4 pos(glm::vec3(sex[3]) * glm::vec3(-1, 1, -1), 1.0);
			glm::vec4 front = sex[2] * glm::vec4(1, -1, 1, 0);
			glm::vec4 up = sex[1] * glm::vec4(-1, 1, -1, 0);

			cam.SetLookAt(pos, front, up);
		}
		void ___SetCameraLookAtFromHmd(uuu::cameraPersp& cam) {
			auto trans = convertHmat4ToGmat4Rot(this->GetHmdPos().mDeviceToAbsoluteTracking);

			glm::vec3 pos = glm::vec4(0, 0, 0, 1) * trans;
			std::swap(pos.y, pos.z);
			glm::vec3 front = glm::vec4(1, 0, 0, 0) * trans;
			std::swap(front.y, front.z);
			glm::vec3 side = glm::vec4(0, 0, 1, 0) * trans;
			std::swap(side.y, side.z);
			glm::vec3 up = glm::vec4(0, 1, 0, 0) * trans;
			std::swap(up.y, up.z);

			system("cls");
			std::cout <<"front\t"<< front.x<<"\t"<< front.y<<"\t"<< front.z<< std::endl;
			std::cout <<"side\t"<< side.x << "\t" << side.y << "\t" << side.z << std::endl;
			std::cout <<"up\t"<< up.x << "\t" << up.y << "\t" << up.z << std::endl;

			cam.SetLookAt(pos, front, up);
		}

		void SetCameraLookAtFromHmd(uuu::cameraPersp& cam) {
			auto trans=this->GetHmdPos().mDeviceToAbsoluteTracking;
			glm::vec3 pos(trans.m[0][3], trans.m[1][3], trans.m[2][3]);
			glm::vec3 front(trans.m[0][2], -trans.m[1][2], trans.m[2][2]);
			glm::vec3 up(-trans.m[0][1], trans.m[1][1], -trans.m[2][1]);

			cam.SetLookAt(pos, front+pos, up);
		}
	};


};