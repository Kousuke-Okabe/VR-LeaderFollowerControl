#include "kenkyuFrame.hpp"
#include "xbeeForWin.hpp"
#include "armOpeTransfer.hpp"

using namespace std;
using namespace kenkyulocal;

int main() {

	try {
		kenkyu::BootUuuSetForKekyu();
		//グラフィックを用意する
		kenkyu::InitGraphics();

		kenkyu::log("boot finish completely!");

		//メインループ
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
		kenkyu::log("OpenVR/XRが立ち上がっていないかヘッドセットがちゃんと接続されていないかも???",kenkyu::logNote);
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