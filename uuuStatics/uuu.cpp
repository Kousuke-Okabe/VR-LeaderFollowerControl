//���J�O�܂��͓��̃G���[����������ꍇ�́��}�[�N���������Ă݂Ă�������
//���͊Ԃɍ��킹�����������ꏊ�ɂ����Ă��܂�

#include <uuu.hpp>

int uuu::app::bind;
std::vector<GLFWwindow*> uuu::app::windows;
std::chrono::system_clock::time_point uuu::app::originTime;

__int8 uuu::app::InitUUUAndCreateWindow(const windowProp& prop) {
	if (!glfwInit())throw unknown_error("�Ȃ�ł��m��񂯂Ǒ厸�s");
	
	//�p���t���O��false�Ȃ猳�̃q���g�ɖ߂�
	if (!prop.enableInheritanceProp)glfwDefaultWindowHints();

	//�q���g���T�N�T�N����
	if (prop.enableBorder)
		glfwWindowHint(GLFW_DECORATED, prop.enableBorder.value());
	if (prop.enableFocused)
		glfwWindowHint(GLFW_FOCUSED, prop.enableFocused.value());
	if (prop.enableMaximized)
		glfwWindowHint(GLFW_MAXIMIZED, prop.enableMaximized.value());
	if (prop.enableResize)
		glfwWindowHint(GLFW_RESIZABLE, prop.enableResize.value());
	if(prop.enableMultiSampling)
		glfwWindowHint(GLFW_SAMPLES, prop.enableMultiSampling.value());
	if(prop.disableDoubleBuffering)
		glfwWindowHint(GLFW_DOUBLEBUFFER, !prop.disableDoubleBuffering.value());

	//	std::cout << (LoadFileAll(R"(..\shaders\common\test.vertex.glsl)"));

	   // �o�[�W����4.3�w��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(prop.width, prop.height, prop.title.c_str(), prop.monitor, NULL);
	if (!window) {
		glfwTerminate();
		throw unknown_error("�E�B���h�E���Ȃ�������");
	}

	//�ʒu��ݒ�
	if (prop.windowPos)
		glfwSetWindowPos(window, prop.windowPos.value().x, prop.windowPos.value().y);

	//�`��Ώۂ����̃E�B���h�E��
	glfwMakeContextCurrent(window);
#ifndef UUU_DISABLE_GLEW_INIT
	//glew�̏�����
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)return NULL;
#endif
	glfwSwapInterval(1);

	uuu::app::bind = 0;
	uuu::app::windows.push_back(window);

	//�J���[�o�b�t�@�̃N���A�F
	glClearColor(0.2, 0.2, 0.2, 0.2);

	//���������I������̂�UUU�N���������擾
	uuu::app::originTime = std::chrono::system_clock::now();

	return true;
}
__int8 uuu::app::TerminateUUU() {
	glfwTerminate();

	return true;
}
int uuu::app::CreateNewWindow(const windowProp& prop) {

	//�p���t���O��false�Ȃ猳�̃q���g�ɖ߂�
	if (!prop.enableInheritanceProp)glfwDefaultWindowHints();

	//�q���g���T�N�T�N����
	if (prop.enableBorder)
		glfwWindowHint(GL_WRAP_BORDER, prop.enableBorder.value());
	if (prop.enableFocused)
		glfwWindowHint(GLFW_FOCUSED, prop.enableFocused.value());
	if (prop.enableMaximized)
		glfwWindowHint(GLFW_MAXIMIZED, prop.enableMaximized.value());
	if (prop.enableResize)
		glfwWindowHint(GLFW_RESIZABLE, prop.enableResize.value());

	GLFWwindow* window = glfwCreateWindow(prop.width, prop.height, prop.title.c_str(), prop.monitor, NULL);

	if (!window)throw unknown_error("�E�B���h�E���쐬����܂���ł���");

	auto lastInd = uuu::app::windows.size();
	uuu::app::windows.push_back(window);

	return lastInd;

}
__int8 uuu::app::BindWindow(const int ind) {
	if (uuu::app::windows.size() <= ind)throw std::invalid_argument("�E�B���h�E�̐��𒴂��Ă��܂�");

	uuu::app::bind = ind;
	glfwMakeContextCurrent(uuu::app::windows[ind]);

	return true;
}
__int8 uuu::app::BindDefaultWindow() {
	try {
		uuu::app::BindWindow(0);
	}
	catch (std::exception& ptr) {
		throw ptr;
	}
	return true;
}
__int8 uuu::app::PollMesageForBind() {

	glfwPollEvents();
	return !glfwWindowShouldClose(uuu::app::windows[uuu::app::bind]);
}
__int8 uuu::app::UpdateForBind() {
	glfwSwapBuffers(uuu::app::windows[uuu::app::bind]);

	return true;
}
uuu::app::windowProp::windowProp(int _width, int _height, std::string _title, bool Inheritance) {
	enableInheritanceProp = true;

	if (_width == 0 || _height == 0)throw std::invalid_argument("����0�̃E�B���h�E������킯�Ȃ����");

	this->width = _width;
	this->height = _height;
	this->title = _title;
	this->monitor = NULL;
	this->enableInheritanceProp = Inheritance;
}
uuu::app::windowProp::windowProp(int _width, int _height, std::string _title, GLFWmonitor* _monitor, bool Inheritance) {
	enableInheritanceProp = true;
	if (_width == 0 || _height == 0)throw std::invalid_argument("����0�̃E�B���h�E������킯�Ȃ����");
	this->width = _width;
	this->height = _height;
	this->title = _title;
	this->monitor = _monitor;
	this->enableInheritanceProp = Inheritance;
}
__int8 uuu::app::ClearBuffer(int clearTarget) {

	if(clearTarget&uuu::app::bufferType::color)glClear(GL_COLOR_BUFFER_BIT);
	if(clearTarget&uuu::app::bufferType::depth)glClear(GL_DEPTH_BUFFER_BIT);

	return true;

}
__int8 uuu::app::SetEnableDepthBufferFlag(bool enableDepthBuffer) {
	if (enableDepthBuffer)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	return true;
}
__int8 uuu::app::SetEnableCullingFlag(bool enableCulling) {
	if (enableCulling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	return true;
}
__int8 uuu::app::SetAlphaBlendMode(bool enableAlphaBlend, uuu::app::blendType blendModeS, uuu::app::blendType blendModeD) {
	if (enableAlphaBlend) {
		glEnable(GL_BLEND);
		GLenum s, d;
		switch (blendModeS) {
		case uuu::app::blendType::srcAlpha:
			s = GL_SRC_ALPHA;
			break;
		case uuu::app::blendType::one:
			s = GL_ONE;
			break;
		case uuu::app::blendType::oneMinusSrcAlpha:
			s = GL_ONE_MINUS_SRC_ALPHA;
			break;
		default:
			throw std::invalid_argument("���������`���N�`���ł�");
		}
		switch (blendModeD) {
		case uuu::app::blendType::srcAlpha:
			d = GL_SRC_ALPHA;
			break;
		case uuu::app::blendType::one:
			d = GL_ONE;
			break;
		case uuu::app::blendType::oneMinusSrcAlpha:
			d = GL_ONE_MINUS_SRC_ALPHA;
			break;
		default:
			throw std::invalid_argument("���������`���N�`���ł�");
		}
		glBlendFunc(s, d);
	}
	else glDisable(GL_BLEND);

	return false;
}
__int8 uuu::app::UseMultiSampling(bool enableAntiAlias) {
	
	if (enableAntiAlias)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);

	return true;
}

GLFWwindow* uuu::app::GetBindWindow() {
	return uuu::app::windows.at(uuu::app::bind);
}

size_t uuu::app::GetTimeFromInit() {
	//���ݎ���
	std::chrono::system_clock::time_point nowP = std::chrono::system_clock::now();
	//���ԃQ�b�g
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(nowP - uuu::app::originTime);

	return dur.count();
}

GLFWwindow* uuu::InitLibsAndCreateWindow() {
	if (!glfwInit())return NULL;
	GLFWwindow* window = glfwCreateWindow(33 * 20, 4 * 20, "test", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return NULL;
	}

	//	std::cout << (LoadFileAll(R"(..\shaders\common\test.vertex.glsl)"));

		//�`��Ώۂ����̃E�B���h�E��
	glfwMakeContextCurrent(window);

	//glew�̏�����
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)return NULL;

	glfwSwapInterval(1);

	return window;
}
__int8 uuu::PollWindowMessage() {
	return true;
}

