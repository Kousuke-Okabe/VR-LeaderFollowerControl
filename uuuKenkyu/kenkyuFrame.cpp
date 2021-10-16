#include "kenkyuFrame.hpp"

using namespace std;
using namespace uuu::easy::usings;
using namespace kenkyulocal;
using namespace Eigen;

const int kenkyu::version = 103;

uuu::vrMgr kenkyu::kenkyuVr;
//typename std::vector<uuu::easy::neo3Dmesh> kenkyu::meshs;
typename std::unordered_map<std::string, std::unique_ptr<uuu::game::drawable>> kenkyu::gmeshs;
typename std::unordered_map<std::string, std::unique_ptr<uuu::game::drawable>> kenkyu::meshesInMonitor;
std::unordered_map<std::string,std::shared_ptr<uuu::shaderProgramObjectVertexFragment>> kenkyu::shaders;
std::unordered_map<std::string, std::unique_ptr<uuu::textureOperator>> kenkyu::textures;
uuu::cameraPersp kenkyu::mainCamera, kenkyu::eyeR, kenkyu::eyeL;
std::unique_ptr<uuu::frameBufferOperator> kenkyu::fboR, kenkyu::fboL;
std::unique_ptr<uuu::textureOperator> kenkyu::colR, kenkyu::colL;
std::chrono::system_clock::time_point kenkyu::origin;
kenkyu::_properties kenkyu::properties;
kenkyu::_systemBootFlags kenkyu::systemBootFlags;

kenkyu::_actionWarehouse kenkyu::actionWarehouse;

std::unique_ptr<umeArmTransfer> kenkyu::armTransfer;

glm::vec3 kenkyu::hmdPos;
//std::list<boost::thread> kenkyu::serialWriteThreads;
std::array<std::unique_ptr<boost::thread>, 6> kenkyu::serialWriteThreads;
std::unique_ptr<boost::thread> kenkyu::solverThread;
std::unique_ptr<boost::thread> kenkyu::logThread;

kenkyu::posAndQuat kenkyu::beforePosR, kenkyu::beforeposL;
kenkyu::posAndQuat kenkyu::reference;

//kenkyulocal::kenkyuArm kenkyu::arm(kenkyuArm::Vector6d::Zero());
std::mutex kenkyu::mutexRefPoint;
std::unique_ptr<armJointSolver::armInverseKineticsSolverForKenkyu<double, 6, 7>> kenkyu::armSolver;

bool kenkyu::N_killSover;

boost::optional<std::ofstream> kenkyu::logStream;

std::pair<unsigned int, unsigned int> kenkyu::windowBounds;;

bool kenkyu::continueLoop;

kenkyu::_solverState kenkyu::solverState;

kenkyu::_specialMeshs kenkyu::specialMeshs;


uuu::textureOperator depr, depl;

kenkyu::_movieBufferCraft kenkyu::movieBufferCraft;
cv::Mat kenkyu::movieFrameMat;
//std::unique_ptr<uuu::textureOperator> debugTex;

//std::unordered_map<std::string,uuu::textureOperator*> kenkyu::texturesRequiringBindAndUniform;

std::mutex kenkyu::solverSpanRateShareMutex;
double kenkyu::solverSpanRateShare;

void kenkyu::Draw() {

	//内臓モニター用のフレームを作る
	kenkyu::specialMeshs.inMonitor->DrawEvent();

	//VRの両目のフレームを生成する
	if (kenkyu::systemBootFlags.vr) {
		kenkyu::DrawVrFrame(*kenkyu::fboR, kenkyu::eyeR);
		kenkyu::DrawVrFrame(*kenkyu::fboL, kenkyu::eyeL);

		//VRにデータを転送する
		kenkyu::TransVrHmd();
	}

	//ウィンドウのフレームを作る
	kenkyu::DrawVrFrame(kenkyu::mainCamera);
	//GUIを上から描画
	kenkyu::DrawGui();

	uuu::app::UpdateForBind();//画面更新
}

glm::mat4 kenkyu::TransVrMatToGmat4(const vr::HmdMatrix34_t& gen) {
	glm::mat4 ret;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			ret[i][j] = gen.m[i][j];

	for (int i = 0; i < 4; i++)
		ret[3][i] = (i == 3) ? 1.0 : 0.0;

	//z軸を入れ替えたらいけるんじゃ?
	auto corr = glm::scale(glm::vec3(-1, 1, -1));

	return glm::transpose(corr * ret * corr);
}

