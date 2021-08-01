#include <uuu.hpp>
 
namespace ct = uuu::commonTools;

std::string uuu::commonTools::LoadFileAll(const std::string path) {
	std::ifstream ifs(path);

	if (!ifs)throw cant_open_file("無効なパスです");

	return std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}
glm::mat4 uuu::commonTools::convertEmat4ToGmat4(const Eigen::Matrix4f& m) {
	glm::mat4 ret;

	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			ret[i][j] = m(i, j);

	return ret;
}
glm::mat4 uuu::commonTools::convertAmat4ToGmat4(const aiMatrix4x4& m) {
	glm::mat4 ret;
	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 4; j++)
			ret[i][j] = m[j][i];

	return ret;
}
aiNode* uuu::commonTools::FindNodeFromName(std::string name, aiNode* target) {

	//ターゲットの名前がそうならリターン
	if (name == target->mName.C_Str())return target;

	//そうじゃなければ子で探す
	for (size_t c = 0; c < target->mNumChildren; c++) {
		aiNode* nowNode = FindNodeFromName(name, target->mChildren[c]);

		//NULLじゃなければ無事発見されたのだ
		if (nowNode != NULL)return nowNode;
	}

	return NULL;
}

glm::quat uuu::commonTools::GetQuatAngleAndAxis(float angle, glm::vec3 axis) {

	float harf = angle / 2.0;
	float angleSin = sin(harf);

	return glm::quat(cos(harf), axis.x*angleSin, axis.y*angleSin, axis.z*angleSin);

}

//クォータニオンから角度を抜き出す
float uuu::commonTools::GetAngleFromQuat(glm::quat& arg) {

	//まずは角度抜き
	float ret = acos(arg.w);
	return isnan(ret) ? 0.0 : ret;


}
//クォータニオンから軸を抜き出す
glm::vec3 uuu::commonTools::GetAxisFromQuat(glm::quat& arg, glm::vec3 optionalV) {

	//まず回転角ゲット
	float ang = uuu::commonTools::GetAngleFromQuat(arg);

	//sin
	float sina = asin(ang);

	glm::vec3 ret = glm::vec3(arg.x / sina, arg.y / sina, arg.z / sina);

	if (isinf(glm::length2(ret)) || isnan(glm::length2(ret)))return optionalV;

	return glm::vec3(arg.x / sina, arg.y / sina, arg.z / sina);
}
glm::quat uuu::commonTools::GetQuatTwoVector(const glm::vec3 a, const glm::vec3 b) {

	//軸
	glm::vec3 axis(1, 0, 0);
	//角度
	glm::vec3::value_type angle = 0;

	//2つのベクトルが等しければおわり
	if (a == b)return glm::quat(0, 0, 0, 1);

	//2つのベクトルが反対ならやっぱおわり
	if (a == -b)return glm::quat(0, 0, 0, -1);

	//軸を計算
	axis = glm::cross(a, b);
	angle = glm::dot(a, b);

	return uuu::commonTools::GetQuatAngleAndAxis(angle, axis);


}

boost::optional<int> uuu::commonTools::atoiEx(const std::string& arg,bool enableException) {

	try {
		if (arg.empty())throw std::invalid_argument("入力が空です");
		//指数オーバーフロー
		if (INT_MAX < pow(10, arg.size() - 1))throw std::logic_error("入力が大きすぎます");

		int ret = 0;

		//符号があるんか
		bool minus = arg.at(0) == '-';

		//指数部分
		int ex = (!minus) ? pow(10, arg.size() - 1) : pow(10, arg.size() - 2);

		for (size_t i = minus; i < arg.size(); i++) {

			unsigned int real = (arg.at(i) - '0');
			if (real > 9)return boost::none;

			int dumpRet = ret + real * ex;

			if (dumpRet < ret)throw std::logic_error("入力が大きすぎます");

			ret = dumpRet;

			//桁下げ
			ex /= 10;

		}
		return minus ? -ret : ret;
	}
	catch (std::exception& ex) {
		if (enableException)throw ex;
		else return boost::none;
	}

	throw std::logic_error("到達不可能です");
	return boost::none;

}
boost::optional<float> uuu::commonTools::atofEx(const std::string& arg, bool enableException) {
	
	try {
		//まず.を除く
		auto splited = uuu::commonTools::split(arg, '.');
		std::string comp;
		for (const auto &i : splited)comp += i;

		auto real = uuu::commonTools::atoiEx(comp, enableException);

		if (real) {
			//価数求めよ
			size_t ePos = arg.find('.', 0);
			if (ePos == std::string::npos)ePos = arg.size() - 1;
			//エラー処理
			if (arg.size() - 1 >= ePos + 1)
				if (arg.find('.', ePos + 1) != std::string::npos)throw("小数点多すぎ");

			int ex = pow(10, (arg.size() - 1) - ePos);
			float exrr = 1.0 / ex;
			float ret = (float)(real.get())*exrr;

			return ret;
		}
		else throw std::invalid_argument("無効な数値です");
	}
	catch (std::exception& ex) {
		if (enableException)throw ex;
		else return boost::none;
	}

	throw std::logic_error("到達不可能です");
	return boost::none;

}

