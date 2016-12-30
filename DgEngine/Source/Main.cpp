////////////////////////////////////////////////////////////////////////////////
// Filename: Main.cpp
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define DETECT_MEMORY_LEAK
#endif

//////////////
// INCLUDES //
//////////////
#ifdef DETECT_MEMORY_LEAK
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifndef DBG_NEW
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#define new DBG_NEW
//#endif
#endif

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "System.h"
/* Test scenes */
#include "Scene_Loader.h"
#include "Scene_Physics.h"
#include "Scene_Shadow.h"
#include "Scene_Whiterun.h"
#include "Scene_SciddicaT.h"
#include "Scene_Wave.h"
#include "Scene_Life2Di.h"
#include "Scene_Life3D.h"
#include "Scene_Life3Di.h"
#include "Scene_HeatTransfer.h"
//#include "Scene_MultiView.h"


int main(int argc, char** argv) {
#ifdef DETECT_MEMORY_LEAK
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(4111);
#endif

	// Create the system object.
	// Graphic API can be choosen by commands arguments!
	if(!dg::System::Instance()) {
		return 0;
	}

	// Initialize and run the system object.
	if(dg::System::Instance()->Initialize()) {
		// Add here new scenes
		ApplicationManager::RegisterApplicationScene(new Scene_Loader("Scene_Loader"));
		ApplicationManager::RegisterApplicationScene(new Scene_Physics("Scene_Physics"));
		ApplicationManager::RegisterApplicationScene(new Scene_Shadow("Scene_Shadow"));
		ApplicationManager::RegisterApplicationScene(new Scene_Whiterun("Scene_Whiterun"));
		ApplicationManager::RegisterApplicationScene(new Scene_SciddicaT("Scene_SciddicaT"));
		ApplicationManager::RegisterApplicationScene(new Scene_Wave("Scene_Wave"));
		ApplicationManager::RegisterApplicationScene(new Scene_Life2Di("Scene_Life2Di"));
		ApplicationManager::RegisterApplicationScene(new Scene_Life3D("Scene_Life3D"));
		ApplicationManager::RegisterApplicationScene(new Scene_Life3Di("Scene_Life3Di"));
		ApplicationManager::RegisterApplicationScene(new Scene_HeatTransfer("Scene_HeatTransfer"));
		//ApplicationManager::RegisterApplicationScene(new Scene_MultiView("Scene_MultiView"));

		//ApplicationManager::LoadScene("Scene_Loader");
		//ApplicationManager::LoadScene("Scene_Physics");
		//ApplicationManager::LoadScene("Scene_Shadow");
		//ApplicationManager::LoadScene("Scene_Whiterun");
		//ApplicationManager::LoadScene("Scene_SciddicaT");
		//ApplicationManager::LoadScene("Scene_Wave");
		//ApplicationManager::LoadScene("Scene_Life2Di");
		//ApplicationManager::LoadScene("Scene_Life3D");
		//ApplicationManager::LoadScene("Scene_Life3Di");
		ApplicationManager::LoadScene("Scene_HeatTransfer");
		//ApplicationManager::LoadScene("Scene_MultiView");

		dg::System::Instance()->Run();
	}

	// Shutdown and release the system object.
	dg::System::Instance()->Shutdown();

	return 0;
}
