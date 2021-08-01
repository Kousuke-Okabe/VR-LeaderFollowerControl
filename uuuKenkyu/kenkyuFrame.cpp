#include "kenkyuFrame.hpp"

using namespace std;
using namespace uuu::easy::usings;
using namespace kenkyulocal;
using namespace Eigen;

uuu::vrMgr kenkyu::kenkyuVr;
//typename std::vector<uuu::easy::neo3Dmesh> kenkyu::meshs;
typename std::unordered_map<std::string, uuu::game::mesh> kenkyu::gmeshs;
std::unordered_map<std::string,std::shared_ptr<uuu::shaderProgramObjectVertexFragment>> kenkyu::shaders;
uuu::cameraPersp kenkyu::mainCamera, kenkyu::eyeR, kenkyu::eyeL;
std::unique_ptr<uuu::frameBufferOperator> kenkyu::fboR, kenkyu::fboL;
std::unique_ptr<uuu::textureOperator> kenkyu::colR, kenkyu::colL;
std::chrono::system_clock::time_point kenkyu::origin;
kenkyu::_properties kenkyu::properties;
kenkyu::_systemBootFlags kenkyu::systemBootFlags;

kenkyu::_actionWarehouse kenkyu::actionWarehouse;

std::unique_ptr<umeArmTransfer> kenkyu::armMgr;

glm::vec3 kenkyu::hmdPos;
//std::list<boost::thread> kenkyu::serialWriteThreads;
std::array<std::unique_ptr<boost::thread>, 6> kenkyu::serialWriteThreads;
std::unique_ptr<boost::thread> kenkyu::solverThread;
std::unique_ptr<boost::thread> kenkyu::logThread;

kenkyu::posAndQuat kenkyu::beforePosR, kenkyu::beforeposL;
kenkyu::posAndQuat kenkyu::reference;

kenkyulocal::kenkyuArm kenkyu::arm(kenkyuArm::Vector6d::Zero());
std::mutex kenkyu::mutexRefPoint;

bool kenkyu::N_killSover;

boost::optional<std::ofstream> kenkyu::logStream;

std::pair<unsigned int, unsigned int> kenkyu::windowBounds;;

bool kenkyu::continueLoop;

kenkyu::_solverState kenkyu::solverState;

void kenkyu::Draw() {
	//hmd�̕ό`���Ƃ�
	//kenkyu::kenkyuVr.SetCameraLookAtFromHmd(kenkyu::mainCamera);
	//kenkyu::kenkyuVr.SetCameraLookAtFromHmd(kenkyu::eyeR);
	//kenkyu::kenkyuVr.SetCameraLookAtFromHmd(kenkyu::eyeL);
	//kenkyu::mainCamera.SetLookAt(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1));
	
	//�E�B���h�E�̃t���[�������
	kenkyu::DrawVrFrame(kenkyu::mainCamera);
	//GUI���ォ��`��
	kenkyu::DrawGui();

	uuu::app::UpdateForBind();//��ʍX�V

	//VR�̗��ڂ̃t���[���𐶐�����
	if (kenkyu::systemBootFlags.vr) {
		kenkyu::DrawVrFrame(*kenkyu::fboR, kenkyu::eyeR);
		kenkyu::DrawVrFrame(*kenkyu::fboL, kenkyu::eyeL);

		//VR�Ƀf�[�^��]������
		kenkyu::TransVrHmd();
	}
	
}

glm::mat4 kenkyu::TransVrMatToGmat4(const vr::HmdMatrix34_t& gen) {
	glm::mat4 ret;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			ret[i][j] = gen.m[i][j];

	for (int i = 0; i < 4; i++)
		ret[3][i] = (i == 3) ? 1.0 : 0.0;

	//z�������ւ����炢����񂶂�?
	auto corr = glm::scale(glm::vec3(-1, 1, -1));

	return glm::transpose(corr * ret * corr);
}

void kenkyu::DrawGui() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void kenkyu::DrawVrFrame(const uuu::cameraPersp& eye) {

	uuu::app::ClearBuffer(uuu::app::color | uuu::app::depth);//��ʃN���A

	for (auto& s : kenkyu::shaders) {
		s.second->SetUniformCameraPersp(eye);
		s.second->SetUniformValue("lightway", glm::vec4(1, 1, 1, 0));
		s.second->SetUniformValue("modelTransform", glm::identity<glm::mat4>());
	}

	for (auto& m : kenkyu::gmeshs) {
		m.second.Draw();
	}

	return;/*
	for (auto& m : kenkyu::meshs)
		m.DrawElements();*/

}
void kenkyu::DrawVrFrame(uuu::frameBufferOperator& fbo, const uuu::cameraPersp& eye) {

	fbo.Bind();

	kenkyu::DrawVrFrame(eye);

	fbo.Unbind();
}

void kenkyu::TransVrHmd() {
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	vr::Texture_t leftEyeTexture = { (void*)colL->GetTexId(), vr::TextureType_OpenGL, vr::ColorSpace_Auto };
	vr::Texture_t rightEyeTexture = { (void*)colR->GetTexId(), vr::TextureType_OpenGL, vr::ColorSpace_Auto };

	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
}


