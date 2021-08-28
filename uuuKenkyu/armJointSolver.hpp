
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
	//	using MatrixK = Eigen::Matrix<T, dimC, dimC>;//�΂ˌW���s��
	//	using MatrixJ = Eigen::Matrix<T, dimC, dimR>;//���R�r�s��

	//	MatrixJ CalcJacobi(const std::function<VectorC(const VectorR&)>& f, const VectorR& q, const T d = M_PI / 1800) {
	//		MatrixJ ret;
	//		//�T�C�Y������ւ��
	//		for (size_t i = 0; i < dimC; i++)//���s
	//			for (size_t j = 0; j < dimR; j++) {//����
	//				//�������쐬����
	//				VectorR dvec;
	//				for (size_t k = 0; k < dimR; k++)
	//					if (k == j)dvec(k) = d;
	//					else dvec(k) = 0.0;
	//				//�s��q ���x,y�����߂�
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

	//	//�ڕW�ʒu�ƌ��݈ʒu�Ƃ��̑��p�����[�^���玟�_�ł̊p�x�����߂�
	//	VectorR SolveDist(const std::function<VectorC(const VectorR&)>& f, const MatrixK k, const VectorC& reg, const VectorR& q, const T d = M_PI / 1800) {
	//		/*
	//		f ���^���w�̎�
	//		k �΂˒萔
	//		reg �ڕW�ʒu
	//		q ���݂̕ψ�
	//		d �f���^
	//		*/
	//		auto current = f(q);
	//		auto jacobi = CalcJacobi(f, q, d);//���ݒn�̃��R�r�s��𓱂�

	//		//t=J^TK(R-c)�Ȃ̂�dQ�ɒu����������?? �Ȃ��?
	//		VectorR deltaQ = jacobi.transpose() * k * (reg - current);

	//		return deltaQ + q;

	//	}
	//};

	//template<typename T, size_t dimR, size_t dimC>class armSolver :public angleSolver<T, dimR, dimC> {
	//public:
	//	using VectorR = Eigen::Matrix<T, dimR, 1>;
	//	using VectorC = Eigen::Matrix<T, dimC, 1>;
	//	using Matrix = Eigen::Matrix<T, dimR, dimC>;
	//	using MatrixK = Eigen::Matrix<T, dimC, dimC>;//�΂ˌW���s��

	//protected:
	//	VectorR nowQ;
	//	std::function<VectorC(const VectorR&)> f;
	//	VectorC ref;

	//	template<typename T> T CorrectAngle(const T& a) {
	//		//0����2pi�܂�
	//		//2pi�ȏよ���Ⴂ���
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

	//		//�o�l�s����쐬
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

	//	//���݂̎��ƖڕW��̍������Ƃ�
	//	T GetDistNowtoRef() {
	//		return(ref - f(nowQ)).norm();
	//	}
	//	//���݂̎��ƖڕW��̍������Ƃ�
	//	T GetDistNowtoRefSquared() {
	//		return(ref - f(nowQ)).squaredNorm();
	//	}

	//	//���݂̎��ʒu�̌덷�Ǝp���̌덷���Ƃ�<�ʒu,�p��>
	//	std::pair<T, T> GetDistPosAndQuat() {
	//		std::pair<T,T> ret;
	//		auto now = f(nowQ);
	//		ret.first = (Eigen::Vector3<T>(ref(0), ref(1), ref(2)) - Eigen::Vector3<T>(now(0), now(1), now(2))).norm();
	//		ret.second = Eigen::Quaternion<T>(ref(6), ref(3), ref(4), ref(5)).dot(Eigen::Quaternion<T>(now(6), now(3), now(4), now(5)));

	//		return ret;
	//	}
	//	//�����Ŏg�p����Ă��錻�݊p�x���Ƃ�
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
		using MatrixK = Eigen::Matrix<T, dimC, dimC>;//�΂ˌW���s��
		using MatrixJ = Eigen::Matrix<T, dimC, dimR>;//���R�r�s��

		std::function<VectorC(const VectorR&)> kinetics;//���^���w�̎�
	protected:
		//�e�֐߂̎��p���x
		VectorR angleVelocities;//�p���x
		VectorR angles;//�p�x

		//�la�̕������c�����܂ܑ傫�����r����
		T AbsMinNorm(const T& a, const T& b) {
			auto sign = a < 0 ? -1.0 : 1.0;

			return sign * min(abs(a), b);
		}

	//�e�v�f���X���b�h�Z�[�u�ɂ�����
		std::mutex anglurVeloMutex, angleMutex;

	public:

		//�����o�C���h����
		struct _defaultArguments {
			VectorR morments;//�e�֐߂̃��[�����g
			VectorC spring;//�΂˒萔
			double distance;//�����̂��߂̔�������
		};
		_defaultArguments defaultArguments;

		void SetKinetics(const std::function<VectorC(const VectorR&)>& func) {
			kinetics = func;
		}

		armInverseKineticsSolver(const std::function<VectorC(const VectorR&)>& func, const VectorR& defAngVelos, const VectorR& defAngs) :angleVelocities(defAngVelos), angles(defAngs) {
			this->SetKinetics(func);

			this->defaultArguments.distance = M_PI / 1800;//0.1��
			for (size_t i = 0; i < dimC; i++)
				this->defaultArguments.spring[i] = 50.0;

			for (size_t i = 0; i < dimR; i++)
				this->defaultArguments.morments[i] = pow((double)(dimR - i), 2.0) / 1.0;
			int def = 0;
		}
		armInverseKineticsSolver(const std::function<VectorC(const VectorR&)>& func) :armInverseKineticsSolver(func, VectorR::Zero(), VectorR::Zero()) {

		}
		//�e�v�f�̃A�N�Z�T
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


		//�e�֐߂̃g���N�����߂� f:���ɉ�����
		VectorR CalcTorquesForJoints(const VectorC& f, const T& d) const {
			auto transedJacobi = this->CalcJacobi(kinetics, angles, d).transpose();

			return transedJacobi * f;
		}

		//�o�l�I�͂��v�Z
		virtual VectorC CalcSpringForce(const VectorC& ref, const VectorC& k) const {
			//�͂�(�ڕW-����)*�΂˂Ōv�Z
			VectorC now = kinetics(angles);
			VectorC f = (ref - now);
			//�΂˒萔���|�����킹��
			for (size_t i = 0; i < dimC; i++)
				f[i] *= k[i];

			return f;
		}

		//���ݎp��q�ƖڕW�ʒuref�ɂ΂˒萔k�̂΂˂𒣂������̊e�֐߂̃g���N�����߂�
		VectorR CalcTorquesForJoints(const VectorC& ref, const VectorC& k, const T& d) const {

			auto f = CalcSpringForce(ref, k);

			return CalcTorquesForJoints(f, d);
		}

		//�e�֐߂̊p���x�I�v�f�̌v�Z
		virtual VectorR CalcAngleVelocitiesForJoints(double t, const VectorC& ref, const VectorC& k, const VectorR& morments, const T& d) {
			auto torques = this->CalcTorquesForJoints(ref, k, d);

			//�g���N/���[�����g���p�����x
			for (size_t i = 0; i < dimR; i++)
				angleVelocities[i] += (torques[i] / morments[i]) * t;

			return angleVelocities;
		}


	public:
		//�e�֐߂̊p�x�I�v�f
		VectorR CalcAngleForJoints(double t, const VectorC& ref, const VectorC& k, const VectorR& morments, const T& d) {
			VectorR angvelo = this->CalcAngleVelocitiesForJoints(t, ref, k, morments, d);

			////�p���x*���ԂŊp�x�̕ω�
			for (size_t i = 0; i < dimR; i++)
				angles[i] += angvelo[i] * t;

			return angles;
		}
		VectorR CalcAngleForJoints(double t, const VectorC& ref) {
			return CalcAngleForJoints(t, ref, defaultArguments.spring, defaultArguments.morments, defaultArguments.distance);
		}


	};

	template<typename T, size_t dimR, size_t dimC>class armInverseKineticsSolverForKenkyu :public armInverseKineticsSolver<T, dimR, dimC> {

		const T maxAnglurVelo = M_PI / 50.0;//�p���x�̍ő�l
		const T jointReflection = 0.3;//�֐߂̃X�e�b�v���Ƃ̌����W��
		const T convergeThreshold = 0.01;//��������̂������l
		const T stepTimeDistance = 1.0;//�ʒu�X�e�b�v�̎��ԕ�
		const T logHorizontalCoeff = 5.0;//�ΐ��̉����̒Z�k

		using super = armInverseKineticsSolver<T, dimR, dimC>;
	public:
		using VectorR = Eigen::Matrix<T, dimR, 1>;
		using VectorC = Eigen::Matrix<T, dimC, 1>;
		using Matrix = Eigen::Matrix<T, dimR, dimC>;
		using MatrixK = Eigen::Matrix<T, dimC, dimC>;//�΂ˌW���s��
		using MatrixJ = Eigen::Matrix<T, dimC, dimR>;//���R�r�s��

	protected:
		VectorC kRatio;//�΂˒萔��

		VectorR CalcAngleVelocitiesForJoints(double t, const VectorC& ref, const VectorC& k, const VectorR& morments, const T& d) {
			auto torques = this->CalcTorquesForJoints(ref, k, d);

			//�g���N/���[�����g���p�����x
			for (size_t i = 0; i < dimR; i++)
				super::angleVelocities[i] += (torques[i] / morments[i]) * t;

			//�֐߂̖��C
			for (size_t i = 0; i < dimR; i++)
				super::angleVelocities[i] *= max(0.0, 1.0 - jointReflection * t);

			//�p�x����
			for (size_t i = 0; i < dimR; i++)
				super::angleVelocities[i] = this->AbsMinNorm(super::angleVelocities[i], maxAnglurVelo);//al

			//std::cout << super::angleVelocities << std::endl;

			return super::angleVelocities;
		}
		//�o�l�I�͂��v�Z �ΐ��I�ȗ͂�������
		VectorC CalcSpringForce(const VectorC& ref, const VectorC& k) const {
			//�͂�log(�ڕW-����)
			VectorC now = super::kinetics(super::angles);
			VectorC junks = (ref - now);

			VectorC f;
			for (size_t i = 0; i < dimC; i++) {
				auto sign = junks[i] > 0 ? 1.0 : -1.0;
				f[i] = sign * kRatio[i] * log(abs(logHorizontalCoeff * junks[i]) + 1);
			}


			return f;
		}

		//���U���Ă��邩�Ď�����
		class oscillationObserver {
			const T passOscillateThreshold = 0.001;//�p���x�̕ω�������ȉ��Ȃ畄���̕ω��𖳎�����
			const size_t sizeChangeOfVeloSign = 10;//�����ω����i�[����X�g���[�W�̑傫���@�傫���قǒ������Ԃ̕ω�����������
			const T judgeOscilateThreshold = 0.5;//���U�𔻒肷�邵�����l�@�������قǂ܂�Ȕ��U�ł����m����
		public:
			//�����ω����i�[����X�g���[�W
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
				//���U�̔���͊p���x�̕������ω����Ă��闱�x�Ō��߂�

				//�܂��͍���̕����ω����������� �������ω������ׂł���Δ��U�Ɋ܂܂Ȃ�
				changeOfVeloSign.at(headChangeOfVeloSign) = false;
				for (size_t i = 0; i < dimR; i++) {
					if (abs(beforeAnglurVelo[i] - nowAnglurVelos[i]) < passOscillateThreshold)continue;
					changeOfVeloSign.at(headChangeOfVeloSign) = changeOfVeloSign.at(headChangeOfVeloSign) | (beforeAnglurVelo[i] * nowAnglurVelos[i] < 0.0);
				}

				headChangeOfVeloSign++;
				if (headChangeOfVeloSign == changeOfVeloSign.size()) headChangeOfVeloSign = 0;
				beforeAnglurVelo = nowAnglurVelos;


				//�p���x�ω��̗��x���Ƃ�
				size_t distSum = 0;
				for (const auto& i : changeOfVeloSign)
					distSum += i;
				double distDensity = (double)distSum / changeOfVeloSign.size();

				//���x��50%�𒴂��Ă���Δ��U
				return distDensity >= judgeOscilateThreshold;
			}
		}observer;

		//��������
		bool CheckConvergence(const VectorC& ref) {
			auto now = this->kinetics(this->GetAngles());

			auto dist = (ref - now).norm();

			return dist <= convergeThreshold;
		}

	public:
		void SolverStep(const VectorC& ref,T span) {

			//���������������Ă���Ζ�������
			if (this->CheckConvergence(ref))return;

			//�X�p�����炷���߂鎞�Ԃ����肷��1/16��1.0s�ɂȂ�悤�ɍ��
			T timeStep = min(1.0, span / (1.0 / 16.0));

			//�V�����p�x���v�Z
			this->CalcAngleForJoints(timeStep, ref, VectorC::Zero(), this->defaultArguments.morments, this->defaultArguments.distance);

			//std::cout << this->observer(this->GetAngleVelocities()) << std::endl;
			//std::cout << this->nowSpringCoeff << std::endl;

			//���U�����m����Ɗp���x�����Z�b�g����
			if (this->observer(this->GetAngleVelocities())) {
				this->angleVelocities = VectorR::Zero();
				std::cout << "\a" << std::endl;
			}


		}

		armInverseKineticsSolverForKenkyu(const std::function<VectorC(const VectorR&)>& func, const VectorR& defAngVelos, const VectorR& defAngs, const VectorC& kRatio) : armInverseKineticsSolver<T, dimR, dimC>(func, defAngVelos, defAngs) {

			this->kRatio = kRatio;
			//���[�����g���œK�����Ă���
			this->defaultArguments.morments = Eigen::Matrix<double, 6, 1>(25, 25, 16, 9, 9, 4);
		}
		armInverseKineticsSolverForKenkyu(const std::function<VectorC(const VectorR&)>& func) :armInverseKineticsSolverForKenkyu(func, VectorR::Zero(), VectorR::Zero(), [&] {VectorC ret; for (size_t i = 0; i < dimC; i++)ret[i] = 5.0; return ret; }()) {

		}
	};

};