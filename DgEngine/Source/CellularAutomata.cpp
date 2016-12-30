////////////////////////////////////////////////////////////////////////////////
// Filename: CellularAutomata.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <chrono>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "CellularAutomata.h"
#include "Behaviour.h"
#include "Configuration.h"
#include "SimulationControllerScript.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////


namespace dg {

#define RUN_SIMULATION_LOOP(calRunInitSimulation, calRunCAStep)\
	if(m_FirstRun) {\
		m_FirstRun = CAL_FALSE;\
		m_StartTime = time(NULL);\
		if(m_Run->init) calRunInitSimulation(m_Run);\
										}\
	m_Run->step++;\
	m_Step = m_Run->step;\
	m_SimulationFinished = !calRunCAStep(m_Run) || (m_Run->final_step > 0 && m_Run->step>=m_Run->final_step);\
	if(m_SimulationFinished) {\
		m_SimulationFinished = true;\
		m_EndTime = time(NULL);\
										}

#pragma region Callbacks
	void TW_CALL PlaySimulationCallback(void *p) {
		CellularAutomata* ca = static_cast<CellularAutomata *>(p);
		ca->PlaySimulation();
	}
	void TW_CALL StopSimulationCallback(void *p) {
		CellularAutomata* ca = static_cast<CellularAutomata *>(p);
		ca->StopSimulation();
	}
	void TW_CALL SetRenderStepCallback(const void *value, void *clientData) {
		static_cast<CellularAutomata *>(clientData)->SetRenderStep(*static_cast<const int *>(value));
	}
	void TW_CALL GetRenderStepCallback(void *value, void *clientData) {
		*static_cast<int *>(value) = static_cast<CellularAutomata*>(clientData)->GetRenderStep();
	}
	void TW_CALL SetSleepTimeCallback(const void *value, void *clientData) {
		static_cast<CellularAutomata *>(clientData)->SetSleepTime(*static_cast<const int *>(value));
	}
	void TW_CALL GetSleepTimeCallback(void *value, void *clientData) {
		*static_cast<int *>(value) = static_cast<CellularAutomata*>(clientData)->GetSleepTime();
	}
#pragma endregion

#pragma region CellularAutomata
	CellularAutomata::CellularAutomata(const CellularAutomataDesc& desc)
		: SceneObject(desc.id, desc.tag, desc.renderLayer) {
		m_StopThread = false;
		m_SimulationPaused = true;
		m_SimulationFinished = false;
		m_CellSize = desc.cellSize;
		m_FirstRun = CAL_TRUE;
		m_SleepTime = 0;
		m_DrawMode = desc.drawMode;
		SetRenderStep(desc.renderStep);
	}

	CellularAutomata::~CellularAutomata() {}

	const float& CellularAutomata::GetCellSize() const {
		return m_CellSize;
	}