uuu::shaderProgramObjectVertexFragment::shaderProgramObjectVertexFragment() {
	this->attribNameIndexMap.clear();
	this->uniformNameIndexMap.clear();
}
uuu::shaderProgramObjectVertexFragment::shaderProgramObjectVertexFragment(const std::string shaderName):uuu::shaderProgramObjectVertexFragment::shaderProgramObjectVertexFragment(){
	this->LoadSource(shaderName);

	return;
}
__int8 uuu::shaderProgramObjectVertexFragment::LoadSource(const std::string shaderName) {
	this->progObj = uuu::glTools::CreateProgramObject(shaderName);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::CreateAttribMap(const std::vector<std::string>& attribNames) {
	uuu::glTools::MakeAttribMapNameAndId(attribNames, this->progObj, this->attribNameIndexMap);

	//for (const auto& i : this->attribNameIndexMap)
	//	glEnableVertexAttribArray(i.second);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::CreateUniformMap(const std::vector<std::string>& uniformNames) {
	uuu::glTools::MakeUniformMapNameAndId(uniformNames, this->progObj, this->uniformNameIndexMap);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::Bind() {
	glUseProgram(this->progObj);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::Unbind() {
	glUseProgram(0);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformInt(std::string name, int value) {
	this->Bind();

	auto i = this->uniformNameIndexMap.find(name);
	//������Ȃ���΃I�[�g����
	if (i == this->uniformNameIndexMap.end())this->CreateUniformMap({ name });

	glUniform1i(this->uniformNameIndexMap.at(name), value);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformValue(std::string name, double val) {
	this->Bind();

	auto i = this->uniformNameIndexMap.find(name);
	//������Ȃ���΃I�[�g����
	if (i == this->uniformNameIndexMap.end())this->CreateUniformMap({ name });

	glUniform1d(this->uniformNameIndexMap.at(name), val);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformValue(std::string name, float val) {
	this->Bind();

	auto i = this->uniformNameIndexMap.find(name);
	//������Ȃ���΃I�[�g����
	if (i == this->uniformNameIndexMap.end())this->CreateUniformMap({ name });

	glUniform1f(this->uniformNameIndexMap.at(name), val);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformValue(std::string name, glm::dmat4 val) {
	this->Bind();

	auto i = this->uniformNameIndexMap.find(name);
	//������Ȃ���΃I�[�g����
	if (i == this->uniformNameIndexMap.end())this->CreateUniformMap({ name });

	glUniformMatrix4dv(this->uniformNameIndexMap.at(name), 1, GL_FALSE, &val[0][0]);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformValue(std::string name, glm::mat4 val) {
	this->Bind();

	auto i = this->uniformNameIndexMap.find(name);
	//������Ȃ���΃I�[�g����
	if (i == this->uniformNameIndexMap.end())this->CreateUniformMap({ name });

	glUniformMatrix4fv(this->uniformNameIndexMap.at(name), 1, GL_FALSE, &val[0][0]);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformValue(std::string name, glm::vec2 val) {
	this->Bind();

	auto i = this->uniformNameIndexMap.find(name);
	//������Ȃ���΃I�[�g����
	if (i == this->uniformNameIndexMap.end())this->CreateUniformMap({ name });

	//glUniformMatrix4fv(this->uniformNameIndexMap.at(name), 1, GL_TRUE, &val[0][0]);
	glUniform2f(this->uniformNameIndexMap.at(name), val.x, val.y);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformValue(std::string name, glm::vec4 val) {
	this->Bind();

	auto i = this->uniformNameIndexMap.find(name);
	//������Ȃ���΃I�[�g����
	if (i == this->uniformNameIndexMap.end())this->CreateUniformMap({ name });

	//glUniformMatrix4fv(this->uniformNameIndexMap.at(name), 1, GL_TRUE, &val[0][0]);
	glUniform4f(this->uniformNameIndexMap.at(name), val.x, val.y, val.z, val.w);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformArray(std::string name, const std::vector<glm::mat4>& val) {

	this->Bind();

	auto i = this->uniformNameIndexMap.find(name);
	//������Ȃ���΃I�[�g����
	if (i == this->uniformNameIndexMap.end())this->CreateUniformMap({ name });

	glUniformMatrix4fv(this->uniformNameIndexMap.at(name), val.size(), GL_TRUE, &val[0][0][0]);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformCameraPersp(std::string name, const uuu::cameraPersp& val) {

	this->SetUniformValue(name+"Projection", val.persp);
	this->SetUniformValue(name+"LookAt", val.lookAt);

	return true;

}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformCameraPersp(const uuu::cameraPersp& val) {
	this->SetUniformValue("projection", val.persp);
	this->SetUniformValue("lookAt", val.lookAt);

	return true;
}
__int8 uuu::shaderProgramObjectVertexFragment::SetUniformTexUnit(std::string name, const uuu::textureOperator& val) {

	return this->SetUniformInt(name, val.texUnit);

}

uuu::vaoOperator::vaoOperator() {
	this->modelTransform = glm::identity<glm::mat4>();

	//�ǂ�������������vao�͂��߂������Ƃ���
	this->CreateVao();

	return;
}
__int8 uuu::vaoOperator::CreateVao() {
	glGenVertexArrays(1, &this->vaoId);

	return true;
}
__int8 uuu::vaoOperator::Bind() {
	glBindVertexArray(this->vaoId);

	return true;
}
__int8 uuu::vaoOperator::Unbind() {
	glBindVertexArray(0);

	return true;
}
GLuint uuu::vaoOperator::CreateIndexBuffer(std::vector<unsigned int>& indices) {
	//�܂���vao���o�C���h
	glBindVertexArray(this->vaoId);

	//�o�b�t�@���m��
	GLuint indiceBuffer;
	glGenBuffers(1, &indiceBuffer);

	//�o�b�t�@���G�������g�Ƃ��ăo�C���h&�f�[�^�]��
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//�G�������g��ۑ�������
	this->elementBuffer = std::make_pair(indiceBuffer, indices.size());

	return this->elementBuffer.value().first;
}
__int8 uuu::vaoOperator::DrawVaoUsingElement() {
	//vao�o�C���h
	this->Bind();
	//�C���f�b�N�X�Ɏ��g�̃C���f�b�N�X���o�C���h
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer.value().first);
	//�h���[�R�[��
	glDrawElements(GL_TRIANGLES, this->elementBuffer.value().second, GL_UNSIGNED_INT, NULL);

	return true;
}

GLuint uuu::vaoOperator::CreateVertexAttribVboForGlmDvec4(GLuint attribId, std::vector<glm::dvec4> & datas) {
	glBindVertexArray(this->vaoId);

	GLuint newVbo;
	glGenBuffers(1, &newVbo);
	glBindBuffer(GL_ARRAY_BUFFER, newVbo);

	//�A�g���r���[�g��newVbo��R�Â�
	glEnableVertexAttribArray(attribId);

	//�f�[�^�]��
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::dvec4)*datas.size(), datas.data(), GL_STATIC_DRAW);

	//�t�H�[�}�b�g
	glVertexAttribPointer(attribId, 4, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), NULL);

	//�}�b�v�ɓo�^
	this->attribVaoMap[attribId] = newVbo;

	return newVbo;
}
GLuint uuu::vaoOperator::CreateVertexAttribVboForGlmDvec4(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::dvec4>& datas) {
	auto i = shader.attribNameIndexMap.find(name);
	//�����Ȃ���΂��킨��� ����Ȃ��ăI�[�g����
	if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ name });

	return this->CreateVertexAttribVboForGlmDvec4(shader.attribNameIndexMap.at(name), datas);
}
GLuint uuu::vaoOperator::CreateVertexAttribVboForGlmVec4(GLuint attribId, std::vector<glm::vec4> & datas) {
	glBindVertexArray(this->vaoId);

	GLuint newVbo;
	glGenBuffers(1, &newVbo);
	glBindBuffer(GL_ARRAY_BUFFER, newVbo);

	//�A�g���r���[�g��newVbo��R�Â�
	glEnableVertexAttribArray(attribId);

	//�f�[�^�]��
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*(datas.size()), datas.data()+sizeof(glm::vec4)*0, GL_STATIC_DRAW);

	//�t�H�[�}�b�g
	glVertexAttribPointer(attribId, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);

	//�}�b�v�ɓo�^
	this->attribVaoMap[attribId] = newVbo;

	return newVbo;
}
GLuint uuu::vaoOperator::CreateDynamicVertexAttribVboForGlmVec4(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec4>& datas) {
	auto i = shader.attribNameIndexMap.find(name);
	//�����Ȃ���΂��킨���@����Ȃ��ăI�[�g����
	if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ name });

	return this->CreateDynamicVertexAttribVboForGlmVec4(shader.attribNameIndexMap.at(name), datas);
}
GLuint uuu::vaoOperator::CreateDynamicVertexAttribVboForGlmVec4(GLuint attribId, std::vector<glm::vec4> & datas) {
	glBindVertexArray(this->vaoId);

	GLuint newVbo;
	glGenBuffers(1, &newVbo);
	glBindBuffer(GL_ARRAY_BUFFER, newVbo);

	//�A�g���r���[�g��newVbo��R�Â�
	glEnableVertexAttribArray(attribId);

	//�f�[�^�]��
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*(datas.size()), datas.data() + sizeof(glm::vec4) * 0, GL_DYNAMIC_DRAW);

	//�t�H�[�}�b�g
	glVertexAttribPointer(attribId, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);

	//�}�b�v�ɓo�^
	this->attribVaoMap[attribId] = newVbo;

	return newVbo;
}
GLuint uuu::vaoOperator::CreateVertexAttribVboForGlmVec4(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec4>& datas) {
	auto i = shader.attribNameIndexMap.find(name);
	//�����Ȃ���΂��킨���@����Ȃ��ăI�[�g����
	if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ name });

	return this->CreateVertexAttribVboForGlmVec4(shader.attribNameIndexMap.at(name), datas);
}
GLuint uuu::vaoOperator::UpdateVertexAttribVboForGlmVec4(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec4>& datas) {
	auto i = shader.attribNameIndexMap.find(name);
	//�����Ȃ���΂��킨���@����Ȃ��ăI�[�g����
	if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ name });

	return this->UpdateVertexAttribVboForGlmVec4(shader.attribNameIndexMap.at(name), datas);
}
GLuint uuu::vaoOperator::UpdateVertexAttribVboForGlmVec4(GLuint attribId, std::vector<glm::vec4> & datas) {

	//vao���o�C���h
	glBindVertexArray(this->vaoId);

	//vbo�𔲂��o��
	GLuint vbo = this->attribVaoMap.at(attribId);

	//���������o�C���h
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//�f�[�^�]��(�X�V)
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*(datas.size()), datas.data() + sizeof(glm::vec4) * 0);

	return vbo;

}
__int8 uuu::vaoOperator::CreateVertexAttribVboFromBoneInfo(const std::vector<GLuint>& attribIdIDs,const std::vector<GLuint>& attribIdweights, boneInfo datas) {
	
	//�`�F�P���b�`��
	if (attribIdIDs.size() < *datas.first)throw std::out_of_range("�����̃A�g���r���[�gID�̐������Ȃ����܂�");
	if (attribIdweights.size() < *datas.first)throw std::out_of_range("�����̃A�g���r���[�gID�̐������Ȃ����܂�");
	glBindVertexArray(this->vaoId);

	GLuint idsVbo, weightsVbo;

	//stride�̂Ԃ��
	for (size_t s = 0; s < *datas.first; s++) {
		//�܂���id����
		//vbo���
		glGenBuffers(1, &idsVbo);
		//�o�C���h
		glBindBuffer(GL_ARRAY_BUFFER, idsVbo);
		//�A�g���r���[�g��newVbo��R�Â�
		glEnableVertexAttribArray(attribIdIDs[s]);

		//id�f�[�^�]�� �T�C�Y��stride���̈� �f�[�^�̓T�C�Y��stride���̈��s�{
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*(datas.second.first->size()/(*datas.first)), &datas.second.first->at((datas.second.first->size() / (*datas.first))*s), GL_STATIC_DRAW);
		//�t�H�[�}�b�g�͕��ʂɂł���
		glVertexAttribPointer(attribIdIDs[s], 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);

		//�}�b�s���O���Y���Ȃ���
		this->attribVaoMap[attribIdIDs[s]] = idsVbo;

		//�E�F�C�g�������悤��
		glGenBuffers(1, &weightsVbo);
		//�o�C���h
		glBindBuffer(GL_ARRAY_BUFFER, weightsVbo);
		//�A�g���r���[�g��newVbo��R�Â�
		glEnableVertexAttribArray(attribIdweights[s]);

		//weight�f�[�^�]�� �T�C�Y��stride���̈� �f�[�^�̓T�C�Y��stride���̈��s�{
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*(datas.second.second->size() / (*datas.first)), &datas.second.second->at((datas.second.second->size() / (*datas.first))*s), GL_STATIC_DRAW);
		//�t�H�[�}�b�g�͕��ʂɂł���
		glVertexAttribPointer(attribIdweights[s], 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);

		//�}�b�s���O���Y���Ȃ���
		this->attribVaoMap[attribIdweights[s]] = weightsVbo;
	}

	return true;
}
__int8 uuu::vaoOperator::CreateVertexAttribVboFromBoneInfo(shaderProgramObjectVertexFragment& shader,const std::vector<std::string> namesID, const std::vector<std::string> namesWeight, boneInfo datas) {

	//��������
	if (namesID.size() < *datas.first)throw std::invalid_argument("���O�����Ȃ����܂�");
	if (namesWeight.size() < *datas.first)throw std::invalid_argument("���O�����Ȃ����܂�");

	//�z���g�̈���
	std::vector<GLuint> argID;

	for (const std::string &n : namesID) {
		auto i = shader.attribNameIndexMap.find(n);
		//�����Ȃ���΂��킨���@����Ȃ��ăI�[�g����
		if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ n });

		//�����ɋl�߂�
		argID.push_back(shader.attribNameIndexMap.at(n));
	}

	std::vector<GLuint> argWeight;

	for (const std::string &n : namesWeight) {
		auto i = shader.attribNameIndexMap.find(n);
		//�����Ȃ���΂��킨���@����Ȃ��ăI�[�g����
		if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ n });

		//�����ɋl�߂�
		argWeight.push_back(shader.attribNameIndexMap.at(n));
	}

	return this->CreateVertexAttribVboFromBoneInfo(argID,argWeight, datas);
}
__int8 uuu::vaoOperator::CreateVertexAttribVboFromBoneInfoAutoNaming(shaderProgramObjectVertexFragment& shader, const std::string nameGenID, const std::string nameGenWeight, boneInfo datas) {
	std::vector<std::string> argID;

	//stride�̐���
	for (size_t s = 0; s < *datas.first; s++)
		argID.push_back(nameGenID + std::to_string(s));

	std::vector<std::string> argWeight;

	//strWeighte�̐���
	for (size_t s = 0; s < *datas.first; s++)
		argWeight.push_back(nameGenWeight + std::to_string(s));

	return this->CreateVertexAttribVboFromBoneInfo(shader, argID,argWeight, datas);
}
GLuint uuu::vaoOperator::CreateVertexAttribVboForGlmVec2(GLuint attribId, std::vector<glm::vec2> & datas) {
	glBindVertexArray(this->vaoId);

	GLuint newVbo;
	glGenBuffers(1, &newVbo);
	glBindBuffer(GL_ARRAY_BUFFER, newVbo);

	//�A�g���r���[�g��newVbo��R�Â�
	glEnableVertexAttribArray(attribId);

	//�f�[�^�]��
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*(datas.size()), datas.data(), GL_STATIC_DRAW);

	//�t�H�[�}�b�g
	glVertexAttribPointer(attribId, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), NULL);

	//�}�b�v�ɓo�^
	this->attribVaoMap[attribId] = newVbo;

	return newVbo;
}
GLuint uuu::vaoOperator::CreateVertexAttribVboForGlmVec2(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec2>& datas) {
	auto i = shader.attribNameIndexMap.find(name);
	//�����Ȃ���΂��킨���@����Ȃ��ăI�[�g����
	if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ name });

	return this->CreateVertexAttribVboForGlmVec2(shader.attribNameIndexMap.at(name), datas);
}
GLuint uuu::vaoOperator::CreateDynamicVertexAttribVboForGlmVec2(GLuint attribId, std::vector<glm::vec2> & datas) {
	glBindVertexArray(this->vaoId);

	GLuint newVbo;
	glGenBuffers(1, &newVbo);
	glBindBuffer(GL_ARRAY_BUFFER, newVbo);

	//�A�g���r���[�g��newVbo��R�Â�
	glEnableVertexAttribArray(attribId);

	//�f�[�^�]��
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*(datas.size()), datas.data(), GL_DYNAMIC_DRAW);

	//�t�H�[�}�b�g
	glVertexAttribPointer(attribId, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), NULL);

	//�}�b�v�ɓo�^
	this->attribVaoMap[attribId] = newVbo;

	return newVbo;
}
GLuint uuu::vaoOperator::CreateDynamicVertexAttribVboForGlmVec2(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec2>& datas) {
	auto i = shader.attribNameIndexMap.find(name);
	//�����Ȃ���΂��킨���@����Ȃ��ăI�[�g����
	if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ name });

	return this->CreateDynamicVertexAttribVboForGlmVec2(shader.attribNameIndexMap.at(name), datas);
}
GLuint uuu::vaoOperator::UpdateVertexAttribVboForGlmVec2(GLuint attribId, std::vector<glm::vec2> & datas) {

	//vao���o�C���h
	glBindVertexArray(this->vaoId);

	//vbo�𔲂��o��
	GLuint vbo = this->attribVaoMap.at(attribId);

	//���������o�C���h
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//�f�[�^�]��(�X�V)
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2)*(datas.size()), datas.data() + sizeof(glm::vec2) * 0);

	return vbo;

}
GLuint uuu::vaoOperator::UpdateVertexAttribVboForGlmVec2(shaderProgramObjectVertexFragment& shader, std::string name, std::vector<glm::vec2>& datas) {
	auto i = shader.attribNameIndexMap.find(name);
	//�����Ȃ���΂��킨���@����Ȃ��ăI�[�g����
	if (i == shader.attribNameIndexMap.end())shader.CreateAttribMap({ name });

	return this->UpdateVertexAttribVboForGlmVec2(shader.attribNameIndexMap.at(name), datas);
}

