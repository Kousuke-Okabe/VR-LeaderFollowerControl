#pragma once

#include <string>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>

#include <vector>
#include <list>
#include <iostream>

#include <regex>
#include <mutex>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace kenkyulocal {
    class serialMgr {
    protected:
        HANDLE com;
        int inited;

        HANDLE OpenCOM(const std::string& portname);
        bool SetBufForCOM(const HANDLE& h);
        bool InitBufForCOM(const HANDLE& h);
        bool SetupCOM(const HANDLE& h);
        bool SetupTimeout(const HANDLE& h);

    public:
        void Init(const std::string& portname);

        serialMgr();
        serialMgr(const std::string& portname);

        void Write(const std::vector<uint8_t>& buf);
        void Read(std::vector<uint8_t>& buf);

        void Write(const std::string& buf);
        void Read(std::string& buf);
    };

    class umeArmTransfer:protected serialMgr {
        size_t count;//���߂̑��M��
        std::string stack;//�ԐM�����߂Ă���
        std::regex successFormat;

        std::mutex commandMutex;//WriteWithCount�ŏ������ݒ��̊��荞�݂������

        void WriteWithCount(const std::string& dat);//�R�}���h���������ށ@�X���b�h�Z�[�u
    public:
        std::list<unsigned int>unsuccess;//�������肪�A���Ă��ĂȂ�id

        umeArmTransfer(const std::string& portname);
        void Update();

        void Emergency();//�댯��ԂɑS�g��E�͂���

        void PowerOnArm();//�S���[�^�[�̃g���N��L���ɂ���

        void Torque(unsigned int id, int flag);//�g���N�𑀍삷��
        void Move(unsigned int id, int angle, int time=1000);//�p�x�𑀍삷��
        void Position(unsigned int id, glm::vec3 pos, int time=1000);//�ʒu�𑀍삷��
        void PositionQuat(unsigned int id, glm::vec3 pos, glm::quat q, int time=1000);//�ʒu�Ǝp���𑀍삷��

    };
};