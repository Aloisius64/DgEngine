////////////////////////////////////////////////////////////////////////////////
// Filename: SubstateRenderer.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "CellularAutomata.h"
#include "InfoBarRenderer.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "OpenCALFiles.h"
#include "Thread.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	struct SubstateRendererDesc {
		string				substateName;
		string				parentObject;
		CellularAutomata*	cellularAutomata;
		CALSubstate2Db*		substate2Db;
		CALSubstate2Di*		substate2Di;
		CALSubstate2Dr*		substate2Dr;
		CALSubstate3Db*		substate3Db;
		CALSubstate3Di*		substate3Di;
		CALSubstate3Dr*		substate3Dr;
		eDataType			dataType;
		MaterialDesc*		materialDesc;
		CALbyte				noByteData;
		CALint				noIntData;
		CALreal				noRealData;
		vec4				colorMinValue;
		vec4				colorMaxValue;
		bool				infoBarDraw;
		eInfoBarOrientation infoBarOrientation;
		int					infoBarXPosition;
		int					infoBarYPosition;
		int					infoBarWidth;
		int					infoBarHeight;

		SubstateRendererDesc() {
			substateName = "EmptyName";
			parentObject = "";
			cellularAutomata = nullptr;
			substate2Db = nullptr;
			substate2Di = nullptr;
			substate2Dr = nullptr;
			substate3Db = nullptr;
			substate3Di = nullptr;
			substate3Dr = nullptr;
			dataType = eDataType::STATIC_DATA;
			materialDesc = nullptr;
			noByteData = CAL_FALSE;
			noIntData = 0;
			noRealData = 0.0;
			colorMinValue = color_black;
			colorMaxValue = color_white;
			infoBarDraw = false;
			infoBarOrientation = eInfoBarOrientation::INFO_BAR_HORIZONTAL;
			infoBarXPosition = 0;
			infoBarYPosition = 0;
			infoBarWidth = 0;
			infoBarHeight = 0;
		}
	};

	class SubstateRenderer : public MeshRenderer {
		friend class UI;

	public:
		static inline eComponentType ComponentType() {
			return eComponentType::MESH_RENDERER;
		}

	public:
		static SubstateRenderer* DefineSubstate(const SubstateRendererDesc&);

	protected:
		static SubstateRenderer* DefineSubstate2D_Flat(const SubstateRendererDesc&);
		static SubstateRenderer* DefineSubstate2D_Surface(const SubstateRendererDesc&);
		static SubstateRenderer* DefineSubstate3D_Cube(const SubstateRendererDesc&);
		static SubstateRenderer* DefineSubstate3D_Volume(const SubstateRendererDesc&);

	public:
		virtual ~SubstateRenderer();
		virtual void DrawUI(TwBar*);

	protected:
		SubstateRenderer(SceneObject*, const SubstateRendererDesc&);
		virtual void Initialize();

	protected:
		// Static data or dynamic data.
		eDataType			m_DataType;
		// OpenCAL data.
		CellularAutomata*	m_CellularAutomata;
		CALbyte				m_ByteMaxValue;
		CALbyte				m_ByteMinValue;
		CALbyte				m_ByteNoDataValue;
		CALint				m_IntMaxValue;
		CALint				m_IntMinValue;
		CALint				m_IntNoDataValue;
		CALreal				m_RealMaxValue;
		CALreal				m_RealMinValue;
		CALreal				m_RealNoDataValue;
		int					m_CodeSubstate;
		vec4				m_ColorMinValue;
		vec4				m_ColorMaxValue;
		// OpenGL data.
		int					m_LastUpdateStep;
		// Infobar data.
		bool				m_InfoBarDraw;
		eInfoBarOrientation m_InfoBarOrientation;
		int					m_InfoBarXPosition;
		int					m_InfoBarYPosition;
		string				m_InfoBarMinValue;
		string				m_InfoBarMaxValue;
		int					m_InfoBarWidth;
		int					m_InfoBarHeight;
		// Vertex data.
		vec3				m_Translation;
		vec3				m_Scaling;
	};

