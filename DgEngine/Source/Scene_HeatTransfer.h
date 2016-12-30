////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_HeatTransfer.h
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


#pragma region OpenCAL
#define HEATTRANSFER_SIZE (64)
#define HEATTRANSFER_ROWS (HEATTRANSFER_SIZE)
#define HEATTRANSFER_COLS (HEATTRANSFER_SIZE)
#define HEATTRANSFER_LAYERS (HEATTRANSFER_SIZE)
#define HEATTRANSFER_STEPS (20000)
#define HEATTRANSFER_EPSILON (0.01)

#define HEATTRANSFER_DELTA_X (0.001)
#define HEATTRANSFER_DELTA_Y (0.001)
#define HEATTRANSFER_DELTA_Z (0.001)
#define HEATTRANSFER_DELTA_T (0.001)
#define HEATTRANSFER_THERMAL_CONDUCTIVITY (1)
#define HEATTRANSFER_MASS_DENSITY (1)
#define HEATTRANSFER_SPECIFIC_HEAT_CAPACITY (1)
#define HEATTRANSFER_THERMAL_DIFFUSIVITY ( (HEATTRANSFER_THERMAL_CONDUCTIVITY)/(HEATTRANSFER_SPECIFIC_HEAT_CAPACITY)*(HEATTRANSFER_MASS_DENSITY) )
#define HEATTRANSFER_THERMAL_DIFFUSIVITY_WATER (1.4563e-4) //C/m^2
#define HEATTRANSFER_INIT_TEMP (1200)

struct CALModel3D* heatModel;			//the cellular automaton
struct CALSubstate3Dr *Q_temperature;	//the substate Q
struct CALSubstate3Db *Q_heat_source;	//the substate Q
struct CALRun3D* heat_simulation;
const CALreal radius = 5;

// The cell's transition function (first and only elementary process)
void heatModel_TransitionFunction(struct CALModel3D* heatModel, int i, int j, int k) {
	if(i > 1&&i < ROWS-1&&j > 1&&j < COLS-1&&k > 1&&k < HEATTRANSFER_LAYERS-1) {
		CALreal currValue = calGet3Dr(heatModel, Q_temperature, i, j, k);

		CALreal dx2 = (calGet3Dr(heatModel, Q_temperature, i+1, j, k)+calGet3Dr(heatModel, Q_temperature, i-1, j, k)-(2*currValue))/(HEATTRANSFER_DELTA_X*HEATTRANSFER_DELTA_X);


		CALreal dy2 = (calGet3Dr(heatModel, Q_temperature, i, j+1, k)+calGet3Dr(heatModel, Q_temperature, i, j-1, k)-(2*currValue))/(HEATTRANSFER_DELTA_Y*HEATTRANSFER_DELTA_Y);


		CALreal dz2 = (calGet3Dr(heatModel, Q_temperature, i, j, k+1)+calGet3Dr(heatModel, Q_temperature, i, j, k-1)-(2*currValue))/(HEATTRANSFER_DELTA_Z*HEATTRANSFER_DELTA_Z);

		CALreal newValue = currValue+HEATTRANSFER_DELTA_T*HEATTRANSFER_THERMAL_DIFFUSIVITY_WATER * (dx2+dy2+dz2);

		if(newValue > HEATTRANSFER_EPSILON  && newValue < 10000) {
			//xprintf("newVal i,j,k = %i, %i, %i -> dx2=%.15f , dy2=%.15f , dz2=%.15f , val =%.15f \n" ,i,j,k, dx2,dy2,dz2,newValue);
			calSet3Dr(heatModel, Q_temperature, i, j, k, newValue);
			newValue = currValue;

		}

		CALint _i = i-(HEATTRANSFER_ROWS/2);
		CALint	_j = j-(HEATTRANSFER_COLS/2);
		CALint	_z = k-(HEATTRANSFER_LAYERS/2);
		if(_i *_i+_j*_j+_z * _z<=radius) {
			//printf("Temp at source is %f  ->>>>>",newValue);
			//printf("newVal i,j,k = %i, %i, %i -> dx2=%.15f , dy2=%.15f , dz2=%.15f , val =%.15f \n" ,i,j,k, dx2,dy2,dz2,newValue);
			//calSet3Dr(heatModel, Q_temperature, i, j, k, newValue-0.1);
			//calSet3Dr(heatModel, Q_temperature, i, j, k, newValue+0.00001);
		}

	} else {
		calSet3Dr(heatModel, Q_temperature, i, j, k, 0);
	}
}