void kenkyu::BootUuuSetForKekyu() {

	//�J�n���Ԃ��Ƃ�
	kenkyu::origin = std::chrono::system_clock::now();
	kenkyu::log("booting...");

	kenkyu::InitAnyMembers();
	kenkyu::log("All flags were inited");

	kenkyu::GetProperty("setup.xml");
	kenkyu::log("All properties were loaded");

	//�^�C���X�^���v������
	std::stringstream ss;
	auto timeStump = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm timeFormatStruct;
	localtime_s(&timeFormatStruct, &timeStump);
	ss << std::put_time(&timeFormatStruct, "%Y%m%d_%H%M%S");
	kenkyu::log(ss.str(), kenkyu::logSaved);

	//�V���A���|�[�g�ɐڑ�
	try {
		kenkyu::armMgr.reset(new umeArmTransfer(properties.serialPort));
		kenkyu::log("Serial port was connected");
		kenkyu::systemBootFlags.serial = true;
	}
	catch (std::exception& ex) {
		log("�V���A���|�[�g�ւ̃A�N�Z�X�ŃG���[���������܂����B���Y�����𖳌������đ��s���܂���?", kenkyu::logError);
		if (GetYorN());
		else throw ex;
	}

	//VR��ڑ�
	try {
		kenkyuVr.InitVr();
		kenkyu::systemBootFlags.vr = true;
	}
	catch (uuu::vr_exception& ex) {
		log("VR�V�X�e���ւ̐ڑ��ŃG���[���������܂����B���Y�����𖳌������đ��s���܂���?", kenkyu::logError);
		if (GetYorN());
		else throw ex;
	}

	log("VR system was started");

	//UUU�̃E�B���h�E��VR�̉�ʃT�C�Y�̔����ɍ��킹�č��
	if (kenkyu::systemBootFlags.vr) {
		kenkyu::windowBounds.first = kenkyuVr.ww / properties.windowSizeRatio;
		kenkyu::windowBounds.second = kenkyuVr.wh / properties.windowSizeRatio;
	}
	else {
		kenkyu::windowBounds.first = 640;
		kenkyu::windowBounds.second = 480;
	}
	
	uuu::app::windowProp prop(kenkyu::windowBounds.first, kenkyu::windowBounds.second, "Ume's kenkyu");
	prop.enableBorder = false;
	uuu::easy::Init(prop, uuu::neoVertexOperators::positions | uuu::neoVertexOperators::norms | uuu::neoVertexOperators::indices | uuu::neoVertexOperators::texCoords0);
	
		
	//easys���N��
	uuu::easy::InitNeo3Dmesh();

	//GUI�V�X�e�����N��
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();//�X�^�C���ݒ�
	ImGui_ImplGlfw_InitForOpenGL(uuu::app::GetBindWindow(), true);
	ImGui_ImplOpenGL3_Init(properties.glslVersion.c_str());
	log("GUI system was started");

	log("uuu subsystem was started");

	//�g���N������
	if (kenkyu::systemBootFlags.serial)kenkyu::armMgr->Torque(255, 1);

	//�\���o�[���N��
	if (kenkyu::systemBootFlags.serial)solverThread.reset(new boost::thread(kenkyu::SolveAngles));

	return;
}

void kenkyu::InitGraphics() {

	//�K���ɏ���������
	kenkyu::shaders.clear();
	kenkyu::gmeshs.clear();

	//���n�V�F�[�_��p��
	kenkyu::shaders["norm"] = std::make_shared<uuu::shaderProgramObjectVertexFragment>(uuu::shaderProgramObjectVertexFragment(assets(shaders/norm)));
	kenkyu::shaders["virtualWindow"] = std::make_shared<uuu::shaderProgramObjectVertexFragment>(uuu::shaderProgramObjectVertexFragment(assets(shaders/virtualWindow)));
	kenkyu::shaders["rainbow"] = std::make_shared<uuu::shaderProgramObjectVertexFragment>(uuu::shaderProgramObjectVertexFragment(assets(shaders/rainbow)));

	//�X�e�[�W�̃��b�V����ǉ�
	//kenkyu::gmeshs["monkey"]=uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "monkey-mesh", glm::translate(glm::identity<glm::mat4>(), gvec3(0, 0, 5)));
	kenkyu::gmeshs["rightHand"] = uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "rightHand-mesh", glm::identity<glm::mat4>(),true);
	kenkyu::gmeshs["rightPointer"] = uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "rightPointer-mesh", glm::identity<glm::mat4>(),false);
	kenkyu::gmeshs["rightGoo"] = uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "rightGoo-mesh", glm::identity<glm::mat4>(),true);

	kenkyu::gmeshs["leftHand"] = uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "leftHand-mesh", glm::identity<glm::mat4>(),true);
	kenkyu::gmeshs["leftPointer"] = uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "leftPointer-mesh", glm::identity<glm::mat4>(),false);
	kenkyu::gmeshs["leftGoo"] = uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "leftGoo-mesh", glm::identity<glm::mat4>(),true);
	kenkyu::gmeshs["cat"] = uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "cat-mesh", kenkyu::reference.toMat());
	//kenkyu::gmeshs["room"] = uuu::game::mesh(shaders["rainbow"], assets(rooms.dae), "room-mesh", glm::identity<glm::mat4>());

	kenkyu::gmeshs["karixplane"] = uuu::game::mesh(shaders["virtualWindow"], assets(plane.dae), "Plane-mesh", glm::translate(glm::identity<glm::mat4>(),glm::vec3(0,0.7,-1)));

	log("assets was loaded");

	//���C���J�������쐬
	mainCamera.SetPersp(properties.cameraNear, properties.cameraFar, M_PI * (properties.fov / 180.0), kenkyu::windowBounds.first / (double)kenkyu::windowBounds.second);
	if (kenkyu::systemBootFlags.vr);
	else
		mainCamera.SetLookAt(glm::vec3(0, 1.6, 0), glm::vec3(0, 0, -5.0), glm::vec3(0, 1.0, 0));

	//vr�̂��߂̃O���t�B�b�N�\���̍쐬
	if (kenkyu::systemBootFlags.vr) {
		//VR�J�������쐬
		eyeR.SetPersp(properties.cameraNear, properties.cameraFar, M_PI * (properties.fov / 180.0), kenkyuVr.ww / (float)kenkyuVr.wh);
		eyeL.SetPersp(properties.cameraNear, properties.cameraFar, M_PI * (properties.fov / 180.0), kenkyuVr.ww / (float)kenkyuVr.wh);

		//VR�̃t���[���o�b�t�@���쐬

		kenkyu::fboR.reset(new uuu::frameBufferOperator());
		kenkyu::fboL.reset(new uuu::frameBufferOperator());

		kenkyu::colR.reset(new uuu::textureOperator());
		kenkyu::colL.reset(new uuu::textureOperator());
		uuu::textureOperator depr, depl;
		colR->CreateManual(kenkyuVr.ww, kenkyuVr.wh, uuu::textureFormat::rgba16);
		depr.CreateManual(kenkyuVr.ww, kenkyuVr.wh, uuu::textureFormat::depth16);
		fboR->AttachTextureOperator(fboR->color0, *colR);
		fboR->AttachTextureOperator(fboR->depth, depr);

		colL->CreateManual(kenkyuVr.ww, kenkyuVr.wh, uuu::textureFormat::rgba16);
		depl.CreateManual(kenkyuVr.ww, kenkyuVr.wh, uuu::textureFormat::depth16);
		fboL->AttachTextureOperator(fboL->color0, *colL);
		fboL->AttachTextureOperator(fboL->depth, depl);

		fboR->Unbind();
		fboL->Unbind();
	}

	log("GPU resources was stored");
}

