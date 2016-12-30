////////////////////////////////////////////////////////////////////////////////
// Filename: ParticleEmitterPressure.h
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
	class ParticleEmitterPressure : public ParticleEmitter {

	public:
		ParticleEmitterPressure(ParticleEmitter::Shape::Enum shape, PxReal extentX, PxReal extentY, PxReal spacing);
		virtual				~ParticleEmitterPressure();

		virtual		void	stepInternal(ParticleData& particles, PxReal dt, const PxVec3& externalAcceleration, PxReal maxParticleVelocity);

		void	setMaxRate(PxReal v) { mMaxRate = v; }
		PxReal	getMaxRate() const { return mMaxRate; }


	private:

		struct SiteData {
			PxVec3 position;
			PxVec3 velocity;
			bool predecessor;
			PxReal noiseZ;
		};

	private:

		PxVec3 mSimulationAcceleration;
		PxReal mSimulationMaxVelocity;

		void clearPredecessors();

		bool stepEmissionSite(
			SiteData& siteData,
			ParticleData& spawnData,
			PxU32& spawnNum,
			const PxU32 spawnMax,
			const PxVec3 &sitePos,
			const PxVec3 &siteVel,
			const PxReal dt);

		void predictPredecessorPos(SiteData& siteData, PxReal dt);
		void updatePredecessor(SiteData& siteData, const PxVec3& position, const PxVec3& velocity);

	private:

		std::vector<SiteData>	mSites;
		PxReal					mMaxZNoiseOffset;
		PxReal					mMaxRate;
	};
}
