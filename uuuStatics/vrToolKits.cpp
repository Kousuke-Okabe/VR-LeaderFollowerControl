#include "vrToolKits.hpp"
using namespace std;
using namespace uuu;
uuu::vrMgr::vrMgr() {
	hmd = NULL;
}
uuu::vrMgr::~vrMgr() {
	if (hmd) {
		vr::VR_Shutdown();
		hmd = NULL;
	}
}

bool uuu::vrMgr::InitVr() {
	vr::EVRInitError initError = vr::VRInitError_None;
	hmd = vr::VR_Init(&initError, vr::VRApplication_Scene);

	if (!hmd)throw uuu::vr_exception("Cannot init uuu VR");

	hmd->GetRecommendedRenderTargetSize(&ww, &wh);

	return initError == vr::VRApplicationError_None;
}

void uuu::vrMgr::Event(const vrEvent& ev) {
	//�C�x���g����
	vr::VREvent_t event;

	for (int i = 0; i < 64; i++) {
		hmd->PollNextEvent(&event, sizeof(event));

		//�C�x���g�����R�[���o�b�N���Ăяo��
		ev(event);
	}
}

vr::TrackedDevicePose_t uuu::vrMgr::GetHmdPos() {
	vr::TrackedDevicePose_t pose;
	hmd->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, &pose, 1);
	return pose;
}