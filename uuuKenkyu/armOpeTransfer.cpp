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
	ret.reserve(buf.size() + 1);//buf�̃T�C�Y�ȏ�͊m��Ȃ̂ł���Ă���

	for (size_t i = 0; i < buf.size(); ++i) {
		uint8_t data = buf.at(i); // i �o�C�g�ڂ̃f�[�^

		if (data == END) // �f�[�^�̓r���� END ����������
		{
			ret.push_back(ESC);     // ESC �Œu������
			ret.push_back(ESC_END); // ESC_END ��ǉ����܂�
		}
		else if (data == ESC) // �f�[�^�̓r���� ESC ���������� 
		{
			ret.push_back(ESC);     // ESC �Œu������
			ret.push_back(ESC_ESC); // ESC_ESC ��ǉ����܂�
		}
		else
		{
			ret.push_back(data); // ����ȊO�͂��̂܂ܑ���܂�
		}
	}
	ret.push_back(END); // �Ōゾ��END�𑗐M

	return ret;
}

std::vector<unsigned char> slip::fromSlip(const std::vector<unsigned char>& buf) {
	std::vector<unsigned char> ret;
	for (size_t i = 0; i < buf.size(); ++i) {
		uint8_t data = buf.at(i); // i �Ԗڂ̎�M�f�[�^

		if (data == END) // �K���p�P�b�g�̏I�[�Ȃ͂�
		{
		}
		else if (data == ESC) // ESC���������玟�̃o�C�g�͕ϊ�����Ă���͂�
		{
			uint8_t next = buf.at(i + 1); // ESC �̎��̃p�P�b�g

			if (next == ESC_END) // ESC �̎��� ESC_END �Ȃ�
			{
				ret.push_back(END); // i �Ԗڂ̃o�C�g�� END
			}
			else if (next == ESC_ESC) // ESC �̎��� ESC_END �Ȃ�
			{
				ret.push_back(ESC); // i �Ԗڂ̃o�C�g�� ESC
			}
			++i; // next �̎��̃o�C�g��
		}
		else
		{
			ret.push_back(data); // ����ȊO�̂Ƃ��͂��̂܂�
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

//�댯��ԂɑS�g��E�͂���
void armTransferSlip::EmergencyCall() {
	data.op = 'T';
	for (size_t i = 0; i < 6; i++)
		data.j[i] = 0;
	data.g[0] = 0;

	Transfer();

}
//�S���[�^�[�̃g���N��L���ɂ���
void armTransferSlip::PowerOn() {
	data.op = 'T';
	for (size_t i = 0; i < 6; i++)
		data.j[i] = 1;
	data.g[0] = 1;

	Transfer();
}

//���[�^�[�̈ʒu�𐧌䂷��
void armTransferSlip::Move7(const std::array<word, 7>& angles, const std::array<word, 6>& speeds) {
	data.op = 'M';
	for (size_t i = 0; i < 6; i++)
		data.j[i] = angles.at(i);
	for (size_t i = 0; i < 6; i++)
		data.s[i] = speeds.at(i);
	data.g[0] = angles.at(6);

	Transfer();
}

//���[�^�[�ɖڕW�ʒu�p���ƃO���b�p�[�𑗂� j[0:5]��s[0]�Ɉʒu�p�����i�[ g�ɃO���b�p�[��������
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