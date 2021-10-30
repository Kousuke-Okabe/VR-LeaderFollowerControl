#pragma once
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <initializer_list>
#include <regex>

namespace kenkyulocal {
	//6�֐�+1�O���b�p�[�����A�[���p�y�C���[�h
	struct payload_6joint_1grip {
		using word = uint16_t;
		using byte = uint8_t;
		word j[6];
		word g[1];
		char op;//T �g���N,M �p�x,(P �ڕW�ʒu(x,y,z,q,r,s,w)=(j[0:5],g[0]))
	public:
		payload_6joint_1grip(const std::array<word,7>& qarg,char ops) {
			for (size_t i = 0; i < 6; i++)
				j[i] = qarg[i];

			g[0] = qarg[6];
			op = ops;
		}
		std::string ToBinary() {
			std::stringstream ss;
			boost::archive::text_oarchive archive(ss);
			archive << *this;

			return ss.str();
		}

	private:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar& op;
			ar& j[0];
			ar& j[1];
			ar& j[2];
			ar& j[3];
			ar& j[4];
			ar& j[5];
			ar& g[0];
		}
	};


	std::string ChangeVersion(std::string arg) {
		std::string str1 = std::regex_replace(arg, std::regex("serialization::archive 19"), "serialization::archive 16");
		return str1;
	}

	//SLIP�v���g�R��
	namespace slip {
		const uint8_t END = 0xC0;
		const uint8_t ESC = 0xDB;
		const uint8_t ESC_END = 0xDC;
		const uint8_t ESC_ESC = 0xDD;

		std::vector<unsigned char> toSlip(const std::vector<unsigned char>& buf){
			std::vector<unsigned char> ret;
			ret.reserve(buf.size()+1);//buf�̃T�C�Y�ȏ�͊m��Ȃ̂ł���Ă���

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

		std::vector<unsigned char> fromSlip(const std::vector<unsigned char>& buf){
			std::vector<unsigned char> ret;
			for (size_t i = 0; i < buf.size(); ++i){
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

	};

};