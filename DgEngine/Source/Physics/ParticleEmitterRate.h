////////////////////////////////////////////////////////////////////////////////
// Filename: ParticleEmitterRate.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ParticleEmitter.h"

////////////////
// NAMESPACES //
////////////////


namespace dg {
	class ParticleEmitterRate : public ParticleEmitter {
	public:
		ParticleEmitterRate(ParticleEmitter::Shape::Enum shape, PxReal extentX, PxReal extentY, PxReal spacing);
		virtual				~ParticleEmitterRate();

		virtual		void	stepInternal(ParticleData& particles, PxReal dt, const PxVec3& externalAcceleration, PxReal maxParticleVelocity);

		void 				setRate(PxReal t) { mRate = t; }
		PxReal 				getRate()						const { return mRate; }

	private:

		void initDenseSites();
		void shuffleDenseSites();
		void initSparseSiteHash(PxU32 numEmit, PxU32 sparseMax);
		PxU32 pickSparseEmissionSite(PxU32 sparseMax);

		PxReal				mRate;
		PxReal				mParticlesToEmit;
		std::vector<PxU32>	mSites;

#pragma region HashFunctions
		PxU32 hash(const PxU32 key) {
			PxU32 k = key;
			k += ~(k<<15);
			k ^= (k>>10);
			k += (k<<3);
			k ^= (k>>6);
			k += ~(k<<11);
			k ^= (k>>16);
			return (PxU32) k;
		}

		PxU32 hash(const PxI32 key) {
			return hash(PxU32(key));
		}

		PxU32 hash(const PxU64 key) {
			PxU64 k = key;
			k += ~(k<<32);
			k ^= (k>>22);
			k += ~(k<<13);
			k ^= (k>>8);
			k += (k<<3);
			k ^= (k>>15);
			k += ~(k<<27);
			k ^= (k>>31);
			return (PxU32) (PX_MAX_U32 & k);
		}
#pragma endregion
	};
}
