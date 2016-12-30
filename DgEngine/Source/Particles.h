//#pragma once
//
//#include <Component.h>
//#include "Physics\Physics.h"
//#include "Physics\DgCollider.h"
//#include "Physics\Particlesystem.h"
//#include "Physics\DgParticleEmitter.h"
//#include <DgUtility\DgEngineCommon.h>
//#include "Texture.h"
//
//using namespace physx;
//
//enum ParticleSystemType {
//	NULL_SYSTEM = 0,
//	PARTICLE_SYSTEM,
//	FLUID_SYSTEM,
//	DRAIN_SYSTEM,
//	NUM_SYSTEM
//};
//
//enum ParticleSystemRenderType {
//	NULL_RENDER = 0,
//	BILLBOARD_RENDER,
//	MESH_RENDER,
//	NUM_RENDER
//};
//
//enum DrainType {
//	PLANE_DRAIN = 0,
//	BOX_DRAIN,
//	SPHERE_DRAIN,
//	NUM_DRAIN	
//};
//
//struct DgParticleData {
//	ParticleSystemType systemType;
//	ParticleSystemRenderType renderType;
//
//	PxTransform systemTransform;
//	PxU32 particleCount;
//	PxF32 emitterWidth;
//	PxF32 emitterVelocity;
//	bool useLifetime;
//	PxF32 lifetime;
//	PxF32 emitterRate;
//	PxU32 maxParticles;
//	PxReal restitution;
//	PxReal viscosity;
//	PxReal stiffness;
//	PxReal dynamicFriction;
//	PxReal particleDistance;
//	PxReal gridSize;
//	PxReal restOffset;
//	PxReal contactOffset;
//	PxReal maxMotionDistance;
//	PxReal damping;
//	PxVec3 externalAcceleration;
//	// Emitter
//	DgParticleEmitter* emitter;
//	DgParticleEmitter::Shape::Enum emitterShape;
//	PxReal emitterExtentX;
//	PxReal emitterExtentY;
//	PxReal emitterSpacing;
//	PxReal emitterRandomAngle;
//	PxVec3 emitterRandomPos;
//	PxReal emitterMaxRate;
//	// DrainData
//	DrainType drainType;
//	PxVec3 boxHalfExtensions;
//	PxReal sphereRadius;
//	DgGeometryDirection planeDirection;
//	// Billboard - Render data
//	string pathBillboardTexture;
//	// Instanced draw - Render data
//	string pathMesh;
//	
//	DgParticleData() {
//		systemType = NULL_SYSTEM;
//		renderType = NULL_RENDER;
//
//		systemTransform = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
//		particleCount = 1000;
//		lifetime = 2.5f;
//		maxParticles = particleCount;
//		// Fluid data
//		restitution = 0.3f;
//		viscosity = 60.0f;
//		stiffness = 45.0f;
//		dynamicFriction = 0.001f;
//		particleDistance = 0.8f;
//		gridSize = 0.5f;
//		restOffset = 0.0f;
//		contactOffset = 0.0f;
//		maxMotionDistance = 0.3f;
//		damping = 0.0f;
//		externalAcceleration = PxVec3(0.0f, 0.0f, 0.0f);
//		useLifetime = true;
//		// Emitter data
//		emitter = nullptr;
//		emitterShape = DgParticleEmitter::Shape::eRECTANGLE;
//		emitterSpacing = 0.81f;
//		emitterRandomAngle = 0.0f;
//		emitterRandomPos = PxVec3(0.0f, 0.0f, 0.0f);
//		emitterWidth = 1.5f;
//		emitterVelocity = 9.0f;
//		emitterRate = 20.0f;
//		emitterMaxRate = emitterRate;
//		emitterExtentX = emitterExtentY = emitterWidth;
//		// DrainData
//		drainType = DrainType::PLANE_DRAIN;
//		boxHalfExtensions = PxVec3(1.0f);
//		sphereRadius = 1.0f;
//		planeDirection = DgGeometryDirection::Y_AXIS;
//		// Billboard - Render data
//		pathBillboardTexture = "";
//		// Instanced draw - Render data
//		pathMesh = "";
//	}
//};
//
//class Particles : public Component {
//public:
//	Particles();
//	Particles(SceneObject* gameObject);
//	virtual ~Particles();
//
//	virtual void PreUpdate();
//	virtual void Update();
//	virtual void PostUpdate();
//	virtual void PreRender();
//	virtual void Render();
//	virtual void PostRender();
//
//	void initParticles(DgParticleData& particlesData); // particlesData must be const
//	
//private:
//	void initFluidSystem(DgParticleData& particlesData);
//	void initParticlesSystem(DgParticleData& particlesData);
//	void initDrainSystem(DgParticleData& particlesData);
//	void updateEmitter(float dt);
//	void billboardRendering();
//	void meshRendering();
//
//	ParticleSystemRenderType renderType;
//	DgParticleEmitter* emitter;
//	Particlesystem* particleSystem;
//	PxRigidStatic* drainActor;
//
//#pragma region RenderSection
//	// Billboard
//	GLuint vaoBillboard;
//	GLuint vboBillboard;
//	Texture* billboardTexture;	// Maybe use a texture array
//	// Mesh
//	string meshPath;
//#pragma endregion
//
//
//#if PX_SUPPORT_GPU_PHYSX
//	void toggleGpu();
//	bool mRunOnGpu;
//#endif
//};
