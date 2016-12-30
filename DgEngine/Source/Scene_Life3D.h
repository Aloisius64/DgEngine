////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_Life3D.h
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
#define LIFE3D_ROWS 32
#define LIFE3D_COLS 32
#define LIFE3D_LAYERS 32
#define LIFE3D_STEPS 4000

CALModel3D*			life3D;				// the cellular automaton
CALSubstate3Db*		lifeSubstate;		// the substate
CALRun3D*			life3Dsimulation;	// the simulartion run

//------------------------------------------------------------------------------
//					life3D transition function
//------------------------------------------------------------------------------

// first elementary process.
void life3DTransitionFunction(struct CALModel3D* ca, int i, int j, int k) {
	int sum = 0, n;

	for(n = 0; n<ca->sizeof_X; n++)
		sum += calGetX3Db(ca, lifeSubstate, i, j, k, n);

	calSet3Db(ca, lifeSubstate, i, j, k, sum%2);
}

//------------------------------------------------------------------------------
//					life3D simulation functions
//------------------------------------------------------------------------------

void life3DSimulationInit(struct CALModel3D* ca) {
	//int i, j, k, state;

	//initializing substate to 0
	calInitSubstate3Db(ca, lifeSubstate, 0);

	calSet3Db(ca, lifeSubstate, 0, 0, 0, 1);

	calUpdate3D(ca);
}

CALbyte life3DSimulationStopCondition(struct CALModel3D* life3D) {
	if(life3Dsimulation->step>=LIFE3D_STEPS)
		return CAL_TRUE;
	return CAL_FALSE;
}

#pragma endregion

class Scene_Life3D : public ApplicationScene {
public:
	Scene_Life3D(string nameScene)
		: ApplicationScene(nameScene) {}
	virtual ~Scene_Life3D() {}

	virtual bool Initialize() {
		// cadef and rundef.
		life3D = calCADef3D(LIFE3D_ROWS, LIFE3D_COLS, LIFE3D_LAYERS, CAL_MOORE_NEIGHBORHOOD_3D, CAL_SPACE_TOROIDAL, CAL_OPT_ACTIVE_CELLS);
		life3Dsimulation = calRunDef3D(life3D, 0, CAL_RUN_LOOP, CAL_UPDATE_IMPLICIT);
		// add transition function's elementary processes.
		calAddElementaryProcess3D(life3D, life3DTransitionFunction);
		// add substates.
		lifeSubstate = calAddSubstate3Db(life3D);
		// simulation run setup.
		calRunAddInitFunc3D(life3Dsimulation, life3DSimulationInit);
		//calRunAddStopConditionFunc3D(life3Dsimulation, life3DSimulationStopCondition);

		// Initialize a default camera.
		Camera::InitializeCamera("Camera");

		// Initialize a default directional light.
		Light::InitializeDefaultDirectionalLight();

		CellularAutomataDesc lifeDesc;
		lifeDesc.model3D = life3D;
		lifeDesc.run3D = life3Dsimulation;
		lifeDesc.id = "LifeCA";
		lifeDesc.tag = "CA";
		lifeDesc.renderLayer = 0;
		lifeDesc.cellSize = 1.0f;
		lifeDesc.renderStep = 10;
		lifeDesc.drawMode = eDrawMode::DRAWMODE_3D_CUBE;

		// Defining a graphics cellular automata object.
		cellularAutomata = CellularAutomata::DefineCellularAutomata(lifeDesc);

		MaterialDesc greyMaterial;
		greyMaterial.materialName = "Grey.Material";
		//greyMaterial.wireframe = true;
		greyMaterial.colorDiffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);

		// Add substates to the scene.
		SubstateRendererDesc lifeSubstateDesc;
		lifeSubstateDesc.substateName = "Life";
		lifeSubstateDesc.parentObject = "";
		lifeSubstateDesc.cellularAutomata = cellularAutomata;
		lifeSubstateDesc.substate3Db = lifeSubstate;
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
	CellularAutomata*	cellularAutomata;
};
