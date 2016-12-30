////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_MultiView.h
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

////////////////
// NAMESPACES //
////////////////
using namespace dg;

#pragma region OPENCAL_SCIDDICA
#define SCIDDICA_ROWS 610
#define SCIDDICA_COLS 496
#define SCIDDICA_P_R 0.5
#define SCIDDICA_P_EPSILON 0.001
#define SCIDDICA_STEPS 10
#define SCIDDICA_DEM_PATH "./Resources/testData/sciddicaT-data/dem.txt"
#define SCIDDICA_SOURCE_PATH "./Resources/testData/sciddicaT-data/source.txt"

#define SCIDDICA_NUMBER_OF_OUTFLOWS 4

#define SCIDDICA_PREFIX_PATH(version,name,pathVarName) \
    if(version==0)\
    pathVarName="./Resources/testsout/serial/"name;\
																															    else if(version>0)\
    pathVarName="./Resources/testsout/other/"name;

struct multiview_sciddicaTSubstates {
	struct CALSubstate2Dr *z;
	struct CALSubstate2Dr *h;
	struct CALSubstate2Dr *f[SCIDDICA_NUMBER_OF_OUTFLOWS];
} sciddica_substates;

struct multiview_sciddicaTParameters {
	CALParameterr epsilon;
	CALParameterr r;
} sciddica_parameters;

int sciddica_numberOfLoops;