	void CellularAutomata::RunSimulation(const bool& value) {
		if(m_SimulationFinished||value!=m_SimulationPaused)
			return;
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_SimulationPaused = !value;
			m_PauseCondition.notify_all();
		}
	}

	void CellularAutomata::PlaySimulation() {
		if(m_SimulationFinished||!m_SimulationPaused)
			return;
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_SimulationPaused = false;
			m_PauseCondition.notify_all();
		}
	}

	void CellularAutomata::StopSimulation() {
		if(m_SimulationPaused)
			return;
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_SimulationPaused = true;
			m_PauseCondition.notify_all();
		}
	}

	bool CellularAutomata::IsSimulationPaused() const {
		return m_SimulationPaused;
	}

	time_t CellularAutomata::GetElapsedTime() const {
		return m_EndTime-m_StartTime;
	}

	void CellularAutomata::SetRenderStep(const int& value) {
		m_RenderStep = value>1 ? value : 1;
	}

	const int& CellularAutomata::GetRenderStep() const {
		return m_RenderStep;
	}

	void CellularAutomata::SetSleepTime(const int& value) {
		m_SleepTime = value;
	}

	const int& CellularAutomata::GetSleepTime() const {
		return m_SleepTime;
	}

	const eDrawMode& CellularAutomata::GetDrawMode() const {
		return m_DrawMode;
	}

	void CellularAutomata::DrawUI(TwBar* bar) {
		char parameter[128];
		memset(parameter, 0, sizeof(parameter));

		// Bar movable?
		SNPRINTF(parameter, sizeof(parameter), "%s movable=false", Id().c_str());
		TwDefine(parameter);

		// Bar size.
		int sizeX = 240;
		int sizeY = Configuration::Instance()->GetScreenHeigth()-20;
		SNPRINTF(parameter, sizeof(parameter), "%s  size='%d %d' ", Id().c_str(), sizeX, sizeY);
		TwDefine(parameter);

		// Bar position.
		int posX = Configuration::Instance()->GetScreenWidth()-sizeX-10;
		SNPRINTF(parameter, sizeof(parameter), "%s position='%d 10' ", Id().c_str(), posX);
		TwDefine(parameter);

		TwAddVarCB(bar, "Enabled", TW_TYPE_BOOL8, EnabledCallback, IsEnabledCallback, this, "");
		string tagStr = "Tag: "+Tag();
		TwAddButton(bar, tagStr.c_str(), NULL, NULL, "");

		TwAddVarCB(bar, "SceneObjectRenderLayer", TW_TYPE_UINT32, SetRenderLayerCallback, GetRenderLayerCallback, this, " label='RenderLayer' ");
		SNPRINTF(parameter, sizeof(parameter), "%s/SceneObjectRenderLayer min=%d ", Id().c_str(), 0);
		TwDefine(parameter);
		SNPRINTF(parameter, sizeof(parameter), "%s/SceneObjectRenderLayer step=%d ", Id().c_str(), 1);
		TwDefine(parameter);

		// Cellular automata parameters.
		TwAddSeparator(bar, "", NULL);
		TwAddButton(bar, "Cellular automata", NULL, NULL, "");

		TwAddVarRO(bar, "Rows:", TW_TYPE_INT32, &GetRows(), "");
		TwAddVarRO(bar, "Columns:", TW_TYPE_INT32, &GetColumns(), "");
		if(GetType()==eCAType::CA_3D)
			TwAddVarRO(bar, "Slices:", TW_TYPE_INT32, &GetSlices(), "");
		TwAddVarRO(bar, "CellSize:", TW_TYPE_FLOAT, &GetCellSize(), "");

		TwAddVarRO(bar, "Step:", TW_TYPE_INT32, &GetStep(), "");
		TwAddVarRO(bar, "FinalStep:", TW_TYPE_INT32, &GetFinalStep(), "");
		TwAddVarCB(bar, "RenderStep:", TW_TYPE_INT32, SetRenderStepCallback, GetRenderStepCallback, this, "");
		TwAddVarCB(bar, "SleepTime(ms):", TW_TYPE_INT32, SetSleepTimeCallback, GetSleepTimeCallback, this, "");

		TwAddButton(bar, "Simulation", NULL, NULL, "");
		TwAddButton(bar, "Play", PlaySimulationCallback, this, "");
		TwAddButton(bar, "Stop", StopSimulationCallback, this, "");

		TwAddSeparator(bar, "", NULL);
		TwAddSeparator(bar, "", NULL);

		// For all components draw the UI.
		for(unsigned int i = 0; i<eComponentType::NUM_COMPONENTs; i++) {
			if(GetComponent(eComponentType(i))) {
				GetComponent(eComponentType(i))->DrawUI(bar);
			}
		}
	}
#pragma endregion

#pragma region Cellular_Automata_2D
	class CellularAutomata_2D : public CellularAutomata {
	public:
		CellularAutomata_2D(const CellularAutomataDesc& desc)
			: CellularAutomata(desc) {
			m_Model = desc.model2D;
			m_Run = desc.run2D;
			m_Slices = 1;
		}

		virtual ~CellularAutomata_2D() {
			// Clean children
			for each (auto child in m_Children) {
				FREE_PTR(child);
			}
			m_Children.clear();

			// Terminate thread.
			{	// Lock
				std::unique_lock<std::mutex> lock(m_Mutex);
				m_StopThread = true;
				m_PauseCondition.notify_all();
			}	// Unlock
			m_UpdaterThread.join();

			// Finalize OpenCAL model adn run objects.
			calRunFinalize2D(m_Run);
			calFinalize2D(m_Model);
		}

		virtual void* GetCALStruct() {
			return m_Model;
		}
		virtual eCAType GetType() const {
			return eCAType::CA_2D;
		}
		virtual const int& GetRows() const {
			return m_Model->rows;
		}
		virtual const int& GetColumns() const {
			return m_Model->columns;
		}
		virtual const int& GetSlices() const {
			return m_Slices;
		}
		virtual const CALSpaceBoundaryCondition& GetSpaceBoundaryCondition() const {
			return m_Model->T;
		}
		virtual const CALOptimization& GetOptimization() const {
			return m_Model->OPTIMIZATION;
		}
		virtual const int& GetNumByteSubstates() const {
			return m_Model->sizeof_pQb_array;
		}
		virtual const int& GetNumIntSubstates() const {
			return m_Model->sizeof_pQi_array;
		}
		virtual const int& GetNumRealSubstates() const {
			return m_Model->sizeof_pQr_array;
		}
		virtual const int& GetNumElementaryProcess() const {
			return m_Model->num_of_elementary_processes;
		}
		virtual const int& GetStep() const {
			return m_Run->step;
		}
		virtual const int& GetFinalStep() const {
			return m_Run->final_step;
		}

	protected:
		virtual void ProcessComputation() {
			RUN_SIMULATION_LOOP(calRunInitSimulation2D, calRunCAStep2D);
		}

	private:
		CALModel2D*			m_Model;
		CALRun2D*			m_Run;
		int					m_Slices;
	};
