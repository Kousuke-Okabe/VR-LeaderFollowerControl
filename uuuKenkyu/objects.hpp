#pragma once

#include <uuu.hpp>
#include <uuuGame.hpp>

namespace _uuu {
	namespace objects {
		//ベースになるオブジェクト　型情報を実装している
		class object {
		protected:
			
		public:
			using fulltype = std::vector<std::string>;
			virtual fulltype GetFullTypeInfo();//完全な型状態を取り出す
			std::string GetMostLikelyTypeInfo();//最も狭い型情報を取り出す

			bool CheckType(const std::string& str);//型がヒットしているか確かめる
		};
		//描画オブジェクト　　描画時に呼び出す描画メソッドを持つ
		class drawableObject :public object {
		public:
			virtual fulltype GetFullTypeInfo();
			virtual void Draw() = 0;

		};

		//位置を持つ&描画できるオブジェクト 位置のアクセサを含む
		class movableObject:public drawableObject {
		public:
			virtual fulltype GetFullTypeInfo();
			virtual void Draw() = 0;

			virtual glm::mat4 GetTransform() = 0;
			virtual void SetTransform(const glm::mat4& tr) = 0;
		};

		//ゲームメッシュのオブジェクト
		class gameMeshObject :public movableObject {
		protected:
			std::unique_ptr<uuu::game::mesh> obj;
		public:

		};
	};
};