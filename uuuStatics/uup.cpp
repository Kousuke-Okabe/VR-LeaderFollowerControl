//#include <uuu.hpp>
//
//std::vector<btDynamicsWorld*> uuu::uup::physicsManager::worlds;
//boost::optional<size_t>  uuu::uup::physicsManager::binded;
//std::unordered_map<btCollisionObject*, uuu::uup::rigidBase*> uuu::uup::physicsManager::collisionObjMap;
//
//uuu::uup::getAllContactObjectCallBack::getAllContactObjectCallBack() :btDynamicsWorld::ContactResultCallback() {
//	result.clear();
//}
//btScalar uuu::uup::getAllContactObjectCallBack::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
//	result.push_back(colObj0Wrap->getCollisionObject());
//	result.push_back(colObj1Wrap->getCollisionObject());
//
//
//	return 0;
//}
//
//uuu::uup::contactOneObjectCallBack::contactOneObjectCallBack(btCollisionObject* oneObj) :btDynamicsWorld::ContactResultCallback() {
//	this->target = oneObj;
//	this->resultFrag = false;
//}
//btScalar uuu::uup::contactOneObjectCallBack::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
//
//	//exit(9);
//
//	if (this->target == colObj0Wrap->getCollisionObject())this->resultFrag = true;
//	if (this->target == colObj1Wrap->getCollisionObject())this->resultFrag = true;
//
//	return 0;
//}
//
//__int8 uuu::uup::physicsManager::CreateNewWorld(worldProp prop) {
//
//	// �Փˌ��o���@�̑I��(�f�t�H���g��I��)
//	btDefaultCollisionConfiguration *config = new btDefaultCollisionConfiguration();
//	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(config);
//
//	// �u���[�h�t�F�[�Y�@�̐ݒ�(Dynamic AABB tree method)
//	btDbvtBroadphase *broadphase = new btDbvtBroadphase();
//
//	// �S��(���̊ԃ����N)�̃\���o�ݒ�
//	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
//
//	// Bullet�̃��[���h�쐬
//	btDynamicsWorld* newWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);
//	newWorld->setGravity(btVector3(prop.gravity.x, prop.gravity.y, prop.gravity.z));
//
//	//if(prop.enableGimpact)
//		btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
//		
//
//	//�o�^
//	binded = worlds.size();
//	worlds.push_back(newWorld);
//
//	return binded.get();
//}
//__int8 uuu::uup::physicsManager::SimulationBinded(size_t sub, float clock) {
//
//	if (uuu::uup::physicsManager::worlds.size() <= uuu::uup::physicsManager::binded.get())throw std::out_of_range("�o�C���h�������ł��@�c�O");
//
//	//���̌Ăяo�����Ԃ�ۑ�
//	static size_t beforeCall = 0;
//
//	if (clock != 0)
//		uuu::uup::physicsManager::worlds.at(uuu::uup::physicsManager::binded.get())->stepSimulation(clock, sub);
//	else {
//		uuu::uup::physicsManager::worlds.at(uuu::uup::physicsManager::binded.get())->stepSimulation((float)(uuu::app::GetTimeFromInit()-beforeCall)/1000.0, sub);
//	}
//	beforeCall = uuu::app::GetTimeFromInit();
//
//	return true;
//}
//
//uuu::uup::rigidBase::rigidBase() {
//	this->rigidObj = NULL;
//}
//__int8 uuu::uup::rigidBase::AddThisFromBindedWorld() {
//	if (uuu::uup::physicsManager::worlds.size() <= uuu::uup::physicsManager::binded.get())throw std::out_of_range("�o�C���h�������ł��@�c�O");
//
//	uuu::uup::physicsManager::worlds.at(uuu::uup::physicsManager::binded.get())->addRigidBody(this->rigidObj);
//
//	return true;
//}
//glm::mat4 uuu::uup::rigidBase::GetTransform() {
//	glm::mat4 ret;
//	this->rigidObj->getCenterOfMassTransform().getOpenGLMatrix(&ret[0][0]);
//
//	return ret;
//}
//glm::vec3 uuu::uup::rigidBase::GetMassPosition() {
//
//	auto pos = this->rigidObj->getCenterOfMassPosition();
//
//	return glm::vec3(pos.x(), pos.y(), pos.z());
//
//}
//__int8 uuu::uup::rigidBase::SetMassPosition(const glm::vec3& pos) {
//
//	this->rigidObj->setCenterOfMassTransform(btTransform(this->rigidObj->getCenterOfMassTransform().getRotation(), btVector3(pos.x, pos.y, pos.z)));
//
//	return true;
//}
//__int8 uuu::uup::rigidBase::ApplyForce(glm::vec3 force, glm::vec3 pos) {
//
//	this->rigidObj->activate();
//
//	this->rigidObj->applyForce(btVector3(force.x, force.y, force.z), btVector3(pos.x, pos.y, pos.z));
//
//	return true;
//}
//glm::vec3 uuu::uup::rigidBase::GetLinearVelocity() {
//	auto v=this->rigidObj->getLinearVelocity();
//
//
//	return glm::vec3(v.x(), v.y(), v.z());
//}
//__int8 uuu::uup::rigidBase::SetLinearVelocity(const glm::vec3& vel) {
//
//	this->rigidObj->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
//
//	return true;
//
//}
//__int8 uuu::uup::rigidBase::CheckHitOtherObject(uuu::uup::rigidBase* tar) {
//
//	uuu::uup::contactOneObjectCallBack callback(tar->rigidObj);//�P��I�u�W�F�N�g�Ƃ̃R�[���o�b�N
//
//	//�e�X�g
//	uuu::uup::physicsManager::worlds[uuu::uup::physicsManager::binded.get()]->contactPairTest(this->rigidObj, tar->rigidObj, callback);
//
//	return callback.resultFrag;
//}
//__int8 uuu::uup::rigidBase::GetHitAllObjects(std::vector<uuu::uup::rigidBase*>& ret){
//
//	//���ׂẴI�u�W�F�N�g�Ɠ����蔻��
//	uuu::uup::getAllContactObjectCallBack callback;
//
//	uuu::uup::physicsManager::worlds[uuu::uup::physicsManager::binded.get()]->contactTest(this->rigidObj, callback);
//
//	//�|�C���^���
//	ret.clear();
//	for (const btCollisionObject* i : callback.result)
//		ret.push_back(uuu::uup::physicsManager::collisionObjMap.at(const_cast<btCollisionObject*>(i)));
//
//	return true;
//}
//
//
//__int8 uuu::uup::modelRigid::__ExportIndices(std::vector<int>& ret, std::vector<unsigned int>& gen) {
//
//	for (size_t i = 0; i < gen.size(); i++)
//		ret.push_back(gen.at(i));
//
//	return true;
//}
//__int8 uuu::uup::modelRigid::__ExportVertices(std::vector<btScalar>& ret, std::vector<glm::vec4>& gen) {
//
//	for (size_t i = 0; i < gen.size(); i++) {
//		ret.push_back(gen.at(i).x);
//		ret.push_back(gen.at(i).y);
//		ret.push_back(gen.at(i).z);
//
//	}
//
//	return true;
//}
//__int8 uuu::uup::modelRigid::CreateFromModel(uuu::modelLoaderSerializer& mod, uuu::uup::modelRigid::rigidProp prop, bool addBinded) {
//
//	//std::vector<int> exportedIndices;
//	//std::vector<btScalar> exportedVertices;
//	this->__ExportVertices(exportedVertices, *mod.GetVerticesList());
//	this->__ExportIndices(exportedIndices, *mod.GetIndicesList());
//
//	btTriangleIndexVertexArray* tri_array = new btTriangleIndexVertexArray(exportedIndices.size() / 3, exportedIndices.data(), sizeof(int) * 3, exportedVertices.size() / 3, exportedVertices.data(), sizeof(btScalar) * 3);
//	shape = new btGImpactMeshShape(tri_array);
//	shape->updateBound();
//
//	btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(prop.qrot, prop.position));
//	shape->calculateLocalInertia(prop.mass, prop.inertia);
//
//	this->rigidObj= new btRigidBody(prop.mass, motion_state, shape, prop.inertia);
//
//	//��������I�v�V����
//	if (prop.restitution)
//		this->rigidObj->setRestitution(prop.restitution.get());
//	if (prop.friction)
//		this->rigidObj->setFriction(prop.friction.get());
//
//	if (addBinded)this->AddThisFromBindedWorld();
//
//	//�}�b�v�ɒǉ��@����œ����蔻��̒ǐՂ��ł���
//	uuu::uup::physicsManager::collisionObjMap[this->rigidObj] = this;
//
//	return true;
//}
//uuu::uup::modelRigid::~modelRigid() {
//	delete this->shape->getMeshInterface();
//	delete this->shape;
//	delete this->rigidObj->getMotionState();
//	delete this->rigidObj;
//
//}
//
//__int8 uuu::uup::sphereRigid::CreateRigid(uuu::uup::sphereRigid::rigidProp prop, bool addBinded) {
//
//	// ���̌`���ݒ�
//	btCollisionShape *sphere_shape = new btSphereShape(prop.radius);
//
//	// ���̂̏����ʒu�E�p��
//	btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(prop.qrot, prop.position));
//
//	// �������[�����g�̌v�Z
//	sphere_shape->calculateLocalInertia(prop.mass, prop.inertia);
//
//	// ���̃I�u�W�F�N�g����(���ʁC�ʒu�p���C�`��C�������[�����g��ݒ�)
//	this->rigidObj = new btRigidBody(prop.mass, motion_state, sphere_shape, prop.inertia);
//
//	//��������I�v�V����
//	if (prop.restitution)
//		this->rigidObj->setRestitution(prop.restitution.get());
//	if (prop.friction)
//		this->rigidObj->setFriction(prop.friction.get());
//
//	// ���[���h�ɍ��̃I�u�W�F�N�g��ǉ�
//	if (addBinded)
//		this->AddThisFromBindedWorld();
//
//	//�}�b�v�ɒǉ��@����œ����蔻��̒ǐՂ��ł���
//	uuu::uup::physicsManager::collisionObjMap[this->rigidObj] = this;
//
//	return true;
//
//}
//uuu::uup::sphereRigid::~sphereRigid() {
//	delete this->rigidObj->getMotionState();
//	delete this->rigidObj->getCollisionShape();
//	delete this->rigidObj;
//
//	return;
//}
//__int8 uuu::uup::boxRigid::CreateRigid(uuu::uup::boxRigid::rigidProp prop, bool addBinded) {
//	// ���̌`���ݒ�
//	btCollisionShape *sphere_shape = new btBoxShape(prop.lengths);
//
//	// ���̂̏����ʒu�E�p��
//	btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(prop.qrot, prop.position));
//
//	// �������[�����g�̌v�Z
//	sphere_shape->calculateLocalInertia(prop.mass, prop.inertia);
//
//	// ���̃I�u�W�F�N�g����(���ʁC�ʒu�p���C�`��C�������[�����g��ݒ�)
//	this->rigidObj = new btRigidBody(prop.mass, motion_state, sphere_shape, prop.inertia);
//
//	//��������I�v�V����
//	if (prop.restitution)
//		this->rigidObj->setRestitution(prop.restitution.get());
//	if (prop.friction)
//		this->rigidObj->setFriction(prop.friction.get());
//
//	// ���[���h�ɍ��̃I�u�W�F�N�g��ǉ�
//	if (addBinded)
//		this->AddThisFromBindedWorld();
//
//	//�}�b�v�ɒǉ��@����œ����蔻��̒ǐՂ��ł���
//	uuu::uup::physicsManager::collisionObjMap[this->rigidObj] = this;
//
//	return true;
//}
//uuu::uup::boxRigid::~boxRigid() {
//	delete this->rigidObj->getMotionState();
//	delete this->rigidObj->getCollisionShape();
//	delete this->rigidObj;
//
//	return;
//}