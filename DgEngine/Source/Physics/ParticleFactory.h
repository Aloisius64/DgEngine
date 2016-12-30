////////////////////////////////////////////////////////////////////////////////
// Filename: ParticleFactory.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "PhysX.h"
#include "PxRandom.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace physx;


namespace dg {
	struct ParticleData {
		ParticleData() : maxParticles(0), numParticles(0) {}

		ParticleData(PxU32 _maxParticles) {
			initialize(_maxParticles);
		}

		void initialize(PxU32 _maxParticles) {
			maxParticles = _maxParticles;
			numParticles = 0;
			positions.resize(maxParticles);
			velocities.resize(maxParticles);
			restOffsets.resize(0);
		}

		vector<PxVec3> positions;
		vector<PxVec3> velocities;
		vector<PxF32> restOffsets;
		PxU32	maxParticles;
		PxU32	numParticles;
	};

	void CreateParticleAABB(ParticleData& particleData, const PxBounds3& aabb, const PxVec3& vel, float distance);
	void CreateParticleSphere(ParticleData& particleData, const PxVec3& pos, const PxVec3& vel, float distance, unsigned sideNum);
	void CreateParticleRand(ParticleData& particleData, const PxVec3& center, const PxVec3& range, const PxVec3& vel);
	void SetParticleRestOffsetVariance(ParticleData& particleData, PxF32 maxRestOffset, PxF32 restOffsetVariance);
}