#pragma region DEFINES
#define CODE_SUBSTATE_BYTE	0
#define CODE_SUBSTATE_INT	1
#define CODE_SUBSTATE_REAL	2

#define DEFINE_SUBSTATE_RENDERER(SUBSTATE_TYPE)\
	SceneObject* sceneObject = new SceneObject(desc.substateName, "Substate");\
	Transform* transform = new Transform(sceneObject);\
	SubstateRenderer* substateRenderer = new SUBSTATE_TYPE(sceneObject, desc);\
	Behaviour* behaviour = new Behaviour(sceneObject);\
	sceneObject->SetRenderLayer(desc.cellularAutomata->GetRenderLayer());\
	SceneObject* parent = SceneObject::FindSceneObjectById(desc.parentObject);\
	if(!parent) { parent = desc.cellularAutomata; }\
	parent->AddChild(sceneObject);\
	vector<shared_ptr<Material>> materials;\
	if(!desc.materialDesc) {\
		MaterialDesc defaultMaterial;\
		defaultMaterial.materialName = "DefaultMaterial";\
		materials.push_back(DataManager::TryGetMaterial(defaultMaterial));\
												} else {\
		materials.push_back(DataManager::TryGetMaterial(*desc.materialDesc));\
												}\
	substateRenderer->SetMaterials(materials);\
	substateRenderer->Initialize();\
	return substateRenderer;

#define COMPUTE_EXTREMES_2D\
	if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {\
		m_ByteMinValue = calReductionComputeMin2Db(m_Model, m_ByteSubstate);\
		m_ByteMaxValue = calReductionComputeMax2Db(m_Model, m_ByteSubstate);\
		m_InfoBarMinValue = std::to_string(m_ByteMinValue);\
		m_InfoBarMaxValue = std::to_string(m_ByteMaxValue);\
						} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {\
		m_IntMinValue = calReductionComputeMin2Di(m_Model, m_IntSubstate);\
		m_IntMaxValue = calReductionComputeMax2Di(m_Model, m_IntSubstate);\
		m_InfoBarMinValue = std::to_string(m_IntMinValue);\
		m_InfoBarMaxValue = std::to_string(m_IntMaxValue);\
		} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {\
		m_RealMinValue = calReductionComputeMin2Dr(m_Model, m_RealSubstate);\
		m_RealMaxValue = calReductionComputeMax2Dr(m_Model, m_RealSubstate);\
		m_InfoBarMinValue = std::to_string(m_RealMinValue);\
		m_InfoBarMaxValue = std::to_string(m_RealMaxValue);\
		}

#define COMPUTE_EXTREMES_3D\
	if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {\
		m_ByteMinValue = calReductionComputeMin3Db(m_Model, m_ByteSubstate);\
		m_ByteMaxValue = calReductionComputeMax3Db(m_Model, m_ByteSubstate);\
		m_InfoBarMinValue = std::to_string(m_ByteMinValue);\
		m_InfoBarMaxValue = std::to_string(m_ByteMaxValue);\
								} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {\
		m_IntMinValue = calReductionComputeMin3Di(m_Model, m_IntSubstate);\
		m_IntMaxValue = calReductionComputeMax3Di(m_Model, m_IntSubstate);\
		m_InfoBarMinValue = std::to_string(m_IntMinValue);\
		m_InfoBarMaxValue = std::to_string(m_IntMaxValue);\
			} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {\
		m_RealMinValue = calReductionComputeMin3Dr(m_Model, m_RealSubstate);\
		m_RealMaxValue = calReductionComputeMax3Dr(m_Model, m_RealSubstate);\
		m_InfoBarMinValue = std::to_string(m_RealMinValue);\
		m_InfoBarMaxValue = std::to_string(m_RealMaxValue);\
			}

