////////////////////////////////////////////////////////////////////////////////
// Filename: TrackingCameraScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <stdlib.h>
#include <time.h> 

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class RandomPositionScript : public Script {
public:
	RandomPositionScript(Transform* target, float ran)
		: Script("RandomPositionScript") {
	
		m_Target = target;
		m_Range = ran;

		SRANDOM;
	}

	virtual void Update() {
		float r2 = static_cast <float> (rand())/(static_cast <float> (RAND_MAX/m_Range));
		m_Target->SetPosition(r2, m_Target->GetPosition()[1], m_Target->GetPosition()[2]);
	}

private:
	Transform*	m_Target;
	float		m_Range;
};
