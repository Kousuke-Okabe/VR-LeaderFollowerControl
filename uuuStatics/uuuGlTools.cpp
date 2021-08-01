#include <uuu.hpp>

GLuint uuu::glTools::CreateProgramObject(const std::string shaderName) {
	//vertexシェーダーを作る
	GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
	//読み込むんじゃ
	auto vsSource = uuu::commonTools::LoadFileAll(shaderName + ".vertex.glsl");
	auto vsLegSource = vsSource.c_str();
	glShaderSource(vsId, 1, &vsLegSource, NULL);
	//コンパイル
	glCompileShader(vsId);

	//fragmentシェーダーを作る
	GLuint fgId = glCreateShader(GL_FRAGMENT_SHADER);
	//読み込み
	auto fgSource = uuu::commonTools::LoadFileAll(shaderName + ".fragment.glsl");
	auto fgLegSource = fgSource.c_str();
	glShaderSource(fgId, 1, &fgLegSource, NULL);
	//コンパイルするやで
	glCompileShader(fgId);

	//プログラムオブジェクトを作るで
	GLuint progObj = glCreateProgram();
	//アタッチ
	glAttachShader(progObj, vsId);
	glAttachShader(progObj, fgId);

	//リンクする
	glLinkProgram(progObj);

	//std::cout << vsSource << std::endl;
	//std::cout << fgSource << std::endl;

	return progObj;
}

__int8 uuu::glTools::MakeAttribMapNameAndId(std::vector<std::string> names, GLuint progObj, std::unordered_map<std::string, GLuint>& ret) {
	//ret.clear();
	for (const auto i : names)
		(ret)[i] = glGetAttribLocation(progObj, i.c_str());

	return true;

}
__int8 uuu::glTools::MakeUniformMapNameAndId(std::vector<std::string> names, GLuint progObj, std::unordered_map<std::string, GLuint>& ret) {
	//ret.clear();
	for (const auto i : names)
		(ret)[i] = glGetUniformLocation(progObj, i.c_str());

	return true;
}