#include <uuuEasyTools.hpp>


uuu::easy::neo3Dmesh::propertyTemp uuu::easy::neo3Dmesh::temp;

__int8 uuu::easy::Init(uuu::app::windowProp prop, int loadAttribs) {
	uuu::app::InitUUUAndCreateWindow(prop);
	uuu::neoVertexOperators::InitNeoVertexOperators();
	uuu::easy::InitNeo3Dmesh();
	uuu::easy::neo3Dmesh::temp.loadAttribute = loadAttribs;

	glClearColor(0, 0, 0, 1);
	uuu::app::SetEnableDepthBufferFlag();

	return true;
}

__int8 uuu::easy::InitNeo3Dmesh() {

	auto& t = uuu::easy::neo3Dmesh::temp;
	t.autoNamingTemp.reset(NULL);
	t.autoUsageTemp.reset(NULL);
	t.fileLoadFlag = aiProcessPreset_TargetRealtime_Fast;
	t.loadAttribute = 
		uuu::neoVertexOperators::vboAttachType::positions | 
		uuu::neoVertexOperators::vboAttachType::indices | 
		uuu::neoVertexOperators::vboAttachType::norms |
		uuu::neoVertexOperators::vboAttachType::texCoords0;




	return true;

}


uuu::easy::neo3Dmesh::neo3Dmesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> add, const std::string& path, const std::string& mesh) {

	this->SetShader(add);
	this->LoadFile(path, mesh);

	return;

}
uuu::easy::neo3Dmesh::neo3Dmesh() {
	return;
}
__int8 uuu::easy::neo3Dmesh::SetShader(const std::string& source) {
	this->shad.reset(new uuu::shaderProgramObjectVertexFragment);

	this->shad->LoadSource(source);

	return true;
}
__int8 uuu::easy::neo3Dmesh::SetShader(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> add) {

	this->shad = add;

	return true;
}

__int8 uuu::easy::neo3Dmesh::LoadFile(const std::string& path, const std::string& mesh) {

	//まずはシリアライザーを作る
	this->mod.reset(new uuu::modelLoaderSerializer(path, mesh, temp.fileLoadFlag));

	//次にvboを作る
	this->vboMap.reset(new uuu::neoVertexOperators::vboMap);
	uuu::neoVertexOperators::CreateVbomapFromModelLoaderSerializer(*(this->mod.get()), *(this->vboMap.get()), temp.loadAttribute);

	//最後にvaoを作る
	this->vao.reset(new uuu::neoVertexOperators::vaoOperatorNeo);
	this->vao.get()->AttachVboMap(*this->shad, *this->vboMap, temp.autoNamingTemp.get(),temp.autoUsageTemp.get());

	//インデックスショートカットのリセットなんやけどNULLやったらおわ
	if (temp.autoUsageTemp.get())
		this->indicesUsage = temp.autoUsageTemp->at(uuu::neoVertexOperators::vboAttachType::indices);
	else
		this->indicesUsage = uuu::neoVertexOperators::vboUsageTemp::GetInd();

	return true;
}
__int8 uuu::easy::neo3Dmesh::DrawElements() {


	return this->vao->DrawElements(this->indicesUsage);
}
std::weak_ptr<uuu::shaderProgramObjectVertexFragment> uuu::easy::neo3Dmesh::Shader() {
	return std::weak_ptr<uuu::shaderProgramObjectVertexFragment>(this->shad);
}
std::weak_ptr<uuu::modelLoaderSerializer>uuu::easy::neo3Dmesh::Serializer() {
	return std::weak_ptr<uuu::modelLoaderSerializer>(this->mod);
}

__int8 uuu::easy::neo3Dmesh::ShareShader(const uuu::easy::neo3Dmesh& gen) {

	this->shad = gen.shad;

	return true;
}
__int8 uuu::easy::neo3Dmesh::ShareFile(const uuu::easy::neo3Dmesh& gen) {

	this->mod = gen.mod;
	this->vboMap = gen.vboMap;

	//最後にvaoを作る
	this->vao.reset(new uuu::neoVertexOperators::vaoOperatorNeo);
	this->vao.get()->AttachVboMap(*this->shad, *this->vboMap, temp.autoNamingTemp.get(), temp.autoUsageTemp.get());

	//インデックスショートカットのリセットなんやけどNULLやったらおわ
	if (temp.autoUsageTemp.get())
		this->indicesUsage = temp.autoUsageTemp->at(uuu::neoVertexOperators::vboAttachType::indices);
	else
		this->indicesUsage = uuu::neoVertexOperators::vboUsageTemp::GetInd();

	return true;

}

//ここからアクセサ
std::shared_ptr<uuu::modelLoaderSerializer>uuu::easy::neo3Dmesh::___accesserNeo3Dmesh::AccessModel(uuu::easy::neo3Dmesh& gen) {
	return std::shared_ptr<uuu::modelLoaderSerializer>(gen.mod);
}

