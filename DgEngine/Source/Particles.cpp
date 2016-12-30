//#include "Particles.h"
//#include "DataManager.h"
//
//#include "Utils.h"
//#include "DgBaseMesh.h"
//#include "ApplicationManager.h"
//#include "DgUtility\DgEngineCommon.h"
//#include "Physics\DgParticleEmitterPressure.h"
//#include "Physics\DgParticleEmitterRate.h"
//#include "DgShaderBillboard.h"
//#include "Transform.h"
//#include "DgShaderBaseMesh.h"
//#include "IInstanceable.h"
//
//#include "PxPhysXConfig.h"
//#include "extensions/PxExtensionsAPI.h"
//
//#pragma region PxFilterDataRegion
//static const PxFilterData collisionGroupDrain(0, 0, 0, 1);
//static const PxFilterData collisionGroupForceSmoke(0, 0, 0, 2);
//static const PxFilterData collisionGroupForceWater(0, 0, 0, 3);
//static const PxFilterData collisionGroupWaterfall(0, 0, 1, 0);
//static const PxFilterData collisionGroupSmoke(0, 0, 1, 1);
//static const PxFilterData collisionGroupDebris(0, 0, 1, 2);
//
//static bool isParticleGroup(const PxFilterData& f) {
//	return f.word2==1;
//}
//
//static bool isDrainGroup(const PxFilterData& f) {
//	return !isParticleGroup(f)&&f.word3==1;
//}
//
//static bool isForce0Group(const PxFilterData& f) {
//	return !isParticleGroup(f)&&f.word3==2;
//}
//
//static bool isForce1Group(const PxFilterData& f) {
//	return !isParticleGroup(f)&&f.word3==3;
//}
//
//static bool operator==(const PxFilterData& f0, const PxFilterData& f1) {
//	return (f0.word0==f1.word0)&&
//		(f0.word1==f1.word1)&&
//		(f0.word2==f1.word2)&&
//		(f0.word3==f1.word3);
//}
//
//static bool operator!=(const PxFilterData& f0, const PxFilterData& f1) {
//	return !(f0==f1);
//}
//
//PxFilterFlags SampleParticlesFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
//	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
//	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
//	// generate contacts for all that were not filtered above
//	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
//
//	// trigger the contact callback for pairs (A,B) where 
//	// the filtermask of A contains the ID of B and vice versa.
//	if((filterData0.word0 & filterData1.word1)&&(filterData1.word0 & filterData0.word1)) {
//		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
//	}
//
//	// allows only particle <-> drain collision (well, to drain particles)
//	// all other collision with the drain ignored
//	if(isDrainGroup(filterData0)||isDrainGroup(filterData1)) {
//		PxFilterData filterDataOther = isDrainGroup(filterData0) ? filterData1 : filterData0;
//		if(!isParticleGroup(filterDataOther))
//			return PxFilterFlag::eKILL;
//	}
//
//	// force0 group just collides with smoke
//	if(isForce0Group(filterData0)||isForce0Group(filterData1)) {
//		PxFilterData filterDataOther = isForce0Group(filterData0) ? filterData1 : filterData0;
//		if(filterDataOther!=collisionGroupSmoke)
//			return PxFilterFlag::eKILL;
//	}
//
//	// force1 group just collides with waterfall
//	if(isForce1Group(filterData0)||isForce1Group(filterData1)) {
//		PxFilterData filterDataOther = isForce1Group(filterData0) ? filterData1 : filterData0;
//		if(filterDataOther!=collisionGroupWaterfall)
//			return PxFilterFlag::eKILL;
//	}
//
//	return PxFilterFlags();
//}
//#pragma endregion
//
////void Particles::customizeSceneDesc(PxSceneDesc& setup) {
////	setup.filterShader = SampleParticlesFilterShader;
////	setup.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;
////}
//
//static PxQuat directionToQuaternion(const PxVec3& direction) {
//	PxVec3 vUp(0.0f, 1.0f, 0.0f);
//	PxVec3 vRight = vUp.cross(direction);
//	vUp = direction.cross(vRight);
//
//	PxQuat qrot(PxMat33(vRight, vUp, direction));
//	qrot.normalize();
//
//	return qrot;
//}
//
//Particles::Particles()
//	: Component(eComponentType::PARTICLES) {
//	mRunOnGpu = false;
//	billboardTexture = nullptr;
//	drainActor = nullptr;
//}
//Particles::Particles(SceneObject* gameObject)
//	: Component(eComponentType::PARTICLES, gameObject) {
//	mRunOnGpu = false;
//	billboardTexture = nullptr;
//	drainActor = nullptr;
//}
//Particles::~Particles() {
//	// Delete particle system
//	FREE_PTR(particleSystem);
//
//	// Delete Emitter	
//	FREE_PTR(emitter);
//
//	if(drainActor)
//		drainActor->release();
//
//	// Clean render data
//	FREE_PTR(billboardTexture);
//	glDeleteVertexArrays(1, &vaoBillboard);
//	glDeleteBuffers(1, &vboBillboard);
//}
//
//void Particles::initParticles(DgParticleData& particlesData) {
//	//PxSceneWriteLock scopedLock(*Physics::getActiveScene());
//
//	//#if PX_SUPPORT_GPU_PHYSX
//	//	// particles support GPU execution from arch 1.1 onwards. 
//	//	mRunOnGpu = Physics::getCudaContextManager()&&
//	//		Physics::getCudaContextManager()->contextIsValid()&&
//	//		Physics::getCudaContextManager()->supportsArchSM11();
//	//#endif
//
//	//debug visualization parameter
//	Physics::getActiveScene()->setVisualizationParameter(PxVisualizationParameter::ePARTICLE_SYSTEM_BROADPHASE_BOUNDS, 1.0f);
//	Physics::getActiveScene()->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 0.0f);
//
//	// Fake Data
//	Transform* transform = GetComponentType(GetSceneObject(), Transform);
//	PxVec3 translation(transform->getGlobalPosition()[0],
//		transform->getGlobalPosition()[1],
//		transform->getGlobalPosition()[2]);
//	PxQuat rotationX(glm::radians(transform->getRotation()[0]), PxVec3(1.0f, 0.0f, 0.0f));
//	PxQuat rotationY(glm::radians(transform->getRotation()[1]), PxVec3(0.0f, 1.0f, 0.0f));
//	PxQuat rotationZ(glm::radians(transform->getRotation()[2]), PxVec3(0.0f, 0.0f, 1.0f));
//	particlesData.systemTransform = PxTransform(translation, rotationX*rotationY*rotationZ);
//
//	switch(particlesData.systemType) {
//	case ParticleSystemType::PARTICLE_SYSTEM: {
//		initParticlesSystem(particlesData);
//	}break;
//	case ParticleSystemType::FLUID_SYSTEM: {
//		initFluidSystem(particlesData);
//	}break;
//	case ParticleSystemType::DRAIN_SYSTEM: {
//		initDrainSystem(particlesData);
//	}break;
//	default: break;
//	}
//
//#pragma region InitRenderingData
//	this->renderType = particlesData.renderType;
//
//	// Billboard
//	billboardTexture = new Texture(GL_TEXTURE_2D, particlesData.pathBillboardTexture, false);
//	if(!billboardTexture->Load()) {
//		dg::DebugConsoleMessage("Failed to load billboard\n");
//	}
//	//glGenBuffers(1, &vaoBillboard);
//	//glBindVertexArray(vaoBillboard);
//	glGenBuffers(1, &vboBillboard);
//	//glBindVertexArray(0);
//
//	// Mesh
//	meshPath = particlesData.pathMesh;
//#pragma endregion
//}
//
//void Particles::initFluidSystem(DgParticleData& particlesData) {
//	PxParticleFluid* px_fluid = Physics::getPhysics()->createParticleFluid(particlesData.maxParticles);
//	px_fluid->setGridSize(particlesData.gridSize);
//	px_fluid->setMaxMotionDistance(particlesData.maxMotionDistance);
//	px_fluid->setRestOffset(particlesData.restOffset);
//	px_fluid->setContactOffset(particlesData.contactOffset);
//	px_fluid->setDamping(particlesData.damping);
//	px_fluid->setRestitution(particlesData.restitution);
//	px_fluid->setDynamicFriction(particlesData.dynamicFriction);
//	px_fluid->setRestParticleDistance(particlesData.particleDistance);
//	px_fluid->setViscosity(particlesData.viscosity);
//	px_fluid->setStiffness(particlesData.stiffness);
//	px_fluid->setParticleReadDataFlag(PxParticleReadDataFlag::eVELOCITY_BUFFER, true);
//#if PX_SUPPORT_GPU_PHYSX
//	px_fluid->setParticleBaseFlag(PxParticleBaseFlag::eGPU, mRunOnGpu);
//#endif
//	Physics::getActiveScene()->addActor(*px_fluid);
//
//#if PX_SUPPORT_GPU_PHYSX
//	//check gpu flags after adding to scene, cpu fallback might have been used.
//	mRunOnGpu = mRunOnGpu&&(px_fluid->getParticleBaseFlags() & PxParticleBaseFlag::eGPU);
//#endif
//	px_fluid->setSimulationFilterData(collisionGroupWaterfall);
//	particleSystem = new Particlesystem(px_fluid);
//
//	// Create emitter
//	if(!particlesData.emitter) {
//		// setup emitter
//		DgParticleEmitterPressure* pressureEmitter = new DgParticleEmitterPressure(DgParticleEmitter::Shape::eRECTANGLE,
//			particlesData.emitterExtentX,
//			particlesData.emitterExtentY,
//			particlesData.emitterSpacing); // shape and size of emitter and spacing between particles being emitted
//		pressureEmitter->setMaxRate(particlesData.emitterMaxRate);
//		pressureEmitter->setVelocity(particlesData.emitterVelocity);
//		pressureEmitter->setRandomAngle(particlesData.emitterRandomAngle);
//		pressureEmitter->setRandomPos(particlesData.emitterRandomPos);
//		pressureEmitter->setLocalPose(particlesData.systemTransform);
//		emitter = pressureEmitter;
//		emitter->isEnabled = true;
//	} else {
//		emitter = particlesData.emitter;
//		emitter->isEnabled = true;
//	}
//}
//
//void Particles::initParticlesSystem(DgParticleData& particlesData) {
//	PxParticleSystem* px_particleSystem = Physics::getPhysics()->createParticleSystem(particlesData.maxParticles);
//	px_particleSystem->setGridSize(particlesData.gridSize);
//	px_particleSystem->setMaxMotionDistance(particlesData.maxMotionDistance);
//	px_particleSystem->setRestOffset(particlesData.restOffset);
//	px_particleSystem->setContactOffset(particlesData.contactOffset);
//	px_particleSystem->setDamping(particlesData.damping);
//	px_particleSystem->setRestitution(particlesData.restitution);
//	px_particleSystem->setDynamicFriction(particlesData.dynamicFriction);
//	px_particleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eVELOCITY_BUFFER, true);
//#if PX_SUPPORT_GPU_PHYSX
//	px_particleSystem->setParticleBaseFlag(PxParticleBaseFlag::eGPU, mRunOnGpu);
//#endif
//	Physics::getActiveScene()->addActor(*px_particleSystem);
//
//#if PX_SUPPORT_GPU_PHYSX
//	//check gpu flags after adding to scene, cpu fallback might have been used.
//	mRunOnGpu = mRunOnGpu&&(px_particleSystem->getParticleBaseFlags() & PxParticleBaseFlag::eGPU);
//#endif
//	px_particleSystem->setExternalAcceleration(particlesData.externalAcceleration);
//	px_particleSystem->setSimulationFilterData(collisionGroupSmoke);
//	// This will be deleted by RenderParticleSystemActor
//	particleSystem = new Particlesystem(px_particleSystem, false);
//	particleSystem->setUseLifetime(particlesData.useLifetime);
//	particleSystem->setLifetime(particlesData.lifetime);
//
//	// Create emitter
//	if(!particlesData.emitter) {
//		DgParticleEmitterRate* rateEmitter = new DgParticleEmitterRate(
//			DgParticleEmitter::Shape::eRECTANGLE,
//			particlesData.emitterExtentX,
//			particlesData.emitterExtentY,
//			particlesData.emitterSpacing); // shape and size of emitter and spacing between particles being emitted
//		rateEmitter->setRate(particlesData.emitterRate);
//		rateEmitter->setVelocity(particlesData.emitterVelocity);
//		rateEmitter->setRandomAngle(particlesData.emitterRandomAngle);
//		rateEmitter->setLocalPose(particlesData.systemTransform);
//		emitter = rateEmitter;
//		emitter->isEnabled = true;
//	} else {
//		emitter = particlesData.emitter;
//		emitter->isEnabled = true;
//	}
//}
//
//void Particles::initDrainSystem(DgParticleData& particlesData) {
//	// Create emitter
//	PxMaterial* physicsMaterial = Physics::getPhysics()->createMaterial(0.5, 0.5, 0.0);
//	bool shapeCreated = false;
//	PxGeometry* geometryShape = nullptr;
//
//	switch(particlesData.drainType) {
//	case DrainType::PLANE_DRAIN: {
//		PxPlaneGeometry planeGeometry;
//		geometryShape = &planeGeometry;
//		shapeCreated = true;
//	} break;
//	case DrainType::BOX_DRAIN: {
//		PxBoxGeometry boxGeometry;
//		boxGeometry.halfExtents = PxVec3(particlesData.boxHalfExtensions.x,
//			particlesData.boxHalfExtensions.y,
//			particlesData.boxHalfExtensions.z);
//		geometryShape = &boxGeometry;
//		shapeCreated = true;
//	} break;
//	case DrainType::SPHERE_DRAIN: {
//		PxSphereGeometry sphereGeometry;
//		sphereGeometry.radius = particlesData.sphereRadius;
//		geometryShape = &sphereGeometry;
//		shapeCreated = true;
//	} break;
//	default:
//		break;
//	}
//
//	if(shapeCreated) {
//		PxRigidStatic* actor = Physics::getPhysics()->createRigidStatic(particlesData.systemTransform); //PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxIdentity))
//		PxShape* shape = actor->createShape(*geometryShape, *physicsMaterial);
//
//		if(particlesData.drainType==DrainType::PLANE_DRAIN) {
//			PxTransform relativePose;
//			switch(particlesData.planeDirection) {
//			case DgGeometryDirection::X_AXIS:
//				relativePose = PxTransform(PxQuat(PxHalfPi, PxVec3(0, 1, 0)));
//				shape->setLocalPose(relativePose);
//				break;
//			case DgGeometryDirection::Z_AXIS:
//				relativePose = PxTransform(PxQuat(PxHalfPi, PxVec3(1, 0, 0)));
//				shape->setLocalPose(relativePose);
//				break;
//			case DgGeometryDirection::Y_AXIS:
//				relativePose = PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
//				shape->setLocalPose(relativePose);
//				break;
//			default:
//				break;
//			}
//		}
//
//		shape->setSimulationFilterData(collisionGroupDrain);
//		shape->setFlags(PxShapeFlag::ePARTICLE_DRAIN|PxShapeFlag::eSIMULATION_SHAPE);
//		//shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
//		//shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
//		Physics::getActiveScene()->addActor(*actor);
//		drainActor = actor;
//	}
//
//	////Creates a drain plane for the particles. This is good practice to avoid unnecessary 
//	////spreading of particles, which is bad for performance. The drain represents a lake in this case.
//	//{
//	//	//PxRigidStatic* actor = Physics::getPhysics()->createRigidStatic(PxTransform(PxVec3(0.0f, 1.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
//	//	//PxShape* shape = actor->createShape(PxPlaneGeometry(), *physicsMaterial);
//	//	//shape->setSimulationFilterData(collisionGroupDrain);
//	//	//shape->setFlags(PxShapeFlag::ePARTICLE_DRAIN|PxShapeFlag::eSIMULATION_SHAPE);
//	//	//Physics::getActiveScene()->addActor(*actor);
//	//	//mPhysicsActors.push_back(actor);
//	//}
//	////Creates the surface of the lake (the particles actually just collide with the underlaying drain).
//	//{
//	//	//PxBoxGeometry bg;
//	//	//bg.halfExtents = PxVec3(20.0f, 1.0f, 20.0f);
//	//	//PxRigidStatic* actor = Physics::getPhysics()->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxIdentity)));
//	//	//PxShape* shape = actor->createShape(bg, *physicsMaterial);
//	//	//shape->setSimulationFilterData(collisionGroupDrain);
//	//	//shape->setFlags(PxShapeFlag::ePARTICLE_DRAIN|PxShapeFlag::eSIMULATION_SHAPE);
//	//	////shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
//	//	////shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
//	//	//Physics::getActiveScene()->addActor(*actor);
//	//	//mPhysicsActors.push_back(actor);
//	//}
//}
//
//void Particles::PreUpdate() {
//
//}
//
//void Particles::Update() {
//	//PxSceneWriteLock scopedLock(*Physics::getActiveScene());
//
//	//if(emitter)
//	//	updateEmitter(DgApplicationManager::getDeltaTime());
//	//if(particleSystem)
//	//	particleSystem->update(DgApplicationManager::getDeltaTime());
//}
//
//void Particles::PostUpdate() {
//
//}
//
//void Particles::PreRender() {
//
//}
//
//void Particles::Render() {
//	//	Do render here, billboard or instanced mesh
//	switch(renderType) {
//	case ParticleSystemRenderType::BILLBOARD_RENDER: {
//		billboardRendering();
//	}break;
//	case ParticleSystemRenderType::MESH_RENDER: {
//		meshRendering();
//	}break;
//	default: break;
//	}
//}
//
//void Particles::PostRender() {
//
//}
//
//void Particles::billboardRendering() {
//	DgShaderBillboard* shader = (DgShaderBillboard*) DataManager::GetShader("Billboard");
//	if(shader) {
//		shader->updateValue(GetSceneObject());
//
//		int blendAttrib;
//		glGetIntegerv(GL_BLEND, &blendAttrib);
//		glEnable(GL_BLEND);
//
//		// lock SDK buffers of *PxParticleSystem* ps for reading
//		PxParticleReadData* rd = particleSystem->getPxParticleBase()->lockParticleReadData();
//		vector<PxVec3> particlesPositions;
//		// access particle data from PxParticleReadData
//		if(rd) {
//			PxStrideIterator<const PxParticleFlags> flagsIt(rd->flagsBuffer);
//			PxStrideIterator<const PxVec3> positionIt(rd->positionBuffer);
//
//			for(unsigned i = 0; i<rd->validParticleRange; ++i, ++flagsIt, ++positionIt) {
//				if(*flagsIt & PxParticleFlag::eVALID) {
//					// access particle position
//					particlesPositions.push_back(*positionIt);
//				}
//			}
//
//			// return ownership of the buffers back to the SDK
//			rd->unlock();
//		}
//
//		if(particlesPositions.size()>0) {
//			//glBindVertexArray(vaoBillboard);
//			billboardTexture->Bind(COLOR_TEXTURE_UNIT);
//			glEnableVertexAttribArray(0);
//			glBufferData(GL_ARRAY_BUFFER,
//				sizeof(particlesPositions[0]) * particlesPositions.size(),
//				&particlesPositions[0],
//				GL_DYNAMIC_DRAW);
//			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//			glDrawArrays(GL_POINTS, 0, particlesPositions.size());
//			glDisableVertexAttribArray(0);
//			//glBindVertexArray(0);
//		}
//
//		if(blendAttrib)
//			glEnable(GL_BLEND);
//		else
//			glDisable(GL_BLEND);
//	}
//}
//
//void Particles::meshRendering() {
//	DgShaderBaseMesh* shader = (DgShaderBaseMesh*) DataManager::GetShader("BaseMesh");
//	shader->updateValue(GetSceneObject());
//
//	IInstanceable* castedShader = dynamic_cast<IInstanceable*>(shader);
//	if(castedShader) {
//		// lock SDK buffers of *PxParticleSystem* ps for reading
//		PxParticleReadData* rd = particleSystem->getPxParticleBase()->lockParticleReadData();
//		vector<PxVec3> particlesPositions;
//		vector<PxMat33> particlesOrientations;
//		// access particle data from PxParticleReadData
//		if(rd) {
//			PxStrideIterator<const PxParticleFlags> flagsIt(rd->flagsBuffer);
//			PxStrideIterator<const PxVec3> positionIt(rd->positionBuffer);
//			PxStrideIterator<const PxVec3> velocityIt(rd->velocityBuffer);
//
//			for(unsigned i = 0; i<rd->validParticleRange; ++i, ++flagsIt, ++positionIt, ++velocityIt) {
//				if(*flagsIt & PxParticleFlag::eVALID) {
//					// access particle position
//					particlesPositions.push_back(*positionIt);
//
//					// update orientations
//					PxMat33 orientationMatrix;
//					PxVec3 delta = PxVec3(orientationMatrix.column1-orientationMatrix.column0).getNormalized() *
//						//DgApplicationManager::getDeltaTime() *
//						(*velocityIt).magnitude();
//					//PxVec3 vUp(0.0f, 1.0f, 0.0f);
//					orientationMatrix.column2 = (orientationMatrix.column2+delta).getNormalized();
//					orientationMatrix.column0 = orientationMatrix.column1.cross(orientationMatrix.column2).getNormalized();
//					orientationMatrix.column1 = orientationMatrix.column2.cross(orientationMatrix.column0).getNormalized();
//					particlesOrientations.push_back(orientationMatrix);
//				}
//			}
//			
//			// return ownership of the buffers back to the SDK
//			rd->unlock();
//		}
//
//		if(particlesPositions.size()>0) {
//			mat4* WVPMatrics = new mat4[particlesPositions.size()];
//			mat4* WorldMatrices = new mat4[particlesPositions.size()];
//
//			//cout<<particlesOrientations.size()<<endl;
//
//			for(uint i = 0; i<particlesPositions.size(); i++) {
//				Transform transform;
//				transform.setPosition(particlesPositions[i].x,
//					particlesPositions[i].y,
//					particlesPositions[i].z);
//				//transform.setRotation(rotation);
//				//transform.setScaling(scale);
//								
//				transform.clearTrasformations();
//				mat4 translateToOrigin = translate(-particlesPositions[i][0],
//					-particlesPositions[i][1],
//					-particlesPositions[i][2]);
//				//translateToOrigin.initTranslationTransform(-particlesPositions[i].x,
//				//	-particlesPositions[i].y,
//				//	-particlesPositions[i].z);
//				mat4 translateToPosition = translate(particlesPositions[i][0],
//					particlesPositions[i][1],
//					particlesPositions[i][2]);
//				//translateToPosition.initTranslationTransform(particlesPositions[i].x,
//				//	particlesPositions[i].y,
//				//	particlesPositions[i].z);
//				transform.pushMatrix(translateToPosition);
//				//mat4 matrix(rotationMatrix.column0[0], rotationMatrix.column1[0], rotationMatrix.column2[0], rotationMatrix.column3[0],
//				//	rotationMatrix.column0[1], rotationMatrix.column1[1], rotationMatrix.column2[1], rotationMatrix.column3[1],
//				//	rotationMatrix.column0[2], rotationMatrix.column1[2], rotationMatrix.column2[2], rotationMatrix.column3[2],
//				//	rotationMatrix.column0[3], rotationMatrix.column1[3], rotationMatrix.column2[3], rotationMatrix.column3[3]);
//				//mat4 matrix(particlesOrientations[i].column0[0], particlesOrientations[i].column1[0], particlesOrientations[i].column2[0], 0.0f,
//				//	particlesOrientations[i].column0[1], particlesOrientations[i].column1[1], particlesOrientations[i].column2[1], 0.0f,
//				//	particlesOrientations[i].column0[2], particlesOrientations[i].column1[2], particlesOrientations[i].column2[2], 0.0f,
//				//	0.0f, 0.0f, 0.0f, 1.0f);
//				//transform.pushMatrix(matrix);
//				transform.pushMatrix(translateToOrigin);
//				
//				shader->setWorldViewProjection(transform.getWorldViewProjectionTransformation());
//
//				WVPMatrics[i] = transform.getWorldViewProjectionTransformation().transpose();
//				WorldMatrices[i] = transform.getWorldTransformation().transpose();
//			}
//
//			castedShader->setInstancingDraw(true);
//			DgBaseMesh* mesh = DataManager::GetMesh(this->meshPath);
//			if(mesh)
//				mesh->render(particlesPositions.size(), WVPMatrics, WorldMatrices);
//			castedShader->setInstancingDraw(false);
//
//			delete WVPMatrics;
//			delete WorldMatrices;
//		}
//	}
//}
//
//void Particles::updateEmitter(float dtime) {
//	if(emitter && !emitter->isEnabled)
//		return;
//	if(!particleSystem)
//		return;
//
//	PxVec3 acceleration = particleSystem->getPxParticleBase()->getExternalAcceleration()+particleSystem->getPxParticleBase()->getScene()->getGravity();
//	float maxVeloctiy = particleSystem->getPxParticleBase()->getMaxMotionDistance()/(dtime);
//	ParticleData initData(particleSystem->getPxParticleBase()->getMaxParticles());
//	emitter->step(initData, dtime, acceleration, maxVeloctiy);
//	particleSystem->createParticles(initData);
//}
//
//void Particles::toggleGpu() {
//	bool canRunOnGpu = false;
//
//	// particles support GPU execution from arch 1.1 onwards. 
//	canRunOnGpu = Physics::getCudaContextManager()&&
//		Physics::getCudaContextManager()->contextIsValid()&&
//		Physics::getCudaContextManager()->supportsArchSM11();
//
//	if(!canRunOnGpu)
//		return;
//
//	mRunOnGpu = !mRunOnGpu;
//
//	// reset all the particles to switch between GPU and CPU
//	static const PxU32 sActorBufferSize = 100;
//	PxActor* actors[sActorBufferSize];
//
//	//PxSceneWriteLock scopedLock(*Physics::getActiveScene());
//
//	PxU32 numParticleFluidActors = Physics::getActiveScene()->getActors(PxActorTypeSelectionFlag::ePARTICLE_FLUID, actors, sActorBufferSize);
//	PX_ASSERT(numParticleFluidActors<sActorBufferSize);
//	for(PxU32 i = 0; i<numParticleFluidActors; i++) {
//		PxParticleFluid& fluid = *actors[i]->is<PxParticleFluid>();
//		fluid.setParticleBaseFlag(PxParticleBaseFlag::eGPU, mRunOnGpu);
//	}
//
//	PxU32 numParticleSystemActors = Physics::getActiveScene()->getActors(PxActorTypeSelectionFlag::ePARTICLE_SYSTEM, actors, sActorBufferSize);
//	PX_ASSERT(numParticleSystemActors<sActorBufferSize);
//	for(PxU32 i = 0; i<numParticleSystemActors; i++) {
//		PxParticleSystem& fluid = *actors[i]->is<PxParticleSystem>();
//		fluid.setParticleBaseFlag(PxParticleBaseFlag::eGPU, mRunOnGpu);
//	}
//}
