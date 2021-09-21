#include "xbeeForWin.hpp"
using namespace std;
using namespace kenkyulocal;

serialMgr::serialMgr(){
    inited=0;
}
serialMgr::~serialMgr() {
    if (com != INVALID_HANDLE_VALUE)
        CloseHandle(com);
}
serialMgr::serialMgr(const std::string&portname):serialMgr(){
    Init(portname);
}

void serialMgr::Write(const std::vector<uint8_t>&buf){
    DWORD writeSize;//実際に送信したデータサイズ
    WriteFile(com,buf.data(),buf.size(),&writeSize,NULL);
}
void serialMgr::Read(std::vector<uint8_t>&buf){
    DWORD errors;
    COMSTAT comStat;
    ClearCommError(com, &errors, &comStat);
    int lengthOfRecieved = comStat.cbInQue; // 受信したメッセージ長を取得する

    buf.resize(lengthOfRecieved);
    DWORD numberOfPut;
    ReadFile(com, buf.data(), lengthOfRecieved, &numberOfPut, NULL); 

    return;
}
void serialMgr::Write(const std::string& buf) {
    std::vector<uint8_t> bufg(buf.begin(), buf.end());

    Write(bufg);

    return;
}
void serialMgr::Read(std::string& buf) {
    std::vector<uint8_t> bufg;
    Read(bufg);
    buf = std::string(bufg.begin(), bufg.end());

    return;
}

void serialMgr::Init(const std::string&portname){
    //COMポートを開く
    bool check=true;

    com=OpenCOM(portname);
    check&=SetBufForCOM(com);//バッファをセット
    check&=InitBufForCOM(com);//初期化
    check&=SetupCOM(com);
    check&=SetupTimeout(com);
    check&=(com>=0);

    if (!check) {
        CloseHandle(com);
        throw std::runtime_error("Any error happened");
    }

    inited=1;
}

HANDLE serialMgr::OpenCOM(const std::string&portname){
    HANDLE hComPort;

    TCHAR* tBuf;
#ifdef _UNICODE
    size_t siz = MultiByteToWideChar(CP_ACP, 0, portname.c_str(), -1, (TCHAR*)NULL, 0);
    tBuf = new TCHAR[siz];
    MultiByteToWideChar(CP_ACP, 0, portname.c_str(), -1, tBuf, siz);
#else
    tBuf = portname.c_str();
#endif

    hComPort = CreateFile(    //ファイルとしてポートを開く
            tBuf,      // ポート名を指すバッファへのポインタ:COM4を開く（デバイスマネージャでどのポートが使えるか確認）
            GENERIC_READ | GENERIC_WRITE, // アクセスモード:読み書き両方する
            0,        //ポートの共有方法を指定:オブジェクトは共有しない
            NULL,       //セキュリティ属性:ハンドルを子プロセスへ継承しない
            OPEN_EXISTING,     //ポートを開き方を指定:既存のポートを開く
            0,   //ポートの属性を指定:同期　非同期にしたいときはFILE_FLAG_OVERLAPPED
            NULL       // テンプレートファイルへのハンドル:NULLって書け
    );
    if (hComPort == INVALID_HANDLE_VALUE){//ポートの取得に失敗
        //CloseHandle(hComPort);//ポートを閉じる
        throw std::runtime_error("Cant open "+portname);
    }
    else{
        printf("finish openning serial port.\n");
    }

    return hComPort;
}
bool serialMgr::SetBufForCOM(const HANDLE&h){
    int check;//エラーチェック用の変数
    check = SetupComm(
        h,//COMポートのハンドラ
        1024,//受信バッファサイズ:1024byte
        1024//送信バッファ:1024byte
    );
    if (check == FALSE){
        printf("cant set buffer settings.\r\n");
        CloseHandle(h);
        return false;
    }
    else{
        printf("finish setting buffer.\r\n");
    }

    return true;
}
bool serialMgr::InitBufForCOM(const HANDLE&h){
    int check;
    check = PurgeComm(
        h,//COMポートのハンドラ
        PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR//出入力バッファをすべてクリア
    );
    if (check == FALSE){
        printf("cant init buffer.\r\n");
        CloseHandle(h);
        return 0;
    }
    else{
        printf("finish initiarize buffer.\r\n");
    }
    return 1;
}
bool serialMgr::SetupCOM(const HANDLE&h){
    DCB dcb;//構成情報を記録する構造体の生成
    GetCommState(h, &dcb);//現在の設定値を読み込み
    dcb.DCBlength = sizeof(DCB);//DCBのサイズ
    dcb.BaudRate = 9600;//ボーレート:9600bps
    dcb.ByteSize = 8;//データサイズ:8bit
    dcb.fBinary = TRUE;//バイナリモード:通常TRUE
    dcb.fParity = NOPARITY;//パリティビット:パリティビットなし
    dcb.StopBits = ONESTOPBIT;//ストップビット:1bit
    dcb.fOutxCtsFlow = FALSE;//CTSフロー制御:フロー制御なし
    dcb.fOutxDsrFlow = FALSE;//DSRハードウェアフロー制御：使用しない
    dcb.fDtrControl = DTR_CONTROL_DISABLE;//DTR有効/無効:DTR無効
    dcb.fRtsControl = RTS_CONTROL_DISABLE;//RTSフロー制御:RTS制御なし

    dcb.fOutX = FALSE;//送信時XON/XOFF制御の有無:なし
    dcb.fInX = FALSE;//受信時XON/XOFF制御の有無:なし
    dcb.fTXContinueOnXoff = TRUE;// 受信バッファー満杯＆XOFF受信後の継続送信可否:送信可
    dcb.XonLim = 512;//XONが送られるまでに格納できる最小バイト数:512
    dcb.XoffLim = 512;//XOFFが送られるまでに格納できる最小バイト数:512
    dcb.XonChar = 0x11;//送信時XON文字 ( 送信可：ビジィ解除 ) の指定:XON文字として11H ( デバイス制御１：DC1 )
    dcb.XoffChar = 0x13;//XOFF文字（送信不可：ビジー通告）の指定:XOFF文字として13H ( デバイス制御3：DC3 )

    dcb.fNull = TRUE;// NULLバイトの破棄:破棄する
    dcb.fAbortOnError = TRUE;//エラー時の読み書き操作終了:終了する
    dcb.fErrorChar = FALSE;// パリティエラー発生時のキャラクタ（ErrorChar）置換:なし
    dcb.ErrorChar = 0x00;// パリティエラー発生時の置換キャラクタ
    dcb.EofChar = 0x03;// データ終了通知キャラクタ:一般に0x03(ETX)がよく使われます。
    dcb.EvtChar = 0x02;// イベント通知キャラクタ:一般に0x02(STX)がよく使われます

    int check = SetCommState(h, &dcb);  //設定値の書き込み
    if (check == FALSE){//エラーチェック
        CloseHandle(h);
        throw std::runtime_error("issues are when chenging com settings");
    }
    else{
        printf("finish chenging com settings.\r\n");
        return 1;
    }
}
bool serialMgr::SetupTimeout(const HANDLE&h){
    COMMTIMEOUTS TimeOut; // COMMTIMEOUTS構造体の変数を宣言
    GetCommTimeouts(h, &TimeOut); // タイムアウトの設定状態を取得

    TimeOut.ReadTotalTimeoutMultiplier = 0;//読込の１文字あたりの時間:タイムアウトなし
    TimeOut.ReadTotalTimeoutConstant = 1000;//読込エラー検出用のタイムアウト時間
    //(受信トータルタイムアウト) = ReadTotalTimeoutMultiplier × (受信予定バイト数) + ReadTotalTimeoutConstant
    TimeOut.WriteTotalTimeoutMultiplier = 0;//書き込み１文字あたりの待ち時間:タイムアウトなし
    TimeOut.WriteTotalTimeoutConstant = 1000;//書き込みエラー検出用のタイムアウト時間
    //(送信トータルタイムアウト) = WriteTotalTimeoutMultiplier ×(送信予定バイト数) + WriteTotalTimeoutConstant

    int check = SetCommTimeouts(h, &TimeOut);//タイムアウト設定の書き換え
    if (check == FALSE){//エラーチェック
        CloseHandle(h);
        throw std::runtime_error("cant set timeout settings.\r\n");
        return 0;
    }
    else{
        printf("finish setting timeout.\r\n");
        return 1;
    }
}

