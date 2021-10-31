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
#include <array>

#include "xbeeForWin.hpp"

namespace kenkyulocal {
	//6関節+1グリッパーをもつアーム用ペイロード
	struct payload_6joint_1grip {
		using word = int16_t;
		using byte = uint8_t;
		word j[6];//ジョイントの角度
		word s[6];//スピード
		word g[1];//グリッパーの角度
		char op;//T トルク,M 角度,(P 目標位置(x,y,z,q,r,s,w)=(j[0:5],g[0]))
	public:
		payload_6joint_1grip(const std::array<word, 7>& qarg, char ops);
		payload_6joint_1grip();
		std::string ToBinary();

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
			ar& s[0];
			ar& s[1];
			ar& s[2];
			ar& s[3];
			ar& s[4];
			ar& s[5];
			ar& g[0];
		}
	};


	std::string ChangeVersion(std::string arg);

	//SLIPプロトコル
	namespace slip {
		const uint8_t END = 0xC0;
		const uint8_t ESC = 0xDB;
		const uint8_t ESC_END = 0xDC;
		const uint8_t ESC_ESC = 0xDD;

		std::vector<unsigned char> toSlip(const std::vector<unsigned char>& buf);

		std::vector<unsigned char> fromSlip(const std::vector<unsigned char>& buf);

	};

	//アームのパケット生成
	class armTransferSlip:public serialMgr {
	protected:
		payload_6joint_1grip data;
		using word = typename payload_6joint_1grip::word;

		void Transfer();
	public:

		armTransferSlip(const std::string& portname);

		//危険状態に全身を脱力する
		void EmergencyCall();
		//全モーターのトルクを有効にする
		void PowerOn();

		//モーターの位置を制御する
		void Move7(const std::array<word, 7>& angles);
	};

};