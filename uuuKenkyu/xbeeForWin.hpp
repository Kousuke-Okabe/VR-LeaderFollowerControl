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
        size_t count;//命令の送信回数
        std::string stack;//返信をためておく
        std::regex successFormat;

        std::mutex commandMutex;//WriteWithCountで書き込み中の割り込みを避ける

        void WriteWithCount(const std::string& dat);//コマンドを書きこむ　スレッドセーブ
    public:
        std::list<unsigned int>unsuccess;//成功判定が帰ってきてないid

        umeArmTransfer(const std::string& portname);
        void Update();

        void Emergency();//危険状態に全身を脱力する

        void PowerOnArm();//全モーターのトルクを有効にする

        void Torque(unsigned int id, int flag);//トルクを操作する
        void Move(unsigned int id, int angle, int time=1000);//角度を操作する
        void Position(unsigned int id, glm::vec3 pos, int time=1000);//位置を操作する
        void PositionQuat(unsigned int id, glm::vec3 pos, glm::quat q, int time=1000);//位置と姿勢を操作する

    };
};