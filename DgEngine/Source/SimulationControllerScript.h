////////////////////////////////////////////////////////////////////////////////
// Filename: SimulationControllerScript.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Script.h"
#include "CellularAutomata.h"
#include "Input.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class SimulationControllerScript : public Script {
public:
	SimulationControllerScript(CellularAutomata* cellularAutomata)
		: Script("SimulationControllerScript") {
		m_CellularAutomata = cellularAutomata;
		m_Simulate = false;
		m_LastKeyState = eKeyState::KEY_STATE_UNDEFINED;
	}

	virtual void Update() {
		eKeyState keyState = Input::KeyState(eKey::KEY_X);
		if(keyState==eKeyState::KEY_STATE_PRESS && m_LastKeyState==eKeyState::KEY_STATE_RELEASE) {
			m_Simulate = !m_Simulate;
			m_CellularAutomata->RunSimulation(m_Simulate);
		}
		if(keyState!=m_LastKeyState) {
			m_LastKeyState = keyState;
		}
	}

private:
	CellularAutomata*	m_CellularAutomata;
	bool				m_LastSimulateValue;
	bool				m_Simulate;
	eKeyState			m_LastKeyState;
};