#define SET_SUBSTATE_2D_DATA\
	m_Model = (CALModel2D*) desc.cellularAutomata->GetCALStruct();\
	if(desc.substate2Db!=nullptr)\
		m_CodeSubstate = CODE_SUBSTATE_BYTE;\
	else if(desc.substate2Di!=nullptr)\
		m_CodeSubstate = CODE_SUBSTATE_INT;\
	else if(desc.substate2Dr!=nullptr)\
		m_CodeSubstate = CODE_SUBSTATE_REAL;\
	if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {\
		m_ByteSubstate = desc.substate2Db;\
		m_ByteNoDataValue = desc.noByteData;\
		m_ByteMinValue = CAL_TRUE;\
		m_ByteMaxValue = CAL_FALSE;\
	} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {\
		m_IntSubstate = desc.substate2Di;\
		m_IntNoDataValue = desc.noByteData;\
		m_IntMinValue = CAL_TRUE;\
		m_IntMaxValue = CAL_FALSE;\
	} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {\
		m_RealSubstate = desc.substate2Dr;\
		m_RealNoDataValue = desc.noByteData;\
		m_RealMinValue = CAL_TRUE;\
		m_RealMaxValue = CAL_FALSE;\
	}\
	m_ColorMinValue = desc.colorMinValue;\
	m_ColorMaxValue = desc.colorMaxValue;

#define SET_SUBSTATE_3D_DATA\
	m_Model = (CALModel3D*) desc.cellularAutomata->GetCALStruct();\
	if(desc.substate3Db!=nullptr)\
		m_CodeSubstate = CODE_SUBSTATE_BYTE;\
	else if(desc.substate3Di!=nullptr)\
		m_CodeSubstate = CODE_SUBSTATE_INT;\
	else if(desc.substate3Dr!=nullptr)\
		m_CodeSubstate = CODE_SUBSTATE_REAL;\
	if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {\
		m_ByteSubstate = desc.substate3Db;\
		m_ByteNoDataValue = desc.noByteData;\
		m_ByteMinValue = CAL_TRUE;\
		m_ByteMaxValue = CAL_FALSE;\
	} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {\
		m_IntSubstate = desc.substate3Di;\
		m_IntNoDataValue = desc.noByteData;\
		m_IntMinValue = CAL_TRUE;\
		m_IntMaxValue = CAL_FALSE;\
	} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {\
		m_RealSubstate = desc.substate3Dr;\
		m_RealNoDataValue = desc.noByteData;\
		m_RealMinValue = CAL_TRUE;\
		m_RealMaxValue = CAL_FALSE;\
	}\
	m_ColorMinValue = desc.colorMinValue;\
	m_ColorMaxValue = desc.colorMaxValue;

#define SET_SHADER_SUBTATE_PARAMETERS(shader)\
	if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {\
		shader->SetInt("SubstateType", m_CodeSubstate);\
		shader->SetInt("NoDataByte", m_ByteNoDataValue);\
		shader->SetInt("MinValueByte", m_ByteMinValue);\
		shader->SetInt("MaxValueByte", m_ByteMaxValue);\
	} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {\
		shader->SetInt("SubstateType", m_CodeSubstate);\
		shader->SetInt("NoDataInt", m_IntNoDataValue);\
		shader->SetInt("MinValueInt", m_IntMinValue);\
		shader->SetInt("MaxValueInt", m_IntMaxValue);\
	} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {\
		shader->SetInt("SubstateType", m_CodeSubstate);\
		shader->SetDouble("NoDataReal", m_RealNoDataValue);\
		shader->SetDouble("MinValueReal", m_RealMinValue);\
		shader->SetDouble("MaxValueReal", m_RealMaxValue);\
	}\
	shader->SetFloatVector4("MinColor", m_ColorMinValue);\
	shader->SetFloatVector4("MaxColor", m_ColorMaxValue);

#define SET_NULLPTR_TO_OPENCAL_DATA\
	m_Model = nullptr;\
	m_ByteSubstate = nullptr;\
	m_IntSubstate = nullptr;\
	m_RealSubstate = nullptr;

#define DRAW_INFOBAR\
	if(m_InfoBarDraw)\
		InfoBarRenderer::Instance()->RenderInfoBar(GetSceneObject()->Id(), m_InfoBarOrientation, m_InfoBarXPosition, m_InfoBarYPosition, m_InfoBarWidth, m_InfoBarHeight, m_InfoBarMinValue, m_InfoBarMaxValue, m_ColorMinValue, m_ColorMaxValue);
#pragma endregion
}
