
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
		using MatrixK = Eigen::Matrix<T, dimC, dimC>;//�΂ˌW���s��
		using MatrixJ = Eigen::Matrix<T, dimC, dimR>;//���R�r�s��

		MatrixJ CalcJacobi(const std::function<VectorC(const VectorR&)>& f, const VectorR& q, const T d = M_PI / 1800) {
			MatrixJ ret;
			//�T�C�Y������ւ��
			for (size_t i = 0; i < dimC; i++)//���s
				for (size_t j = 0; j < dimR; j++) {//����
					//�������쐬����
					VectorR dvec;
					for (size_t k = 0; k < dimR; k++)
						if (k == j)dvec(k) = d;
						else dvec(k) = 0.0;
					//�s��q ���x,y�����߂�
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

		//�ڕW�ʒu�ƌ��݈ʒu�Ƃ��̑��p�����[�^���玟�_�ł̊p�x�����߂�
		VectorR SolveDist(const std::function<VectorC(const VectorR&)>& f, const MatrixK k, const VectorC& reg, const VectorR& q, const T d = M_PI / 1800) {
			/*
			f ���^���w�̎�
			k �΂˒萔
			reg �ڕW�ʒu
			q ���݂̕ψ�
			d �f���^
			*/
			auto current = f(q);
			auto jacobi = CalcJacobi(f, q, d);//���ݒn�̃��R�r�s��𓱂�

			//t=J^TK(R-c)�Ȃ̂�dQ�ɒu����������?? �Ȃ��?
			VectorR deltaQ = jacobi.transpose() * k * (reg - current);

			return deltaQ + q;

		}
	};

	template<typename T, size_t dimR, size_t dimC>class armSolver :public angleSolver<T, dimR, dimC> {
	public:
		using VectorR = Eigen::Matrix<T, dimR, 1>;
		using VectorC = Eigen::Matrix<T, dimC, 1>;
		using Matrix = Eigen::Matrix<T, dimR, dimC>;
		using MatrixK = Eigen::Matrix<T, dimC, dimC>;//�΂ˌW���s��

	protected:
		VectorR nowQ;
		std::function<VectorC(const VectorR&)> f;
		VectorC ref;

		template<typename T> T CorrectAngle(const T& a) {
			//0����2pi�܂�
			//2pi�ȏよ���Ⴂ���
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

			//�o�l�s����쐬
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

		//���݂̎��ƖڕW��̍������Ƃ�
		T GetDistNowtoRef() {
			return(ref - f(nowQ)).norm();
		}
		//���݂̎��ƖڕW��̍������Ƃ�
		T GetDistNowtoRefSquared() {
			return(ref - f(nowQ)).squaredNorm();
		}

		//���݂̎��ʒu�̌덷�Ǝp���̌덷���Ƃ�<�ʒu,�p��>
		std::pair<T, T> GetDistPosAndQuat() {
			std::pair<T,T> ret;
			auto now = f(nowQ);
			ret.first = (Eigen::Vector3<T>(ref(0), ref(1), ref(2)) - Eigen::Vector3<T>(now(0), now(1), now(2))).norm();
			ret.second = Eigen::Quaternion<T>(ref(6), ref(3), ref(4), ref(5)).dot(Eigen::Quaternion<T>(now(6), now(3), now(4), now(5)));

			return ret;
		}
		//�����Ŏg�p����Ă��錻�݊p�x���Ƃ�
		VectorR GetNowQ() {
			return nowQ;
		}

		
	};

};