GLuint uuu::vaoOperator::CreateVertexAttribVboDefaultNameFromModelLoader(uuu::shaderProgramObjectVertexFragment& prog,uuu::modelLoaderSerializer& mod, int attribs) {

	//���ꂼ��̖��O
#define __nPOSITION "position"
#define __nNORNAL "normal"
//#define __nCOLOR "color"
#define __nTEXCOORD0 "texCoord0"
#define __nTEXCOORD1 "texCoord1"
#define __nTEXCOORD2 "texCoord2"
#define __nTEXCOORD3 "texCoord3"
#define __nBONEINFO_W "boneWeight"
#define __nBONEINFO_I "boneId"

	//�ʒu���I�[�g�l�[�~���O
	if (attribs&attribNodeName::POSITION)
		this->CreateVertexAttribVboForGlmVec4(prog, __nPOSITION, *mod.GetVerticesList());
	if (attribs&attribNodeName::NORMAL)
		this->CreateVertexAttribVboForGlmVec4(prog, __nNORNAL, *mod.GetNormalsList());
	//if (attribs&attribNodeName::NORMAL)
	//	this->CreateVertexAttribVboForGlmVec4(prog, __nCOLOR, *mod.get);
	if (attribs&attribNodeName::TEX_COORD0)
		this->CreateVertexAttribVboForGlmVec2(prog, __nTEXCOORD0, *mod.GetTexCoordsList(0));
	if (attribs&attribNodeName::TEX_COORD1)
		this->CreateVertexAttribVboForGlmVec2(prog, __nTEXCOORD1, *mod.GetTexCoordsList(1));
	if (attribs&attribNodeName::TEX_COORD2)
		this->CreateVertexAttribVboForGlmVec2(prog, __nTEXCOORD2, *mod.GetTexCoordsList(2));
	if (attribs&attribNodeName::TEX_COORD3)
		this->CreateVertexAttribVboForGlmVec2(prog, __nTEXCOORD3, *mod.GetTexCoordsList(3));

	if (attribs&attribNodeName::BONE_INFO)
		this->CreateVertexAttribVboFromBoneInfoAutoNaming(prog, std::string(__nBONEINFO_I), std::string(__nBONEINFO_W), mod.GetBoneInfosList());

	if(attribs&attribNodeName::INDICES)
		this->CreateIndexBuffer(*mod.GetIndicesList());

	return 0;
}

uuu::cameraPersp::cameraPersp() {
	this->lookAt = glm::identity<glm::mat4>();
	this->persp = glm::identity<glm::mat4>();

	return;
}
__int8 uuu::cameraPersp::SetPerspMatrix(glm::mat4& p) {
	this->persp = p;
	return true;
}
__int8 uuu::cameraPersp::SetPersp(float near, float far, float fov, float aspect) {
	glm::mat4 p = glm::perspective(fov, aspect, near, far);
	
	return this->SetPerspMatrix(p);
}
__int8 uuu::cameraPersp::SetLookAtMatrix(glm::mat4& l) {
	this->lookAt = l;
	return true;
}
__int8 uuu::cameraPersp::SetLookAt(glm::vec3 eye, glm::vec3 tar, glm::vec3 up) {
	glm::mat4 l = glm::lookAt(eye, tar, up);

	return this->SetLookAtMatrix(l);
}

uuu::modelLoaderSerializer::modelLoaderSerializer() {
	this->loader = new Assimp::Importer();
	this->weightsList.clear();
	this->boneIdsList.clear();
	this->texCoordsList.resize(TEXCOORD_CHANNEL_MAX);
}
uuu::modelLoaderSerializer::modelLoaderSerializer(const std::string path, const std::string meshName,unsigned int flag) :uuu::modelLoaderSerializer::modelLoaderSerializer() {
	this->LoadModelFromFile(path, meshName,flag);

	return;
}
uuu::modelLoaderSerializer::modelLoaderSerializer(const std::string path, size_t index,unsigned int flag) : uuu::modelLoaderSerializer::modelLoaderSerializer() {
	this->LoadModelFromFile(path, index,flag);

	return;
}
uuu::modelLoaderSerializer::~modelLoaderSerializer() {
	delete this->loader;
}
__int8 uuu::modelLoaderSerializer::LoadModelFromFile(std::string path,size_t index,unsigned int flag) {
	this->scene = this->loader->ReadFile(path, flag);
	this->mesh = this->scene->mMeshes[index];

	return true;
}
__int8 uuu::modelLoaderSerializer::LoadModelFromFile(const std::string path, const std::string meshName, unsigned int flag) {
	this->scene = this->loader->ReadFile(path, flag);

	if (!scene)throw std::runtime_error("Cannot load "+path);

	//���b�V���T�� �������󔒂�0�Ԗڂ̃��b�V�����[�h
	if(meshName.empty()) {
		this->mesh = this->scene->mMeshes[0];
		std::cout << "uuu warning\"Unnamed mesh is loaded! First mesh at "+path+" is loaded as this." << std::endl;
		return true;
	}

	for (size_t m = 0; m < scene->mNumMeshes; m++) {
		if (meshName == scene->mMeshes[m]->mName.C_Str()) {
			this->mesh = this->scene->mMeshes[m];

			return true;
		}
	}

	throw std::invalid_argument("����Ȗ��O�̃��b�V���͂Ȃ��ł�");

	return false;
}
__int8 uuu::modelLoaderSerializer::LoadModelFromScene(const aiScene const* scene, size_t index) {
	this->scene = scene;
	this->mesh = this->scene->mMeshes[index];

	return true;
}

