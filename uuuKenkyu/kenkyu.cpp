#include "kenkyuFrame.hpp"
#include "xbeeForWin.hpp"
#include "armOpeTransfer.hpp"

using namespace std;
using namespace kenkyulocal;

int main() {

	serialMgr myserial("COM3");
	payload_6joint_1grip hell({0,0,0,0,0,0,0},'T');
	while (1) {
		std::cin >> hell.op;
		for (size_t i = 0; i < 6; i++)
			std::cin >> hell.j[i];
		std::cin >> hell.g[0];
		auto data = ChangeVersion(hell.ToBinary());
		auto sliped = slip::toSlip(std::vector<unsigned char>(data.begin(), data.end()));
		std::cout << std::string(sliped.begin(), sliped.end()) << std::endl;
		myserial.Write(sliped);
	}

	return 0;
}

int xmain() {

	try {
		kenkyu::BootUuuSetForKekyu();
		//�O���t�B�b�N��p�ӂ���
		kenkyu::InitGraphics();

		kenkyu::log("boot finish completely!");

		//���C�����[�v
		while (uuu::app::PollMesageForBind() && kenkyu::continueLoop) {
			kenkyu::Event();
			kenkyu::Draw();

			//break;
		}

		kenkyu::log("Main loop was broken.");

		kenkyu::Terminate();
		return 0;
	}
	catch (uuu::vr_exception& vrex) {
		kenkyu::log(vrex.what(), kenkyu::logError);
		kenkyu::log("OpenVR/XR�������オ���Ă��Ȃ����w�b�h�Z�b�g�������Ɛڑ�����Ă��Ȃ�����???",kenkyu::logNote);
	}
	catch (exception& ex) {
		kenkyu::log(ex.what(), kenkyu::logError);
	}
	catch (const char* ex) {
		kenkyu::log(ex, kenkyu::logError);
	}
	catch (...) {
		kenkyu::log("any error happened!!!", kenkyu::logError);
	}
	
	kenkyu::log("Errors are happening and break this process.", kenkyu::logError);

	kenkyu::Terminate();
	return -1;
}