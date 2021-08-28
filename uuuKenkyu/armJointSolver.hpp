
#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <iostream>
#include <Eigen/QR>
#include <functional>

#define M_PI       3.14159265358979323846   // pi

namespace armJointSolver {
	//template<typename T, size_t dimR, size_t dimC>class angleSolver {
	//public:
	//	using VectorR = Eigen::Matrix<T, dimR, 1>;
	//	using VectorC = Eigen::Matrix<T, dimC, 1>;
	//	using Matrix = Eigen::Matrix<T, dimR, dimC>;
	//	using MatrixK = Eigen::Matrix<T, dimC, dimC>;//ばね係数行列
	//	using MatrixJ = Eigen::Matrix<T, dimC, dimR>;//ヤコビ行列

	//	MatrixJ CalcJacobi(const std::function<VectorC(const VectorR&)>& f, const VectorR& q, const T d = M_PI / 1800) {
	//		MatrixJ ret;
	//		//サイズが入れ替わる
	//		for (size_t i = 0; i < dimC; i++)//何行
	//			for (size_t j = 0; j < dimR; j++) {//何列
	//				//差分を作成する
	//				VectorR dvec;
	//				for (size_t k = 0; k < dimR; k++)
	//					if (k == j)dvec(k) = d;
	//					else dvec(k) = 0.0;
	//				//行でq 列でx,yを決める
	//				ret(i, j) = VectorC(f(q + dvec) - f(q))(i) / d;
	//			}

	//		return ret;
	//	}

	//	template<typename Tpow, size_t dimpow> Eigen::Matrix<Tpow, dimpow, 1> powv(const Eigen::Matrix<Tpow, dimpow, 1>& ini) {
	//		Eigen::Matrix<Tpow, dimpow, 1> ret;
	//		for (size_t i = 0; i < dimpow; i++)
	//			ret(i) = pow(ini(i), 2);

	//		return ret;
	//	}

	//	//目標位置と現在位置とその他パラメータから次点での角度を求める
	//	VectorR SolveDist(const std::function<VectorC(const VectorR&)>& f, const MatrixK k, const VectorC& reg, const VectorR& q, const T d = M_PI / 1800) {
	//		/*
	//		f 正運動学の式
	//		k ばね定数
	//		reg 目標位置
	//		q 現在の変位
	//		d デルタ
	//		*/
	//		auto current = f(q);
	//		auto jacobi = CalcJacobi(f, q, d);//現在地のヤコビ行列を導く

	//		//t=J^TK(R-c)なのでdQに置き換えられる?? なんで?
	//		VectorR deltaQ = jacobi.transpose() * k * (reg - current);

	//		return deltaQ + q;

	//	}
	//};

	//template<typename T, size_t dimR, size_t dimC>class armSolver :public angleSolver<T, dimR, dimC> {
	//public:
	//	using VectorR = Eigen::Matrix<T, dimR, 1>;
	//	using VectorC = Eigen::Matrix<T, dimC, 1>;
	//	using Matrix = Eigen::Matrix<T, dimR, dimC>;
	//	using MatrixK = Eigen::Matrix<T, dimC, dimC>;//ばね係数行列

	//protected:
	//	VectorR nowQ;
	//	std::function<VectorC(const VectorR&)> f;
	//	VectorC ref;

	//	template<typename T> T CorrectAngle(const T& a) {
	//		//0から2piまで
	//		//2pi以上ｈじゃいらん
	//		auto disroop = fmod(a, 2 * M_PI);

	//		if (disroop < 0.0)disroop += 2 * M_PI;

	//		return disroop;
	//	}
	//	template<typename T, size_t dim>Eigen::Matrix<T, dim, 1> CorrectAngleVec(const Eigen::Matrix<T, dim, 1>& m) {
	//		Eigen::Matrix<T, dim, 1> ret;

	//		for (size_t i = 0; i < dim; i++)
	//			ret(i) = CorrectAngle<T>(m(i));

	//		return ret;
	//	}
	//public:


	//	armSolver(const VectorR& defQ, const std::function<VectorC(const VectorR&)>& defF) :nowQ(defQ), f(defF) {
	//		ref = f(defQ);
	//	}

	//	VectorR GetNextQ(const VectorC& k) {

	//		//バネ行列を作成
	//		Eigen::Matrix<T, dimC, dimC> kmat = Eigen::Matrix<T, dimC, dimC>::Zero();
	//		for (size_t i = 0; i < dimC; i++)kmat(i, i) = k(i);