void multiview_sciddicaT_flows_computation(struct CALModel2D* sciddicaT, int i, int j) {
	CALbyte eliminated_cells[5] = {CAL_FALSE, CAL_FALSE, CAL_FALSE, CAL_FALSE, CAL_FALSE};
	CALbyte again;
	CALint cells_count;
	CALreal average;
	CALreal m;
	CALreal u[5];
	CALint n;
	CALreal z, h;

	int k;
	for(k = 0; k<sciddica_numberOfLoops; k++) {
		if(calGet2Dr(sciddicaT, sciddica_substates.h, i, j)<=sciddica_parameters.epsilon)
			return;

		m = calGet2Dr(sciddicaT, sciddica_substates.h, i, j)-sciddica_parameters.epsilon;
		u[0] = calGet2Dr(sciddicaT, sciddica_substates.z, i, j)+sciddica_parameters.epsilon;
		for(n = 1; n<sciddicaT->sizeof_X; n++) {
			z = calGetX2Dr(sciddicaT, sciddica_substates.z, i, j, n);
			h = calGetX2Dr(sciddicaT, sciddica_substates.h, i, j, n);
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
				calSet2Dr(sciddicaT, sciddica_substates.f[n-1], i, j, 0.0);
			else
				calSet2Dr(sciddicaT, sciddica_substates.f[n-1], i, j, (average-u[n])*sciddica_parameters.r);
	}
}

void multiview_sciddicaT_width_update(struct CALModel2D* sciddicaT, int i, int j) {
	CALreal h_next;
	CALint n;
	int k;

	for(k = 0; k<sciddica_numberOfLoops; k++) {
		h_next = calGet2Dr(sciddicaT, sciddica_substates.h, i, j);
		for(n = 1; n<sciddicaT->sizeof_X; n++)
			h_next += calGetX2Dr(sciddicaT, sciddica_substates.f[SCIDDICA_NUMBER_OF_OUTFLOWS-n], i, j, n)-calGet2Dr(sciddicaT, sciddica_substates.f[n-1], i, j);

		calSet2Dr(sciddicaT, sciddica_substates.h, i, j, h_next);
	}
}

void multiview_sciddicaT_simulation_init(struct CALModel2D* sciddicaT) {
	CALreal z, h;
	CALint i, j;

	//initializing substates to 0
	calInitSubstate2Dr(sciddicaT, sciddica_substates.f[0], 0);
	calInitSubstate2Dr(sciddicaT, sciddica_substates.f[1], 0);
	calInitSubstate2Dr(sciddicaT, sciddica_substates.f[2], 0);
	calInitSubstate2Dr(sciddicaT, sciddica_substates.f[3], 0);

	//sciddicaT parameters setting
	sciddica_parameters.r = SCIDDICA_P_R;
	sciddica_parameters.epsilon = SCIDDICA_P_EPSILON;

	//sciddicaT source initialization
	for(i = 0; i<sciddicaT->rows; i++)
		for(j = 0; j<sciddicaT->columns; j++) {
			h = calGet2Dr(sciddicaT, sciddica_substates.h, i, j);

			if(h>0.0) {
				z = calGet2Dr(sciddicaT, sciddica_substates.z, i, j);
				calSet2Dr(sciddicaT, sciddica_substates.z, i, j, z-h);
			}
		}
}

void multiview_sciddicaTSteering(struct CALModel2D* sciddicaT) {
	//initializing substates to 0
	calInitSubstate2Dr(sciddicaT, sciddica_substates.f[0], 0);
	calInitSubstate2Dr(sciddicaT, sciddica_substates.f[1], 0);
	calInitSubstate2Dr(sciddicaT, sciddica_substates.f[2], 0);
	calInitSubstate2Dr(sciddicaT, sciddica_substates.f[3], 0);
}
#pragma endregion

class Scene_MultiView : public ApplicationScene {
public:
	Scene_MultiView(string nameScene)
		: ApplicationScene(nameScene) {}
	virtual ~Scene_MultiView() {}

	virtual bool Initialize() {
#pragma region SciddicaT_Init
		version = 0;
		steps = SCIDDICA_STEPS;
		sciddica_numberOfLoops = 1;

		//cadef and rundef
		sciddicaT = calCADef2D(SCIDDICA_ROWS, SCIDDICA_COLS, CAL_VON_NEUMANN_NEIGHBORHOOD_2D, CAL_SPACE_TOROIDAL, CAL_NO_OPT);
		sciddicaT_simulation = calRunDef2D(sciddicaT, 1, steps, CAL_UPDATE_IMPLICIT);

		//add transition function's elementary processes
		calAddElementaryProcess2D(sciddicaT, multiview_sciddicaT_flows_computation);
		calAddElementaryProcess2D(sciddicaT, multiview_sciddicaT_width_update);

		//add substates
		sciddica_substates.z = calAddSubstate2Dr(sciddicaT);
		sciddica_substates.h = calAddSubstate2Dr(sciddicaT);
		sciddica_substates.f[0] = calAddSubstate2Dr(sciddicaT);
		sciddica_substates.f[1] = calAddSubstate2Dr(sciddicaT);
		sciddica_substates.f[2] = calAddSubstate2Dr(sciddicaT);
		sciddica_substates.f[3] = calAddSubstate2Dr(sciddicaT);

		//load configuration
		calLoadSubstate2Dr(sciddicaT, sciddica_substates.z, SCIDDICA_DEM_PATH);
		calLoadSubstate2Dr(sciddicaT, sciddica_substates.h, SCIDDICA_SOURCE_PATH);

		//simulation run
		calRunAddInitFunc2D(sciddicaT_simulation, multiview_sciddicaT_simulation_init);
		calRunAddSteeringFunc2D(sciddicaT_simulation, multiview_sciddicaTSteering);
#pragma endregion

#pragma region Life_Init
		// cadef and rundef.
		life3D = calCADef3D(LIFE3D_ROWS, LIFE3D_COLS, LIFE3D_LAYERS, CAL_MOORE_NEIGHBORHOOD_3D, CAL_SPACE_TOROIDAL, CAL_NO_OPT);
		life3Dsimulation = calRunDef3D(life3D, 1, CAL_RUN_LOOP, CAL_UPDATE_IMPLICIT);
		// add transition function's elementary processes.
		calAddElementaryProcess3D(life3D, life3DTransitionFunction);
		// add substates.
		lifeSubstate = calAddSubstate3Db(life3D);
		// simulation run setup.
		calRunAddInitFunc3D(life3Dsimulation, life3DSimulationInit);
		calRunAddStopConditionFunc3D(life3Dsimulation, life3DSimulationStopCondition);
#pragma endregion

#pragma region SciddicaView

		// Initialize a default camera.
		Camera::InitializeCamera("Camera1", 0.0f, 0.0f, 0.5f, 1.0f);

		CellularAutomataDesc sciddicaDesc;
		sciddicaDesc.model2D = sciddicaT;
		sciddicaDesc.run2D = sciddicaT_simulation;
		sciddicaDesc.id = "Sciddica";
		sciddicaDesc.tag = "CA";
		sciddicaDesc.renderLayer = 0;
		sciddicaDesc.cellSize = 1.0f;
		sciddicaDesc.renderStep = 10;

		// Defining a graphics cellular automata object.
		cellularAutomata = CellularAutomata::DefineCellularAutomata(sciddicaDesc);

		MaterialDesc greyMaterial;
		greyMaterial.materialName = "Grey.Material";
		//greyMaterial.wireframe = true;
		greyMaterial.colorDiffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		//greyMaterial.texturesPath[eTextureType::DIFFUSE_TEXTURE] = "./Resources/Textures/field.jpg";

		MaterialDesc redMaterial;
		redMaterial.materialName = "Red.Material";
		redMaterial.colorDiffuse = vec4(1.0f, 0.0f, 0.0f, 1.0f);

		// Add substates to the scene. METHOD 2
		SubstateRendererDesc morfologyDesc;
		morfologyDesc.substateName = "Morfology";
		morfologyDesc.parentObject = "";
		morfologyDesc.cellularAutomata = cellularAutomata;
		morfologyDesc.substate2Dr = sciddica_substates.z;
		morfologyDesc.dataType = eDataType::STATIC_DATA;
		morfologyDesc.drawMode = eDrawMode::DRAWMODE_2D_SURFACE;
		morfologyDesc.materialDesc = &greyMaterial;

		SubstateRendererDesc debrisDesc;
		debrisDesc.substateName = "Debris";
		debrisDesc.parentObject = "Morfology";
		debrisDesc.cellularAutomata = cellularAutomata;
		debrisDesc.substate2Dr = sciddica_substates.h;
		debrisDesc.dataType = eDataType::DYNAMIC_DATA;
		debrisDesc.drawMode = eDrawMode::DRAWMODE_2D_SURFACE;
		debrisDesc.materialDesc = &redMaterial;

		SubstateRenderer::DefineSubstate(morfologyDesc);
		SubstateRenderer::DefineSubstate(debrisDesc);
#pragma endregion

#pragma region Life_View
		// Initialize a default camera.
		Camera::InitializeCamera("Camera2", 0.5f, 0.0f, 0.5f, 1.0f);

		CellularAutomataDesc lifeDesc;
		lifeDesc.model3D = life3D;
		lifeDesc.run3D = life3Dsimulation;
		lifeDesc.id = "LifeCA";
		lifeDesc.tag = "CA";
		lifeDesc.renderLayer = 0;
		lifeDesc.cellSize = 1.0f;
		//lifeDesc.renderStep = 10;

		// Defining a graphics cellular automata object.
		life_cellularAutomata = CellularAutomata::DefineCellularAutomata(lifeDesc);

		MaterialDesc cubeGreyMaterial;
		cubeGreyMaterial.materialName = "CubeGrey.Material";
		//cubeGreyMaterial.wireframe = true;
		cubeGreyMaterial.colorDiffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		//cubeGreyMaterial.texturesPath[eTextureType::DIFFUSE_TEXTURE] = "./Resources/Textures/field.jpg";

		// Add substates to the scene. METHOD 2
		SubstateRendererDesc lifeSubstateDesc;
		lifeSubstateDesc.substateName = "Life";
		lifeSubstateDesc.parentObject = "";
		lifeSubstateDesc.cellularAutomata = life_cellularAutomata;
		lifeSubstateDesc.substate3Db = lifeSubstate;
		lifeSubstateDesc.dataType = eDataType::DYNAMIC_DATA;
		lifeSubstateDesc.drawMode = eDrawMode::DRAWMODE_3D_CUBE;
		lifeSubstateDesc.materialDesc = &cubeGreyMaterial;

		SubstateRenderer::DefineSubstate(lifeSubstateDesc);
#pragma endregion

		// Initialize a default directional light.
		Light::InitializeDefaultDirectionalLight();

		// Set the dimension of the grid.
		Gizmo::SetGridSize(50);

		return ApplicationScene::Initialize();
	}

private:
	// Sciddica
	CALModel2D*			sciddicaT;
	CALRun2D*			sciddicaT_simulation;
	int					version;
	int					steps;
	CellularAutomata*	cellularAutomata;
	// Life
	CellularAutomata*	life_cellularAutomata;
};