//補助関数
void EulerAnglesToQuaternion(float roll, float pitch, float yaw,
	float& q0, float& q1, float& q2, float& q3)
{
	float cosRoll = cos(roll / 2.0);
	float sinRoll = sin(roll / 2.0);
	float cosPitch = cos(pitch / 2.0);
	float sinPitch = sin(pitch / 2.0);
	float cosYaw = cos(yaw / 2.0);
	float sinYaw = sin(yaw / 2.0);

	q0 = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
	q1 = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
	q2 = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
	q3 = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
}
glm::vec4 uuu::commonTools::convertEulerToQuat(const glm::vec3& m) {

	glm::vec4 ret;
	EulerAnglesToQuaternion(m.x, m.y, m.z, ret.x, ret.y, ret.z, ret.w);

	return ret;
}

std::string uuu::commonTools::toString(glm::vec2 arg) {
	return std::to_string(arg.x) + " " + std::to_string(arg.y);
}
std::string uuu::commonTools::toString(glm::vec3 arg) {
	return std::to_string(arg.x) + " " + std::to_string(arg.y) + " " + std::to_string(arg.z);
}
std::string uuu::commonTools::toString(glm::vec4 arg) {
	return std::to_string(arg.x) + " " + std::to_string(arg.y) + " " + std::to_string(arg.z) + " " + std::to_string(arg.w);
}
std::string uuu::commonTools::toString(glm::quat arg) {
	return std::to_string(arg.x) + " " + std::to_string(arg.y) + " " + std::to_string(arg.z) + " " + std::to_string(arg.w);
}
std::string uuu::commonTools::toString(glm::mat4 arg) {

	const std::string sepa = " ";
	const std::string newline = "\n";

	std::string ret;

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			ret += std::to_string(arg[i][j]);
			if (j < 3)ret += sepa;
		}
		if (i < 3)ret += newline;
	}


	return ret;
}

glm::mat4 uuu::commonTools::GetTranslateMat(const glm::vec3& pvec){

	glm::mat4 junk(glm::identity<glm::mat4>());
	return glm::translate(junk, pvec);

}
glm::mat4 uuu::commonTools::GetRotateMat(const glm::vec3& eular) {

	glm::quat q = glm::identity<glm::quat>();
	return glm::quat(glm::rotate(q, eular)).operator glm::mat<4, 4, glm::f32, glm::packed_highp>();

}
glm::mat4 uuu::commonTools::GetRotateMat(const glm::vec3& axis, const float& angle) {

	glm::quat q = glm::identity<glm::quat>();
	return glm::rotate(q, angle, axis).operator glm::mat<4, 4, glm::f32, glm::packed_highp>();

}
glm::mat4 uuu::commonTools::GetRotateMat(glm::quat qrot) {
	return qrot.operator glm::mat<4, 4, glm::f32, glm::packed_highp>();
}
glm::mat4 uuu::commonTools::GetScaleMat(const float& ratio) {

	glm::mat4 junk(glm::identity<glm::mat4>());
	return glm::scale(junk, glm::vec3(ratio, ratio, ratio));

}
glm::mat4 uuu::commonTools::GetScaleMat(const glm::vec3& ratio) {

	glm::mat4 junk(glm::identity<glm::mat4>());
	return glm::scale(junk, ratio);

}