std::vector<glm::vec4>* uuu::modelLoaderSerializer::GetVerticesList(bool checkMeshUpdate) {

	//���łɒ��_���X�g���X�V����Ă邩�m�F����
	if (checkMeshUpdate)
		if (!this->verticesList.empty())return &this->verticesList;

	//������
	this->verticesList.clear();
	this->verticesList.resize(this->mesh->mNumVertices);


	for (size_t v = 0; v < this->verticesList.size(); v++)
		this->verticesList[v] = glm::vec4(this->mesh->mVertices[v].x, this->mesh->mVertices[v].y, this->mesh->mVertices[v].z, 1.0);

	return &this->verticesList;
}
std::vector<glm::vec4>* uuu::modelLoaderSerializer::GetNormalsList(bool checkMeshUpdate) {
	//���łɖ@�����X�g���X�V����Ă邩�m�F����
	if (checkMeshUpdate)
		if (!this->normalsList.empty())return &this->normalsList;

	//�������m��
	this->normalsList.clear();
	this->normalsList.resize(this->mesh->mNumVertices);

	//�@���̓x�N�g����w=0
	for (size_t n = 0; n < this->mesh->mNumVertices; n++)
		this->normalsList[n] = glm::vec4(this->mesh->mNormals[n].x, this->mesh->mNormals[n].y, this->mesh->mNormals[n].z, 0.0);

	return &this->normalsList;
}
std::vector<unsigned int>* uuu::modelLoaderSerializer::GetIndicesList(bool checkMeshUpdate) {

	//���łɃC���f�b�N�X���X�g���X�V����Ă��邩�m�F����
	if (checkMeshUpdate)
		if (!this->indicesList.empty())return &this->indicesList;

	//�m��
	this->indicesList.clear();
	this->indicesList.resize(this->mesh->mNumFaces * 3);

	for (size_t f = 0; f < this->mesh->mNumFaces; f++)
		for (size_t i = 0; i < 3; i++)
			this->indicesList[f * 3 + i] = this->mesh->mFaces[f].mIndices[i];

	return &this->indicesList;
}
std::vector<glm::vec2>* uuu::modelLoaderSerializer::GetTexCoordsList(size_t channel,bool checkMeshUpdate) {

	//���łɖ@�����X�g���X�V����Ă邩�m�F����
	if (checkMeshUpdate)
		//�`�����l��������Ă��邩
		if (this->texCoordsList.size() > channel)
			if (this->texCoordsList[channel])return this->texCoordsList[channel];

	std::vector<glm::vec2>* tar = new std::vector<glm::vec2>;
	this->texCoordsList[channel] = (tar);

	tar->resize(this->mesh->mNumVertices);
	//���_����J��Ԃ�
	for (size_t t = 0; t < this->mesh->mNumVertices; t++)
		tar->at(t) = glm::vec2(this->mesh->mTextureCoords[channel][t].x, this->mesh->mTextureCoords[channel][t].y);
	//�����
	return this->texCoordsList[channel];
}
boneInfo uuu::modelLoaderSerializer::GetBoneInfosList(bool checkMeshUpdate) {
	//���łɃC���f�b�N�X���X�g���X�V����Ă��邩�m�F����
	if (checkMeshUpdate) {
		if (!this->weightsList.empty())
			if (!this->boneIdsList.empty())
				return std::make_pair(&this->boneInfosStride,std::make_pair(&this->boneIdsList, &this->weightsList));

		//���_�ɂ������Ă����̃E�F�C�g���ǉ�����Ă��邩�����̍s��̍ő�l
		std::vector<unsigned int> bufferNumVertexWeights;
		size_t maxBufferNumVertexWeights = 0;
		bufferNumVertexWeights.resize(this->mesh->mNumVertices);
		for (auto &i : bufferNumVertexWeights)i = 0;

		//���z�������Ă����ɂ��[�߂���V�X�e��
		std::vector<std::vector<float>> boneIds;
		std::vector<std::vector<float>> weights;

		//���z����m��
		boneIds.resize(this->mesh->mNumVertices);
		weights.resize(this->mesh->mNumVertices);

		//�{�[���̐��J��Ԃ����
		for(size_t b=0;b<this->mesh->mNumBones;b++)
			//�E�F�C�g�̐��J��Ԃ����
			for (size_t w = 0; w < this->mesh->mBones[b]->mNumWeights; w++) {
				//���̏����ɂ����钸�_�̃C���f�b�N�X
				size_t ind = this->mesh->mBones[b]->mWeights[w].mVertexId;

				//�E�F�C�g����������
				weights[ind].push_back(this->mesh->mBones[b]->mWeights[w].mWeight);
				//���̂Ƃ��̃{�[��id "b"
				boneIds[ind].push_back(b);

				//��J�L�R�����̂�bufferNumVertexWeights������₷
				bufferNumVertexWeights[ind]++;
				//�}�b�N�X�𒴂��Ă��邩�`�F�b�N
				maxBufferNumVertexWeights = maxBufferNumVertexWeights < bufferNumVertexWeights[ind] ? bufferNumVertexWeights[ind] : maxBufferNumVertexWeights;
			}

		//���s��Ƀp���p���ɋl�܂�����
		//���Ɏ��ۂɃ����o������������
		this->boneInfosStride = ceil((float)maxBufferNumVertexWeights / 4.0);

		this->boneIdsList.clear();
		this->boneIdsList.resize(this->mesh->mNumVertices*this->boneInfosStride);

		this->weightsList.clear();
		this->weightsList.resize(this->mesh->mNumVertices*this->boneInfosStride);

		//stride�̂�����
		for(size_t s=0;s<this->boneInfosStride;s++)
			//���_�̐���
			for(size_t v=0;v< this->mesh->mNumVertices;v++)
				//4��(�x�N�g���̗v�f����)��
				for (size_t n = 0; n < 4; n++) {
					//�{�[��id����
					this->boneIdsList[v + this->mesh->mNumVertices*s][n] = (boneIds[v].size() <= n + (s * 4)) ? 0 : boneIds[v][n + (s * 4)];
					//�E�F�C�g��
					this->weightsList[v + this->mesh->mNumVertices*s][n] = (weights[v].size() <= n + (s * 4)) ? 0.0 : weights[v][n + (s * 4)];
				}

		//���ʋl�߂Ă����@��
		return std::make_pair(&this->boneInfosStride, std::make_pair(&this->boneIdsList, &this->weightsList));
	}
	return std::make_pair(&this->boneInfosStride, std::make_pair(&this->boneIdsList, &this->weightsList));
}

uuu::uud::boneStructer::boneStructer(const aiScene const* scene, aiMesh* mesh) {

	this->m_pScene = scene;
	this->m_pMesh = mesh;

	this->m_rootInverse = uuu::commonTools::convertAmat4ToGmat4(scene->mRootNode->mTransformation);
	this->m_rootInverse[3][3] = 1.0;
	this->m_rootInverse = glm::inverse(this->m_rootInverse);

	this->boneIndexMap.clear();

	this->boneOffsets.resize(mesh->mNumBones);
	this->finalTransform.resize(mesh->mNumBones);
	for (size_t i = 0; i < mesh->mNumBones; i++) {
		this->boneIndexMap[mesh->mBones[i]->mName.C_Str()] = i;
		this->boneOffsets[i] = uuu::commonTools::convertAmat4ToGmat4(mesh->mBones[i]->mOffsetMatrix);
		this->boneOffsets[i][3][3] = 1.0;
	}

}
uuu::uud::boneStructer::boneStructer(const uuu::modelLoaderSerializer& loader):uuu::uud::boneStructer::boneStructer(loader.scene, loader.mesh) {}
aiAnimation* uuu::uud::boneStructer::GetAnimationPtr(size_t index) {
	if (this->m_pScene->mNumAnimations <= index)throw std::out_of_range("�A�j���[�V�����̍ő吔�������������Ă��܂�");
	return this->m_pScene->mAnimations[index];
}
size_t uuu::uud::boneStructer::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	for (size_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
size_t uuu::uud::boneStructer::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (size_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
size_t uuu::uud::boneStructer::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (size_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

void uuu::uud::boneStructer::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	size_t PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	size_t NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}
void uuu::uud::boneStructer::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	size_t RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	size_t NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}
void uuu::uud::boneStructer::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	size_t ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	size_t NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

const aiNodeAnim* uuu::uud::boneStructer::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (size_t i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}
void uuu::uud::boneStructer::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform, aiAnimation* anim) {
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = anim;

	glm::mat4 NodeTransformation(uuu::commonTools::convertAmat4ToGmat4(pNode->mTransformation));
	NodeTransformation[3][3] = 1.0;//��

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM;
		ScalingM = glm::scale(glm::identity<glm::mat4>(), glm::vec3(Scaling.x, Scaling.y, Scaling.z));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		glm::mat4 RotationM = glm::mat4(glm::quat(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z));//glm::mat4(RotationQ.GetMatrix())

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM;
		TranslationM = glm::translate(glm::identity<glm::mat4>(), glm::vec3(Translation.x, Translation.y, Translation.z));

		// Combine the above transformations
		glm::mat4 corredMat(1, 0, 0, 0,/**/ 0, 1, 0, 0,/**/0, 0, 1, 0,/**/ 0, 0, 0, 1);
		NodeTransformation = TranslationM * RotationM * ScalingM;
		//NodeTransformation = NodeTransformation* glm::mat4(1, 0, 0, 0, /**/ 0, 0, 1, 0, /**/ 0, -1, 0, 0,/**/  0, 0, 0, 1);
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (this->boneIndexMap.find(NodeName) != this->boneIndexMap.end()) {
		size_t BoneIndex = this->boneIndexMap[NodeName];
		this->finalTransform[BoneIndex] = this->m_rootInverse * GlobalTransformation * boneOffsets[BoneIndex];
	}

	for (size_t i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, anim);
	}
}
void uuu::uud::boneStructer::BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms, aiAnimation* anim) {
	glm::mat4 Identity = glm::identity<glm::mat4>();

	float TicksPerSecond = (float)(anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)anim->mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity, anim);

	Transforms.resize(finalTransform.size());

	for (size_t i = 0; i < finalTransform.size(); i++) {
		Transforms[i] = finalTransform[i];
	}
}

boost::optional<FT_Library*> uuu::bitMapAtlas::library;
size_t uuu::bitMapAtlas::faceCounter;

