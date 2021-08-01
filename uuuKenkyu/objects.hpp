#pragma once

#include <uuu.hpp>
#include <uuuGame.hpp>

namespace _uuu {
	namespace objects {
		//�x�[�X�ɂȂ�I�u�W�F�N�g�@�^�����������Ă���
		class object {
		protected:
			
		public:
			using fulltype = std::vector<std::string>;
			virtual fulltype GetFullTypeInfo();//���S�Ȍ^��Ԃ����o��
			std::string GetMostLikelyTypeInfo();//�ł������^�������o��

			bool CheckType(const std::string& str);//�^���q�b�g���Ă��邩�m���߂�
		};
		//�`��I�u�W�F�N�g�@�@�`�掞�ɌĂяo���`�惁�\�b�h������
		class drawableObject :public object {
		public:
			virtual fulltype GetFullTypeInfo();
			virtual void Draw() = 0;

		};

		//�ʒu������&�`��ł���I�u�W�F�N�g �ʒu�̃A�N�Z�T���܂�
		class movableObject:public drawableObject {
		public:
			virtual fulltype GetFullTypeInfo();
			virtual void Draw() = 0;

			virtual glm::mat4 GetTransform() = 0;
			virtual void SetTransform(const glm::mat4& tr) = 0;
		};

		//�Q�[�����b�V���̃I�u�W�F�N�g
		class gameMeshObject :public movableObject {
		protected:
			std::unique_ptr<uuu::game::mesh> obj;
		public:

		};
	};
};