void kenkyu::Terminate() {

	N_killSover = false;

	if (solverThread)
		if (solverThread->joinable())
			solverThread->join();

	if (logThread)
		if (logThread->joinable())
			logThread->join();

	kenkyu::logStream.get().close();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	log("system was terminated");
}

void kenkyu::SaveALog(std::string s) {
	logStream.get() << s << "\n";
}

void kenkyu::log(const std::string& str, logState st) {
	switch (st) {
	case logState::logInfo:
		cout<<"\033[0mINFO";
		break;
	case logState::logWarning:
		cout << "\033[33;4mWARNING";
		break;
	case logState::logError:
		cout << "\033[31;4mERROR";
		break;
	case logState::logNote:
		cout << "\033[32;4mNOTE";
		break;
	case kenkyu::logDebug:
		cout << "\033[35mDEBUG";
		break;
	case kenkyu::logSaved:
		if (logThread)
			if (logThread->joinable())//�X���b�h����������
				logThread->join();

		if (!kenkyu::logStream)throw std::runtime_error("���O�t�@�C���������ł��Ă��܂���B�Ăяo�������m�F���Ă��������B");

		logThread.reset(new boost::thread(kenkyu::SaveALog, str));
		
		break;
	default:
		throw std::logic_error("�s���ȃ��O���"+logInfo);
	}

	if (st != kenkyu::logSaved) {
		auto now = std::chrono::system_clock::now();
		auto count = std::chrono::duration_cast<std::chrono::milliseconds>(now - kenkyu::origin).count();
		std::cout << "[" << count << "]:\t" << str << "\033[0m" << std::endl;
	}

	return;
}
void kenkyu::log(const std::string name, glm::vec3 v3, logState st) {
	log(name + ":" + std::to_string(v3.x) + "\t" + std::to_string(v3.y) + "\t" + std::to_string(v3.z), st);
}
void kenkyu::log(const std::string name, glm::quat v4, logState st) {
	log(name + ":" + std::to_string(v4.x) + "\t" + std::to_string(v4.y) + "\t" + std::to_string(v4.z) + "\t" + std::to_string(v4.w), st);
}

void kenkyu::Event() {
	//VR�֌W�̃C�x���g
	if (kenkyu::systemBootFlags.vr)kenkyu::kenkyuVr.Event(kenkyu::CallbackVrEvents);

	//GUI�֌W�̃C�x���g
	kenkyu::GuiEvents();
	
	
	//�I����Ă���X���b�h������ΎE��
	/*if (kenkyu::serialWriteThread)
		if (kenkyu::serialWriteThread->try_join_for(boost::chrono::milliseconds(20))) {
			kenkyu::serialWriteThread.release();
		}*/

	/*if (kenkyu::serialWriteThreads.size() > 0)
		for (auto b = kenkyu::serialWriteThreads.begin(); b != kenkyu::serialWriteThreads.end();b++) {
			if (b->try_join_for(boost::chrono::milliseconds(10)))
				kenkyu::serialWriteThreads.erase(b);
			log("�c��" + to_string(kenkyu::serialWriteThreads.size()));
		}*/
	
}

void kenkyu::CallbackVrEvents(vr::VREvent_t event) {

	kenkyu::GeneralEvents(event);
	kenkyu::SceneEvents(event);
	kenkyu::TrackingEvents(event);

}

