////////////////////////////////////////////////////////////////////////////////
// Filename: ChangeSceneScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "ApplicationManager.h"
#include "Input.h"
#include "Configuration.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class ChangeSceneScript : public Script {
public:
	ChangeSceneScript()
		: Script("ChangeSceneScript") {}
	
	virtual void Update() {
		if(Input::Instance()->IsKeyPress(eKey::KEY_0)) {
			ApplicationManager::LoadScene("Scene_Loader");
		}
		else if (Input::Instance()->IsKeyPress(eKey::KEY_1)) {
			ApplicationManager::LoadScene("Scene_Physics");
		}
	}

};
