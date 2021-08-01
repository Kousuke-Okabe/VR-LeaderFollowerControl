#ifndef UUU_UUU_EASY_TOOLS_HPP
#define UUU_UUU_EASY_TOOLS_HPP

#include <uuu.hpp>

namespace uuu {

	template<typename T>using wptr=uuu::commonTools::float_ptr<T>;//��Q�Ƃ̒Z�k�p�G�C���A�X

	namespace easy {
		namespace usings {
			using gvec2 = glm::vec2;
			using gvec3 = glm::vec3;
			using gvec4 = glm::vec4;
			using gmat4 = glm::mat4;

			template<typename T>using sptr = std::shared_ptr<T>;
			template<typename T>using uptr = std::unique_ptr<T>;

			template<typename T>using fptr = uuu::commonTools::float_ptr<T>;

			template<typename T>using opt = boost::optional<T>;
		};

		//uuu�ėp�@�\���ׂĂ̏�����������Ă������
		__int8 Init(uuu::app::windowProp prop,int loadAttribs = uuu::neoVertexOperators::positions | uuu::neoVertexOperators::norms | uuu::neoVertexOperators::indices);

		//neo3Dmesh�̊�{�ݒ�e���v���[�g������������
		__int8 InitNeo3Dmesh();
		//�p�X�ƃV�F�[�_�[���畁�ʂɕ`��\�ȃ��f���f�[�^(vao)����� �J�X�^�����͔����@neoVertex�Ή�!
		class neo3Dmesh {
		//���̂Ƌ��ʂɂȂ�\���̂���V�F�[�_�[�@���f���@vbo��shared�ō��
			std::unique_ptr<uuu::neoVertexOperators::vaoOperatorNeo> vao;
			std::shared_ptr<uuu::neoVertexOperators::vboMap> vboMap;
			std::shared_ptr<uuu::modelLoaderSerializer> mod;
			//�V�F�[�_�[
			std::shared_ptr<uuu::shaderProgramObjectVertexFragment> shad;

		public:
			//��{�ݒ�e���v���[�g�@������ς��邱�ƂŐ����̎菇�Ȃǂ̐ݒ肪�ł���
			struct propertyTemp {
				int fileLoadFlag;//���f���t�@�C����ǂݍ��ލۂ�Assimp�p�t���O
				int loadAttribute;//vbo�����钸�_�����̃t���OvboAttachType�g���Ăǂ���
				std::unique_ptr<std::unordered_map<int, std::string>>autoNamingTemp;//�������t���̃e���v���[�g
				std::unique_ptr<std::unordered_map<int, uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage>>autoUsageTemp;//�����p�@(�ƃt�H�[�}�b�g)�ݒ�̃e���v���[�g �C���f�b�N�X�̃t�H�[�}�b�g��ς���Ƃ��̓V���[�g�J�b�g���ꏏ�ɕύX���Ă�������
				
			};
			static propertyTemp temp;//���̃e���v���[�g��ǂݏo���Ďg��
			uuu::neoVertexOperators::vaoOperatorNeo::vboAttachUsage indicesUsage;//�C���f�b�N�X�̗p�@�̃V���[�g�J�b�g ��Q�Ɓ@���f������

			//�V�F�[�_�[��ݒ�
			__int8 SetShader(const std::string& source);
			__int8 SetShader(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> add);
			//�����ɐݒ肵�����b�V���ƃV�F�[�_�[���V�F�A����
			__int8 ShareShader(const uuu::easy::neo3Dmesh& gen);

			//�p�X����Vao�𐶐� ���̂Ƃ���indices�̃V���[�g�J�b�g���Œ�
			__int8 LoadFile(const std::string& path, const std::string& mesh);
			//vbo�ƃ��f���V���A���C�U�[�������ɐݒ肵�����b�V���ƃV�F�A���� vao�͓Ǝ�
			__int8 ShareFile(const uuu::easy::neo3Dmesh& gen);

			//�G�������g��`�悷��
			__int8 DrawElements();

			//�V�F�[�_�[�ɃA�N�Z�X����
			std::weak_ptr<uuu::shaderProgramObjectVertexFragment> Shader();
			//���f�������A���C�U�[�ɃA�N�Z�X����
			std::weak_ptr<uuu::modelLoaderSerializer>Serializer();

			neo3Dmesh(std::shared_ptr<uuu::shaderProgramObjectVertexFragment> add, const std::string& path, const std::string& mesh);
			neo3Dmesh();


			//�񐄏�:�J�v�Z�����j��@neo3Dmesh�̃v���C�x�[�g�ɐN������A�N�Z�T
			struct ___accesserNeo3Dmesh{
				std::shared_ptr<uuu::modelLoaderSerializer>AccessModel(uuu::easy::neo3Dmesh& gen);
			};
		};


		//

	};
};

#endif