	//		//VectorR nextq = CorrectAngleVec(this->SolveDist(f, kmat, ref, nowQ));
	//		VectorR nextq = this->SolveDist(f, kmat, ref, nowQ);

	//		nowQ = nextq;

	//		return nextq;
	//	}

	//	void SetRef(const VectorC& s) {
	//		ref = s;
	//	}
	//	void ResetNow(const VectorR& s) {
	//		nowQ = s;
	//	}

	//	//現在の手先と目標一の差分をとる
	//	T GetDistNowtoRef() {
	//		return(ref - f(nowQ)).norm();
	//	}
	//	//現在の手先と目標一の差分をとる
	//	T GetDistNowtoRefSquared() {
	//		return(ref - f(nowQ)).squaredNorm();
	//	}

	//	//現在の手先位置の誤差と姿勢の誤差をとる<位置,姿勢>
	//	std::pair<T, T> GetDistPosAndQuat() {
	//		std::pair<T,T> ret;
	//		auto now = f(nowQ);
	//		ret.first = (Eigen::Vector3<T>(ref(0), ref(1), ref(2)) - Eigen::Vector3<T>(now(0), now(1), now(2))).norm();
	//		ret.second = Eigen::Quaternion<T>(ref(6), ref(3), ref(4), ref(5)).dot(Eigen::Quaternion<T>(now(6), now(3), now(4), now(5)));

	//		return ret;
	//	}
	//	//内部で使用されている現在角度をとる
	//	VectorR GetNowQ() {
	//		return nowQ;
	//	}

	//	
	//};

	template<typename T, size_t dimR, size_t dimC>class armInverseKineticsSolver {
	public:
		using VectorR = Eigen::Matrix<T, dimR, 1>;
		using VectorC = Eigen::Matrix<T, dimC, 1>;
		using Matrix = Eigen::Matrix<T, dimR, dimC>;
		using MatrixK = Eigen::Matrix<T, dimC, dimC>;//ばね係数行列
		using MatrixJ = Eigen::Matrix<T, dimC, dimR>;//ヤコビ行列

		std::function<VectorC(const VectorR&)> kinetics;//順運動学の式
	protected:
		//各関節の持つ角速度
		VectorR angleVelocities;//角速度
		VectorR angles;//角度

		//値aの符号を残したまま大きさを比較する
		T AbsMinNorm(const T& a, const T& b) {
			auto sign = a < 0 ? -1.0 : 1.0;

			return sign * min(abs(a), b);
		}

	//各要素をスレッドセーブにするやつ
		std::mutex anglurVeloMutex, angleMutex;

	public:

		//引数バインドたち
		struct _defaultArguments {
			VectorR morments;//各関節のモーメント
			VectorC spring;//ばね定数
			double distance;//微分のための微小距離
		};
		_defaultArguments defaultArguments;

		void SetKinetics(const std::function<VectorC(const VectorR&)>& func) {
			kinetics = func;
		}

		armInverseKineticsSolver(const std::function<VectorC(const VectorR&)>& func, const VectorR& defAngVelos, const VectorR& defAngs) :angleVelocities(defAngVelos), angles(defAngs) {
			this->SetKinetics(func);

			this->defaultArguments.distance = M_PI / 1800;//0.1°
			for (size_t i = 0; i < dimC; i++)
				this->defaultArguments.spring[i] = 50.0;

			for (size_t i = 0; i < dimR; i++)
				this->defaultArguments.morments[i] = pow((double)(dimR - i), 2.0) / 1.0;
			int def = 0;
		}
		armInverseKineticsSolver(const std::function<VectorC(const VectorR&)>& func) :armInverseKineticsSolver(func, VectorR::Zero(), VectorR::Zero()) {

		}
		//各要素のアクセサ
		VectorR GetAngleVelocities(){
			std::lock_guard<std::mutex> lock(this->anglurVeloMutex);
			return this->angleVelocities;
		}
		VectorR GetAngles(){
			std::lock_guard<std::mutex> lock(this->angleMutex);
			return this->angles;
		}

	protected:

		MatrixJ CalcJacobi(const std::function<VectorC(const VectorR&)>& f, const VectorR& q, const T d)const {
			MatrixJ ret;
			//サイズが入れ替わる
			for (size_t i = 0; i < dimC; i++)//何行
				for (size_t j = 0; j < dimR; j++) {//何列
					//差分を作成する
					VectorR dvec;
					for (size_t k = 0; k < dimR; k++)
						if (k == j)dvec(k) = d;
						else dvec(k) = 0.0;
					//行でq 列でx,yを決める
					ret(i, j) = VectorC(f(q + dvec) - f(q))(i) / d;
				}

			return ret;
		}


		//各関節のトルクを求める f:手先に加わる力
		VectorR CalcTorquesForJoints(const VectorC& f, const T& d) const {
			auto transedJacobi = this->CalcJacobi(kinetics, angles, d).transpose();

			return transedJacobi * f;
		}

		//バネ的力を計算
		virtual VectorC CalcSpringForce(const VectorC& ref, const VectorC& k) const {
			//力は(目標-現在)*ばねで計算
			VectorC now = kinetics(angles);
			VectorC f = (ref - now);
			//ばね定数を掛け合わせる
			for (size_t i = 0; i < dimC; i++)
				f[i] *= k[i];

			return f;
		}

		//現在姿勢qと目標位置refにばね定数kのばねを張った時の各関節のトルクを求める
		VectorR CalcTorquesForJoints(const VectorC& ref, const VectorC& k, const T& d) const {

			auto f = CalcSpringForce(ref, k);

			return CalcTorquesForJoints(f, d);
		}

		//各関節の角速度的要素の計算
		virtual VectorR CalcAngleVelocitiesForJoints(double t, const VectorC& ref, const VectorC& k, const VectorR& morments, const T& d) {
			auto torques = this->CalcTorquesForJoints(ref, k, d);

			//トルク/モーメントが角加速度
			for (size_t i = 0; i < dimR; i++)
				angleVelocities[i] += (torques[i] / morments[i]) * t;

			return angleVelocities;
		}


	public:
		//各関節の角度的要素
		VectorR CalcAngleForJoints(double t, const VectorC& ref, const VectorC& k, const VectorR& morments, const T& d) {
			VectorR angvelo = this->CalcAngleVelocitiesForJoints(t, ref, k, morments, d);

			////角速度*時間で角度の変化
			for (size_t i = 0; i < dimR; i++)
				angles[i] += angvelo[i] * t;

			return angles;
		}
		VectorR CalcAngleForJoints(double t, const VectorC& ref) {
			return CalcAngleForJoints(t, ref, defaultArguments.spring, defaultArguments.morments, defaultArguments.distance);
		}


	};