__int8 uuu::bitMapAtlas::InitFreeTypeLib() {

	if (uuu::bitMapAtlas::library)return true;
	uuu::bitMapAtlas::library = new FT_Library;
	//������
	FT_Init_FreeType(uuu::bitMapAtlas::library.get());

	faceCounter = 0;

	return true;
}
__int8 uuu::bitMapAtlas::CreateFontFaceFromFilePath(const std::string path,int index) {

	//���C�u���������邩�`�F�b�N
	if (!this->library)throw bad_calling("��Ƀt���[�^�C�v�����������Ă�������");

	//�t�F�C�X���쐬
	if (!fontFace)
		this->fontFace = new FT_Face;
	//���[�h
	FT_New_Face(*this->library.get(), path.c_str(), index, this->fontFace.get());
	FT_Set_Char_Size(*this->fontFace.get(), 0, 12, 12, 12);
	FT_Set_Pixel_Sizes(*this->fontFace.get(), 0, 96);

	faceCounter++;
	return true;
}
__int8 uuu::bitMapAtlas::_CreateAtlasTexture(size_t u) {

	this->atlas = new GLuint;
	this->unit = u;

	glActiveTexture(GL_TEXTURE0 + u);
	glGenTextures(1, this->atlas.get());

	return true;
}
__int8 uuu::bitMapAtlas::AddCharasToBuffer(const std::u16string ini) {

	for (const auto& i : ini)
		this->charas.push_back(i);

	return true;

}
__int8 uuu::bitMapAtlas::AddCharasToBuffer(const char16_t from, const char16_t to) {

	for (char16_t i = from; i <= to; i++)
		this->charas.push_back(i);

	return true;
}
__int8 uuu::bitMapAtlas::_AddBufferedCharasToAtlas() {
	//�t�F�C�X�͍쐬����Ă�?
	if (!this->fontFace)throw bad_calling("�t�F�C�X���Ȃ��ł�");
	if (!this->atlas)throw bad_calling("�e�N�X�`�����Ȃ��ł�");

	//�e�N�X�`���̃o�C���h�ƃe�N�X�`��������
	glBindTexture(GL_TEXTURE_2D, *this->atlas.get());
	glActiveTexture(GL_TEXTURE0 + this->unit);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//���܂̃e�N�X�`���̑傫��
	size_t texW = 0, texH = 0;

	//�܂��̓e�N�X�`���̊m��
	//�o�b�t�@�̐��Ԃ��
	for (size_t i = 0; i < this->charas.size(); i++) {
		//�O���t��ǂݍ���
		FT_Load_Glyph(*this->fontFace.get(), FT_Get_Char_Index(*this->fontFace.get(), this->charas[i]), FT_LOAD_RENDER);
		//�r�b�g�}�b�v���e�N�X�`����傫������
		texW += (*this->fontFace.get())->glyph->bitmap.width;
		texH = (texH < (*this->fontFace.get())->glyph->bitmap.rows) ? (*this->fontFace.get())->glyph->bitmap.rows : texH;

	}
	//�e�N�X�`���쐬
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texW, texH, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//�I�t�Z�b�g
	size_t offW = 0,offH = 0;
	//���ɃJ�L�R����&�}�b�s���O
	for (size_t i = 0; i < this->charas.size(); i++) {
		//�O���t��ǂݍ���
		FT_Load_Glyph(*this->fontFace.get(), FT_Get_Char_Index(*this->fontFace.get(), this->charas[i]), FT_LOAD_RENDER);
		//�r�b�g�}�b�v��`��
		glTexSubImage2D(GL_TEXTURE_2D, 0, offW, texH - (*this->fontFace.get())->glyph->bitmap.rows, (*this->fontFace.get())->glyph->bitmap.width, (*this->fontFace.get())->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, (*this->fontFace.get())->glyph->bitmap.buffer);

		//���C�A�E�g�}�b�v�ɏ�������
		this->charasLayout[this->charas[i]] = glm::vec4(offW, texH - (*this->fontFace.get())->glyph->bitmap.rows, (*this->fontFace.get())->glyph->bitmap.width, (*this->fontFace.get())->glyph->bitmap.rows);

		//�I�t�Z�b�g�}�V�}�V
		offW += (*this->fontFace.get())->glyph->bitmap.width;
	}

	this->sampler = new GLuint;
	glGenSamplers(1, sampler.get());
	glSamplerParameteri(*sampler.get(), GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(*sampler.get(), GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(*sampler.get(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(*sampler.get(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE0 + this->unit);
	glBindSampler(this->unit, *this->sampler.get());
	
	return true;
}
uuu::bitMapAtlas::~bitMapAtlas() {

	glDeleteTextures(1, this->atlas.get());
	delete this->atlas.get();

	glDeleteSamplers(1, this->sampler.get());
	delete this->sampler.get();

	delete this->fontFace.get();
	faceCounter--;


	if (faceCounter <= 0) {
		delete this->library.get();
		this->library = boost::none;
	}

	return;
}
__int8 uuu::bitMapAtlas::bind() {

	glActiveTexture(GL_TEXTURE0 + this->unit);
	glBindTexture(GL_TEXTURE_2D, *this->atlas.get());
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindSampler(this->unit, *this->sampler.get());

	return true;
}
__int8 uuu::bitMapAtlas::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(this->unit, 0);

	return true;
}
int uuu::bitMapAtlas::GetUnitIndex() {
	return this->unit;
}


uuu::fontRenderFT::fontRenderFT() {

	return;
}
uuu::fontRenderFT::fontRenderFT(const std::string path, int index) {

	this->CreateFontFace(path, index);

	return;
}

__int8 uuu::fontRenderFT::CreateFontFace(const std::string path, int index) {

	//��ɏ����������ƃt�F�[�X�Â���
	this->InitFreeTypeLib();
	this->CreateFontFaceFromFilePath(path, index);

	return true;
}
__int8 uuu::fontRenderFT::AddCharasToBuffer(const std::u16string ini) {
	
	
	return this->uuu::bitMapAtlas::AddCharasToBuffer(ini);

}
__int8 uuu::fontRenderFT::AddCharasToBuffer(const char16_t from, char16_t to) {


	return this->uuu::bitMapAtlas::AddCharasToBuffer(from, to);

}

__int8 uuu::fontRenderFT::AddCharasToBufferAndRendering(const std::u16string ini, size_t texUnit) {

	this->AddCharasToBuffer(ini);
	this->Rendering(texUnit);

	return true;
}
__int8 uuu::fontRenderFT::AddCharasToBufferAndRendering(const char16_t from, char16_t to, size_t texUnit) {

	this->AddCharasToBuffer(from, to);
	this->Rendering(texUnit);

	return true;
}

__int8 uuu::fontRenderFT::Rendering(size_t texUnit) {

	//�e�N�X�`���쐬
	this->_CreateAtlasTexture(texUnit);
	//�����_�����O
	this->_AddBufferedCharasToAtlas();

	//�Ԃ�l�ɋl�߂�
	try {
		this->texUnit = this->GetUnitIndex();
		this->texture = *this->atlas.get();
		this->uuu::textureOperator::sampler = *this->uuu::bitMapAtlas::sampler.get();
	}
	catch (...) {
		throw unknown_error("�e�N�X�`���������Ɛ�������Ă��Ȃ��񂶂�Ȃ�����");
	}

	return true;
}
__int8 uuu::fontRenderFT::Bind() {

	return this->uuu::textureOperator::Bind();
}
__int8 uuu::fontRenderFT::Unbind() {

	return this->uuu::textureOperator::Unbind();
}

std::list<GLuint> uuu::textureOperator::reservedTexUnits;//�\��ς݃e�N�X�`�����j�b�g�@�̖{��

__int8 uuu::textureOperator::__CreateTexture(GLuint texUnit, GLuint width, GLuint height, GLuint format, GLuint informat,GLuint type, void* data) {

	this->texUnit = texUnit;
	//�g�p�ς݂̒��ԓ����
	bool daburi = false;
	for (GLuint& ch : uuu::textureOperator::reservedTexUnits)
		if (ch == texUnit)daburi = true;
	//�_�u�����Ȃ���Ύg�p�ς݂ɒǉ�
	if (!daburi)uuu::textureOperator::reservedTexUnits.push_back(texUnit);

	//���j�b�g���A�N�e�B�u��&�e�N�X�`������
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glGenTextures(1, &this->texture);

	glBindTexture(GL_TEXTURE_2D, this->texture);

	////�t�H�[�}�b�g�����߂�(���S�����ł͂���܂���)
	//int informat;//�O���I�t�H�[�}�b�g
	//if (format==uuu::textureFormat::depth16 | format==uuu::textureFormat::depth24 | format==uuu::textureFormat::depth32)informat = GL_DEPTH;
	//else informat = GL_RGBA;


	//�e�N�X�`���ɂ���C���[�W����������
	glTexImage2D(GL_TEXTURE_2D, 0, informat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//�T���v���[�̍쐬
	glGenSamplers(1, &this->sampler);
	glSamplerParameteri(this->sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(this->sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(this->sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(this->sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}
__int8 uuu::textureOperator::CreateManual(GLuint texUnit, GLuint width, GLuint height,GLuint format,GLuint informat, GLuint type,void* data) {
	//�_�u�����f�[�^������
	auto ite = std::find(reservedTexUnits.begin(), reservedTexUnits.end(), texUnit);
	if (ite != reservedTexUnits.end())
		glDeleteTextures(1, &(ite.operator*()));

	return __CreateTexture(texUnit, width, height, format, informat,type, data);
}
__int8 uuu::textureOperator::CreateManual(GLuint width, GLuint height, GLuint format, GLuint informat, GLuint type,void* data) {

	//�e�N�X�`�����j�b�g����
	GLuint texUnit = this->FindFreeTexUnit();

	return this->__CreateTexture(texUnit, width, height, format, informat,type, data);
	//this->texUnit = texUnit;
	////�g�p�ς݂̒��ԓ����
	//bool daburi = false;
	//for (GLuint& ch : uuu::textureOperator::reservedTexUnits)
	//	if (ch == texUnit)daburi = true;
	////�_�u�����Ȃ���Ύg�p�ς݂ɒǉ�
	//if (!daburi)uuu::textureOperator::reservedTexUnits.push_back(texUnit);

	////���j�b�g���A�N�e�B�u��&�e�N�X�`������
	//glActiveTexture(GL_TEXTURE0 + texUnit);
	//glGenTextures(1, &this->texture);

	//glBindTexture(GL_TEXTURE_2D, this->texture);

	////�t�H�[�}�b�g�����߂�(���S�����ł͂���܂���)
	//int informat;//�O���I�t�H�[�}�b�g
	//int type;//�����^�C�v
	//if ((format == uuu::textureFormat::depth16) || (format == uuu::textureFormat::depth24) || (format == uuu::textureFormat::depth32)) {
	//	informat = GL_DEPTH_COMPONENT;
	//	type = GL_FLOAT;
	//}
	//else if (format == uuu::textureFormat::stencil8) {
	//	informat = GL_STENCIL_COMPONENTS;
	//	type = GL_BOOL;
	//}
	//else {
	//	informat = GL_RGBA;
	//	type = GL_UNSIGNED_BYTE;
	//}

	////�e�N�X�`���ɂ���C���[�W���������� �� informat�𖳌���
	//glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, informat, type, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	////�T���v���[�̍쐬
	//glGenSamplers(1, &this->sampler);
	//glSamplerParameteri(this->sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glSamplerParameteri(this->sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glSamplerParameteri(this->sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glSamplerParameteri(this->sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//return true;
}
__int8 uuu::textureOperator::Bind() const{
	//�e�N�X�`���{�̂ƃT���v�����o�C���h
	glActiveTexture(GL_TEXTURE0+this->texUnit);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glBindSampler(this->texUnit, this->sampler);

	return true;
}
__int8 uuu::textureOperator::Unbind() const{

	//���ꂼ��0���o�C���h
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(this->texUnit, 0);

	return true;
}
GLuint uuu::textureOperator::FindFreeTexUnit() {

	GLuint ret = 0;

	//�܂��͏Ə��Ƀ\�[�g
	uuu::textureOperator::reservedTexUnits.sort();
	//���ɂ��Ԃ�����++���Ă����ΏH��������
	for (GLuint& ch : uuu::textureOperator::reservedTexUnits)
		if (ch == ret)ret++;

	GLint num_tex_units;//�ő吔���擾
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num_tex_units);

	if (num_tex_units < ret)throw std::runtime_error("texture overflow");

	return ret;

}


__int8 uuu::textureOperator::ReleaseTexUnit(GLuint tar) {

	uuu::textureOperator::reservedTexUnits.remove(tar);

	return true;
}
GLuint uuu::textureOperator::GetTexUnit() const{
	return this->texUnit;
}

__int8 uuu::textureLoaderFromImageFile::CreateTextureFromPNG(const std::string path, uuu::textureOperator& ret,size_t texUnit) {

//	ret.texUnit = texUnit;
//
//	//�g�p�ς݂̒��ԓ����
//	bool daburi = false;
//	for (GLuint& ch : uuu::textureOperator::reservedTexUnits)
//		if (ch == texUnit)daburi = true;
//	//�_�u�����Ȃ���Ύg�p�ς݂ɒǉ�
//	if (!daburi)uuu::textureOperator::reservedTexUnits.push_back(texUnit);
//
//	//png�ǂݍ��݂ƃf�[�^�B
//	FILE* fp;
//	png_structp myPng;
//	png_infop pinfo;
//	int depth, colorType, interlaceType;
//	unsigned int width, height;
//	int rowSize, imgSize;
//	unsigned int i;
//	unsigned char *data;
//
//	//PNG�t�@�C�����J��
//	fopen_s(&fp, path.c_str(), "rb");
//	if (!fp)
//		throw unknown_error("�t�@�C�����J���܂���ł����@�p�X���m�F���Ă�������");
//
//	//PNG�t�@�C����ǂݍ��ނ��߂̍\���̂��쐬
//	myPng = png_create_read_struct(
//		PNG_LIBPNG_VER_STRING,
//		NULL, NULL, NULL
//	);
//	pinfo = png_create_info_struct(myPng);
//
//	//������
//	png_init_io(myPng, fp);
//
//	//�摜����ǂݍ���
////�摜�̕��A�����A�r�b�g�[�x�A�F�̕\�����@�A�C���^�[���[�X�̏����擾����
//	png_read_info(myPng, pinfo);
//	png_get_IHDR(myPng, pinfo,
//		&width, &height,
//		&depth, &colorType,
//		&interlaceType, NULL, NULL
//	);
//
//	//RGB��RGBA�݂̂ɑΉ�
//	if (colorType != PNG_COLOR_TYPE_RGB && colorType != PNG_COLOR_TYPE_RGBA)
//		throw unknown_error("�����_�ł̓J���[�^�C�v��RGB�y��RGBA�݂̂̑Ή��ƂȂ�܂��B");
//
//	//�C���^�[���[�X�͔�Ή�
//	if (interlaceType != PNG_INTERLACE_NONE)
//		throw unknown_error("�����_�ł̓C���^�[���[�X�摜�ɂ͔�Ή��ł��B");
//
//	//1�s�̃f�[�^�T�C�Y�Ɖ摜�̍�������K�v�ȃ������ʂ��v�Z���āA�������m��
//	rowSize = png_get_rowbytes(myPng, pinfo);
//	imgSize = rowSize * height;
//	data = new unsigned char[imgSize];
//
//	//�s�N�Z���̓ǂݍ���
//	for (i = 0; i < height; i++) {
//		png_read_row(myPng, &data[i * rowSize], NULL);
//	}
//
//	png_read_end(myPng, pinfo);
//
//
//	//���j�b�g���A�N�e�B�u��&�e�N�X�`������
//	glActiveTexture(GL_TEXTURE0 + texUnit);
//	glGenTextures(1, &ret.texture);
//
//	glBindTexture(GL_TEXTURE_2D, ret.texture);
//
//
//	//�e�N�X�`���ɂ���C���[�W����������
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	//�T���v���[�̍쐬
//	glGenSamplers(1, &ret.sampler);
//	glSamplerParameteri(ret.sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glSamplerParameteri(ret.sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glSamplerParameteri(ret.sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glSamplerParameteri(ret.sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	//��Еt�� �f�[�^�̊J���ƃt�@�C���̃N���[�Y
//	delete[] data;
//	png_destroy_info_struct(myPng, &pinfo);
//	png_destroy_read_struct(&myPng, NULL, NULL);
//	fclose(fp);

	typedef unsigned char ubyte_t;

	//unsigned int id; // �e�N�X�`��ID
	ubyte_t* data; // ���f�[�^��ێ�����
	unsigned int width, height;
	int depth, colortype, interlacetype;
	//std::string filename;

		// png�摜�t�@�C���̃��[�h
	png_structp sp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop   ip = png_create_info_struct(sp);
	FILE* fp;
	fopen_s(&fp, path.c_str(), "rb");
	if (!fp) {
		perror(path.c_str());
	}
	png_init_io(sp, fp);
	png_read_info(sp, ip);
	png_get_IHDR(sp, ip, (png_uint_32*)&width, (png_uint_32*)&height,
		&depth, &colortype, &interlacetype, NULL, NULL);
	// �������̈�m��
	int rb = png_get_rowbytes(sp, ip);
	data = new ubyte_t[height * rb];
	ubyte_t** recv = new ubyte_t * [height];
	for (int i = 0; i < height; i++)
		recv[i] = &data[i * rb];
	png_read_image(sp, recv);
	png_read_end(sp, ip);
	png_destroy_read_struct(&sp, &ip, NULL);
	fclose(fp);
	delete[] recv;

	ret.texUnit = texUnit;
	//�g�p�ς݂̒��ԓ����
	bool daburi = false;
	for (GLuint& ch : uuu::textureOperator::reservedTexUnits)
		if (ch == texUnit)daburi = true;
	//�_�u�����Ȃ���Ύg�p�ς݂ɒǉ�
	if (!daburi)uuu::textureOperator::reservedTexUnits.push_back(texUnit);

	//���j�b�g���A�N�e�B�u��&�e�N�X�`������
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glGenTextures(1, &ret.texture);

	glBindTexture(GL_TEXTURE_2D, ret.texture);

	//�e�N�X�`���ɂ���C���[�W����������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//�T���v���[�̍쐬
	glGenSamplers(1, &ret.sampler);
	glSamplerParameteri(ret.sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(ret.sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(ret.sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(ret.sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}
__int8 uuu::textureLoaderFromImageFile::CreateTextureFromPNG(const std::string path, uuu::textureOperator& ret) {

	ret.texUnit = uuu::textureOperator::FindFreeTexUnit();

	//�g�p�ς݂̒��ԓ����
	bool daburi = false;
	for (GLuint& ch : uuu::textureOperator::reservedTexUnits)
		if (ch == ret.texUnit)daburi = true;

	//�_�u�����Ȃ���Ύg�p�ς݂ɒǉ�
	if (!daburi)uuu::textureOperator::reservedTexUnits.push_back(ret.texUnit);

	return uuu::textureLoaderFromImageFile::CreateTextureFromPNG(path, ret, ret.texUnit);
}
__int8 uuu::textureLoaderFromImageFile::CreateFontTextureAtlasFromFontFile(const std::string path, uuu::textureOperator& ret) {

	return true;
}

__int8 uuu::gameControllerInterface::GetGconButtonInput(size_t gConIndex, std::vector<unsigned char>& ret,bool checkExistGconOfIndex) {

	//�W���C�X�e�B�b�N�����݂��Ă��邩�`�F�b�N
	if (checkExistGconOfIndex)
		if (!glfwJoystickPresent(GLFW_JOYSTICK_1+gConIndex))
			throw std::invalid_argument("�W���C�X�e�B�b�N���h�����ĂȂ��������͈����̃W���C�X�e�B�b�N��������܂���");

	//�l�̎擾
	int buttonNum;
	const unsigned char* buttons;
	buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + gConIndex, &buttonNum);

	//�Ԃ�l�ɏ�������
	ret.resize(buttonNum);
	for (size_t i = 0; i < buttonNum; i++)
		ret[i] = buttons[i];

	return true;
}
__int8 uuu::gameControllerInterface::GetGconJoystickInput(size_t gConIndex, std::vector<float>& ret, bool checkExistGconOfIndex) {

	//�W���C�X�e�B�b�N�����݂��Ă��邩�`�F�b�N
	if (checkExistGconOfIndex)
		if (!glfwJoystickPresent(GLFW_JOYSTICK_1 + gConIndex))
			throw std::invalid_argument("�W���C�X�e�B�b�N���h�����ĂȂ��������͈����̃W���C�X�e�B�b�N��������܂���");

	//�l�̎擾
	int axisNum;
	const float* axises;
	axises = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + gConIndex, &axisNum);

	//�Ԃ�l�ɏ�������
	ret.resize(axisNum);
	for (size_t i = 0; i < axisNum; i++)
		ret[i] = axises[i];

	return true;
}
__int8 uuu::keyboardInterface::GetKeyInput(int KeyCode) {
	//�����ꂢ����true false
	return glfwGetKey(uuu::app::windows.at(uuu::app::bind), KeyCode);
}

int uuu::uua::ReadHeaderWav(FILE* fp, int *channel, int* bit, int *size, int* freq){
	short res16;
	int res32;
	int dataSize, chunkSize;
	short channelCnt, bitParSample, blockSize;
	int samplingRate, byteParSec;

	int dataPos;
	int flag = 0;

	fread(&res32, 4, 1, fp);
	if (res32 != 0x46464952) {	//"RIFF"
		printf("RIFF���Ⴄ���\n");
		return 1;	//error 1
	}

	//�f�[�^�T�C�Y = �t�@�C���T�C�Y - 8 byte �̎擾
	fread(&dataSize, 4, 1, fp);

	//WAVE�w�b�_�[�̓ǂ�
	fread(&res32, 4, 1, fp);
	if (res32 != 0x45564157) {	//"WAVE"
		return 2;	//error 2
	}

	while (flag != 3) {
		//�`�����N�̓ǂ�
		fread(&res32, 4, 1, fp);
		fread(&chunkSize, 4, 1, fp);

		switch (res32) {
		case 0x20746d66:	//"fmt "
			//format �ǂݍ���
			//PCM��ނ̎擾
			fread(&res16, 2, 1, fp);
			if (res16 != 1 && res16 != 3) {
				//��Ή��t�H�[�}�b�g
				return 4;

			}
			//���m����(1)or�X�e���I(2)
			fread(&channelCnt, 2, 1, fp);
			if (channelCnt > 2) {
				//�`�����l�����ԈႢ
				return 5;
			}
			//�T���v�����O���[�g
			fread(&samplingRate, 4, 1, fp);
			//�f�[�^���x(byte/sec)=�T���v�����O���[�g*�u���b�N�T�C�Y
			fread(&byteParSec, 4, 1, fp);
			//�u���b�N�T�C�Y(byte/sample)=�`�����l����*�T���v��������̃o�C�g��
			fread(&blockSize, 2, 1, fp);
			//�T���v���������bit��(bit/sample)�F8 or 16
			fread(&bitParSample, 2, 1, fp);

			*channel = (int)channelCnt;
			*bit = (int)bitParSample;
			*freq = samplingRate;

			flag += 1;

			break;
		case 0x61746164:	//"data"
		case 0x64617461:

			*size = chunkSize;

			dataPos = ftell(fp);

			flag += 2;
			break;
		default://����ȊO�̃`�����N�Ȃ�T�C�Y���ǂݔ�΂�
			fseek(fp, chunkSize, SEEK_CUR);
		}

	}

	//���o��("fmt "��"data"����ɂ������ꍇ�̂ݓ���)
	if (dataPos != ftell(fp)) {
		fseek(fp, dataPos, SEEK_SET);
	}

	return 0;
}

ALCdevice* uuu::uua::wavOperator::device;
ALCcontext* uuu::uua::wavOperator::context;

__int8 uuu::uua::wavOperator::PushNewQueue() {

	//�c�ʂ𓾂�
	fpos_t current;
	fgetpos(fp, &current);
	size_t nokori = wavSize - current;

	//�c�ʂ��o�b�t�@�T�C�Y���傫�����
	if (bufferSize <= nokori) {
		//�f�[�^��ǂݍ���
		fread(data, bufferSize, 1, fp);
		//�o�b�t�@���쐬
		ALuint buffer;
		alGenBuffers(1, &buffer);

		//�o�b�t�@�ւ̃f�[�^�o�^
		if (wavChannel == 1) {
			if (wavBit == 8) {
				alBufferData(buffer, AL_FORMAT_MONO8, data, bufferSize, wavFreq);
			}
			else {
				alBufferData(buffer, AL_FORMAT_MONO16, data, bufferSize, wavFreq);
			}
		}
		else {
			if (wavBit == 8) {
				alBufferData(buffer, AL_FORMAT_STEREO8, data, bufferSize, wavFreq);
			}
			else {
				alBufferData(buffer, AL_FORMAT_STEREO16, data, bufferSize, wavFreq);
			}
		}

		//�����1�`�����N������
		//��������o�b�t�@���L���[�C���O�i�P�c�ɍ������ށj
		alSourceQueueBuffers(source, 1, &buffer);



	}
	//�����c�ʂ��Ȃ��Ȃ�
	else return false;

	//�܂��t�@�C���̎c�ʂ����̃��^�[��
	return true;
}
__int8 uuu::uua::wavOperator::PlayQueueBuffer(size_t thou) {
	ALint state, num;
	//���̂��\�[�X���Đ�������Ȃ���΍Đ���
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING) {
	
		alSourcePlay(source);
		
	}

	//�Đ��ς݃o�b�t�@��1�ȏ�ɂȂ�܂őҋ@
	while (alGetSourcei(source, AL_BUFFERS_PROCESSED, &num), num < thou) {
		//boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		//Sleep(processedBufferCheckDelay);
		std::this_thread::sleep_for(std::chrono::milliseconds(processedBufferCheckDelay));
	}

//	std::cout << num << std::endl;

	//���while�ōĐ����I������̂ō��̃o�b�t�@����菜���i�����O���j
	for (size_t i = 0; i < num; i++) {
		ALuint dammy;
		alSourceUnqueueBuffers(source, 1, &dammy);
	}

	return true;
}
__int8 uuu::uua::wavOperator::PlaySingle(size_t thou, size_t loadNumMax) {
	while (this->Update(thou, loadNumMax) && !stopPlaySingle);

	printf("��������\n");

	return true;
}
uuu::uua::wavOperator::wavOperator() {
	device = NULL;
	context = NULL;
	pPlayTh = NULL;

	bufferQueueMaxNum = 32;
	bufferSize = 4096;

	processedBufferCheckDelay = 100;

	stopPlaySingle = false;

	data = new unsigned char[bufferSize];


}
uuu::uua::wavOperator::wavOperator(const std::string path) :uuu::uua::wavOperator::wavOperator() {
	try {
		this->InitAudioAndMakeContextCurrent();
		this->LoadFile(path);
	}
	catch (std::exception& ex) {
		throw ex;
	}
}
uuu::uua::wavOperator::~wavOperator() {

	delete this->data;
	if (CreateFPMyself)fclose(fp);

	//�X���b�h��������~
	this->stopPlaySingle = true;

	if (pPlayTh)
		pPlayTh->join();

	return;

}
__int8 uuu::uua::wavOperator::InitAudioAndMakeContextCurrent() {
	//�f�o�C�X���Q�b�g���ăR���e�L�X�g�����o�C���h
	if (!device)
		device = alcOpenDevice(NULL);
	if (!context)
		context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	//���͂��������\�[�X������Ă݂���
	alGenSources(1, &this->source);
	//alSourcei(this->source, AL_LOO, AL_TRUE);
	return true;
}
__int8 uuu::uua::wavOperator::SetBufferProp(size_t _bufferQueueMaxNum, size_t _bufferSize) {
	this->bufferQueueMaxNum = _bufferQueueMaxNum;
	this->bufferSize = _bufferSize;

	return true;
}
__int8 uuu::uua::wavOperator::LoadFile(FILE* _fp,bool lop) {
	this->fp = _fp;

	//���o��&�w�b�_���[�h
	if (uuu::uua::ReadHeaderWav(fp, &this->wavChannel, &this->wavBit, &this->wavSize, &this->wavFreq))
		throw std::invalid_argument("���̃t�@�C���͖����ȃt�H�[�}�b�g�ł�");

	this->CreateFPMyself = false;

	//�ꉞ���[�v���肵�Ƃ���
	if (lop)
		alSourcei(this->source, AL_LOOPING, AL_TRUE);

	return true;
}
__int8 uuu::uua::wavOperator::LoadFile(const std::string path,bool lop) {
	FILE *fp;
	auto ret = fopen_s(&fp, path.c_str(), "rb");

	try {
		this->LoadFile(fp);
	}
	catch (std::exception& it) {
		throw it;
	}

	this->sourceBegin = ftell(fp);
	this->CreateFPMyself = true;

	return ret;
}
__int8 uuu::uua::wavOperator::Update(size_t thou, size_t loadNumMax) {

	//���ݑ��݂��Ă���o�b�t�@�̐��𓾂�
	int existBufferQueueNum;
	alGetSourcei(source, AL_BUFFERS_QUEUED, &existBufferQueueNum);
	//�t�@�C���̎c�ʂ��܂����邩
	bool fileNokotteru = true;


	for (size_t i = 0; i < loadNumMax; i++)
		//�ő吔��菭�Ȃ���΍��
		if (existBufferQueueNum < this->bufferQueueMaxNum) {
			fileNokotteru = this->PushNewQueue();

			//�t�@�C�����c���Ă��Ȃ���΃t���b�V�����邽�߂ɂ�
			if (!fileNokotteru) {
				ALint num;
				alGetSourcei(source, AL_BUFFERS_PROCESSED, &num);
				//���while�ōĐ����I������̂ō��̃o�b�t�@����菜���i�����O���j
				for (size_t i = 0; i < num; i++) {
					ALuint dammy;
					alSourceUnqueueBuffers(source, 1, &dammy);
				}

			}
		}
		else {
			this->PlayQueueBuffer(thou);
			break;
		}


	//�I����Ă邩����@�t�@�C���̎c�ʂ��Ȃ��L���[�̐����Ȃ��Ȃ���������܂����낤
	if (!(fileNokotteru || existBufferQueueNum)) return false;
	return true;
}
__int8 uuu::uua::wavOperator::PlayMultiThread(size_t thou, size_t loadNumMax) {
	pPlayTh = new std::thread(&wavOperator::PlaySingle, this, thou, loadNumMax);

	return true;
}
__int8 uuu::uua::wavOperator::JoinPlayThread(bool noThreadReturn) {
	if (pPlayTh) {
		ALint state;

		do {
			alGetSourcei(source, AL_SOURCE_STATE, &state);
			alSourceStop(source);
		} while (state == AL_PLAYING);

		if (!this->stopPlaySingle)
			int a = 0;

		while (!pPlayTh->joinable())
			int a = 0;
		pPlayTh->join();

		delete pPlayTh;
		pPlayTh = NULL;
		return true;

	}
	return noThreadReturn;
}
__int8 uuu::uua::wavOperator::SetProcessedBufferCheckDelay(size_t ii) {
	processedBufferCheckDelay = ii;

	return true;
}
__int8 uuu::uua::wavOperator::Rewind() {

	alSourceStop(this->source);

	ALint num, beg;
	//���while�ōĐ����I������̂ō��̃o�b�t�@����菜���i�����O���j
	do {
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &num);
		alGetSourcei(source, AL_BUFFERS_QUEUED, &beg);
		printf("processed%d,queue%d\n", num,beg);
		for (size_t i = 0; i < num; i++) {
			ALuint dammy;
			alSourceUnqueueBuffers(source, 1, &dammy);
		}
		alGetSourcei(source, AL_BUFFERS_QUEUED, &num);
	} while (beg > 0);

	fseek(this->fp, this->sourceBegin, SEEK_SET);

	return true;
}
__int8 uuu::uua::wavOperator::Stop() {
	alSourceStop(this->source);

	return true;
}

uuu::frameBufferOperator::frameBufferOperator() {

	glGenFramebuffers(1, &this->fbo);

}
__int8 uuu::frameBufferOperator::Bind() const{
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

	return true;
}
__int8 uuu::frameBufferOperator::Unbind() const{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}
__int8 uuu::BindDefaultFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

uuu::renderBufferOperator::renderBufferOperator() {
	glGenRenderbuffers(1, &this->rbo);

}
__int8 uuu::renderBufferOperator::Bind() const{
	glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);

	return true;
}
__int8 uuu::renderBufferOperator::Unbind() const{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	return true;
}
__int8 uuu::renderBufferOperator::SetStrage(int format, GLuint width, GLuint height) {

	this->Bind();
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);

	return true;

}

__int8 uuu::frameBufferOperator::AttachRenderBufferOperator(const attachTargetType target, const renderBufferOperator& obj) {

	this->Bind();
	obj.Bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, target, GL_RENDERBUFFER, obj.rbo);

	return true;
}
__int8 uuu::frameBufferOperator::AttachTextureOperator(int target, const uuu::textureOperator& obj) {
	this->Bind();
	obj.Bind();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER, target, GL_TEXTURE_2D, obj.texture, 0);

	return true;
}

std::unordered_map<int, std::string>uuu::neoVertexOperators::vaoOperatorNeo::autoNamingTemp;



__int8 uuu::neoVertexOperators::InitNeoVertexOperators() {
	uuu::neoVertexOperators::vaoOperatorNeo::__init_auto_naming_temp();
	uuu::neoVertexOperators::vboUsageTemp::Init();


	return true;
}

uuu::neoVertexOperators::vboOperator::vboOperator() {

	glGenBuffers(1, &this->vbo);

	return;
}
uuu::neoVertexOperators::vboOperator::~vboOperator() {

	glDeleteBuffers(1, &this->vbo);

	return;
}
__int8 uuu::neoVertexOperators::vboOperator::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	return true;
}
__int8 uuu::neoVertexOperators::vboOperator::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER,0);

	return true;
}

__int8 uuu::neoVertexOperators::vboOperatorIndices::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo);

	return true;
}
__int8 uuu::neoVertexOperators::vboOperatorIndices::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