void kenkyu::SceneEvents(vr::VREvent_t event) {

	enum { invalid = 0, right = 1, left = 2 }hand = invalid;//�ǂ̎��?
	{
		//�f�o�C�X���r�Ȃ�
		auto dev = kenkyuVr.hmd->GetTrackedDeviceClass(event.trackedDeviceIndex);
		if (dev == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
			auto role = kenkyuVr.hmd->GetControllerRoleForTrackedDeviceIndex(event.trackedDeviceIndex);

			//���[���Ō�������
			switch (role) {
			case vr::TrackedControllerRole_RightHand://�E��
				hand = right;
				break;
			case vr::TrackedControllerRole_LeftHand://����
				hand = left;
				break;
			}
		}
	}
	enum { other = 0, grip = 1, trigger = 2 }action = other;//�ǂ̃A�N�V������
	{
		if (event.data.controller.button == vr::k_EButton_SteamVR_Trigger)
			action = trigger;
		else if (event.data.controller.button == vr::k_EButton_Grip)
			action = grip;
	}
	enum { none = 0, press = 1, unpress = 2 } edge = none;//�ǂ̃G�b�W�ōs���Ă邩
	switch (event.eventType) {
	case vr::VREvent_ButtonPress:
		edge = press;
		break;
	case vr::VREvent_ButtonUnpress:
		edge = unpress;
		break;
	}

	//�����炪�L���Ȃ�A�N�V�����������s��
	if (hand && action && edge) {
		//log("scene event " + std::to_string(hand) + " " + std::to_string(action) + " " + std::to_string(edge), logDebug);
		//�g���K�[���̓t���O�𗧂Ă�
		auto& nowtype = (hand == right) ? actionWarehouse.rhandtype : actionWarehouse.lhandtype;

		if (action == trigger && edge == press)nowtype = 2;
		else if (action == grip && edge == press)nowtype = 0;
		else if (edge == unpress)nowtype = 1;

		//�A�N�V�������������Ƃ��\����Ԃ�؂�ւ���
		if (hand == right) {
			kenkyu::gmeshs["rightHand"].skipDraw = !(actionWarehouse.rhandtype == 0);
			kenkyu::gmeshs["rightPointer"].skipDraw = !(actionWarehouse.rhandtype == 1);
			kenkyu::gmeshs["rightGoo"].skipDraw = !(actionWarehouse.rhandtype == 2);
		}
		else if (hand == left) {
			kenkyu::gmeshs["leftHand"].skipDraw = !(actionWarehouse.lhandtype == 0);
			kenkyu::gmeshs["leftPointer"].skipDraw = !(actionWarehouse.lhandtype == 1);
			kenkyu::gmeshs["leftGoo"].skipDraw = !(actionWarehouse.lhandtype == 2);
		}

	}
}
void kenkyu::GeneralEvents(vr::VREvent_t event) {
	if (event.eventType == vr::VREvent_TrackedDeviceActivated) {
		log("device " + std::to_string(event.trackedDeviceIndex) + "attached");
	}
}
void kenkyu::TrackingEvents(vr::VREvent_t event) {
	//�f�o�C�X�̍ő吔���o��
	for (size_t id = 0; id < vr::k_unMaxTrackedDeviceCount; id++) {

		//���ڑ���id�Ȃ�I���
		if (!kenkyu::kenkyuVr.hmd->IsTrackedDeviceConnected(id))
			continue;

		auto dev = kenkyu::kenkyuVr.hmd->GetTrackedDeviceClass(id);

		vr::TrackedDevicePose_t devicePose;

		//�R���g���[���Ȃ�
		if (dev == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
			//���[��������o��
			auto role = kenkyuVr.hmd->GetControllerRoleForTrackedDeviceIndex(id);

			//�R���g���[���p���o����
			vr::VRControllerState_t controllerState;
			kenkyu::kenkyuVr.hmd->GetControllerStateWithPose(vr::TrackingUniverseStanding, id, &controllerState, sizeof(controllerState), &devicePose);

			
			auto trans = kenkyu::TransVrMatToGmat4(devicePose.mDeviceToAbsoluteTracking);
			glm::vec3 pos = trans * gvec4(0, 0, 0, 1);
			glm::quat q(trans);

			//log("quat",q);

			//�E��or����Ȃ�ʒu�Ǐ]������
			if (role == vr::TrackedControllerRole_RightHand) {
				kenkyu::gmeshs["rightHand"].SetTransform(trans);
				kenkyu::gmeshs["rightPointer"].SetTransform(trans);
				kenkyu::gmeshs["rightGoo"].SetTransform(trans);

				//�A�N�V����������Ȃ瑗�M
				//�O�[�͑��΃��[�h
				if (actionWarehouse.rhandtype == 2) {
					//���ꂼ��̍����x�N�g�����Ƃ�
					auto posdist = pos - beforePosR.pos;
					auto quatdist = glm::inverse(beforePosR.quat) * q;

					//�ڕW�p����ϊ�
					{
						std::lock_guard<std::mutex> lock(mutexRefPoint);
						kenkyu::reference.pos += posdist;
						kenkyu::reference.quat *= quatdist;
					}

					//���R���f���ʒu���X�V
					kenkyu::gmeshs["cat"].SetTransform(reference.toMat());
				}
				//�p�[�̓A�u�\�����[�g���[�h
				else if (actionWarehouse.rhandtype == 0) {
					//���ꂼ��̍����x�N�g�����Ƃ�
					auto posdist = pos - beforePosR.pos;

					//�ڕW�p����ϊ�
					{
						std::lock_guard<std::mutex> lock(mutexRefPoint);
						kenkyu::reference.pos += posdist;
						kenkyu::reference.quat = q;
					}

					//���R���f���ʒu���X�V
					kenkyu::gmeshs["cat"].SetTransform(reference.toMat());
				}

				//���W���L�^
				beforePosR.pos = pos;
				beforePosR.quat = q;

				//����ɍ��W�𑗐M
				//if (!kenkyu::serialWriteThread)
					//kenkyu::serialWriteThread.reset(new boost::thread(&umeArmTransfer::Position, kenkyu::armMgr.get(), 1919, pos, 1000));
				/*if (serialWriteThreads.size() < 1)
					kenkyu::serialWriteThreads.push_back(boost::thread(&umeArmTransfer::Position, kenkyu::armMgr.get(), 1919, pos, 1000));
					*/
			}

			if (role == vr::TrackedControllerRole_LeftHand) {
				kenkyu::gmeshs["leftHand"].SetTransform(trans);
				kenkyu::gmeshs["leftPointer"].SetTransform(trans);
				kenkyu::gmeshs["leftGoo"].SetTransform(trans);
			}
		}
		//hmd�Ȃ�
		else if (dev == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD) {
			kenkyu::kenkyuVr.hmd->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, id, &devicePose, 1);

			auto trans = kenkyu::TransVrMatToGmat4(devicePose.mDeviceToAbsoluteTracking);
			gvec3 pos =  trans* gvec4(0, 0, 0, 1);
			gvec3 front = trans* gvec4(0, 0, 1, 0);
			gvec3 up = trans* gvec4(0, 1, 0, 0);

			gvec3 rpos = trans*gvec4(-properties.pd / 2.0, 0, 0, 1);
			gvec3 lpos = trans*gvec4(+properties.pd / 2.0, 0, 0, 1);

			kenkyu::mainCamera.SetLookAt(pos, pos+front, up);
			kenkyu::eyeR.SetLookAt(rpos, rpos+front, up);
			kenkyu::eyeL.SetLookAt(lpos, lpos + front, up);

			hmdPos = pos;
		}
		else continue;//���ꂢ�����͑ł��؂�
	}

	//�ڕW�ʒu���X�V
	/* {
		lock_guard<std::mutex> lock(mutexArm);//�A�[�����S��
		arm.SetRef(kenkyuArm::Vector7d(reference.pos.x, reference.pos.y, reference.pos.z, reference.quat.x, reference.quat.y, reference.quat.z, reference.quat.w));
	}*/
	 
	 
	//�r�̖ڕW�p���𑗐M����
	/*if (!kenkyu::serialWriteThread)
		kenkyu::serialWriteThread.reset(new boost::thread(&umeArmTransfer::PositionQuat, kenkyu::armMgr.get(), 1919, reference.pos, reference.quat, 1000));
	*/
}

