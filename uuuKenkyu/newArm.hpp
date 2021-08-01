
#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <iostream>
#include <Eigen/QR>
#include <functional>

#define M_PI       3.14159265358979323846   // pi

namespace newarm {
	template<typename T, size_t dimR, size_t dimC>class angleSolver {
	public:
		using VectorR = Eigen::Matrix<T, dimR, 1>;
		using VectorC = Eigen::Matrix<T, dimC, 1>;
		using Matrix = Eigen::Matrix<T, dimR, dimC>;
		using MatrixK = Eigen::Matrix<T, dimC, dimC>;//ばね係数行列
		using MatrixJ = Eigen::Matrix<T, dimC, dimR>;//ヤコビ行列

		MatrixJ CalcJacobi(const std::function<VectorC(const VectorR&)>& f, const VectorR& q, const T d = M_PI / 1800) {
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

		template<typename Tpow, size_t dimpow> Eigen::Matrix<Tpow, dimpow, 1> powv(const Eigen::Matrix<Tpow, dimpow, 1>& ini) {
			Eigen::Matrix<Tpow, dimpow, 1> ret;
			for (size_t i = 0; i < dimpow; i++)
				ret(i) = pow(ini(i), 2);

			return ret;
		}

		//目標位置と現在位置とその他パラメータから次点での角度を求める
		VectorR SolveDist(const std::function<VectorC(const VectorR&)>& f, const MatrixK k, const VectorC& reg, const VectorR& q, const T d = M_PI / 1800) {
			/*
			f 正運動学の式
			k ばね定数
			reg 目標位置
			q 現在の変位
			d デルタ
			*/
			auto current = f(q);
			auto jacobi = CalcJacobi(f, q, d);//現在地のヤコビ行列を導く

			//t=J^TK(R-c)なのでdQに置き換えられる?? なんで?
			VectorR deltaQ = jacobi.transpose() * k * (reg - current);

			return deltaQ + q;

		}
	};

	template<typename T, size_t dimR, size_t dimC>class armSolver :public angleSolver<T, dimR, dimC> {
	public:
		using VectorR = Eigen::Matrix<T, dimR, 1>;
		using VectorC = Eigen::Matrix<T, dimC, 1>;
		using Matrix = Eigen::Matrix<T, dimR, dimC>;
		using MatrixK = Eigen::Matrix<T, dimC, dimC>;//ばね係数行列

	protected:
		VectorR nowQ;
		std::function<VectorC(const VectorR&)> f;
		VectorC ref;

		template<typename T> T CorrectAngle(const T& a) {
			//0から2piまで
			//2pi以上ｈじゃいらん
			auto disroop = fmod(a, 2 * M_PI);

			if (disroop < 0.0)disroop += 2 * M_PI;

			return disroop;
		}
		template<typename T, size_t dim>Eigen::Matrix<T, dim, 1> CorrectAngleVec(const Eigen::Matrix<T, dim, 1>& m) {
			Eigen::Matrix<T, dim, 1> ret;

			for (size_t i = 0; i < dim; i++)
				ret(i) = CorrectAngle<T>(m(i));

			return ret;
		}
	public:


		armSolver(const VectorR& defQ, const std::function<VectorC(const VectorR&)>& defF) :nowQ(defQ), f(defF) {
			ref = f(defQ);
		}

		VectorR GetNextQ(const VectorC& k) {

			//バネ行列を作成
			Eigen::Matrix<T, dimC, dimC> kmat = Eigen::Matrix<T, dimC, dimC>::Zero();
			for (size_t i = 0; i < dimC; i++)kmat(i, i) = k(i);


			//VectorR nextq = CorrectAngleVec(this->SolveDist(f, kmat, ref, nowQ));
			VectorR nextq = this->SolveDist(f, kmat, ref, nowQ);

			nowQ = nextq;

			return nextq;
		}

		void SetRef(const VectorC& s) {
			ref = s;
		}
		void ResetNow(const VectorR& s) {
			nowQ = s;
		}

		//現在の手先と目標一の差分をとる
		T GetDistNowtoRef() {
			return(ref - f(nowQ)).norm();
		}
		//現在の手先と目標一の差分をとる
		T GetDistNowtoRefSquared() {
			return(ref - f(nowQ)).squaredNorm();
		}

		//現在の手先位置の誤差と姿勢の誤差をとる<位置,姿勢>
		std::pair<T, T> GetDistPosAndQuat() {
			std::pair<T,T> ret;
			auto now = f(nowQ);
			ret.first = (Eigen::Vector3<T>(ref(0), ref(1), ref(2)) - Eigen::Vector3<T>(now(0), now(1), now(2))).norm();
			ret.second = Eigen::Quaternion<T>(ref(6), ref(3), ref(4), ref(5)).dot(Eigen::Quaternion<T>(now(6), now(3), now(4), now(5)));

			return ret;
		}
		//内部で使用されている現在角度をとる
		VectorR GetNowQ() {
			return nowQ;
		}

		
	};

};