void kenkyu::DrawGui() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void kenkyu::DrawVrFrame(const uuu::cameraPersp& eye) {

	uuu::app::ClearBuffer(uuu::app::color | uuu::app::depth);//画面クリア

	for (auto& s : kenkyu::shaders) {
		s.second->SetUniformCameraPersp(eye);
		s.second->SetUniformValue("lightway", glm::vec4(1, 1, 1, 0));
		s.second->SetUniformValue("modelTransform", glm::identity<glm::mat4>());

		/*if (s.first == "virtualWindow")
			for (auto& tx : kenkyu::texturesRequiringBindAndUniform)
				s.second->SetUniformTexUnit(tx.first, *tx.second);*/
	}

	//仮想ウィンドウの描画
	//{
	//	kenkyu::shaders.at("virtualWindow")->SetUniformValue("modelTransform", glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 1, -1)));

	//	inMonitor->plane->DrawElements();
	//}

	//ゲームメッシュの描画
	for (auto& m : kenkyu::gmeshs) {
		m.second->Draw();
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
void kenkyu::DrawHmdFrame() {
	//VRの両目のフレームを生成する
	if (kenkyu::systemBootFlags.vr) {
		kenkyu::DrawVrFrame(*kenkyu::fboR, kenkyu::eyeR);
		kenkyu::DrawVrFrame(*kenkyu::fboL, kenkyu::eyeL);

		////VRにデータを転送する
		//kenkyu::TransVrHmd();
	}
}


void kenkyu::BootUuuSetForKekyu() {

	//開始時間をとる
	kenkyu::origin = std::chrono::system_clock::now();
	kenkyu::log("booting...");

	kenkyu::InitAnyMembers();
	kenkyu::log("All flags were inited");

	kenkyu::GetProperty("setup.xml");
	kenkyu::log("All properties were loaded");

	//タイムスタンプを押す
	std::stringstream ss;
	auto timeStump = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm timeFormatStruct;
	localtime_s(&timeFormatStruct, &timeStump);
	ss << std::put_time(&timeFormatStruct, "%Y%m%d_%H%M%S");
	kenkyu::log(ss.str(), kenkyu::logSaved);

	//シリアルポートに接続
	if (properties.enableSerialSystem) {
		try {

			kenkyu::armTransfer.reset(new umeArmTransfer(properties.serialPort));
			kenkyu::log("Serial port was connected");
			kenkyu::systemBootFlags.serial = true;
		}
		catch (std::exception& ex) {
			log("シリアルポートへのアクセスでエラーが発生しました。当該部分を無効化して続行しますか?", kenkyu::logError);
			if (GetYorN());
			else throw ex;
		}
	}
	else kenkyu::log("Surpress arm transfer system by \"enable Serial = false\"");

	//VRを接続
	if (properties.enableVrSystem) {
		try {
			kenkyuVr.InitVr();
			log("VR system was started");
			kenkyu::systemBootFlags.vr = true;
		}
		catch (uuu::vr_exception& ex) {
			log("VRシステムへの接続でエラーが発生しました。当該部分を無効化して続行しますか?", kenkyu::logError);
			if (GetYorN());
			else throw ex;
		}
	}
	else kenkyu::log("Surpress VR system by \"enable VR = false\"");


	//UUUのウィンドウをVRの画面サイズの半分に合わせて作る
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
	prop.swapInterval = properties.swapInterval;
	uuu::easy::Init(prop, uuu::neoVertexOperators::positions | uuu::neoVertexOperators::norms | uuu::neoVertexOperators::indices | uuu::neoVertexOperators::texCoords0);
	
		
	//easysを起動
	uuu::easy::InitNeo3Dmesh();

	//GUIシステムを起動
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();//スタイル設定
	ImGui_ImplGlfw_InitForOpenGL(uuu::app::GetBindWindow(), true);
	ImGui_ImplOpenGL3_Init(properties.glslVersion.c_str());
	log("GUI system was started");

	log("uuu subsystem was started");

	//テクスチャを起動
	FreeImage_Initialise();

	//トルクを入れる
	if (kenkyu::systemBootFlags.serial)kenkyu::armTransfer->Torque(255, 1);

	//ソルバーを起動 シリアルかデバッグモード
	if (kenkyu::systemBootFlags.serial||kenkyu::properties.enableDebugMode)solverThread.reset(new boost::thread(kenkyu::SolveAngles));

	return;
}

void kenkyu::InitGraphics() {

	log("Now loading...");

	//適当に初期化して
	kenkyu::shaders.clear();
	kenkyu::gmeshs.clear();

	//無地シェーダを用意
	kenkyu::shaders["norm"] = std::make_shared<uuu::shaderProgramObjectVertexFragment>(uuu::shaderProgramObjectVertexFragment(assets("shaders/norm")));
	kenkyu::shaders["red"] = std::make_shared<uuu::shaderProgramObjectVertexFragment>(uuu::shaderProgramObjectVertexFragment(assets("shaders/redNorm")));
	kenkyu::shaders["virtualWindow"] = std::make_shared<uuu::shaderProgramObjectVertexFragment>(uuu::shaderProgramObjectVertexFragment(assets("shaders/virtualWindow")));
	kenkyu::shaders["rainbow"] = std::make_shared<uuu::shaderProgramObjectVertexFragment>(uuu::shaderProgramObjectVertexFragment(assets("shaders/rainbow")));
	kenkyu::shaders["sprite"] = std::make_shared<uuu::shaderProgramObjectVertexFragment>(uuu::shaderProgramObjectVertexFragment(assets("shaders/sprite")));

	//テクスチャを用意
	//uuu::textureLoaderFromImageFile load;
	textures["cat"].reset(new uuu::textureOperator());
	_uuu::textureLoaderByFreeImage::LoadTextureFromFile(assets("cat.png"), *textures.at("cat").get());
	textures["challenge"].reset(new uuu::textureOperator());
	_uuu::textureLoaderByFreeImage::LoadTextureFromFile(assets("challenge.png"), *textures.at("challenge").get());

	//ステージのメッシュを追加
	//kenkyu::gmeshs["monkey"]=uuu::game::mesh(shaders["norm"], assets(kenkyuSet.dae), "monkey-mesh", glm::translate(glm::identity<glm::mat4>(), gvec3(0, 0, 5)));
	kenkyu::gmeshs["rightHand"].reset(new uuu::game::mesh(shaders["norm"], assets("kenkyuSet.dae"), "rightHand-mesh", glm::identity<glm::mat4>(), true));
	kenkyu::gmeshs["rightPointer"].reset(new uuu::game::mesh(shaders["norm"], assets("kenkyuSet.dae"), "rightPointer-mesh", glm::identity<glm::mat4>(), false));
	kenkyu::gmeshs["rightGoo"].reset(new uuu::game::mesh(shaders["norm"], assets("kenkyuSet.dae"), "rightGoo-mesh", glm::identity<glm::mat4>(), true));

	kenkyu::gmeshs["leftHand"].reset(new uuu::game::mesh(shaders["norm"], assets("kenkyuSet.dae"), "leftHand-mesh", glm::identity<glm::mat4>(), true));
	kenkyu::gmeshs["leftPointer"].reset(new uuu::game::mesh(shaders["norm"], assets("kenkyuSet.dae"), "leftPointer-mesh", glm::identity<glm::mat4>(), false));
	kenkyu::gmeshs["leftGoo"].reset(new uuu::game::mesh(shaders["norm"], assets("kenkyuSet.dae"), "leftGoo-mesh", glm::identity<glm::mat4>(),true));
	kenkyu::gmeshs["cat"].reset(new kenkyulocal::offsetMesh(shaders["norm"], assets("kenkyuSet.dae"), "cat-mesh", kenkyu::reference.toMat(), glm::translate(glm::vec3(0, 1.5, -1.5))));
	kenkyu::gmeshs["room"].reset(new uuu::game::mesh(shaders["rainbow"], assets("rooms.dae"), "", glm::identity<glm::mat4>()));

	kenkyu::gmeshs["catplane"].reset(new uuu::game::texturedMesh(shaders["virtualWindow"], assets("plane.dae"), "Plane-mesh", textures.at("cat").get(), glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 2, -3))));
	kenkyu::specialMeshs.inMonitor = new uuu::game::virtualWindow(shaders["virtualWindow"], assets("screen.dae"), "Plane-mesh", kenkyu::windowBounds.first, kenkyu::windowBounds.second, [&] {
		uuu::app::ClearBuffer(uuu::app::color | uuu::app::depth);//画面クリア
		DrawGui();

		kenkyulocal::viewportSetterAndAutoReverter vp(std::array<GLint, 4>({ 0, 0, (GLint)(kenkyu::windowBounds.first * 0.6), (GLint)kenkyu::windowBounds.second }).data());
		kenkyu::meshesInMonitor["challenge"]->Draw();
		}, glm::translate(glm::identity<glm::mat4>(), glm::vec3(-2, 1.5, -2)));
	kenkyu::gmeshs["inMonitor"].reset(kenkyu::specialMeshs.inMonitor);

	kenkyu::gmeshs["arm"].reset(new kenkyuArmMeshSet(&shaders,glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 1.5, -1.5))));

	kenkyu::meshesInMonitor["challenge"].reset(new uuu::game::texturedMesh(shaders["sprite"], assets("plane.dae"), "Plane-mesh", textures.at("challenge").get(), glm::identity<glm::mat4>()));
	if (systemBootFlags.serial)kenkyu::movieFrameMat = cv::Mat::zeros(cv::Size(800, 600), CV_8U);

	log("assets was loaded");

	//メインカメラを作成
	mainCamera.SetPersp(properties.cameraNear, properties.cameraFar, M_PI * (properties.fov / 180.0), kenkyu::windowBounds.first / (double)kenkyu::windowBounds.second);
	if (kenkyu::systemBootFlags.vr);
	else
		mainCamera.SetLookAt(glm::vec3(0, 1.6, 0), glm::vec3(0, 0, -5.0), glm::vec3(0, 1.0, 0));

	//vrのためのグラフィック構造の作成
	if (kenkyu::systemBootFlags.vr) {
		//VRカメラを作成
		eyeR.SetPersp(properties.cameraNear, properties.cameraFar, M_PI * (properties.fov / 180.0), kenkyuVr.ww / (float)kenkyuVr.wh);
		eyeL.SetPersp(properties.cameraNear, properties.cameraFar, M_PI * (properties.fov / 180.0), kenkyuVr.ww / (float)kenkyuVr.wh);

		//VRのフレームバッファを作成

		kenkyu::fboR.reset(new kenkyulocal::fboOperatorWithViewport(kenkyuVr.ww, kenkyuVr.wh));
		kenkyu::fboL.reset(new kenkyulocal::fboOperatorWithViewport(kenkyuVr.ww, kenkyuVr.wh));

		kenkyu::colR.reset(new uuu::textureOperator());
		kenkyu::colL.reset(new uuu::textureOperator());
		//uuu::textureOperator depr, depl;
		colR->CreateManual(kenkyuVr.ww, kenkyuVr.wh,GL_RGBA, GL_RGBA,GL_UNSIGNED_BYTE);
		depr.CreateManual(kenkyuVr.ww, kenkyuVr.wh, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
		fboR->AttachTextureOperator(fboR->color0, *colR);
		fboR->AttachTextureOperator(fboR->depth, depr);

		colL->CreateManual(kenkyuVr.ww, kenkyuVr.wh, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
		depl.CreateManual(kenkyuVr.ww, kenkyuVr.wh, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
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

	FreeImage_DeInitialise();

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
			if (logThread->joinable())//スレッドをくっつける
				logThread->join();

		if (!kenkyu::logStream)throw std::runtime_error("ログファイルが準備できていません。呼び出し順を確認してください。");

		logThread.reset(new boost::thread(kenkyu::SaveALog, str));
		
		break;
	default:
		throw std::logic_error("不正なログ状態"+logInfo);
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

	//VR関係のイベント
	if (kenkyu::systemBootFlags.vr)kenkyu::kenkyuVr.Event(kenkyu::CallbackVrEvents);

	//シリアルポートからフレームもらえるかもね
	if (systemBootFlags.serial && properties.enableMovie)kenkyu::MovieEvent();

	//GUI関係のイベント
	kenkyu::GuiEvents();
	
	//デバッグモードが有効ならデバッグイベントを行う
	if (properties.enableDebugMode)
		kenkyu::DebugEvent();
	
}

void kenkyu::MovieEvent() {
	std::vector<uint8_t> buffer;
	kenkyu::armTransfer->Read(buffer);

	movieBufferCraft(buffer);

	//フレームが存在するのなら
	auto frameNum = movieBufferCraft.stack.size() / 4;
	if (!frameNum)return;


	//ムービーフレームを作る
	kenkyu::movieFrameMat = cv::Mat::zeros(cv::Size(kenkyu::movieFrameMat.size().width, kenkyu::movieFrameMat.size().height), CV_8U);

	for (size_t i = 0; i < frameNum; i++)
		cv::line(kenkyu::movieFrameMat, cv::Point2i((double)movieBufferCraft.stack.at(i*4+0)/255.0*800.0, (double)movieBufferCraft.stack.at(i * 4 + 1) / 255.0 * 600.0), cv::Point2i((double)movieBufferCraft.stack.at(i * 4 + 2) / 255.0 * 800.0, (double)movieBufferCraft.stack.at(i * 4 + 3) / 255.0 * 600.0), cv::Scalar(255, 255, 255), 5);

	//テクスチャを書き換え
	textures.at("challenge").get()->UpdateTextureData(kenkyu::movieFrameMat.size().width, kenkyu::movieFrameMat.size().height, GL_LUMINANCE, movieFrameMat.data);

}

void kenkyu::_movieBufferCraft::operator()(const std::vector<uint8_t>& buf) {

	//4フレーム連続0ならフレーム開始符号なのでそれを探す
	std::vector<uint8_t>::const_iterator begite;
	if (this->findBegin(buf,begite)) {
		stack.clear();

		//開始符号が見つかったらスタックする
		for (auto i = begite; i != buf.end(); i++)
			stack.push_back(*i);
	}
	else {
		//そうでなければスタックに貯める
		for (auto i = buf.begin(); i != buf.end(); i++)
			stack.push_back(*i);
	}

	std::cout << std::string(stack.begin(), stack.end())<<endl;
}
kenkyu::_movieBufferCraft::_movieBufferCraft() {
}
bool kenkyu::_movieBufferCraft::findBegin(const std::vector<uint8_t>& buf, std::vector<uint8_t>::const_iterator& ret) {
	
	ret = buf.end();

	if (buf.empty())return false;

	//スタックの後ろから0を探す
	size_t bornus = 0;
	for (size_t i = 0; i<3 && stack.size()>i; i++)
		if (this->stack.at(stack.size() - 1 - i) == 0)bornus++;
		else break;

	auto ite = buf.begin();
	while (1) {
		auto found = std::find(ite, buf.end(), 0);
		//みつからなければbreak
		if (found == buf.end())return false;

		//開始符号を含めるかチェック
		if (std::distance(found, buf.end()) < 4 - bornus)return false;

		//必要な分0を数える
		int processdone = -1;
		for (size_t i = 0; i < 4 - bornus; i++) {
			if (*(found + i) == 0) {
				processdone = i;
				continue;
			}
			else {
				//この0ではないのかもしれない
				ite = found + i;
				break;
			}
		}
		if (processdone != -1) {
			ret = found + (4 - bornus);
			return true;
		}
		
	}
}


void kenkyu::DebugEvent() {

	if (!properties.enableVrSystem) {
		glm::vec3 dist(0, 0, 0);
		//キーボードでリファレンスが動かせる
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_A))
			dist += glm::vec3(-1, 0, 0);
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_D))
			dist += glm::vec3(1, 0, 0);
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_Q))
			dist += glm::vec3(0, 0, -1);
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_E))
			dist += glm::vec3(0, 0, 1);
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_W))
			dist += glm::vec3(0, 1, 0);
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_S))
			dist += glm::vec3(0, -1, 0);

		reference.pos += [&] {
			if (dist == glm::zero<glm::vec3>())return dist;

			auto no=glm::normalize(dist);
			return glm::vec3(no.x * 0.01, no.y * 0.01, no.z * 0.01);
		}();

		//さらに回転も行う
		glm::quat disq(1.0,0,0,0);
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_R))
			disq = glm::quat(cos(0.01), 0, sin(0.01), 0) * disq;
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_F))
			disq = glm::quat(cos(-0.01), 0, sin(-0.01), 0) * disq;
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_T))
			disq = glm::quat(cos(0.01), sin(0.01), 0, 0) * disq;
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_G))
			disq = glm::quat(cos(-0.01), sin(-0.01), 0, 0) * disq;
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_Y))
			disq = glm::quat(cos(0.01), 0, 0, sin(0.01)) * disq;
		if (uuu::keyboardInterface::GetKeyInput(GLFW_KEY_H))
			disq = glm::quat(cos(-0.01), 0, 0, sin(-0.01)) * disq;

		reference.quat = disq * reference.quat;


		kenkyu::gmeshs["cat"]->SetTransform(reference.toMat());
	}
}