void kenkyu::GuiEvents() {

	//�t���[���J�n
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//window�X�^�C��
	ImGui::SetNextWindowPos(ImVec2(windowBounds.first * 0.6, 0));
	ImGui::SetNextWindowSize(ImVec2(windowBounds.first * 0.4, windowBounds.second*0.5));

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.3f, 0.1f, 1.0f));
	ImGui::Begin("Arm health");
	{
		ImGui::Text("reference pos");
		ImGui::Text((" " + to_string(kenkyu::reference.pos.x) + "," + to_string(kenkyu::reference.pos.y) + "," + to_string(kenkyu::reference.pos.z)).c_str());
		ImGui::Text("reference quat");
		ImGui::Text((" " + to_string(kenkyu::reference.quat.x) + "," + to_string(kenkyu::reference.quat.y) + "," + to_string(kenkyu::reference.quat.z) + "," + to_string(kenkyu::reference.quat.w)).c_str());
		ImGui::Text("moter positions");
		if (kenkyu::systemBootFlags.serial) {
			auto sendAngle = ToDegreeFrom10TimesDegree<int, 6>(ToHutabaDegreeFromRadiansVec(CorrectAngleVecAreaForHutaba<double, 6>(CorrectAngleCenteredVec<double, 6>(CorrectAngleVec<double, 6>(arm.GetNowQ())))));
			for (size_t m = 1; m <= 6; m++)
				ImGui::Text((" m" + to_string(m) + ": " + to_string(sendAngle(m - 1)) + "deg").c_str());
		}
		else ImGui::Text(" Not avalable");

		ImGui::Text("solver state");
		ImGui::Text((std::string(" ") + kenkyu::solverState.operator std::string()).c_str());

		//�X�V���ꂽ�Ȃ�
		kenkyu::solverState.DecrementCount();
		//cout << kenkyu::solverState.Get() << endl;
		if (kenkyu::solverState.Get() != 0) {
			auto rectBegin = ImGui::GetCursorScreenPos();
			auto updateShowerSize = std::max<double>(kenkyu::windowBounds.first / 20.0, kenkyu::windowBounds.second / 20.0);
			ImGui::GetWindowDrawList()->AddRectFilled(rectBegin, AddImVec2s(rectBegin, ImVec2(updateShowerSize, updateShowerSize)), ImGui::GetColorU32(ImVec4(0.3, 1, 0.3, 1)),updateShowerSize/10.0);
		}
		else {
			auto rectBegin = ImGui::GetCursorScreenPos();
			auto updateShowerSize = std::max<double>(kenkyu::windowBounds.first / 20.0, kenkyu::windowBounds.second / 20.0);
			ImGui::GetWindowDrawList()->AddRectFilled(rectBegin, AddImVec2s(rectBegin, ImVec2(updateShowerSize, updateShowerSize)), ImGui::GetColorU32(ImVec4(0.3, 0.3, 0.3, 1)),updateShowerSize/10.0);
		}
	}
	ImGui::End();
	ImGui::PopStyleColor(2);

	ImGui::SetNextWindowPos(ImVec2(windowBounds.first * 0.6, windowBounds.second * 0.5));
	ImGui::SetNextWindowSize(ImVec2(windowBounds.first * 0.4, windowBounds.second*0.5));

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.7f, 0.0f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.3f, 0.0, 0.1f, 1.0f));
	ImGui::Begin("Controls");
	{
		if (kenkyu::systemBootFlags.serial||true) {
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, kenkyu::systemBootFlags.serial ? ImVec4(0.6f, 0.1f, 0.1f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			if (ImGui::Button("Emergency", ImVec2(windowBounds.first * 0.4 * 1.0, windowBounds.second * 0.5 * 0.25))&&kenkyu::systemBootFlags.serial)
				kenkyu::armMgr->Torque(255, 0);//�ً}��~�{�^��
			ImGui::PopStyleColor(2);

			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 7.0f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, kenkyu::systemBootFlags.serial ? ImVec4(0.0f, 0.3f, 0.1f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			auto retorquePos = ImGui::GetCursorPos();
			if (ImGui::Button("Retorque", ImVec2(windowBounds.first * 0.4 * 0.48, windowBounds.second * 0.5 * 0.25)) && kenkyu::systemBootFlags.serial)
				kenkyu::armMgr->Torque(255, 1);//�ăg���N����{�^��
			ImGui::PopStyleColor(2);
			
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.7f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, kenkyu::systemBootFlags.serial ? ImVec4(0.0f, 0.0f, 0.2f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::SetCursorPos(ImVec2(windowBounds.first * 0.4 * 0.5 + retorquePos.x, 0 + retorquePos.y));
			if (ImGui::Button("Zero", ImVec2(windowBounds.first * 0.4 * 0.48, windowBounds.second * 0.5 * 0.25)) && kenkyu::systemBootFlags.serial) {
				constexpr double l1 = 0.28, l2 = 0.35, l3 = 0.0;
				std::lock_guard<std::mutex> lock(mutexRefPoint);
				kenkyu::reference.pos = glm::vec3(0, -l1 - l2 - l3, 0);
				kenkyu::reference.quat = glm::quat(1, 0, 0, 0);

				kenkyu::armMgr->Move(255, 0, 500);
			}
			ImGui::PopStyleColor(2);

			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.0f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.5f, 1.0f));
			if (ImGui::Button("Exit", ImVec2(windowBounds.first * 0.4 * 0.48, windowBounds.second * 0.5 * 0.25)))
				kenkyu::continueLoop = false;//�A�v���I���{�^��
			ImGui::PopStyleColor(2);
		}
		else ImGui::Text("Not available.");
	}
	ImGui::End();
	ImGui::PopStyleColor(2);

	return;
}