// Simulation init callback function used to set a seed at position (24, 0, 0)
void heatModel_SimulationInit(struct CALModel3D* heatModel) {

	calInitSubstate3Dr(heatModel, Q_temperature, (CALreal) 0);
	calInitSubstate3Db(heatModel, Q_heat_source, CAL_FALSE);

	//for(int i=1 ; i < HEATTRANSFER_ROWS ; ++i){
	for(int j = 1; j < HEATTRANSFER_COLS; ++j) {
		for(int z = 1; z < HEATTRANSFER_LAYERS; ++z) {
			CALreal chunk = HEATTRANSFER_ROWS/2;
			calSet3Dr(heatModel, Q_temperature, (int) chunk, j, z, HEATTRANSFER_INIT_TEMP);
			calSet3Dr(heatModel, Q_temperature, (int) chunk+1, j, z, HEATTRANSFER_INIT_TEMP);
			calSet3Dr(heatModel, Q_temperature, (int) chunk-1, j, z, HEATTRANSFER_INIT_TEMP);
		}
	}
}

CALbyte heatModel_SimulationStopCondition(struct CALModel3D* heatModel) {
	if(heat_simulation->step>=HEATTRANSFER_STEPS)
		return CAL_TRUE;
	return CAL_FALSE;
}
#pragma endregion


class Scene_HeatTransfer : public ApplicationScene {
public:
	Scene_HeatTransfer(string nameScene)
		: ApplicationScene(nameScene) {}
	virtual ~Scene_HeatTransfer() {}

	virtual bool Initialize() {
		//cadef and rundef
		heatModel = calCADef3D(HEATTRANSFER_ROWS, HEATTRANSFER_COLS, HEATTRANSFER_LAYERS, CAL_MOORE_NEIGHBORHOOD_3D, CAL_SPACE_FLAT, CAL_NO_OPT);
		heat_simulation = calRunDef3D(heatModel, 1, HEATTRANSFER_STEPS, CAL_UPDATE_IMPLICIT);
		//add substates
		Q_temperature = calAddSubstate3Dr(heatModel);
		Q_heat_source = calAddSubstate3Db(heatModel);
		//add transition function's elementary processes
		calAddElementaryProcess3D(heatModel, heatModel_TransitionFunction);
		//simulation run setup
		calRunAddInitFunc3D(heat_simulation, heatModel_SimulationInit);
		calRunInitSimulation3D(heat_simulation);	//It is required in the case the simulation main loop is explicitated; similarly for calRunFinalizeSimulation3D
		calRunAddStopConditionFunc3D(heat_simulation, heatModel_SimulationStopCondition);
		
		// Initialize a default camera.
		Camera::InitializeCamera("Camera");
		//Camera::ActiveCamera()->SetPosition(vec3(0.0f, 50.0f, 200.0f));
		Camera::ActiveCamera()->SetPosition(vec3(0.0f, 1.5f, 3.0f));

		// Initialize a default directional light.
		Light::InitializeDefaultDirectionalLight();

		CellularAutomataDesc heatTransferDesc;
		heatTransferDesc.model3D = heatModel;
		heatTransferDesc.run3D = heat_simulation;
		heatTransferDesc.id = "HeatTransfer";
		heatTransferDesc.tag = "CA";
		heatTransferDesc.renderLayer = 0;
		heatTransferDesc.cellSize = 1.0f;
		//lifeDesc.renderStep = 10;
		heatTransferDesc.drawMode = eDrawMode::DRAWMODE_3D_VOLUME;

		// Defining a graphics cellular automata object.
		CellularAutomata* cellularAutomata = CellularAutomata::DefineCellularAutomata(heatTransferDesc);

		MaterialDesc whiteMaterial;
		whiteMaterial.materialName = "White.Material";
		//whiteMaterial.wireframe = true;
		whiteMaterial.colorDiffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Add substates to the scene.
		SubstateRendererDesc heatTransferSubstateDesc;
		heatTransferSubstateDesc.substateName = "HeatTransferSubstate";
		heatTransferSubstateDesc.parentObject = "";
		heatTransferSubstateDesc.cellularAutomata = cellularAutomata;
		heatTransferSubstateDesc.substate3Dr = Q_temperature;
		heatTransferSubstateDesc.dataType = eDataType::DYNAMIC_DATA;
		heatTransferSubstateDesc.materialDesc = &whiteMaterial;
		heatTransferSubstateDesc.colorMinValue = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		heatTransferSubstateDesc.colorMaxValue = vec4(1.0f, 0.0157f, 0.0f, 1.0f);
		heatTransferSubstateDesc.infoBarDraw = true;
		heatTransferSubstateDesc.infoBarOrientation = eInfoBarOrientation::INFO_BAR_VERTICAL;
		heatTransferSubstateDesc.infoBarXPosition = 32;
		heatTransferSubstateDesc.infoBarYPosition = 128;
		heatTransferSubstateDesc.infoBarWidth = 32;
		heatTransferSubstateDesc.infoBarHeight = 256;

		SubstateRenderer::DefineSubstate(heatTransferSubstateDesc);

		// Set the dimension of the grid.
		Gizmo::SetGridSize(50);

		return true;
	}

	virtual void LoadBehaviourScripts() {
		SceneObject::AddScriptToSceneObject("Camera", new CameraFPSControllerScript());
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());
	}

	virtual void Release() {}

};
