////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_SciddicaT.h
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
#define ROWS 610
#define COLS 496
#define P_R 0.5
#define P_EPSILON 0.001
#define STEPS 1000
#define DEM_PATH "./Resources/testData/sciddicaT-data/dem.txt"
#define SOURCE_PATH "./Resources/testData/sciddicaT-data/source.txt"

#define NUMBER_OF_OUTFLOWS 4

CALModel2D* sciddicaT;
CALRun2D*	sciddicaT_simulation;

struct sciddicaTSubstates {
	struct CALSubstate2Dr *z;
	struct CALSubstate2Dr *h;
	struct CALSubstate2Dr *f[NUMBER_OF_OUTFLOWS];
} substates;

struct sciddicaTParameters {
	CALParameterr epsilon;
	CALParameterr r;
} parameters;

void sciddicaT_flows_computation(struct CALModel2D* sciddicaT, int i, int j) {
	CALbyte eliminated_cells[5] = {CAL_FALSE, CAL_FALSE, CAL_FALSE, CAL_FALSE, CAL_FALSE};
	CALbyte again;
	CALint cells_count;
	CALreal average;
	CALreal m;
	CALreal u[5];
	CALint n;
	CALreal z, h;

	if(calGet2Dr(sciddicaT, substates.h, i, j)<=parameters.epsilon)
		return;

	m = calGet2Dr(sciddicaT, substates.h, i, j)-parameters.epsilon;
	u[0] = calGet2Dr(sciddicaT, substates.z, i, j)+parameters.epsilon;
	for(n = 1; n<sciddicaT->sizeof_X; n++) {
		z = calGetX2Dr(sciddicaT, substates.z, i, j, n);
		h = calGetX2Dr(sciddicaT, substates.h, i, j, n);
		u[n] = z+h;
	}

	//computes outflows
	do {
		again = CAL_FALSE;
		average = m;
		cells_count = 0;

		for(n = 0; n<sciddicaT->sizeof_X; n++)
			if(!eliminated_cells[n]) {
				average += u[n];
				cells_count++;
			}

		if(cells_count!=0)
			average /= cells_count;

		for(n = 0; n<sciddicaT->sizeof_X; n++)
			if((average<=u[n])&&(!eliminated_cells[n])) {
				eliminated_cells[n] = CAL_TRUE;
				again = CAL_TRUE;
			}

	} while(again);

	for(n = 1; n<sciddicaT->sizeof_X; n++)
		if(eliminated_cells[n])
			calSet2Dr(sciddicaT, substates.f[n-1], i, j, 0.0);
		else
			calSet2Dr(sciddicaT, substates.f[n-1], i, j, (average-u[n])*parameters.r);

}

void sciddicaT_width_update(struct CALModel2D* sciddicaT, int i, int j) {
	CALreal h_next;
	CALint n;

	h_next = calGet2Dr(sciddicaT, substates.h, i, j);
	for(n = 1; n<sciddicaT->sizeof_X; n++)
		h_next += calGetX2Dr(sciddicaT, substates.f[NUMBER_OF_OUTFLOWS-n], i, j, n)-calGet2Dr(sciddicaT, substates.f[n-1], i, j);

	calSet2Dr(sciddicaT, substates.h, i, j, h_next);
}

void sciddicaT_simulation_init(struct CALModel2D* sciddicaT) {
	CALreal z, h;
	CALint i, j;

	//initializing substates to 0
	calInitSubstate2Dr(sciddicaT, substates.f[0], 0);
	calInitSubstate2Dr(sciddicaT, substates.f[1], 0);
	calInitSubstate2Dr(sciddicaT, substates.f[2], 0);
	calInitSubstate2Dr(sciddicaT, substates.f[3], 0);

	//sciddicaT parameters setting
	parameters.r = P_R;
	parameters.epsilon = P_EPSILON;

	//sciddicaT source initialization
	for(i = 0; i<sciddicaT->rows; i++)
		for(j = 0; j<sciddicaT->columns; j++) {
			h = calGet2Dr(sciddicaT, substates.h, i, j);

			if(h>0.0) {
				z = calGet2Dr(sciddicaT, substates.z, i, j);
				calSet2Dr(sciddicaT, substates.z, i, j, z-h);
			}
		}
}

void sciddicaTSteering(struct CALModel2D* sciddicaT) {
	//initializing substates to 0
	calInitSubstate2Dr(sciddicaT, substates.f[0], 0);
	calInitSubstate2Dr(sciddicaT, substates.f[1], 0);
	calInitSubstate2Dr(sciddicaT, substates.f[2], 0);
	calInitSubstate2Dr(sciddicaT, substates.f[3], 0);
}
#pragma endregion