void kenkyu::GetProperty(const std::string& path) {
	using namespace boost::property_tree;

	ptree pt;
	read_xml(path, pt);

	//
	if (boost::optional<double> neardt = pt.get_optional<double>("kenkyu.setup.persp.<xmlattr>.near")) {
		properties.cameraNear = neardt.get();
		kenkyu::log("property \"near\" = " + std::to_string(properties.cameraNear),logDebug);
	}
	else throw std::runtime_error("not found \"near\" property");

	if (boost::optional<double> fardt = pt.get_optional<double>("kenkyu.setup.persp.<xmlattr>.far")) {
		properties.cameraFar = fardt.get();
		kenkyu::log("property \"far\" = " + std::to_string(properties.cameraFar), logDebug);
	}
	else throw std::runtime_error("not found \"far\" property");

	if (boost::optional<double> fovdt = pt.get_optional<double>("kenkyu.setup.persp.<xmlattr>.fov")) {
		properties.fov = fovdt.get();
		kenkyu::log("property \"fov\" = " + std::to_string(properties.fov), logDebug);
	}
	else throw std::runtime_error("not found \"fov\" property");

	if (boost::optional<double> pddt = pt.get_optional<double>("kenkyu.setup.vr.<xmlattr>.pd")) {
		properties.pd = pddt.get();
		kenkyu::log("property \"pd\" = " + std::to_string(properties.pd), logDebug);
	}
	else throw std::runtime_error("not found \"pd\" property");

	if (boost::optional<std::string> assetdt = pt.get_optional<std::string>("kenkyu.setup.system.<xmlattr>.asset")) {
		properties.assetpath = assetdt.get();
		kenkyu::log("property \"asset\" = " + properties.assetpath, logDebug);
	}
	else throw std::runtime_error("not found \"asset\" property");

	if (boost::optional<double> windowsizeratiodt = pt.get_optional<double>("kenkyu.setup.system.<xmlattr>.windowSizeRatio")) {
		properties.windowSizeRatio = windowsizeratiodt.get();
		kenkyu::log("property \"window size ratio\" = " + std::to_string(properties.windowSizeRatio), logDebug);
	}
	else throw std::runtime_error("not found \"windowSizeRatio\" property");

	if (boost::optional<std::string> serialPortdt = pt.get_optional<std::string>("kenkyu.setup.system.<xmlattr>.serialPort")) {
		properties.serialPort = serialPortdt.get();
		kenkyu::log("property \"serial port name\" = " + properties.serialPort, logDebug);
	}
	else throw std::runtime_error("not found \"serialPort\" property");

	if (boost::optional<bool> autoSkipdt = pt.get_optional<bool>("kenkyu.setup.system.<xmlattr>.autoSkip")) {
		properties.autoSkip = autoSkipdt.get();
		kenkyu::log("property \"auto skip flag\" = " + properties.autoSkip, logDebug);

		if (properties.autoSkip)
			kenkyu::log("�I������AutoSkip���L���ɂȂ��Ă��܂��B�ꕔ�̋@�\������ɓ����Ȃ��ꍇ������܂��B", kenkyu::logWarning);
	}
	else {
		properties.autoSkip = false;
		log("not found \"serialPort\" property. default value = false", logDebug);
	}

	if (boost::optional<std::string> logpathdt = pt.get_optional<std::string>("kenkyu.setup.system.<xmlattr>.logfile")) {
		properties.logpath = logpathdt.get();
		kenkyu::log("property \"logfile path\" = " + properties.logpath, logDebug);

		kenkyu::logStream = std::ofstream(properties.logpath);
		//kenkyu::logStream.get() << "hel";
	}
	else throw std::runtime_error("not found \"logfile path\" property");

	if (boost::optional<std::string> glslVersiondt = pt.get_optional<std::string>("kenkyu.setup.system.<xmlattr>.glslVersion")) {
		properties.glslVersion = glslVersiondt.get();
		kenkyu::log("property \"glsl version\" = " + properties.glslVersion, logDebug);
	}
	else throw std::runtime_error("not found \"glsl version\" property");

	if (boost::optional<double> baseVdt = pt.get_optional<double>("kenkyu.setup.arm.<xmlattr>.baseVelocity")) {
		properties.baseVelocity = baseVdt.get();
		kenkyu::log("property \"base velocity\" = " + std::to_string(properties.baseVelocity), logDebug);
	}
	else throw std::runtime_error("not found \"base velocity\" property");

	//���[�^�[�̐�����
	for (size_t m = 1; m <= 6; m++) {
		std::string pathname = "kenkyu.setup.arm.m" + to_string(m) + ".<xmlattr>";

		//�͂₳
		if (boost::optional<double> coeffdt = pt.get_optional<double>(pathname+".velocity")) {
			properties.armVelocitycoefficients.at(m - 1) = coeffdt.get();
			kenkyu::log("property \"m" + to_string(m) + " velocity ratio\"=" + std::to_string(properties.armVelocitycoefficients.at(m - 1)), logDebug);
		}
		else {
			properties.armVelocitycoefficients.at(m - 1) = 1.0;
			kenkyu::log("property \"m" + to_string(m) + " velocity ratio\" is not defined.default value=1.0", logDebug);
		}
		//�ő�p�x
		if (boost::optional<double> maxdt = pt.get_optional<double>(pathname + ".max")) {
			properties.armAngleArea.at(m - 1).first = maxdt.get();
			kenkyu::log("property \"m" + to_string(m) + " max angle\"=" + std::to_string(properties.armAngleArea.at(m - 1).first), logDebug);
		}
		else {
			properties.armAngleArea.at(m - 1).first = +160.0;
			kenkyu::log("property \"m" + to_string(m) + " max angle\" is not defined.default value=160.0", logDebug);
		}
		//�ŏ��p�x
		if (boost::optional<double> mindt = pt.get_optional<double>(pathname + ".min")) {
			properties.armAngleArea.at(m - 1).second = mindt.get();
			kenkyu::log("property \"m" + to_string(m) + " min angle\"=" + std::to_string(properties.armAngleArea.at(m - 1).second), logDebug);
		}
		else {
			properties.armAngleArea.at(m - 1).second = -160.0;
			kenkyu::log("property \"m" + to_string(m) + " min angle\" is not defined.default value=-160.0", logDebug);
		}


	}


	return;
}

void kenkyu::InitAnyMembers() {
	kenkyu::actionWarehouse.rhandtype = 1;
	kenkyu::actionWarehouse.lhandtype = 1;

	for (int th = 0; th < 6; th++)
		kenkyu::serialWriteThreads.at(th).release();

	kenkyu::solverThread.release();
	kenkyu::logThread.release();

	//�A�[���̏����p���͂܂������L�΂������
	constexpr double l1 = 0.28, l2 = 0.35, l3 = 0.0;
	{
		std::lock_guard<std::mutex> lock(mutexRefPoint);
		kenkyu::reference.pos = glm::vec3(0, -l1 - l2 - l3, 0);
		kenkyu::reference.quat = glm::quat(1, 0, 0, 0);
	}

	//before��������
	kenkyu::beforePosR = kenkyu::reference;
	kenkyu::beforeposL = kenkyu::reference;

	kenkyu::N_killSover = true;//!false �����true

	kenkyu::continueLoop = true;
}

glm::mat4 kenkyu::posAndQuat::toMat() {
	return glm::translate(this->pos) * this->quat.operator glm::mat<4, 4, glm::f32, glm::packed_highp>();
}

const Vector3d x(1, 0, 0);
const Vector3d y(0, 1, 0);
const Vector3d z(0, 0, 1);

template<typename T>T signNot0(const T& a) {
	return (a > 0.0) ? 1.0 : -1.0;
}

