////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_Life3Di.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <stdlib.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ApplicationScene.h"
#include "Camera.h"
#include "CellularAutomata.h"
#include "Common.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Gizmo.h"
#include "Light.h"
#include "OpenCALFiles.h"
#include "Shader.h"
#include "SceneObject.h"
#include "System.h"
#include "SubstateRenderer.h"
#include "Transform.h"
#include "UI.h"
#include "Utils.h"
/*Scripts*/
#include "CameraFPSControllerScript.h"
#include "FPSRendererScript.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

#pragma region OPENCAL
#define LIFE3DI_ROWS 32
#define LIFE3DI_COLS 32
#define LIFE3DI_LAYERS 32
#define LIFE3DI_STEPS 4000

CALModel3D*			life3Di;				// the cellular automaton
CALSubstate3Di*		life3DiSubstate;		// the substate
CALRun3D*			life3Disimulation;	// the simulartion run

//------------------------------------------------------------------------------
//					life3D transition function
//------------------------------------------------------------------------------

// first elementary process.
void life3DiTransitionFunction(struct CALModel3D* ca, int i, int j, int k) {
	int sum = 0, n;

	for(n = 0; n<ca->sizeof_X; n++)
		sum += calGetX3Di(ca, life3DiSubstate, i, j, k, n);

	calSet3Di(ca, life3DiSubstate, i, j, k, sum%2);
}

//------------------------------------------------------------------------------
//					life3D simulation functions
//------------------------------------------------------------------------------

void life3DiSimulationInit(struct CALModel3D* ca) {
	//int i, j, k, state;

	//initializing substate to 0
	calInitSubstate3Di(ca, life3DiSubstate, 0);

	//life3D random init
	//srand(0);
	//for(int k = 0; k<ca->slices; k++)
	//	for(int i = 0; i<ca->rows; i++)
	//		for(int j = 0; j<ca->columns; j++) {
	//			int state = rand()%100;
	//			if(state==1)
	//				calSet3Di(ca, life3DiSubstate, i, j, k, state);
	//		}

	calSet3Di(ca, life3DiSubstate, 0, 0, 0, 1);

	calUpdate3D(ca);
}
#pragma endregion

class Scene_Life3Di : public ApplicationScene {
public:
	Scene_Life3Di(string nameScene)
		: ApplicationScene(nameScene) {}
	virtual ~Scene_Life3Di() {}

	virtual bool Initialize() {
		// cadef and rundef.
		life3Di = calCADef3D(LIFE3DI_ROWS, LIFE3DI_COLS, LIFE3DI_LAYERS, CAL_VON_NEUMANN_NEIGHBORHOOD_3D, CAL_SPACE_FLAT, CAL_NO_OPT);
		life3Disimulation = calRunDef3D(life3Di, 0, CAL_RUN_LOOP, CAL_UPDATE_IMPLICIT);
		// add transition function's elementary processes.
		calAddElementaryProcess3D(life3Di, life3DiTransitionFunction);
		// add substates.
		life3DiSubstate = calAddSubstate3Di(life3Di);
		// simulation run setup.
		calRunAddInitFunc3D(life3Disimulation, life3DiSimulationInit);

		// Initialize a default camera.
		Camera::InitializeCamera("Camera");
		//Camera::ActiveCamera()->SetBackgroundColor(vec4(0.4f, 0.0f, 0.0f, 1.0f));

		// Initialize a default directional light.
		Light::InitializeDefaultDirectionalLight();

		CellularAutomataDesc lifeDesc;
		lifeDesc.model3D = life3Di;
		lifeDesc.run3D = life3Disimulation;
		lifeDesc.id = "LifeCA";
		lifeDesc.tag = "CA";
		lifeDesc.renderLayer = 0;
		lifeDesc.cellSize = 1.0f;
		//lifeDesc.renderStep = 10;
		lifeDesc.drawMode = eDrawMode::DRAWMODE_3D_VOLUME;

		// Defining a graphics cellular automata object.
		CellularAutomata* cellularAutomata = CellularAutomata::DefineCellularAutomata(lifeDesc);

		MaterialDesc greyMaterial;
		greyMaterial.materialName = "Grey.Material";
		//greyMaterial.wireframe = true;
		greyMaterial.colorDiffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);

		// Add substates to the scene.
		SubstateRendererDesc lifeSubstateDesc;
		lifeSubstateDesc.substateName = "Life";
		lifeSubstateDesc.parentObject = "";
		lifeSubstateDesc.cellularAutomata = cellularAutomata;
		lifeSubstateDesc.substate3Di = life3DiSubstate;
		lifeSubstateDesc.dataType = eDataType::DYNAMIC_DATA;
		lifeSubstateDesc.materialDesc = &greyMaterial;
		//lifeSubstateDesc.noByteData = ;
		lifeSubstateDesc.colorMinValue = vec4(0.0f, 0.0f, 0.0f, 0.01f);
		lifeSubstateDesc.colorMaxValue = vec4(1.0f, 1.0f, 1.0f, 0.8f);

		SubstateRenderer::DefineSubstate(lifeSubstateDesc);

		// Set the dimension of the grid.
		Gizmo::SetGridSize(50);

		return true;
	}

	virtual void LoadBehaviourScripts() {
		SceneObject::AddScriptToSceneObject("Camera", new CameraFPSControllerScript());
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());
	}

private:
	int					version;
	int					steps;
};
