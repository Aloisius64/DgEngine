////////////////////////////////////////////////////////////////////////////////
// Filename: CellularAutomata.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>
#include <time.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"
#include "OpenCALFiles.h"
#include "SceneObject.h"
#include "Thread.h"
#include "UI.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	struct CellularAutomataDesc {
		CALModel2D*		model2D;
		CALRun2D*		run2D;
		CALModel3D*		model3D;
		CALRun3D*		run3D;
		string			id;
		string			tag;
		unsigned int	renderLayer;
		float			cellSize;
		int				renderStep;
		eDrawMode		drawMode;

		CellularAutomataDesc() {
			model2D = nullptr;
			run2D = nullptr;
			model3D = nullptr;
			run3D = nullptr;
			id = "Unknow_ID";
			tag = "Unknow_TAG";
			renderLayer = 0;
			cellSize = 1.0f;
			renderStep = 1;
			drawMode = eDrawMode::DRAWMODE_NONE;
		}
	};

	class CellularAutomata : public SceneObject {
	public:
		static CellularAutomata* DefineCellularAutomata(const CellularAutomataDesc&);

	public:
		virtual ~CellularAutomata();

		virtual void* GetCALStruct() = 0;
		virtual eCAType GetType() const = 0;
		virtual const int& GetRows() const = 0;
		virtual const int& GetColumns() const = 0;
		virtual const int& GetSlices() const = 0;
		virtual const CALSpaceBoundaryCondition& GetSpaceBoundaryCondition() const = 0;
		virtual const CALOptimization& GetOptimization() const = 0;
		virtual const int& GetNumByteSubstates() const = 0;
		virtual const int& GetNumIntSubstates() const = 0;
		virtual const int& GetNumRealSubstates() const = 0;
		virtual const int& GetNumElementaryProcess() const = 0;
		virtual const int& GetStep() const = 0;
		virtual const int& GetFinalStep() const = 0;
		virtual const float& GetCellSize() const;
		virtual void DrawUI(TwBar*);
		
		void RunSimulation(const bool&);
		void PlaySimulation();
		void StopSimulation();
		bool IsSimulationPaused() const;
		time_t GetElapsedTime() const;
		void SetRenderStep(const int&);
		const int& GetRenderStep() const;
		void SetSleepTime(const int&);
		const int& GetSleepTime() const;
		const eDrawMode& GetDrawMode() const;

	protected:
		CellularAutomata(const CellularAutomataDesc&);
		
		virtual void ProcessComputation() = 0;

	protected:
		thread				m_UpdaterThread;
		mutex				m_Mutex;
		condition_variable	m_PauseCondition;
		bool				m_StopThread;
		bool				m_SimulationPaused;
		bool				m_SimulationFinished;
		float				m_CellSize;
		CALbyte				m_FirstRun;
		time_t				m_StartTime;
		time_t				m_EndTime;
		CALint				m_Step;
		CALint				m_FinalStep;
		int					m_RenderStep;
		int					m_SleepTime;
		eDrawMode			m_DrawMode;
	};
}
