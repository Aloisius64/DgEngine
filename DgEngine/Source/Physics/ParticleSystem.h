////////////////////////////////////////////////////////////////////////////////
// Filename: ParticleSystem.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <vector>
#include "PxPhysicsAPI.h"
#include "extensions/PxVisualDebuggerExt.h"
#include "extensions/PxParticleExt.h"

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Physics.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace physx;


// fwd to avoid including cuda.h
#if defined(__x86_64) || defined(AMD64) || defined(_M_AMD64)
typedef unsigned long long CUdeviceptr;
#else
typedef unsigned int CUdeviceptr;
#endif

namespace physx {
	namespace pxtask {
		class CudaContextManager;
	}
}

namespace dg {
	class Particlesystem {
	public:
		Particlesystem(PxParticleBase* _particle_system, bool _useInstancedMeshes = false);
		~Particlesystem();

		/* enables limiting particles lifetime */
		void setUseLifetime(bool use);

		/* returns true if limiting particles lifetime is enabled */
		bool useLifetime();

		/* NOTE! setUseLifetime(true) before setting this. Sets lifetime of particle to "lt" on emit */
		void setLifetime(PxReal lt);

		/* fetches particles positions from the PhysX SDK,
		removes invalid particles
		(intersected with drain, non-positive lifetime),
		creates new particles */
		void update(float deltaTime);

		/* creates particles in the PhysX SDK */
		void createParticles(struct ParticleData& particles);

		/* Returns pointer to the internal PxParticleBase */
		PxParticleBase* getPxParticleBase();
		/* Returns pointer to the particles positions */
		const std::vector<PxVec3>& getPositions();
		/* Returns pointer to the particles lifetimes */
		const std::vector<PxReal>& getLifetimes();
		/* Returns pointer to the particles velocities */
		const std::vector<PxVec3>& getVelocities();
		/* Returns pointer to the particles orientations */
		const std::vector<PxMat33>& getOrientations();
		/* Returns pointer to the particles validity */
		const PxU32* getValidity();
		/* Returns range of vaild particles index */
		PxU32 getValidParticleRange();
		/* Returns number of particles */
		PxU32 getNumParticles();

		/* Return the device copy of particles validity */
		CUdeviceptr getValiditiesDevice() const { return mParticleValidityDevice; }
		/* Return the device copy of particles orientation */
		CUdeviceptr getOrientationsDevice() const { return mParticlesOrientationsDevice; }
		/* Return the device copy of particle lifetimes*/
		CUdeviceptr getLifetimesDevice() const { return mParticleLifetimeDevice; }
		/* Return cuda context manager if set, which is only the case if interop is enabled*/
		PxCudaContextManager* getCudaContextManager() const { return mCtxMgr; }

		PxParticleSystem& getPxParticleSystem() { PX_ASSERT(mParticleSystem); return static_cast<PxParticleSystem&>(*mParticleSystem); }
		PxParticleFluid& getPxParticleFluid() { PX_ASSERT(mParticleSystem); return static_cast<PxParticleFluid&>(*mParticleSystem); }

		PxU32 createParticles(const PxParticleCreationData& particles, PxStrideIterator<PxU32>* particleIndices = nullptr, PxReal lifetime = 0.0f);
		PxU32 createParticleSphere(PxU32 maxParticles, float particleDistance, const PxVec3& center, const PxVec3& vel, PxReal lifetime = 0.0f, PxReal restOffsetVariance = 0.0f);
		PxU32 createParticleCube(PxU32 numX, PxU32 numY, PxU32 numZ, float particleDistance, const PxVec3& center, const PxVec3& vel, PxReal lifetime = 0.0f, PxReal restOffsetVariance = 0.0f);
		PxU32 createParticleCube(const PxBounds3& aabb, float particleDistance, const PxVec3& vel, PxReal lifetime = 0.0f, PxReal restOffsetVariance = 0.0f);
		PxU32 createParticleRand(PxU32 numParticles, const PxVec3& particleRange, const PxVec3& center, const PxVec3& vel, PxReal lifetime = 0.0f, PxReal restOffsetVariance = 0.0f);
		//PxU32 createParticlesFromFile(const char* particleFileName);
		//bool dumpParticlesToFile(const char* particleFileName);
		void releaseParticles(const vector<PxU32>& indices);
		void releaseParticles();

		class Visitor {
		public:
			virtual ~Visitor() {}
			virtual void visit(Particlesystem& particleSystem) = 0;
		};
		void accept(Visitor& visitor) { visitor.visit(*this); }

	private:
		PxParticleBase*			mParticleSystem;
		PxParticleExt::IndexPool* mIndexPool;
		std::vector<PxVec3>		mParticlesPositions;
		std::vector<PxVec3>		mParticlesVelocities;
		std::vector<PxMat33>	mParticlesOrientations;
		PxU32					mNumParticles;
		PxReal					mParticleLifetime;
		std::vector<PxReal>		mParticleLifes;
		PxU32*					mParticleValidity;
		PxU32                   mValidParticleRange;
		bool					mUseLifetime;
		bool					mUseInstancedMeshes;

		CUdeviceptr				mParticlesOrientationsDevice;
		CUdeviceptr				mParticleLifetimeDevice;
		CUdeviceptr				mParticleValidityDevice;
		PxCudaContextManager*	mCtxMgr;

		void					modifyRotationMatrix(PxMat33& rotMatrix, PxReal deltaTime, const PxVec3& velocity);
		void					initializeParticlesOrientations();

		PxU32 createParticles(const ParticleData& particles, PxReal lifetime);
	};
}