kenkyulocal::kenkyuArm::Vector7d kenkyulocal::kenkyuArm::fjikken(const kenkyulocal::kenkyuArm::Vector6d& q) {

	//�������u�ɍ��킹���A�[��()
	constexpr double l1 = 0.28, l2 = 0.35, l3 = 0.0;
	Affine3d trans = AngleAxisd(q(0), y) * AngleAxisd(q(1), z) * Translation<double, 3>(0, -l1, 0) * AngleAxisd(q(2), z) * Translation<double, 3>(0, -l2, 0) * AngleAxisd(q(3), y) * AngleAxisd(q(4), z) * Translation<double, 3>(0, -l3, 0) * AngleAxisd(q(5), y);

	//��������p���ƍ��W�𔲂��o�� �p���̕\����ς��Ă݂�
	Eigen::Vector3d pos(trans.translation());
	//�p�������Z����`�ł���悤�ɂ���킷�@��]����z+�̋��ʂ�z=0���ʂ�y>0�Ő��K������
	Eigen::Quaterniond quat(trans.rotation());
	/*if (quat.z() < 0.0)quat = Quaterniond(-quat.w(), -quat.x(), -quat.y(), -quat.x());
	else if (quat.z() == 0.0 && quat.y() < 0.0)quat = Quaterniond(-quat.w(), -quat.x(), -quat.y(), -quat.x());
	else if (quat.z() == 0.0 && quat.y() == 0.0 && quat.x() < 0.0)quat = Quaterniond(-quat.w(), -quat.x(), -quat.y(), -quat.x());
	else if (quat.z() == 0.0 && quat.y() == 0.0 && quat.x() == 0.0)quat = Quaterniond(1, 0, 0, 0);*/

	return Eigen::Matrix<double, 7, 1>(pos.x(), pos.y(), pos.z(), quat.x(), quat.y(), quat.z(), quat.w());
}


//�p�����߂����Ƃ�\���]��
bool CheckNearRefToNowArmPosAndQuat(const std::pair<double, double>& pq) {
	return pq.first < 0.01 && abs<double>(pq.second) > cos(M_PI * (5.0 / 180.0));
}
//���Z�q�@�ʒu�p�����ߕt���Ă��邱�Ƃ�\��


void kenkyu::_deleted_SolveAngles() {
	/*
	const kenkyulocal::kenkyuArm::Vector7d kvec(1, 1, 1, 1, 1, 1, 1);

	while (kenkyu::N_killSover) {

		//���t�@�����X��ݒ�
		{
			std::lock_guard<std::mutex> lock(mutexRefPoint);
			arm.SetRef(kenkyuArm::Vector7d(reference.pos.x, reference.pos.y, reference.pos.z, reference.quat.x, reference.quat.y, reference.quat.z, reference.quat.w));
		}

		//�p�����ڕW���牓����Ή�͂����{����
		if (!CheckNearRefToNowArmPosAndQuat(arm.GetDistPosAndQuat())) {
			kenkyulocal::kenkyuArm::Vector6d next, hold;//���̊p�x�ƍ��̊p�x

			hold = arm.GetNowQ();//��͑O�̊p�x���擾

			//���
			double beforeRenge = std::numeric_limits<double>::infinity();
			for (size_t i = 0; i < 100; i++) {
				next = arm.GetNextQ(kvec);

				//�X�e�b�v��ǂ����тɋߕt���Ȃ��Ă͂Ȃ�Ȃ��B
				auto newRenge = arm.GetDistNowtoRefSquared();

				//�ł��؂�̕]���@�ڕW�Ƃ̋������ߕt���Ȃ����
				if (newRenge >= beforeRenge) {
					beforeRenge = newRenge;
					kenkyu::solverState.Set(kenkyu::_solverState::impossible);
					break;
				}
				else beforeRenge = newRenge;

				//�ł��؂�̕]�� �\���ɖڕW�ɋ߂Â�����
				if (CheckNearRefToNowArmPosAndQuat(arm.GetDistPosAndQuat())) {
					kenkyu::solverState.Set(kenkyu::_solverState::solved);
					break;
				}

				//�ߕt���Ă��邯�ǋ��e��܂ł����Ȃ�����
				if (i == 99) {
					kenkyu::solverState.Set(kenkyu::_solverState::closer);
					break;
				}
			}

			//�f�[�^�𑗐M�@�ł������Ȃ�
			auto state = kenkyu::solverState.Get();
			if (state == kenkyu::_solverState::solved || state == kenkyu::_solverState::closer) {

				//��̒l�̒l��𐮂���@�l��𒆉��Ɋ񂹂� +-180->�l��𐧌����� +-150�x
				auto correctedHold = CorrectAngleVecAreaForHutaba<double, 6>(CorrectAngleCenteredVec<double, 6>(CorrectAngleVec(hold)));
				auto correctedNext = CorrectAngleVecAreaForHutaba<double, 6>(CorrectAngleCenteredVec<double, 6>(CorrectAngleVec(next)));

				//��͑O�p�x�ƌ��݊p�x����Ίp�ɔ�Ⴕ�����Ԃ��v�Z����
				auto distAngle = correctedNext - correctedHold;
				array<double, 6> angleWait;
				for (size_t i = 0; i < 6; i++)
					angleWait.at(i) = fabs(distAngle(i)) * (properties.baseVelocity / M_PI) * properties.armVelocitycoefficients.at(i);
				auto maxWait = std::max_element(angleWait.begin(), angleWait.end());//�ҋ@���Ԃ̍ő���o��

				//���肵�Ă�����p�x�𑗐M����
				for (size_t m = 1; m <= 6; m++)
					kenkyu::serialWriteThreads.at(m - 1).reset(new boost::thread(&umeArmTransfer::Move, kenkyu::armMgr.get(), m, ToHutabaDegreeFromRadians(correctedNext(m - 1)), angleWait.at(m - 1)));

				//�X�V�ɃJ�E���g���Z�b�g����
				kenkyu::solverState.SetUpdate(3);

				//�p�x�ω����o��
				auto now = std::chrono::system_clock::now();
				auto count = std::chrono::duration_cast<std::chrono::milliseconds>(now - kenkyu::origin).count();
				kenkyu::log<double, 6>(to_string(count), correctedNext * (180.0 / M_PI), kenkyu::logSaved, " ");

				//��Ԓx���p�x�܂ł܂� �Z���`�b�Ȃ̂Ń~���b�P�ʂ�
				std::this_thread::sleep_for(std::chrono::milliseconds((unsigned int)(maxWait.operator*() * 10.0)));
				for (size_t m = 0; m < 6; m++)
					kenkyu::serialWriteThreads.at(m)->join();//�X���b�h���J��
			}
		}
		//�߂���΂��΂炭�҂�
		else std::this_thread::sleep_for(std::chrono::milliseconds(10));


	}
	*/
}