	template<typename T, size_t dimR, size_t dimC>class armInverseKineticsSolverForKenkyu :public armInverseKineticsSolver<T, dimR, dimC> {

		const T maxAnglurVelo = M_PI / 50.0;//角速度の最大値
		const T jointReflection = 0.3;//関節のステップごとの減衰係数
		const T convergeThreshold = 0.01;//収束判定のしきい値
		const T stepTimeDistance = 1.0;//位置ステップの時間幅
		const T logHorizontalCoeff = 5.0;//対数の横軸の短縮

		using super = armInverseKineticsSolver<T, dimR, dimC>;
	public:
		using VectorR = Eigen::Matrix<T, dimR, 1>;
		using VectorC = Eigen::Matrix<T, dimC, 1>;
		using Matrix = Eigen::Matrix<T, dimR, dimC>;
		using MatrixK = Eigen::Matrix<T, dimC, dimC>;//ばね係数行列
		using MatrixJ = Eigen::Matrix<T, dimC, dimR>;//ヤコビ行列

	protected:
		VectorC kRatio;//ばね定数比

		VectorR CalcAngleVelocitiesForJoints(double t, const VectorC& ref, const VectorC& k, const VectorR& morments, const T& d) {
			auto torques = this->CalcTorquesForJoints(ref, k, d);

			//トルク/モーメントが角加速度
			for (size_t i = 0; i < dimR; i++)
				super::angleVelocities[i] += (torques[i] / morments[i]) * t;

			//関節の摩擦
			for (size_t i = 0; i < dimR; i++)
				super::angleVelocities[i] *= max(0.0, 1.0 - jointReflection * t);

			//角度制限
			for (size_t i = 0; i < dimR; i++)
				super::angleVelocities[i] = this->AbsMinNorm(super::angleVelocities[i], maxAnglurVelo);//al

			//std::cout << super::angleVelocities << std::endl;

			return super::angleVelocities;
		}
		//バネ的力を計算 対数的な力をかける
		VectorC CalcSpringForce(const VectorC& ref, const VectorC& k) const {
			//力はlog(目標-現在)
			VectorC now = super::kinetics(super::angles);
			VectorC junks = (ref - now);

			VectorC f;
			for (size_t i = 0; i < dimC; i++) {
				auto sign = junks[i] > 0 ? 1.0 : -1.0;
				f[i] = sign * kRatio[i] * log(abs(logHorizontalCoeff * junks[i]) + 1);
			}


			return f;
		}

		//発振しているか監視する
		class oscillationObserver {
			const T passOscillateThreshold = 0.001;//角速度の変化がこれ以下なら符号の変化を無視する
			const size_t sizeChangeOfVeloSign = 10;//符号変化を格納するストレージの大きさ　大きいほど長い時間の変化を加味する
			const T judgeOscilateThreshold = 0.5;//発振を判定するしきい値　小さいほどまれな発振でも検知する
		public:
			//符号変化を格納するストレージ
			std::vector<bool> changeOfVeloSign;
			size_t headChangeOfVeloSign;
			VectorR beforeAnglurVelo;

			oscillationObserver() {
				changeOfVeloSign.resize(sizeChangeOfVeloSign);
				std::fill(changeOfVeloSign.begin(), changeOfVeloSign.end(), false);
				headChangeOfVeloSign = 0;
				beforeAnglurVelo = VectorR::Zero();
			}

			bool operator()(const VectorR& nowAnglurVelos) {
				//発振の判定は角速度の符号が変化している粒度で決める

				//まずは今回の符号変化を書き込む ただし変化が微細であれば発振に含まない
				changeOfVeloSign.at(headChangeOfVeloSign) = false;
				for (size_t i = 0; i < dimR; i++) {
					if (abs(beforeAnglurVelo[i] - nowAnglurVelos[i]) < passOscillateThreshold)continue;
					changeOfVeloSign.at(headChangeOfVeloSign) = changeOfVeloSign.at(headChangeOfVeloSign) | (beforeAnglurVelo[i] * nowAnglurVelos[i] < 0.0);
				}

				headChangeOfVeloSign++;
				if (headChangeOfVeloSign == changeOfVeloSign.size()) headChangeOfVeloSign = 0;
				beforeAnglurVelo = nowAnglurVelos;


				//角速度変化の粒度をとる
				size_t distSum = 0;
				for (const auto& i : changeOfVeloSign)
					distSum += i;
				double distDensity = (double)distSum / changeOfVeloSign.size();

				//粒度が50%を超えていれば発振
				return distDensity >= judgeOscilateThreshold;
			}
		}observer;

		//収束判定
		bool CheckConvergence(const VectorC& ref) {
			auto now = this->kinetics(this->GetAngles());

			auto dist = (ref - now).norm();

			return dist <= convergeThreshold;
		}

	public:
		void SolverStep(const VectorC& ref,T span) {

			//そもそも収束していれば無視する
			if (this->CheckConvergence(ref))return;

			//スパンからすすめる時間を決定する1/16で1.0sになるように作る
			T timeStep = min(1.0, span / (1.0 / 16.0));

			//新しい角度を計算
			this->CalcAngleForJoints(timeStep, ref, VectorC::Zero(), this->defaultArguments.morments, this->defaultArguments.distance);

			//std::cout << this->observer(this->GetAngleVelocities()) << std::endl;
			//std::cout << this->nowSpringCoeff << std::endl;

			//発振を検知すると角速度をリセットする
			if (this->observer(this->GetAngleVelocities())) {
				this->angleVelocities = VectorR::Zero();
				std::cout << "\a" << std::endl;
			}


		}

		armInverseKineticsSolverForKenkyu(const std::function<VectorC(const VectorR&)>& func, const VectorR& defAngVelos, const VectorR& defAngs, const VectorC& kRatio) : armInverseKineticsSolver<T, dimR, dimC>(func, defAngVelos, defAngs) {

			this->kRatio = kRatio;
			//モーメントを最適化しておく
			this->defaultArguments.morments = Eigen::Matrix<double, 6, 1>(25, 25, 16, 9, 9, 4);
		}
		armInverseKineticsSolverForKenkyu(const std::function<VectorC(const VectorR&)>& func) :armInverseKineticsSolverForKenkyu(func, VectorR::Zero(), VectorR::Zero(), [&] {VectorC ret; for (size_t i = 0; i < dimC; i++)ret[i] = 5.0; return ret; }()) {

		}
	};

};