void kenkyu::CallbackVrEvents(vr::VREvent_t event) {

	kenkyu::VrGeneralEvents(event);
	kenkyu::VrSceneEvents(event);
	kenkyu::VrTrackingEvents(event);

}

void kenkyu::VrSceneEvents(vr::VREvent_t event) {

	enum { invalid = 0, right = 1, left = 2 }hand = invalid;//どの手の?
	{
		//デバイスが腕なら
		auto dev = kenkyuVr.hmd->GetTrackedDeviceClass(event.trackedDeviceIndex);
		if (dev == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
			auto role = kenkyuVr.hmd->GetControllerRoleForTrackedDeviceIndex(event.trackedDeviceIndex);

			//ロールで見分ける
			switch (role) {
			case vr::TrackedControllerRole_RightHand://右手
				hand = right;
				break;
			case vr::TrackedControllerRole_LeftHand://左手
				hand = left;
				break;
			}
		}
	}
	enum { other = 0, grip = 1, trigger = 2, handing = 3 }action = other;//どのアクションか
	{
		if (event.data.controller.button == vr::k_EButton_SteamVR_Touchpad) {
			action = handing;
		}
		if (event.data.controller.button == vr::k_EButton_SteamVR_Trigger)
			action = trigger;
		else if (event.data.controller.button == vr::k_EButton_Grip)
			action = grip;
	}
	enum { none = 0, press = 1, unpress = 2 } edge = none;//どのエッジで行われてるか
	switch (event.eventType) {
	case vr::VREvent_ButtonPress:
		edge = press;
		break;
	case vr::VREvent_ButtonUnpress:
		edge = unpress;
		break;
	}

	//こいつらが有効ならアクション処理を行う
	if (hand && action && edge) {
		//手先の処理かトラッキング処理か
		if (action == handing) {
			auto& nowtype = (hand == right) ? actionWarehouse.rHandingAngle : actionWarehouse.lHandingAngle;
			auto angle = (edge == press) ? -300 : 300;
			nowtype = angle;//ターゲットに目標角を代入
		}
		else {
			//log("scene event " + std::to_string(hand) + " " + std::to_string(action) + " " + std::to_string(edge), logDebug);
			//トリガー中はフラグを立てる
			auto& nowtype = (hand == right) ? actionWarehouse.rhandtype : actionWarehouse.lhandtype;

			if (action == trigger && edge == press)nowtype = 2;
			else if (action == grip && edge == press)nowtype = 0;
			else if (edge == unpress)nowtype = 1;

			//アクションがあったとき表示状態を切り替える
			if (hand == right) {
				kenkyu::gmeshs["rightHand"]->skipDraw = !(actionWarehouse.rhandtype == 0);
				kenkyu::gmeshs["rightPointer"]->skipDraw = !(actionWarehouse.rhandtype == 1);
				kenkyu::gmeshs["rightGoo"]->skipDraw = !(actionWarehouse.rhandtype == 2);
			}
			else if (hand == left) {
				kenkyu::gmeshs["leftHand"]->skipDraw = !(actionWarehouse.lhandtype == 0);
				kenkyu::gmeshs["leftPointer"]->skipDraw = !(actionWarehouse.lhandtype == 1);
				kenkyu::gmeshs["leftGoo"]->skipDraw = !(actionWarehouse.lhandtype == 2);
			}
		}
	}
}
void kenkyu::VrGeneralEvents(vr::VREvent_t event) {
	if (event.eventType == vr::VREvent_TrackedDeviceActivated) {
		log("device " + std::to_string(event.trackedDeviceIndex) + "attached");
	}
}
void kenkyu::VrTrackingEvents(vr::VREvent_t event) {
	//デバイスの最大数取り出す
	for (size_t id = 0; id < vr::k_unMaxTrackedDeviceCount; id++) {

		//未接続のidなら終わり
		if (!kenkyu::kenkyuVr.hmd->IsTrackedDeviceConnected(id))
			continue;

		auto dev = kenkyu::kenkyuVr.hmd->GetTrackedDeviceClass(id);

		vr::TrackedDevicePose_t devicePose;

		//コントローラなら
		if (dev == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
			//ロールを割り出す
			auto role = kenkyuVr.hmd->GetControllerRoleForTrackedDeviceIndex(id);

			//コントローラ用抽出処理
			vr::VRControllerState_t controllerState;
			kenkyu::kenkyuVr.hmd->GetControllerStateWithPose(vr::TrackingUniverseStanding, id, &controllerState, sizeof(controllerState), &devicePose);

			
			auto trans = properties.vrRotYAxis * kenkyu::TransVrMatToGmat4(devicePose.mDeviceToAbsoluteTracking);
			glm::vec3 pos = trans * gvec4(0, 0, 0, 1);
			glm::quat q(trans);

			//log("quat",q);

			//右手or左手なら位置追従させる
			if (role == vr::TrackedControllerRole_RightHand) {
				kenkyu::gmeshs["rightHand"]->SetTransform(trans);
				kenkyu::gmeshs["rightPointer"]->SetTransform(trans);
				kenkyu::gmeshs["rightGoo"]->SetTransform(trans);

				//アクションがあるなら送信
				//グーは相対モード
				if (actionWarehouse.rhandtype == 2) {
					//それぞれの差分ベクトルをとる
					auto posdist = pos - beforePosR.pos;
					auto quatdist = q * glm::inverse(beforePosR.quat);

					//目標姿勢を変換
					{
						std::lock_guard<std::mutex> lock(mutexRefPoint);
						kenkyu::reference.pos += posdist;
						kenkyu::reference.quat = quatdist * kenkyu::reference.quat;
					}

					//当然モデル位置も更新
					kenkyu::gmeshs["cat"]->SetTransform(reference.toMat());
				}
				//パーはアブソリュートモード
				else if (actionWarehouse.rhandtype == 0) {
					//それぞれの差分ベクトルをとる
					auto posdist = pos - beforePosR.pos;

					//目標姿勢を変換
					{
						std::lock_guard<std::mutex> lock(mutexRefPoint);
						kenkyu::reference.pos += posdist;
						kenkyu::reference.quat = q;
					}

					//当然モデル位置も更新
					kenkyu::gmeshs["cat"]->SetTransform(reference.toMat());
				}

				//座標を記録
				beforePosR.pos = pos;
				beforePosR.quat = q;

				//さらに座標を送信
				//if (!kenkyu::serialWriteThread)
					//kenkyu::serialWriteThread.reset(new boost::thread(&umeArmTransfer::Position, kenkyu::armMgr.get(), 1919, pos, 1000));
				/*if (serialWriteThreads.size() < 1)
					kenkyu::serialWriteThreads.push_back(boost::thread(&umeArmTransfer::Position, kenkyu::armMgr.get(), 1919, pos, 1000));
					*/
			}

			if (role == vr::TrackedControllerRole_LeftHand) {
				kenkyu::gmeshs["leftHand"]->SetTransform(trans);
				kenkyu::gmeshs["leftPointer"]->SetTransform(trans);
				kenkyu::gmeshs["leftGoo"]->SetTransform(trans);
			}
		}
		//hmdなら
		else if (dev == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD) {
			kenkyu::kenkyuVr.hmd->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, id, &devicePose, 1);

			auto trans = properties.vrRotYAxis * kenkyu::TransVrMatToGmat4(devicePose.mDeviceToAbsoluteTracking);
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
		else continue;//それいがいは打ち切り
	}

	//目標位置を更新
	/* {
		lock_guard<std::mutex> lock(mutexArm);//アームを拘束
		arm.SetRef(kenkyuArm::Vector7d(reference.pos.x, reference.pos.y, reference.pos.z, reference.quat.x, reference.quat.y, reference.quat.z, reference.quat.w));
	}*/
	 
	 
	//腕の目標姿勢を送信する
	/*if (!kenkyu::serialWriteThread)
		kenkyu::serialWriteThread.reset(new boost::thread(&umeArmTransfer::PositionQuat, kenkyu::armMgr.get(), 1919, reference.pos, reference.quat, 1000));
	*/
}