size_t uuu::neoVertexOperators::vboOperator::GetVboSize() {
	return this->size;
}
size_t uuu::neoVertexOperators::vboOperator::GetVerticesNum() {
	return this->verticesNum;
}

__int8 uuu::neoVertexOperators::CreateVbomapFromModelLoaderSerializer(uuu::modelLoaderSerializer& mod, std::unordered_map<int, vboOperator*>& ret,int loadAttribute) {

	/*
		positions=1<<1,
			indices = 1 << 2,
			colors = 1 << 3,
			norms = 1 << 4,
			texCoords0 = 1 << 5,
			texCoords1 = 1 << 6,
			texCoords2 = 1 << 7,
			texCoords3 = 1 << 8,//����ȏ�̃e�N�X�`�����W�̓J�X�^�������ł��肢���܂�
			boneInfos = 1 << 9,
	
	*/
#define inc(arg)(arg)!=0
	if (inc(loadAttribute & positions)) {
		auto j = new vboOperator;
		j->CreateAttribute(*(mod.GetVerticesList()), uuu::neoVertexOperators::vboOperator::staticDraw);
		ret[positions] = j;
	}
	if(inc(loadAttribute & indices)) {
		auto j = new vboOperatorIndices;
		j->CreateAttribute(*(mod.GetIndicesList()), uuu::neoVertexOperators::vboOperator::staticDraw);
		ret[indices] = j;
	}
	if (inc(loadAttribute & norms)) {
		auto j = new vboOperator;
		j->CreateAttribute(*(mod.GetNormalsList()), uuu::neoVertexOperators::vboOperator::staticDraw);
		ret[norms] = j;
	}
	if (inc(loadAttribute & texCoords0)) {
		auto j = new vboOperator;
		j->CreateAttribute(*(mod.GetTexCoordsList(0)), uuu::neoVertexOperators::vboOperator::staticDraw);
		ret[texCoords0] = j;
	}
	if (inc(loadAttribute & texCoords1)) {
		auto j = new vboOperator;
		j->CreateAttribute(*(mod.GetTexCoordsList(1)), uuu::neoVertexOperators::vboOperator::staticDraw);
		ret[texCoords1] = j;
	}
	if (inc(loadAttribute & texCoords2)) {
		auto j = new vboOperator;
		j->CreateAttribute(*(mod.GetTexCoordsList(2)), uuu::neoVertexOperators::vboOperator::staticDraw);
		ret[texCoords2] = j;
	}
	if (inc(loadAttribute & texCoords3)) {
		auto j = new vboOperator;
		j->CreateAttribute(*(mod.GetTexCoordsList(3)), uuu::neoVertexOperators::vboOperator::staticDraw);
		ret[texCoords3] = j;
	}
	
	return true;

}