#pragma endregion

#pragma region Cellular_Automata_3D
	class CellularAutomata_3D : public CellularAutomata {
	public:
		CellularAutomata_3D(const CellularAutomataDesc& desc)
			: CellularAutomata(desc) {
			m_Model = desc.model3D;
			m_Run = desc.run3D;
		}

		virtual ~CellularAutomata_3D() {
			// Terminate thread.
			{	// Lock
				std::unique_lock<std::mutex> lock(m_Mutex);
				m_StopThread = true;
				m_PauseCondition.notify_all();
			}	// Unlock
			m_UpdaterThread.join();

			// Finalize OpenCAL model adn run objects.
			calRunFinalize3D(m_Run);
			calFinalize3D(m_Model);
		}

		virtual void* GetCALStruct() {
			return m_Model;
		}
		virtual eCAType GetType() const {
			return eCAType::CA_3D;
		}
		virtual const int& GetRows() const {
			return m_Model->rows;
		}
		virtual const int& GetColumns() const {
			return m_Model->columns;
		}
		virtual const int& GetSlices() const {
			return m_Model->slices;
		}
		virtual const CALSpaceBoundaryCondition& GetSpaceBoundaryCondition() const {
			return m_Model->T;
		}
		virtual const CALOptimization& GetOptimization() const {
			return m_Model->OPTIMIZATION;
		}
		virtual const int& GetNumByteSubstates() const {
			return m_Model->sizeof_pQb_array;
		}
		virtual const int& GetNumIntSubstates() const {
			return m_Model->sizeof_pQi_array;
		}
		virtual const int& GetNumRealSubstates() const {
			return m_Model->sizeof_pQr_array;
		}
		virtual const int& GetNumElementaryProcess() const {
			return m_Model->num_of_elementary_processes;
		}
		virtual const int& GetStep() const {
			return m_Run->step;
		}
		virtual const int& GetFinalStep() const {
			return m_Run->final_step;
		}

	protected:
		virtual void ProcessComputation() {
			RUN_SIMULATION_LOOP(calRunInitSimulation3D, calRunCAStep3D);
		}

	private:
		CALModel3D*			m_Model;
		CALRun3D*			m_Run;
	};
#pragma endregion

#define DEFINE_CELLULAR_AUTOMATA(CELLULAR_AUTOMATA_TYPE, desc)\
	CellularAutomata* ca = new CELLULAR_AUTOMATA_TYPE(desc);\
	Transform* transform = new Transform(ca);\
	Behaviour* behaviour = new Behaviour(ca);\
	ca->AddScript(new SimulationControllerScript(ca));\
	SceneObject::Root()->AddChild(ca);\
	ca->m_UpdaterThread = thread(\
		[ca] {\
		bool exit = false;\
				while(!exit) {\
							{\
				std::unique_lock<std::mutex> lock(ca->m_Mutex);\
				ca->m_PauseCondition.wait(lock,\
				[ca] { return !ca->m_SimulationPaused||ca->m_StopThread; });\
				exit = ca->m_StopThread||ca->m_SimulationFinished;\
							}\
			ca->ProcessComputation();\
			if(ca->m_SleepTime>0)\
				std::this_thread::sleep_for(std::chrono::milliseconds(ca->m_SleepTime));\
						}\
		});\
	return ca;

	CellularAutomata* CellularAutomata::DefineCellularAutomata(const CellularAutomataDesc& cellularAutomataDesc) {
		if(cellularAutomataDesc.model2D && cellularAutomataDesc.run2D) {
			DEFINE_CELLULAR_AUTOMATA(CellularAutomata_2D, cellularAutomataDesc);
		} else if(cellularAutomataDesc.model3D && cellularAutomataDesc.run3D) {
			DEFINE_CELLULAR_AUTOMATA(CellularAutomata_3D, cellularAutomataDesc);
		}
		return NULL;
	}
}
