#include <uuu.hpp>

GLuint uuu::glTools::CreateProgramObject(const std::string shaderName) {
	//vertex�V�F�[�_�[�����
	GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
	//�ǂݍ��ނ񂶂�
	auto vsSource = uuu::commonTools::LoadFileAll(shaderName + ".vertex.glsl");
	auto vsLegSource = vsSource.c_str();
	glShaderSource(vsId, 1, &vsLegSource, NULL);
	//�R���p�C��
	glCompileShader(vsId);

	//fragment�V�F�[�_�[�����
	GLuint fgId = glCreateShader(GL_FRAGMENT_SHADER);
	//�ǂݍ���
	auto fgSource = uuu::commonTools::LoadFileAll(shaderName + ".fragment.glsl");
	auto fgLegSource = fgSource.c_str();
	glShaderSource(fgId, 1, &fgLegSource, NULL);
	//�R���p�C��������
	glCompileShader(fgId);

	//�v���O�����I�u�W�F�N�g������
	GLuint progObj = glCreateProgram();
	//�A�^�b�`
	glAttachShader(progObj, vsId);
	glAttachShader(progObj, fgId);

	//�����N����
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