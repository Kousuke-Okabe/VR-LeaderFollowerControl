#include "armOpeTransfer.hpp"

using namespace kenkyulocal;

payload_6joint_1grip::payload_6joint_1grip(const std::array<word, 7>& qarg, char ops) {
	for (size_t i = 0; i < 6; i++)
		j[i] = qarg[i];

	g[0] = qarg[6];
	op = ops;
}
payload_6joint_1grip::payload_6joint_1grip() {
	op = 0;
}
std::string payload_6joint_1grip::ToBinary() {
	std::stringstream ss;
	boost::archive::text_oarchive archive(ss);
	archive << *this;

	return ss.str();
}

std::string kenkyulocal::ChangeVersion(std::string arg) {
	std::string str1 = std::regex_replace(arg, std::regex("serialization::archive 19"), "serialization::archive 16");
	return str1;
}

std::vector<unsigned char> slip::toSlip(const std::vector<unsigned char>& buf) {
	std::vector<unsigned char> ret;
	ret.reserve(buf.size() + 1);//bufのサイズ以上は確定なのでやっておく

	for (size_t i = 0; i < buf.size(); ++i) {
		uint8_t data = buf.at(i); // i バイト目のデータ

		if (data == END) // データの途中に END があったら
		{
			ret.push_back(ESC);     // ESC で置き換え
			ret.push_back(ESC_END); // ESC_END を追加します
		}
		else if (data == ESC) // データの途中に ESC があったら 
		{
			ret.push_back(ESC);     // ESC で置き換え
			ret.push_back(ESC_ESC); // ESC_ESC を追加します
		}
		else
		{
			ret.push_back(data); // それ以外はそのまま送ります
		}
	}
	ret.push_back(END); // 最後だけENDを送信

	return ret;
}

std::vector<unsigned char> slip::fromSlip(const std::vector<unsigned char>& buf) {
	std::vector<unsigned char> ret;
	for (size_t i = 0; i < buf.size(); ++i) {
		uint8_t data = buf.at(i); // i 番目の受信データ

		if (data == END) // 必ずパケットの終端なはず
		{
		}
		else if (data == ESC) // ESCがあったら次のバイトは変換されているはず
		{
			uint8_t next = buf.at(i + 1); // ESC の次のパケット

			if (next == ESC_END) // ESC の次が ESC_END なら
			{
				ret.push_back(END); // i 番目のバイトは END
			}
			else if (next == ESC_ESC) // ESC の次が ESC_END なら
			{
				ret.push_back(ESC); // i 番目のバイトは ESC
			}
			++i; // next の次のバイトへ
		}
		else
		{
			ret.push_back(data); // それ以外のときはそのまま
		}
	}


	return ret;
}

void armTransferSlip::Transfer() {
	auto bin = ChangeVersion(data.ToBinary());
	auto sliped = slip::toSlip(std::vector<unsigned char>(bin.begin(), bin.end()));
	//std::cout << std::string(sliped.begin(), sliped.end()) << std::endl;
	this->Write(sliped);
}

armTransferSlip::armTransferSlip(const std::string& portname) :serialMgr(portname) {}

//危険状態に全身を脱力する
void armTransferSlip::EmergencyCall() {
	data.op = 'T';
	for (size_t i = 0; i < 6; i++)
		data.j[i] = 0;
	data.g[0] = 0;

	Transfer();

}
//全モーターのトルクを有効にする
void armTransferSlip::PowerOn() {
	data.op = 'T';
	for (size_t i = 0; i < 6; i++)
		data.j[i] = 1;
	data.g[0] = 1;

	Transfer();
}

//モーターの位置を制御する
void armTransferSlip::Move7(const std::array<word, 7>& angles, const std::array<word, 6>& speeds) {
	data.op = 'M';
	for (size_t i = 0; i < 6; i++)
		data.j[i] = angles.at(i);
	for (size_t i = 0; i < 6; i++)
		data.s[i] = speeds.at(i);
	data.g[0] = angles.at(6);

	Transfer();
}

//モーターに目標位置姿勢とグリッパーを送る j[0:5]とs[0]に位置姿勢を格納 gにグリッパー情報を入れる
void armTransferSlip::Posquat(const kenkyu::posAndQuat& posquat, const word& grip) {
	data.op = 'P';
	data.j[0] = posquat.pos.x*10000;
	data.j[1] = posquat.pos.y*10000;
	data.j[2] = posquat.pos.z*10000;
	data.j[3] = posquat.quat.x*10000;
	data.j[4] = posquat.quat.y*10000;
	data.j[5] = posquat.quat.z*10000;
	data.s[0] = posquat.quat.w*10000;

	data.g[0] = grip;

	Transfer();
}

void armTransferSlip::Extra(const char op[6]) {
	data.op = 'X';
	for (size_t i = 0; i < 6; i++)
		data.j[i] = op[i];

	Transfer();
}