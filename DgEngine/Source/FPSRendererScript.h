////////////////////////////////////////////////////////////////////////////////
// Filename: FPSRendererScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "TextRenderer.h"
#include "TimeClock.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class FPSRendererScript : public Script {
public:
	FPSRendererScript()
		: Script("FPSRendererScript") {		
		memset(m_Message, 0, sizeof(m_Message));
	}

	virtual void Update() {
		// Render FPS
		float fps = TimeClock::Instance()->FPS();
		vec3 color = vec_up;
		if(fps<30) {
			color.r = 1.0f;
			color.g = 0.0f;
			color.b = 0.0f;
		} else if(fps<40) {
			color.r = 1.0f;
			color.g = 0.8f;
			color.b = 0.0f;
		}
		SNPRINTF(m_Message, sizeof(m_Message), "FPS %g ", fps);
		TextRenderer::Instance()->RenderText(m_Message, 16.0f, 16.0f, 1.0f, color);
	}

private:
	char m_Message[32];
};
