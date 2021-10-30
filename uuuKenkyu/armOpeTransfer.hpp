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
	//6関節+1グリッパーをもつアーム用ペイロード
	struct payload_6joint_1grip {
		using word = uint16_t;
		using byte = uint8_t;
		word j[6];
		word g[1];
		char op;//T トルク,M 角度,(P 目標位置(x,y,z,q,r,s,w)=(j[0:5],g[0]))
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

	//SLIPプロトコル
	namespace slip {
		const uint8_t END = 0xC0;
		const uint8_t ESC = 0xDB;
		const uint8_t ESC_END = 0xDC;
		const uint8_t ESC_ESC = 0xDD;

		std::vector<unsigned char> toSlip(const std::vector<unsigned char>& buf){
			std::vector<unsigned char> ret;
			ret.reserve(buf.size()+1);//bufのサイズ以上は確定なのでやっておく

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

		std::vector<unsigned char> fromSlip(const std::vector<unsigned char>& buf){
			std::vector<unsigned char> ret;
			for (size_t i = 0; i < buf.size(); ++i){
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

	};

};