void kenkyu::GuiEvents() {

	//フレーム開始
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	//arm healthは緑系
	ImGui::SetNextWindowPos(ImVec2(windowBounds.first * 0.6, 0));
	ImGui::SetNextWindowSize(ImVec2(windowBounds.first * 0.4, windowBounds.second*0.3));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.3f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.3f, 0.1f, 1.0f));
	ImGui::Begin("Arm health");
	{

		//更新されたなら四角をピコピコする
		auto rectBegin = [&] {auto gen = ImGui::GetCursorScreenPos(); return ImVec2(gen.x+(kenkyu::windowBounds.first / 4.0), gen.y); }();
		auto updateShowerSize = std::max<double>(kenkyu::windowBounds.first / 20.0, kenkyu::windowBounds.second / 20.0);
		if (kenkyu::solverState.GetUpdate() != 0) {
			ImGui::GetWindowDrawList()->AddRectFilled(rectBegin, AddImVec2s(rectBegin, ImVec2(updateShowerSize, updateShowerSize)), ImGui::GetColorU32(ImVec4(0.3, 1, 0.3, 1)), updateShowerSize / 10.0);
		}
		else {
			ImGui::GetWindowDrawList()->AddRectFilled(rectBegin, AddImVec2s(rectBegin, ImVec2(updateShowerSize, updateShowerSize)), ImGui::GetColorU32(ImVec4(0.3, 0.3, 0.3, 1)), updateShowerSize / 10.0);
		}
		kenkyu::solverState.DecrementCount();

		ImGui::Text("reference pos");
		ImGui::Text((" " + to_stringf(kenkyu::reference.pos.x) + "," + to_stringf(kenkyu::reference.pos.y) + "," + to_stringf(kenkyu::reference.pos.z)).c_str());

		ImGui::Text("reference quat");
		ImGui::Text((" " + to_stringf(kenkyu::reference.quat.x) + "," + to_stringf(kenkyu::reference.quat.y) + "," + to_stringf(kenkyu::reference.quat.z) + "," + to_stringf(kenkyu::reference.quat.w)).c_str());
		ImGui::Text("moter positions");
		if (kenkyu::systemBootFlags.serial||properties.enableDebugMode) {
			auto sendAngle = ToDegreeFrom10TimesDegree<int, 6>(ToHutabaDegreeFromRadiansVec(CorrectAngleVecAreaForHutaba<double, 6>(CorrectAngleCenteredVec<double, 6>(CorrectAngleVec<double, 6>(kenkyu::GetMoterAngles())))));
			for (size_t m = 1; m <= 6; m++)
				ImGui::Text((" m" + to_string(m) + ": " + to_string(sendAngle(m - 1)) + "deg").c_str());
		}
		else ImGui::Text(" Not avalable");

		ImGui::Text("solver state");
		ImGui::Text((std::string(" ") + kenkyu::solverState.operator std::string()).c_str());

	}
	ImGui::End();
	ImGui::PopStyleColor(2);

	//system healthは青系
	ImGui::SetNextWindowPos(ImVec2(windowBounds.first * 0.6, windowBounds.second * 0.3));
	ImGui::SetNextWindowSize(ImVec2(windowBounds.first * 0.4, windowBounds.second * 0.2));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.1f, 0.1f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.1f, 0.1f, 0.3f, 1.0f));
	ImGui::Begin("System health");
	{
		auto mainSpan=kenkyu::GetSpan();
		ImGui::Text("main span rate(FPS)");
		ImGui::Text((std::string(" ") + to_stringf(1.0/((double)mainSpan.count()/1000.0))).c_str());

		double solverSpan;//単位はセンチ秒
		{
			std::lock_guard<std::mutex> lock(solverSpanRateShareMutex);
			solverSpan = 1.0 / solverSpanRateShare;
		}
		ImGui::Text("solver span rate");
		if (systemBootFlags.serial||properties.enableDebugMode) ImGui::Text((std::string(" ") + to_stringf(1.0 / (solverSpan / 100.0))).c_str());
		else ImGui::Text(" Not booted yet.");
	}
	ImGui::End();
	ImGui::PopStyleColor(2);


	ImGui::SetNextWindowPos(ImVec2(windowBounds.first * 0.6, windowBounds.second * 0.5));
	ImGui::SetNextWindowSize(ImVec2(windowBounds.first * 0.4, windowBounds.second*0.5));

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.3f, 0.0, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.3f, 0.0, 0.1f, 1.0f));
	ImGui::Begin("Controls");
	{
		if (kenkyu::systemBootFlags.serial||true) {
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, kenkyu::systemBootFlags.serial ? ImVec4(0.6f, 0.1f, 0.1f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			if (ImGui::Button("Emergency", ImVec2(windowBounds.first * 0.4 * 1.0, windowBounds.second * 0.5 * 0.25))&&kenkyu::systemBootFlags.serial)
				kenkyu::armTransfer->Torque(255, 0);//緊急停止ボタン
			ImGui::PopStyleColor(2);

			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 7.0f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, kenkyu::systemBootFlags.serial ? ImVec4(0.0f, 0.3f, 0.1f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			auto retorquePos = ImGui::GetCursorPos();
			if (ImGui::Button("Retorque", ImVec2(windowBounds.first * 0.4 * 0.48, windowBounds.second * 0.5 * 0.25)) && kenkyu::systemBootFlags.serial)
				kenkyu::armTransfer->Torque(255, 1);//再トルク印加ボタン
			ImGui::PopStyleColor(2);
			
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.7f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, kenkyu::systemBootFlags.serial ? ImVec4(0.0f, 0.0f, 0.2f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::SetCursorPos(ImVec2(windowBounds.first * 0.4 * 0.5 + retorquePos.x, 0 + retorquePos.y));
			if (ImGui::Button("Zero", ImVec2(windowBounds.first * 0.4 * 0.48, windowBounds.second * 0.5 * 0.25)) && kenkyu::systemBootFlags.serial) {
				constexpr double l1 = 0.28, l2 = 0.35, l3 = 0.0;
				std::lock_guard<std::mutex> lock(mutexRefPoint);
				kenkyu::reference.pos = glm::vec3(0, -l1 - l2 - l3, 0);
				kenkyu::reference.quat = glm::quat(1, 0, 0, 0);

				kenkyu::armTransfer->Move(255, 0, 500);
			}
			ImGui::PopStyleColor(2);

			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.0f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.5f, 1.0f));
			if (ImGui::Button("Exit", ImVec2(windowBounds.first * 0.4 * 0.48, windowBounds.second * 0.5 * 0.25)))
				kenkyu::continueLoop = false;//アプリ終了ボタン
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

	//バージョンチェック
	if (boost::optional<int> verdt = pt.get_optional<int>("kenkyu.setup.<xmlattr>.version")) {

		if (kenkyu::version != verdt.get())throw std::runtime_error("the version(" + std::to_string(kenkyu::version) + ") doesn't match setup version(" + std::to_string(verdt.get()) + ")");

		kenkyu::log("property \"version\" = " + std::to_string(verdt.get()), logDebug);
	}
	else throw std::runtime_error("not found \"version\" property");

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
			kenkyu::log("選択肢のAutoSkipが有効になっています。一部の機能が正常に働かない場合があります。", kenkyu::logWarning);
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

	//モーターの数だけ
	for (size_t m = 1; m <= 6; m++) {
		std::string pathname = "kenkyu.setup.arm.m" + to_string(m) + ".<xmlattr>";

		//はやさ
		if (boost::optional<double> coeffdt = pt.get_optional<double>(pathname+".velocity")) {
			properties.armVelocitycoefficients.at(m - 1) = coeffdt.get();
			kenkyu::log("property \"m" + to_string(m) + " velocity ratio\"=" + std::to_string(properties.armVelocitycoefficients.at(m - 1)), logDebug);
		}
		else {
			properties.armVelocitycoefficients.at(m - 1) = 1.0;
			kenkyu::log("property \"m" + to_string(m) + " velocity ratio\" is not defined.default value=1.0", logDebug);
		}
		//最大角度
		if (boost::optional<double> maxdt = pt.get_optional<double>(pathname + ".max")) {
			properties.armAngleArea.at(m - 1).first = maxdt.get();
			kenkyu::log("property \"m" + to_string(m) + " max angle\"=" + std::to_string(properties.armAngleArea.at(m - 1).first), logDebug);
		}
		else {
			properties.armAngleArea.at(m - 1).first = +160.0;
			kenkyu::log("property \"m" + to_string(m) + " max angle\" is not defined.default value=160.0", logDebug);
		}
		//最小角度
		if (boost::optional<double> mindt = pt.get_optional<double>(pathname + ".min")) {
			properties.armAngleArea.at(m - 1).second = mindt.get();
			kenkyu::log("property \"m" + to_string(m) + " min angle\"=" + std::to_string(properties.armAngleArea.at(m - 1).second), logDebug);
		}
		else {
			properties.armAngleArea.at(m - 1).second = -160.0;
			kenkyu::log("property \"m" + to_string(m) + " min angle\" is not defined.default value=-160.0", logDebug);
		}


	}

	if (boost::optional<unsigned int> swapintervaldt = pt.get_optional<unsigned int>("kenkyu.setup.system.<xmlattr>.swapInterval")) {
		properties.swapInterval = swapintervaldt.get();
		kenkyu::log("property \"swap interval\" = " + std::to_string(properties.swapInterval), logDebug);
	}
	else {
		properties.swapInterval = 0;
		log("not found \"swap interval\" property. default value = 0", logDebug);
	}

	if (boost::optional<bool> enableVrdt = pt.get_optional<bool>("kenkyu.setup.system.<xmlattr>.enableVR")) {
		properties.enableVrSystem = enableVrdt.get();
		kenkyu::log("property \"enable VR\" = " + std::to_string(properties.enableVrSystem), logDebug);
	}
	else {
		properties.enableVrSystem = true;
		log("not found \"enable VR\" property. default value = true", logDebug);
	}
	if (boost::optional<bool> enableSerialdt = pt.get_optional<bool>("kenkyu.setup.system.<xmlattr>.enableSerial")) {
		properties.enableSerialSystem = enableSerialdt.get();
		kenkyu::log("property \"enable Serial\" = " + std::to_string(properties.enableSerialSystem), logDebug);
	}
	else {
		properties.enableSerialSystem = true;
		log("not found \"enable Serial\" property. default value = true", logDebug);
	}

	if (boost::optional<bool> enableDebugModedt = pt.get_optional<bool>("kenkyu.setup.system.<xmlattr>.debugMode")) {
		properties.enableDebugMode = enableDebugModedt.get();
		kenkyu::log("property \"debugMode\" = " + std::to_string(properties.enableDebugMode), logDebug);
	}
	else {
		properties.enableDebugMode = false;
		log("not found \"debugMode\" property. default value = false", logDebug);
	}
	
	if (boost::optional<double> rotationYAxisdt = pt.get_optional<double>("kenkyu.setup.vr.<xmlattr>.rotationYAxis")) {
		properties.vrRotYAxis = glm::rotate((float)(rotationYAxisdt.get() * (M_PI / 180.0)), glm::vec3(0, 1, 0));
		kenkyu::log("property \"rotationYAxis\" = " + std::to_string(rotationYAxisdt.get()), logDebug);
	}
	else {
		properties.vrRotYAxis = glm::identity<glm::mat4>();
		log("not found \"rotation YAxis\" property. default value = 0.0", logDebug);
	}

	if (boost::optional<bool> enableMoviedt = pt.get_optional<bool>("kenkyu.setup.system.<xmlattr>.enableMovie")) {
		properties.enableMovie = enableMoviedt.get();
		kenkyu::log("property \"enable movie\" = " + std::to_string(properties.enableMovie), logDebug);
	}
	else {
		properties.enableMovie = false;
		log("not found \"enable movie\" property. default value = false", logDebug);
	}
	return;
}

void kenkyu::InitAnyMembers() {
	//kenkyu::actionWarehouse.rhandtype = 1;
	//kenkyu::actionWarehouse.lhandtype = 1;

	for (int th = 0; th < 6; th++)
		kenkyu::serialWriteThreads.at(th).release();

	kenkyu::solverThread.release();
	kenkyu::logThread.release();

	//アームの初期姿勢はまっすぐ伸ばした状態
	constexpr double l1 = 0.28, l2 = 0.35, l3 = 0.0;
	{
		std::lock_guard<std::mutex> lock(mutexRefPoint);
		kenkyu::reference.pos = glm::vec3(0, -l1 - l2 - l3, 0);
		kenkyu::reference.quat = glm::quat(1, 0, 0, 0);
	}

	//beforeも同じく
	kenkyu::beforePosR = kenkyu::reference;
	kenkyu::beforeposL = kenkyu::reference;

	kenkyu::N_killSover = true;//!false よってtrue

	kenkyu::continueLoop = true;

	solverSpanRateShare = 0.0;
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

kenkyu::Vector7 kenkyu::fjikken(const Vector6& q) {

	//実験装置に合わせたアーム()
	constexpr double l1 = 0.28, l2 = 0.35, l3 = 0.0;
	Affine3d trans = AngleAxisd(q(0), y) * AngleAxisd(q(1), z) * Translation<double, 3>(0, -l1, 0) * AngleAxisd(q(2), z) * Translation<double, 3>(0, -l2, 0) * AngleAxisd(q(3), y) * AngleAxisd(q(4), z) * Translation<double, 3>(0, -l3, 0) * AngleAxisd(q(5), y);

	//ここから姿勢と座標を抜き出す 姿勢の表現を変えてみる
	Eigen::Vector3d pos(trans.translation());
	//姿勢を減算が定義できるようにあらわす　回転軸をz+の球面とz=0平面でy>0で正規化する
	Eigen::Quaterniond quat(trans.rotation());

	return Eigen::Matrix<double, 7, 1>(pos.x(), pos.y(), pos.z(), quat.x(), quat.y(), quat.z(), quat.w());
}
kenkyu::Vector7 kenkyu::fjikkenWithGen(const Vector6& q,const Eigen::Quaterniond& gen) {

	//実験装置に合わせたアーム()
	constexpr double l1 = 0.28, l2 = 0.35, l3 = 0.0;
	Affine3d trans = AngleAxisd(q(0), y) * AngleAxisd(q(1), z) * Translation<double, 3>(0, -l1, 0) * AngleAxisd(q(2), z) * Translation<double, 3>(0, -l2, 0) * AngleAxisd(q(3), y) * AngleAxisd(q(4), z) * Translation<double, 3>(0, -l3, 0) * AngleAxisd(q(5), y);

	//ここから姿勢と座標を抜き出す 姿勢の表現を変えてみる
	Eigen::Vector3d pos(trans.translation());
	//姿勢を減算が定義できるようにあらわす　回転軸をz+の球面とz=0平面でy>0で正規化する
	Eigen::Quaterniond quat(trans.rotation());
	Eigen::Vector4d pq(quat.x(), quat.y(), quat.z(), quat.w()), mq = -pq, gq(gen.x(), gen.y(), gen.z(), gen.w());


	if ((gq - pq).squaredNorm() < (gq - mq).squaredNorm()) {
		return Eigen::Matrix<double, 7, 1>(pos.x(), pos.y(), pos.z(), quat.x(), quat.y(), quat.z(), quat.w());
	}
	else {
		return Eigen::Matrix<double, 7, 1>(pos.x(), pos.y(), pos.z(), -quat.x(), -quat.y(), -quat.z(), -quat.w());
	}
}


//姿勢が近いことを表す評価
bool CheckNearRefToNowArmPosAndQuat(const std::pair<double, double>& pq) {
	return pq.first < 0.01 && abs<double>(pq.second) > cos(M_PI * (5.0 / 180.0));
}

void kenkyu::SolveAngles() {

	using VectorC = Eigen::Matrix<double, 7, 1>;

	//ソルバーを構成
	kenkyu::armSolver.reset(new armJointSolver::armInverseKineticsSolverForKenkyu<double, 6, 7>(&kenkyu::fjikkenWithGen));

	//ループのスパンを取るための時間
	size_t beforeTimepoint=uuu::app::GetTimeFromInit();

	VectorC ref;
	while (kenkyu::N_killSover) {
		//リファレンスを読み込み
		{
			std::lock_guard<std::mutex> lock(mutexRefPoint);
			ref = VectorC(reference.pos.x, reference.pos.y, reference.pos.z, reference.quat.x, reference.quat.y, reference.quat.z, reference.quat.w);
		}

		auto beforeAngles = armSolver->GetAngles();
		armSolver->SolverStep(ref, 1.0 / 16.0);//解析

		auto jointAngles = armSolver->GetAngles();
		if (true) {
			//フォーマットをそろえる
			//二つの値の値域を整える　0~2piにする　値域を中央に寄せる +-pi->値域を制限する +-150度(単位はラジアン)
			auto correctedAngles = CorrectAngleVecAreaForHutaba<double, 6>(CorrectAngleCenteredVec<double, 6>(CorrectAngleVec(jointAngles)));

			//sleepの直後から現在までの時間をとる
			auto distWithoutSleep = uuu::app::GetTimeFromInit() - beforeTimepoint;

			//スレッドのスパン管理をする
			std::this_thread::sleep_for(std::chrono::milliseconds(max(0, (int)(1000.0 / 20.0 - distWithoutSleep))));

			//スパンをシェアする
			double span = max(100.0 / 20.0,distWithoutSleep * 10.0);
			{
				std::lock_guard<std::mutex> lock(solverSpanRateShareMutex);
				solverSpanRateShare = span;
			}

			beforeTimepoint = uuu::app::GetTimeFromInit();

			//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			
			//角度を送信する
			if (properties.enableSerialSystem) {
				for (size_t m = 1; m <= 6; m++) {
					//スレッドが存在すればjoin
					//if (kenkyu::serialWriteThreads.at(m - 1))kenkyu::serialWriteThreads.at(m - 1)->join();
					//kenkyu::serialWriteThreads.at(m - 1).reset(new boost::thread([] {std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }));

					//kenkyu::armTransfer->Move(m, ToHutabaDegreeFromRadians(correctedAngles(m - 1)), span);
					//kenkyu::serialWriteThreads.at(m - 1).reset(new boost::thread(&umeArmTransfer::Move, kenkyu::armTransfer.get(), m, ToHutabaDegreeFromRadians(correctedAngles(m - 1)), span));
				}
				//グリッパーも送信
				kenkyu::armTransfer->Move(7, kenkyu::actionWarehouse.rHandingAngle);
				std::cout << kenkyu::actionWarehouse.rHandingAngle << std::endl;
			}


			//更新にカウントをセットする
			kenkyu::solverState.SetUpdate(1);

			//ログファイルに書き込む
			//auto now = std::chrono::system_clock::now();
			//auto count = std::chrono::duration_cast<std::chrono::milliseconds>(now - kenkyu::origin).count();
			//kenkyu::log<double, 6>(to_string(count), correctedAngles, kenkyu::logSaved, " ");

		}

	}
}

bool kenkyu::GetYorN() {

	if (properties.autoSkip) {
		kenkyu::log("自動スキップ　システムが正常に動作しない可能性があります。", kenkyu::logWarning);
		return true;
	}
	while (1) {
		cout << ">>";

		std::string inp;
		std::cin >> inp;

		if (inp == "Y" || inp == "y")return true;
		else if (inp == "N" || inp == "n")return false;
		else cout << "yかnかで答えてください" << endl;
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

	switch (this->GetRawFormat()) {
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

void kenkyu::_solverState::SetRawFormat(const rawFormat& w) {
	std::lock_guard<std::mutex> lock(this->rawMutex);

	this->raw = w;
}
kenkyu::_solverState::rawFormat kenkyu::_solverState::GetRawFormat(){
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
std::chrono::milliseconds kenkyu::GetSpan() {
	static std::chrono::time_point bef = std::chrono::system_clock::now();//以前呼び出された時刻

	auto now = std::chrono::system_clock::now();

	auto ret = (now - bef);
	bef = now;

	return std::chrono::duration_cast<std::chrono::milliseconds>(ret);
}

std::string kenkyu::assets(const std::string& details) {
	return properties.assetpath + details;
}

kenkyu::Vector6 kenkyu::GetMoterAngles() {
	if(systemBootFlags.serial||properties.enableDebugMode)
		return kenkyu::armSolver->GetAngles();

	return Vector6::Zero();
}

inline string kenkyulocal::to_stringf(double _Val, const char* format) {
	const auto _Len = static_cast<size_t>(_CSTD _scprintf(format, _Val));
	string _Str(_Len, '\0');
	_CSTD sprintf_s(&_Str[0], _Len + 1, format, _Val);
	return _Str;
}

kenkyulocal::fboOperatorWithViewport::fboOperatorWithViewport(size_t u, size_t v, size_t defu, size_t defv):super(), u(u), v(v), defu(defu), defv(defv) {
	this->returnAutoSize = false;
}
kenkyulocal::fboOperatorWithViewport::fboOperatorWithViewport(size_t u, size_t v) : super(), u(u), v(v){
	this->returnAutoSize = true;
}

__int8 kenkyulocal::fboOperatorWithViewport::Bind(){

	if (returnAutoSize) {
		GLint viewport[4];
		glGetIntegerv(
			GL_VIEWPORT,// GLenum pname
			viewport);

		this->defu = viewport[2];
		this->defv = viewport[3];
	}

	glViewport(0, 0, u, v);
	return super::Bind();
}
__int8 kenkyulocal::fboOperatorWithViewport::Unbind()const {

	glViewport(0, 0, defu, defv);

	return super::Unbind();
}

kenkyulocal::viewportSetterAndAutoReverter::viewportSetterAndAutoReverter(GLint vp[4]) {

	glGetIntegerv(GL_VIEWPORT, retvp);

	glViewport(vp[0], vp[1], vp[2], vp[3]);

}
kenkyulocal::viewportSetterAndAutoReverter::~viewportSetterAndAutoReverter() {
	glViewport(retvp[0], retvp[1], retvp[2], retvp[3]);
}



kenkyu::_actionWarehouse::_actionWarehouse() {
	this->rhandtype = 1;
	this->lhandtype = 1;
	this->rHandingAngle = 300;
	this->lHandingAngle = 300;
}