void kenkyu::SolveAngles() {

	const kenkyulocal::kenkyuArm::Vector7d kvec(1, 1, 1, 1, 1, 1, 1);

	while (kenkyu::N_killSover) {

		//���t�@�����X��ݒ�
		{
			std::lock_guard<std::mutex> lock(mutexRefPoint);
			arm.SetRef(kenkyuArm::Vector7d(reference.pos.x, reference.pos.y, reference.pos.z, reference.quat.x, reference.quat.y, reference.quat.z, reference.quat.w));
		}

		//�p�����ڕW���牓����Ή�͂����{����
		if (!CheckNearRefToNowArmPosAndQuat(arm.GetDistPosAndQuat())) {
			kenkyulocal::kenkyuArm::Vector6d next, hold;//���̊p�x�ƍ��̊p�x

			//���
			hold = arm.GetNowQ();//��͑O�̊p�x���擾
			next = arm.GetNextQ(kvec);//next�̓g���N

			
			//�t�H�[�}�b�g�����낦��
			//��̒l�̒l��𐮂���@0~2pi�ɂ���@�l��𒆉��Ɋ񂹂� +-pi->�l��𐧌����� +-150�x(�P�ʂ̓��W�A��)
			auto correctedHold = CorrectAngleVecAreaForHutaba<double, 6>(CorrectAngleCenteredVec<double, 6>(CorrectAngleVec(hold)));
			auto correctedNext = CorrectAngleVecAreaForHutaba<double, 6>(CorrectAngleCenteredVec<double, 6>(CorrectAngleVec(next)));

			//��͑O�p�x�ƌ��݊p�x����Ίp�ɔ�Ⴕ�����Ԃ��v�Z����
			auto distAngle = correctedNext - correctedHold;
			array<double, 6> angleWait;
			for (size_t i = 0; i < 6; i++)
				angleWait.at(i) = fabs(distAngle(i)) * (properties.baseVelocity / M_PI) * properties.armVelocitycoefficients.at(i);
			auto maxWait = std::max_element(angleWait.begin(), angleWait.end());//�ҋ@���Ԃ̍ő���o��

			//���肵�Ă�����p�x�𑗐M����
			for (size_t m = 1; m <= 6; m++)
				kenkyu::serialWriteThreads.at(m - 1).reset(new boost::thread(&umeArmTransfer::Move, kenkyu::armMgr.get(), m, ToHutabaDegreeFromRadians(correctedNext(m - 1)), angleWait.at(m - 1)));

			//�X�V�ɃJ�E���g���Z�b�g����
			kenkyu::solverState.SetUpdate(3);

			//�p�x�ω����o��
			auto now = std::chrono::system_clock::now();
			auto count = std::chrono::duration_cast<std::chrono::milliseconds>(now - kenkyu::origin).count();
			kenkyu::log<double, 6>(to_string(count), correctedNext * (180.0 / M_PI), kenkyu::logSaved, " ");

			//��Ԓx���p�x�܂ł܂� �Z���`�b�Ȃ̂Ń~���b�P�ʂ�
			std::this_thread::sleep_for(std::chrono::milliseconds((unsigned int)(maxWait.operator*() * 10.0)));
			for (size_t m = 0; m < 6; m++)
				kenkyu::serialWriteThreads.at(m)->join();//�X���b�h���J��
			
		}
		//�߂���΂��΂炭�҂�
		else std::this_thread::sleep_for(std::chrono::milliseconds(10));


	}
}


kenkyulocal::kenkyuArm::kenkyuArm(const Vector6d& defQ):armSolver(defQ, kenkyulocal::kenkyuArm::fjikken){}

bool kenkyu::GetYorN() {

	if (properties.autoSkip) {
		kenkyu::log("�����X�L�b�v�@�V�X�e��������ɓ��삵�Ȃ��\��������܂��B", kenkyu::logWarning);
		return true;
	}
	while (1) {
		cout << ">>";

		std::string inp;
		std::cin >> inp;

		if (inp == "Y" || inp == "y")return true;
		else if (inp == "N" || inp == "n")return false;
		else cout << "y��n���œ����Ă�������" << endl;
	}
}

kenkyu::_systemBootFlags::_systemBootFlags(){
	this->vr = false;
	this->serial = false;
}

int kenkyu::ToHutabaDegreeFromRadians(const double& rad) {
	return rad * (180.0 / M_PI) * 10;
}

kenkyu::_solverState::_solverState() {
	std::lock_guard<std::mutex> lockr(this->rawMutex);
	std::lock_guard<std::mutex> locku(this->updatedMutex);

	this->raw = kenkyu::_solverState::notYet;
	this->updated = 0;
}
kenkyu::_solverState::operator std::string(){

	switch (this->Get()) {
	case kenkyu::_solverState::notYet:
		return "not yet";
	case kenkyu::_solverState::solved:
		return "solved";
	case kenkyu::_solverState::closer:
		return "closer to the solution";
	case kenkyu::_solverState::impossible:
		return "can't reach the solution";
	default:
		return "error!!";
	}
}

void kenkyu::_solverState::Set(const rawFormat& w) {
	std::lock_guard<std::mutex> lock(this->rawMutex);

	this->raw = w;
}
kenkyu::_solverState::rawFormat kenkyu::_solverState::Get(){
	std::lock_guard<std::mutex> lock(this->rawMutex);

	return this->raw;
}

void kenkyu::_solverState::SetUpdate(unsigned int up) {
	std::lock_guard<std::mutex> locku(this->updatedMutex);

	this->updated = up;
}
void kenkyu::_solverState::DecrementCount() {
	std::lock_guard<std::mutex> locku(this->updatedMutex);

	if (this->updated != 0)
		this->updated--;
}
unsigned int kenkyu::_solverState::GetUpdate() {
	std::lock_guard<std::mutex> locku(this->updatedMutex);

	return this->updated;
}

ImVec2 kenkyu::AddImVec2s(const ImVec2& a, const ImVec2& b) {
	return ImVec2(a.x + b.x, a.y + b.y);
}