class Scene_SciddicaT : public ApplicationScene {
public:
	Scene_SciddicaT(string nameScene)
		: ApplicationScene(nameScene) {}
	virtual ~Scene_SciddicaT() {}

	virtual bool Initialize() {
		//cadef and rundef
		sciddicaT = calCADef2D(ROWS, COLS, CAL_VON_NEUMANN_NEIGHBORHOOD_2D, CAL_SPACE_TOROIDAL, CAL_NO_OPT);
		sciddicaT_simulation = calRunDef2D(sciddicaT, 1, STEPS, CAL_UPDATE_IMPLICIT);

		//add transition function's elementary processes
		calAddElementaryProcess2D(sciddicaT, sciddicaT_flows_computation);
		calAddElementaryProcess2D(sciddicaT, sciddicaT_width_update);

		//add substates
		substates.z = calAddSubstate2Dr(sciddicaT);
		substates.h = calAddSubstate2Dr(sciddicaT);
		substates.f[0] = calAddSubstate2Dr(sciddicaT);
		substates.f[1] = calAddSubstate2Dr(sciddicaT);
		substates.f[2] = calAddSubstate2Dr(sciddicaT);
		substates.f[3] = calAddSubstate2Dr(sciddicaT);

		//load configuration
		calLoadSubstate2Dr(sciddicaT, substates.z, DEM_PATH);
		calLoadSubstate2Dr(sciddicaT, substates.h, SOURCE_PATH);

		//simulation run
		calRunAddInitFunc2D(sciddicaT_simulation, sciddicaT_simulation_init);
		calRunAddSteeringFunc2D(sciddicaT_simulation, sciddicaTSteering);

		// Initialize a default camera.
		Camera::InitializeCamera("Camera");

		// Initialize a default directional light.
		Light::InitializeDefaultDirectionalLight();

		CellularAutomataDesc sciddicaDesc;
		sciddicaDesc.model2D = sciddicaT;
		sciddicaDesc.run2D = sciddicaT_simulation;
		sciddicaDesc.id = "Sciddica";
		sciddicaDesc.tag = "CA";
		sciddicaDesc.renderLayer = 0;
		sciddicaDesc.cellSize = 2.0f;
		//sciddicaDesc.renderStep = 1;
		sciddicaDesc.drawMode = eDrawMode::DRAWMODE_2D_SURFACE;

		// Defining a graphics cellular automata object.
		CellularAutomata* cellularAutomata = CellularAutomata::DefineCellularAutomata(sciddicaDesc);

		MaterialDesc greyMaterial;
		greyMaterial.materialName = "Grey.Material";
		greyMaterial.colorDiffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);

		MaterialDesc redMaterial;
		redMaterial.materialName = "Red.Material";
		redMaterial.colorDiffuse = vec4(1.0f, 0.0f, 0.0f, 1.0f);

		// Add substates to the scene.
		SubstateRendererDesc morfologyDesc;
		morfologyDesc.substateName = "Morfology";
		morfologyDesc.parentObject = "";
		morfologyDesc.cellularAutomata = cellularAutomata;
		morfologyDesc.substate2Dr = substates.z;
		morfologyDesc.dataType = eDataType::STATIC_DATA;
		morfologyDesc.materialDesc = &greyMaterial;
		// Add Layer here.

		SubstateRendererDesc debrisDesc;
		debrisDesc.substateName = "Debris";
		debrisDesc.parentObject = "Morfology";
		debrisDesc.cellularAutomata = cellularAutomata;
		debrisDesc.substate2Dr = substates.h;
		debrisDesc.dataType = eDataType::DYNAMIC_DATA;
		debrisDesc.materialDesc = &redMaterial;
		debrisDesc.colorMinValue = color_red;
		debrisDesc.colorMaxValue = color_red;
		debrisDesc.infoBarDraw = false;
		debrisDesc.infoBarOrientation = eInfoBarOrientation::INFO_BAR_HORIZONTAL;
		debrisDesc.infoBarXPosition = 256;
		debrisDesc.infoBarYPosition = 32;
		debrisDesc.infoBarWidth = 512;
		debrisDesc.infoBarHeight = 32;

		SubstateRenderer::DefineSubstate(morfologyDesc);
		SubstateRenderer::DefineSubstate(debrisDesc);

		// Set the dimension of the grid.
		Gizmo::SetGridSize(50);

		return true;
	}

	virtual void LoadBehaviourScripts() {
		SceneObject::AddScriptToSceneObject("Camera", new CameraFPSControllerScript());
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());
	}

};