uuu::neoVertexOperators::vaoOperatorNeo::vaoOperatorNeo() {
	glGenVertexArrays(1, &this->vao);

	this->indices = NULL;

	return;
}
uuu::neoVertexOperators::vaoOperatorNeo::~vaoOperatorNeo() {

	glDeleteVertexArrays(1, &this->vao);

	return;
}
__int8 uuu::neoVertexOperators::vaoOperatorNeo::Bind() {
	glBindVertexArray(this->vao);

	return true;
}
__int8 uuu::neoVertexOperators::vaoOperatorNeo::Unbind() {
	glBindVertexArray(0);

	return true;
}

__int8 uuu::neoVertexOperators::vaoOperatorNeo::__init_auto_naming_temp() {
	uuu::neoVertexOperators::vaoOperatorNeo::autoNamingTemp[uuu::neoVertexOperators::positions] = "position";
	uuu::neoVertexOperators::vaoOperatorNeo::autoNamingTemp[uuu::neoVertexOperators::norms] = "normal";
	uuu::neoVertexOperators::vaoOperatorNeo::autoNamingTemp[uuu::neoVertexOperators::colors] = "color";
	uuu::neoVertexOperators::vaoOperatorNeo::autoNamingTemp[uuu::neoVertexOperators::indices] = "__indices element";

	uuu::neoVertexOperators::vaoOperatorNeo::autoNamingTemp[uuu::neoVertexOperators::texCoords0] = "texCoord0";

	return true;
}
__int8 uuu::neoVertexOperators::vaoOperatorNeo::AttachVboByID(GLuint atId, uuu::neoVertexOperators::vboOperator& tar,const uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage& usage) {

	//vao��BIND
	this->Bind();
	//vbo��BIND
	tar.Bind();

	//�L����
	glEnableVertexAttribArray(atId);

	//���͂̐ݒ�
	glVertexAttribPointer(atId, usage.shift, usage.type, usage.enableNormalize, 0, NULL);


	return true;

}
__int8 uuu::neoVertexOperators::vaoOperatorNeo::AttachVboByString(uuu::shaderProgramObjectVertexFragment& shad, std::string name, uuu::neoVertexOperators::vboOperator& tar, const vboAttachUsage usage) {
	//uuu::glTools::MakeAttribMapNameAndId()
	GLuint h = glGetAttribLocation(shad.progObj, name.c_str());

	return this->AttachVboByID(h, tar, usage);

}
__int8 uuu::neoVertexOperators::vaoOperatorNeo::SetIndexBuffer(uuu::neoVertexOperators::vboOperator* in) {


	this->indices = in;

	return true;
}
__int8 uuu::neoVertexOperators::vaoOperatorNeo::DrawElements(uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage usage) {

	this->Bind();

	//�C���f�b�N�X�̃Z�b�g�A�b�v
	this->indices->Bind();

	glDrawElements(GL_TRIANGLES, this->indices->GetVboSize()/4, usage.type, NULL);

	return true;


}
__int8 uuu::neoVertexOperators::vaoOperatorNeo::AttachVboMap(uuu::shaderProgramObjectVertexFragment& shad, uuu::neoVertexOperators::vboMap& tar, std::unordered_map<int, std::string>* namingTemp, std::unordered_map<int, uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage>* usageTemp) {

	//�܂��}�b�v������Ȃ�
	if (!namingTemp)namingTemp = &(this->autoNamingTemp);
	if (!usageTemp)usageTemp = &(uuu::neoVertexOperators::vboUsageTemp::usageMap);

	for (auto& t : tar) {

		if (t.first == uuu::neoVertexOperators::vboAttachType::indices)  
			this->SetIndexBuffer(tar.at(uuu::neoVertexOperators::vboAttachType::indices));
		else
			this->AttachVboByString(shad, namingTemp->at(t.first), *(t.second), usageTemp->at(t.first));

	}

	return true;
}


uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage uuu::neoVertexOperators::vboUsageTemp::pos;//4�����ʒu
uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage uuu::neoVertexOperators::vboUsageTemp::norm;//�l�����@��
uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage uuu::neoVertexOperators::vboUsageTemp::ind;//�C���f�b�N�X
uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage uuu::neoVertexOperators::vboUsageTemp::texCoord;//�e�N�X�`�����W
std::unordered_map<int, uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage> uuu::neoVertexOperators::vboUsageTemp::usageMap;//

__int8 uuu::neoVertexOperators::vboUsageTemp::Init() {
	pos.type = GL_FLOAT;
	pos.shift = 4;
	pos.enableNormalize = GL_FALSE;

	norm.type = GL_FLOAT;
	norm.shift = 4;
	norm.enableNormalize = GL_TRUE;

	ind.type = GL_UNSIGNED_INT;
	ind.shift = 1;
	ind.enableNormalize = GL_FALSE;

	texCoord.type = GL_FLOAT;
	texCoord.shift = 2;
	texCoord.enableNormalize = GL_FALSE;

	//�}�b�v����Ă����
	usageMap.clear();
	usageMap[uuu::neoVertexOperators::vboAttachType::positions] = pos;
	usageMap[uuu::neoVertexOperators::vboAttachType::norms] = norm;
	usageMap[uuu::neoVertexOperators::vboAttachType::indices] = ind;
	usageMap[uuu::neoVertexOperators::vboAttachType::texCoords0] = texCoord;
	usageMap[uuu::neoVertexOperators::vboAttachType::texCoords1] = texCoord;
	usageMap[uuu::neoVertexOperators::vboAttachType::texCoords2] = texCoord;
	usageMap[uuu::neoVertexOperators::vboAttachType::texCoords3] = texCoord;

	return true;
}

uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage uuu::neoVertexOperators::vboUsageTemp::GetPos() {
	return pos;
}
uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage uuu::neoVertexOperators::vboUsageTemp::GetNorm() {
	return norm;
}
uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage uuu::neoVertexOperators::vboUsageTemp::GetInd() {
	return ind;
}
uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage uuu::neoVertexOperators::vboUsageTemp::GetTexCoord() {
	return texCoord;
}

uuu::mouseInterface::pos uuu::mouseInterface::before;

void uuu::mouseInterface::Init() {

	glfwGetCursorPos(uuu::app::GetBindWindow(),&before.x, &before.y);

	return;
}
uuu::mouseInterface::pos uuu::mouseInterface::GetMousePosDifference() {

	pos ret;

	glfwGetCursorPos(uuu::app::GetBindWindow(), &ret.x, &ret.y);

	pos temp = ret - before;

	//before�̍X�V
	before = ret;

	return temp;
}

ALCdevice* uuu::uua::neoWavOperator::device;
ALCcontext* uuu::uua::neoWavOperator::context;

void uuu::uua::neoWavOperator::Init() {

	auto list = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);//���X�g���󂯎����
//null�ŃZ�p���[�g
	size_t pointer = 0;
	std::vector<std::string>sepa;
	sepa.resize(1);
	while (list[pointer] != '\0' || list[pointer + 1] != '\0') {
		//��Ԃ�����ɒǉ�
		sepa.back() += list[pointer];
		//pointer�̂Ƃ��낪null�����Ȃ�J�b�g
		if (list[pointer] == '\0') {
			sepa.push_back("");
		}
		pointer++;
	}

	//���X�g��\��
	printf("select current sound device\n");
	for (int i = 0; i < sepa.size(); i++)
		printf("%d:%s\n", i, sepa.at(i).data());

	int inn = 0;
	while (1) {
		std::cin >> inn;

		//�����͈͊O�Ȃ�
		if (inn<0 || inn>sepa.size())
			printf("out of range\n");
		else break;
	}

	//device
	uuu::uua::neoWavOperator::device = alcOpenDevice(sepa.at(inn).data());
	uuu::uua::neoWavOperator::context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
}
void uuu::uua::neoWavOperator::Terminate() {
	alcDestroyContext(uuu::uua::neoWavOperator::context);
	alcCloseDevice(uuu::uua::neoWavOperator::device);
}

uuu::uua::neoWavOperator::neoWavOperator() {
	this->source = 0;

}
void uuu::uua::neoWavOperator::LoadFile(const std::string  path) {

	//�\�[�X�ƃo�b�t�@�͍���Ă���
	alGenBuffers(1, &this->buffer);
	alGenSources(1, &this->source);

	FILE* fp;
	auto ret = fopen_s(&fp, path.c_str(), "rb");

	if (!fp)throw std::invalid_argument("�p�X�����߂ł�");

	uuu::uua::ReadHeaderWav(fp, &this->wavChannel, &this->wavBit, &this->wavSize, &this->wavFreq);

	//�t�H�[�}�b�g�����
	int myFormat = 0;
	//24���g�����邩�̃t���O
	bool ex24to32f = false;
	if (this->wavChannel == 1) {
		if (this->wavBit == 8)
			myFormat = AL_FORMAT_MONO8;
		else if (this->wavBit == 16)
			myFormat = AL_FORMAT_MONO16;
		else if (this->wavBit == 24)
			myFormat = AL_FORMAT_MONO16, ex24to32f = true;
		else if (this->wavBit == 32)
			myFormat = AL_FORMAT_MONO_FLOAT32;
	}
	else if (this->wavChannel == 2) {
		if (this->wavBit == 8)
			myFormat = AL_FORMAT_STEREO8;
		else if (this->wavBit == 16)
			myFormat = AL_FORMAT_STEREO16;
		else if (this->wavBit == 24)
			myFormat = AL_FORMAT_STEREO16, ex24to32f = true;
		else if (this->wavBit == 32)
			myFormat = AL_FORMAT_STEREO_FLOAT32;
	}

	if (ex24to32f) {
		//���݈ʒu����T�C�Y���ǂݍ���
		auto dat = new char[this->wavSize];
		fread(dat, this->wavSize, 1, fp);

		//3�o�C�g�ÂL���X�g����32�ɓ˂����ނ̂ł��������m��
		this->dataBuffer.reset(new char[(this->wavSize/3)*2], std::default_delete<char[]>());

		//�o�C�g�P�ʂł�
		for (size_t i = 0; i < this->wavSize/3; i++) {
			dataBuffer.get()[i*2] = dat[i*3 + 1];
			dataBuffer.get()[i*2+1] = dat[i*3 + 2];
		}

		//wav�̃T�C�Y���X�V
		this->wavSize = (this->wavSize / 3) * 2;

	}
	else {
		//���݈ʒu����T�C�Y���ǂݍ���
		this->dataBuffer.reset(new char[this->wavSize], std::default_delete<char[]>());
		fread(dataBuffer.get(), this->wavSize, 1, fp);
	}


	alBufferData(buffer, myFormat, dataBuffer.get(), this->wavSize * sizeof(char), this->wavFreq);

	alSourcei(this->source, AL_BUFFER, this->buffer);

}
void uuu::uua::neoWavOperator::Play() {
	alSourcePlay(this->source);
}
//�����߂�
void uuu::uua::neoWavOperator::Rewind() {
	alSourceRewind(this->source);
}
//�ꎞ��~
void uuu::uua::neoWavOperator::Pause() {
	alSourcePause(this->source);
}
//��~
void uuu::uua::neoWavOperator::Stop() {
	alSourceStop(this->source);
}
//���[�v
void uuu::uua::neoWavOperator::Loop(bool enable) {
	alSourcei(this->source, AL_LOOPING, enable ? AL_TRUE:AL_FALSE);
}
bool uuu::uua::neoWavOperator::IsPlaying() {

	ALint temp;

	if (!this->source)return false;

	alGetSourcei(source, AL_SOURCE_STATE, &temp);


	return temp==AL_PLAYING;
}

void uuu::uua::neoWavOperator::DeleteFileData() {

	if (this->dataBuffer.use_count() == 0)return;

	this->dataBuffer.reset((char*)NULL);

	// �o�b�t�@�̔j��
	alDeleteBuffers(1, &buffer);
	// �\�[�X�̔j��
	alDeleteSources(1, &source);

}
uuu::uua::neoWavOperator::~neoWavOperator() {

	this->DeleteFileData();

}