umeArmTransfer::umeArmTransfer(const string&portname):serialMgr(portname) {
    this->count = 0;
    successFormat = regex(R"(SUCCESS\s+(\d+)\s*)");
}

void umeArmTransfer::WriteWithCount(const std::string& dat) {
    std::lock_guard<std::mutex> lock(this->commandMutex);

    this->count++;
    this->Write(dat + "?" + std::to_string(this->count) + "\n");
}

void umeArmTransfer::Update() {
    std::string add;
    super::Read(add);
    auto nowbuf=this->stack += add;

    smatch rez;
    while (regex_search(nowbuf, rez, successFormat)) {
        auto found=find(this->unsuccess.begin(), unsuccess.end(), strtoul(rez[1].str().c_str(), NULL, 10));

        if (found == unsuccess.end())throw("invalid answer from xbee");

        cout << "delete" << *found << endl;

        this->unsuccess.erase(found);//成功したものを削除
        nowbuf = string(rez[1].second, rez[0].second);//読み取った分を削除
    }

    this->stack = nowbuf;
}

void umeArmTransfer::Emergency() {
    this->WriteWithCount("T0:255");//速攻動作させるため直書き
    //this->Torque(255, 0);
}
void umeArmTransfer::PowerOnArm() {
    this->Torque(255, 1);
}

void umeArmTransfer::Torque(unsigned int id, int flag) {
    this->WriteWithCount("T" + to_string(flag) + ":" + to_string(id));
}
void umeArmTransfer::Move(unsigned int id, int angle, int time) {
    this->WriteWithCount("M" + to_string(angle) + "," + to_string(time) + ":" + to_string(id));
}
void umeArmTransfer::Position(unsigned int id, glm::vec3 pos, int time) {
    throw logic_error("古い関数です　PositionQuatを使用してください");
    this->WriteWithCount("P" + to_string(pos.x) + "," + to_string(pos.y) + "," + to_string(pos.z) + "," + to_string(time) + ":" + to_string(id));
}
void umeArmTransfer::PositionQuat(unsigned int id, glm::vec3 pos, glm::quat q, int time) {
    this->WriteWithCount("P" + to_string(pos.x) + "," + to_string(pos.y) + "," + to_string(pos.z) + "," + to_string(q.x) + "," + to_string(q.y) + "," + to_string(q.z) + "," + to_string(q.w) + "," + to_string(time) + ":" + to_string(id));
}

void umeArmTransfer::Read(std::vector<uint8_t>& buf) {
    super::Read(buf);
}











