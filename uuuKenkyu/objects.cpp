#include "objects.hpp"

using namespace std;
using namespace _uuu::objects;

object::fulltype _uuu::objects::object::GetFullTypeInfo() {
	return { "object" };
}

string _uuu::objects::object::GetMostLikelyTypeInfo() {
	return this->GetFullTypeInfo().back();
}

bool _uuu::objects::object::CheckType(const std::string& typenam) {
	object::fulltype typeinfo = this->GetFullTypeInfo();
	return std::find(typeinfo.begin(), typeinfo.end(), typenam) != typeinfo.end();
}

object::fulltype _uuu::objects::drawableObject::GetFullTypeInfo() {
	object::fulltype ret = object::GetFullTypeInfo();
	ret.push_back("drawableObject");
	return ret;
}

object::fulltype _uuu::objects::movableObject::GetFullTypeInfo() {
	object::fulltype ret = drawableObject::GetFullTypeInfo();
	ret.push_back("movableObject");
	return ret;
}

object::fulltype _uuu::objects::gameMeshObject::GetFullTypeInfo() {
	object::fulltype ret = movableObject::GetFullTypeInfo();
	ret.push_back("gameMeshObject");
	return ret;
}