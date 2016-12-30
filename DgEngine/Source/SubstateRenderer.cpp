////////////////////////////////////////////////////////////////////////////////
// Filename: SubstateRenderer.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <AntTweakBar.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SubstateRenderer.h"
#include "Behaviour.h"
#include "Mesh.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Glossary.h"
#include "Transform.h"
#include "Utils.h"
/*Scripts*/
#include "SimulationControllerScript.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


SubstateRenderer::SubstateRenderer(SceneObject* sceneObject, const SubstateRendererDesc& desc)
	: MeshRenderer(sceneObject) {
	m_CellularAutomata = desc.cellularAutomata;
	m_DataType = desc.dataType;
	m_LastUpdateStep = 0;
	m_InfoBarDraw = desc.infoBarDraw;
	m_InfoBarOrientation = desc.infoBarOrientation;
	m_InfoBarXPosition = desc.infoBarXPosition;
	m_InfoBarYPosition = desc.infoBarYPosition;
	m_InfoBarWidth = desc.infoBarWidth;
	m_InfoBarHeight = desc.infoBarHeight;
}

SubstateRenderer::~SubstateRenderer() {
	m_CellularAutomata = nullptr;
	m_Mesh = nullptr;
}

void SubstateRenderer::DrawUI(TwBar* bar) {
	MeshRenderer::DrawUI(bar);

	TwAddButton(bar, "SubstateRenderer", NULL, NULL, "");
	
	if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
		TwAddVarRO(bar, "ByteNoDataValue", TW_TYPE_INT8, &m_ByteNoDataValue, " label='No data value' ");
		TwAddVarRO(bar, "ByteMinValue", TW_TYPE_INT8, &m_ByteMinValue, " label='Minimum value' ");
		TwAddVarRO(bar, "ByteMaxValue", TW_TYPE_INT8, &m_ByteMaxValue, " label='Maximum value' ");
	} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
		TwAddVarRO(bar, "IntNoDataValue", TW_TYPE_INT32, &m_IntNoDataValue, " label='No data value' ");
		TwAddVarRO(bar, "IntMinValue", TW_TYPE_INT32, &m_IntMinValue, " label='Minimum value' ");
		TwAddVarRO(bar, "IntMaxValue", TW_TYPE_INT32, &m_IntMaxValue, " label='Maximum value' ");
	} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
		TwAddVarRO(bar, "RealNoDataValue", TW_TYPE_DOUBLE, &m_RealNoDataValue, " label='No data value' ");
		TwAddVarRO(bar, "RealMinValue", TW_TYPE_DOUBLE, &m_RealMinValue, " label='Minimum value' ");
		TwAddVarRO(bar, "RealMaxValue", TW_TYPE_DOUBLE, &m_RealMaxValue, " label='Maximum value' ");
	}

	TwAddVarRW(bar, "ColorMinValue", TW_TYPE_COLOR4F, &m_ColorMinValue, "");
	TwAddVarRW(bar, "ColorMaxValue", TW_TYPE_COLOR4F, &m_ColorMaxValue, "");

	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);
}

void SubstateRenderer::Initialize() {
	bool rootSubstate = true;
	MeshRenderer* meshRenderer = GetComponentType(GetSceneObject()->Parent(), MeshRenderer);
	if(meshRenderer) {
		SubstateRenderer* substateRenderer = dynamic_cast<SubstateRenderer*>(meshRenderer);
		rootSubstate = substateRenderer==nullptr;
	}

	if(rootSubstate) {
		// Add data to DataManager.
		m_FilePath = GetSceneObject()->Id();
		// Add to map for instanced rendering.
		DataManager::AddSceneObjectToObjectsMap(m_FilePath, GetSceneObject());
	}
}

SubstateRenderer* SubstateRenderer::DefineSubstate(const SubstateRendererDesc& desc) {
	if(desc.cellularAutomata->GetDrawMode()==eDrawMode::DRAWMODE_2D_FLAT)
		return DefineSubstate2D_Flat(desc);
	else if(desc.cellularAutomata->GetDrawMode()==eDrawMode::DRAWMODE_2D_SURFACE)
		return DefineSubstate2D_Surface(desc);
	else if(desc.cellularAutomata->GetDrawMode()==eDrawMode::DRAWMODE_3D_CUBE)
		return DefineSubstate3D_Cube(desc);
	else if(desc.cellularAutomata->GetDrawMode()==eDrawMode::DRAWMODE_3D_VOLUME)
		return DefineSubstate3D_Volume(desc);

	return NULL;
}
