////////////////////////////////////////////////////////////////////////////////
// Filename: Common.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Glossary.h"
#include "Mathematics.h"

#pragma region Defines
#define COLOR_TEXTURE_UNIT							GL_TEXTURE0
#define COLOR_TEXTURE_UNIT_INDEX					0
#define NORMAL_TEXTURE_UNIT							GL_TEXTURE1
#define NORMAL_TEXTURE_UNIT_INDEX					1
#define INPUT_TEXTURE_UNIT							GL_TEXTURE2
#define INPUT_TEXTURE_UNIT_INDEX					2
#define POSITION_TEXTURE_UNIT						GL_TEXTURE3
#define POSITION_TEXTURE_UNIT_INDEX					3
#define AO_TEXTURE_UNIT								GL_TEXTURE4
#define AO_TEXTURE_UNIT_INDEX						4
#define SKYBOX_TEXTURE_UNIT							GL_TEXTURE5
#define SKYBOX_TEXTURE_UNIT_INDEX					5
#define SPECULAR_TEXTURE_UNIT						GL_TEXTURE6
#define SPECULAR_TEXTURE_UNIT_INDEX					6
#define AMBIENT_TEXTURE_UNIT						GL_TEXTURE7
#define AMBIENT_TEXTURE_UNIT_INDEX					7
#define EMISSIVE_TEXTURE_UNIT						GL_TEXTURE8
#define EMISSIVE_TEXTURE_UNIT_INDEX					8
#define OPACITY_TEXTURE_UNIT						GL_TEXTURE9
#define OPACITY_TEXTURE_UNIT_INDEX					9
#define SHININESS_TEXTURE_UNIT						GL_TEXTURE10
#define SHININESS_TEXTURE_UNIT_INDEX				10
#define BLOOM_TEXTURE_UNIT							GL_TEXTURE11
#define BLOOM_TEXTURE_UNIT_INDEX					11
#define SHADOW_MAP_ARRAY_TEXTURE_UNIT				GL_TEXTURE13
#define SHADOW_MAP_ARRAY_TEXTURE_UNIT_INDEX			13
#define SHADOW_CUBE_MAP_ARRAY_TEXTURE_UNIT			GL_TEXTURE14
#define SHADOW_CUBE_MAP_ARRAY_TEXTURE_UNIT_INDEX	14
//#define SHADOW_TEXTURE_UNIT							GL_TEXTURE15
//#define SHADOW_TEXTURE_UNIT_INDEX					15
//#define RANDOM_TEXTURE_UNIT             GL_TEXTURE3
//#define RANDOM_TEXTURE_UNIT_INDEX       3
//#define DISPLACEMENT_TEXTURE_UNIT       GL_TEXTURE4
//#define DISPLACEMENT_TEXTURE_UNIT_INDEX 4
//#define MOTION_TEXTURE_UNIT             GL_TEXTURE5
//#define MOTION_TEXTURE_UNIT_INDEX       5
//#define REFLECTION_TEXTURE_UNIT				GL_TEXTURE10
//#define REFLECTION_TEXTURE_UNIT_INDEX			10

// Common colors.
#define color_white glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define color_black glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
#define color_red glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
#define color_green glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
#define color_cyan glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)
#define color_blue glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)

// Common vectors.
#define vec_one glm::vec3(1.0f, 1.0f, 1.0f)
#define vec_zero glm::vec3(0.0f, 0.0f, 0.0f)
#define vec_right glm::vec3(1.0f, 0.0f, 0.0f)
#define vec_left glm::vec3(-1.0f, 0.0f, 0.0f)
#define vec_up glm::vec3(0.0f, 1.0f, 0.0f)
#define vec_down glm::vec3(0.0f, -1.0f, 0.0f)
#define vec_forward glm::vec3(0.0f, 0.0f, 1.0f)
#define vec_backward glm::vec3(0.0f, 0.0f, -1.0f)

#define quaternion_zero	glm::quat(1.0f, 0.0f, 0.0f, 0.0f)

// Mesh properties.
#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION 2
#define TANGENT_LOCATION 3
#define WVP_LOCATION 4
#define WORLD_LOCATION 8
#define BONE_ID_LOCATION 12
#define BONE_WEIGHT_LOCATION 13
#define BONE_WEIGHT_LOCATION 13

// Substate mesh properties.
#define SUBSTATE_POSITION_LOCATION 0
#define SUBSTATE_TEX_COORD_LOCATION 1
#define SUBSTATE_NORMAL_LOCATION 2
#define SUBSTATE_TANGENT_LOCATION 3
#define	SUBSTATE_MVP_LOCATION 4
#define SUBSTATE_BYTE_DATA_LOCATION 8
#define SUBSTATE_INT_DATA_LOCATION 9
#define SUBSTATE_REAL_DATA_LOCATION 10

// Skinned mesh properties.
#define NUM_BONES_PER_VERTEX 4

// Material properties.
#define INVALID_MATERIAL 0xFFFFFFFF

#define NULL_QUERY 0

#define MAX_SHADOWS 20
#pragma endregion

#pragma region Enums
namespace dg {
	enum eComponentType {
		TRANSFORM = 0,
		CAMERA,
		MESH_RENDERER,
		LIGHT,
		BEHAVIOUR,
		PHYSICS,
		ANIMATOR,
		//AUDIO,
		//PARTICLES,
		NUM_COMPONENTs
	};

	enum eProjection {
		PERSPECTIVE_PROJECTION = 0,
		ORTHOGRAPHIC_PROJECTION
	};

	enum eLightType {
		DIRECTIONAL_LIGHT = 0,
		POINT_LIGHT,
		SPOT_LIGHT,
		NUM_LIGHT_TYPE
	};

	enum eShaderData {
		VERTEX_SHADER = 0,
		TESSELLATION_CONTROL_SHADER,
		TESSELLATION_EVALUATION_SHADER,
		GEOMETRY_SHADER,
		FRAGMENT_SHADER,
		COMPUTE_SHADER,
		NUM_SHADERs
	};

	enum eVertexBuffer {
		INDEX_VB = 0,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		TANGENT_VB,
		MODEL_VIEW_PROJECTION_MATRIX_VB,
		MODEL_MATRIX_VB,
		BONE_VB,
		NUM_VBs
	};

	enum eSubstateVertexBuffer {
		SUBSTATE_INDEX_VB = 0,
		SUBSTATE_POS_VB,
		SUBSTATE_NORMAL_VB,
		SUBSTATE_TEXCOORD_VB,
		SUBSTATE_TANGENT_VB,
		SUBSTATE_MODEL_MATRIX_VB,
		SUBSTATE_DATA_BYTE,
		SUBSTATE_DATA_INT,
		SUBSTATE_DATA_REAL,
		SUBSTATE_NUM_VBs
	};

	enum eTextureType {
		NONE_TEXTURE = 0,
		DIFFUSE_TEXTURE,
		SPECULAR_TEXTURE,
		AMBIENT_TEXTURE,
		EMISSIVE_TEXTURE,
		HEIGHT_TEXTURE,
		NORMAL_TEXTURE,
		SHININESS_TEXTURE,
		OPACITY_TEXTURE,
		DISPLACEMENT_TEXTURE,
		LIGHTMAP_TEXTURE,
		REFLECTION_TEXTURE,
		UNKNOW_TEXTURE,
		NUM_TEXTUREs
	};

	enum eTextureWrapParameter {
		WRAP_REPEAT = GL_REPEAT,
		WRAP_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
		WRAP_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
		WRAP_MIRRORED_REPEAT = GL_MIRRORED_REPEAT
	};

	enum eTextureFilterParameter {
		FILTER_NEAREST = GL_NEAREST,
		FILTER_LINEAR = GL_LINEAR,
		FILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		FILTER_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		FILTER_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		FILTER_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
	};

	enum eFogType {
		LINEAR_FOG = 0,
		EXPONENTIAL_FOG,
		EXPONENTIAL_SQUARED_FOG,
		ALTITUDE_FOG,
		NUM_FOGs
	};

	enum eBlendFunc {
		BF_ZERO = GL_ZERO,
		BF_ONE = GL_ONE,
		BF_SRC_COLOR = GL_SRC_COLOR,
		BF_ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
		BF_DST_COLOR = GL_DST_COLOR,
		BF_ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
		BF_SRC_ALPHA = GL_SRC_ALPHA,
		BF_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
		BF_DST_ALPHA = GL_DST_ALPHA,
		BF_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
		BF_CONSTANT_COLOR = GL_CONSTANT_COLOR,
		BF_ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
		BF_CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
		BF_ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
		BF_SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE,
		BF_SRC1_COLOR = GL_SRC1_COLOR,
		BF_ONE_MINUS_SRC1_COLOR = GL_ONE_MINUS_SRC1_COLOR,
		BF_SRC1_ALPHA = GL_SRC1_ALPHA,
		BF_ONE_MINUS_SRC1_ALPHA = GL_ONE_MINUS_SRC1_ALPHA
	};

	enum eBlendEquation {
		BE_FUNC_ADD = GL_FUNC_ADD,
		BE_FUNC_SUBTRACT = GL_FUNC_SUBTRACT,
		BE_FUNC_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT,
		BE_MIN = GL_MIN,
		BE_MAX = GL_MAX		
	};

	enum eMeshType {
		BASE_MESH = 0,
		SKINNED_MESH,
		NUM_MESHs
	};

	enum eGizmo {
		AXIS_GIZMO = 0,
		DIRECTIONAL_LIGHT_GIZMO,
		POINT_LIGHT_GIZMO,
		SPOT_LIGHT_GIZMO,
		CUBE_GIZMO,
		NUM_GIZMOs
	};

	enum eCAType {
		CA_2D = 0,
		CA_3D,
		NUM_CAs
	};

	enum eDataType {
		STATIC_DATA = 0,
		DYNAMIC_DATA
	};

	enum eDrawMode {
		DRAWMODE_NONE = 0,
		DRAWMODE_2D_FLAT,
		DRAWMODE_2D_SURFACE,
		DRAWMODE_3D_CUBE,
		DRAWMODE_3D_VOLUME,
		DRAWMODE_NUMs
	};
	
	enum eShadowType {
		NO_SHADOW = 0,
		HARD_SHADOW,
		SOFT_SHADOW
	};

	enum eShadowMapSize {
		TINY_SHADOW_MAP = 256,
		SMALL_SHADOW_MAP = 512,
		MEDIUM_SHADOW_MAP = 1024,
		BIG_SHADOW_MAP = 2048,
		HUGE_SHADOW_MAP = 4096
	};

	enum eInfoBarOrientation {
		INFO_BAR_HORIZONTAL = 0,
		INFO_BAR_VERTICAL
	};

	enum eRenderMode {
		OPAQUE_RENDER = 0,
		TRANSPARENT_RENDER
	};

	enum eKey {
		KEY_SPACE = 32,
		KEY_APOSTROPHE = 39,
		KEY_PLUS = 43,
		KEY_COMMA = 44,
		KEY_MINUS = 45,
		KEY_PERIOD = 46,
		KEY_SLASH = 47,
		KEY_0 = 48,
		KEY_1 = 49,
		KEY_2 = 50,
		KEY_3 = 51,
		KEY_4 = 52,
		KEY_5 = 53,
		KEY_6 = 54,
		KEY_7 = 55,
		KEY_8 = 56,
		KEY_9 = 57,
		KEY_SEMICOLON = 58,
		KEY_EQUAL = 61,
		KEY_A = 65,
		KEY_B = 66,
		KEY_C = 67,
		KEY_D = 68,
		KEY_E = 69,
		KEY_F = 70,
		KEY_G = 71,
		KEY_H = 72,
		KEY_I = 73,
		KEY_J = 74,
		KEY_K = 75,
		KEY_L = 76,
		KEY_M = 77,
		KEY_N = 78,
		KEY_O = 79,
		KEY_P = 80,
		KEY_Q = 81,
		KEY_R = 82,
		KEY_S = 83,
		KEY_T = 84,
		KEY_U = 85,
		KEY_V = 86,
		KEY_W = 87,
		KEY_X = 88,
		KEY_Y = 89,
		KEY_Z = 90,
		KEY_LEFT_BRACKET = 91,
		KEY_BACKSLASH = 92,
		KEY_RIGHT_BRACKET = 93,
		KEY_a = 97,
		KEY_b = 98,
		KEY_c = 99,
		KEY_d = 100,
		KEY_e = 101,
		KEY_f = 102,
		KEY_g = 103,
		KEY_h = 104,
		KEY_i = 105,
		KEY_j = 106,
		KEY_k = 107,
		KEY_l = 108,
		KEY_m = 109,
		KEY_n = 110,
		KEY_o = 111,
		KEY_p = 112,
		KEY_q = 113,
		KEY_r = 114,
		KEY_s = 115,
		KEY_t = 116,
		KEY_u = 117,
		KEY_v = 118,
		KEY_w = 119,
		KEY_x = 120,
		KEY_y = 121,
		KEY_z = 122,
		KEY_ESCAPE,
		KEY_ENTER,
		KEY_TAB,
		KEY_BACKSPACE,
		KEY_INSERT,
		KEY_DELETE,
		KEY_RIGHT,
		KEY_LEFT,
		KEY_DOWN,
		KEY_UP,
		KEY_PAGE_UP,
		KEY_PAGE_DOWN,
		KEY_HOME,
		KEY_END,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_UNDEFINED = 999,
	};

	enum eKeyState {
		KEY_STATE_RELEASE = 0,
		KEY_STATE_PRESS,
		KEY_STATE_HOLD,
		NUM_KEY_STATEs,
		KEY_STATE_UNDEFINED = 999
	};

	enum eMouseButton {
		MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_MIDDLE,
		MOUSE_BUTTON_RIGHT,
		NUM_MOUSE_BUTTONs,
		MOUSE_UNDEFINED = 999
	};
}
#pragma endregion

#pragma region Structs
namespace dg {
	struct Vertex {
		glm::vec3 m_pos;
		glm::vec2 m_tex;
		glm::vec3 m_normal;
		glm::vec3 m_tangent;

		Vertex() {}

		Vertex(const glm::vec3& pos,
			const glm::vec2& tex,
			const glm::vec3& normal,
			const glm::vec3& tangent) {
			m_pos = pos;
			m_tex = tex;
			m_normal = normal;
			m_tangent = tangent;
		}
	};

	struct MeshEntry {
		MeshEntry() {
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	struct TextureDesc {
		std::string	filePath;
		GLenum		textureTarget;
		bool		mipMap;
		GLint		wrapS;
		GLint		wrapT;
		GLint		magFilter;
		GLint		minFilter;

		TextureDesc() {
			filePath = "";
			textureTarget = GL_TEXTURE_2D;
			mipMap = false;
			wrapS = eTextureWrapParameter::WRAP_REPEAT;
			wrapT = eTextureWrapParameter::WRAP_REPEAT;
			magFilter = eTextureFilterParameter::FILTER_NEAREST;
			minFilter = eTextureFilterParameter::FILTER_LINEAR_MIPMAP_LINEAR;
		}
	};

	struct MaterialDesc {
		std::string shaderName;
		std::string materialName;
		bool		wireframe;
		float		opacity;
		float		bumpScaling;
		float		shininess;
		float		shininessStrength;
		float		reflectivity;
		float		refraction;
		vec4		colorDiffuse;
		vec4		colorAmbient;
		vec4		colorSpecular;
		vec4		colorEmissive;
		TextureDesc textureDesc[eTextureType::NUM_TEXTUREs];
		vec2		tc_scale;
		vec2		tc_offset;

		MaterialDesc() {
			shaderName = SHADER_BASE_MESH;
			materialName = "";
			wireframe = false;
			opacity = 1.0f;
			bumpScaling = 1.0f;
			shininess = 32.0f;
			shininessStrength = 1.0f;
			reflectivity = 0.0f;
			refraction = 1.0f;
			colorDiffuse = color_white;
			colorAmbient = color_white;
			colorSpecular = color_white;
			colorEmissive = color_white;
			tc_scale = vec2(1.0f, 1.0f);
			tc_offset = vec2(0.0f, 0.0f);
		}
	};

	struct VertexBoneData {
		unsigned int IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];

		VertexBoneData() {
			reset();
		};

		void reset() {
			memset(IDs, 0, sizeof(IDs));
			memset(Weights, 0, sizeof(Weights));
		}

		void addBoneData(unsigned int BoneID, float Weight) {
			for(unsigned int i = 0; i<(sizeof(IDs)/sizeof(IDs[0])); i++) {
				if(Weights[i]==0.0) {
					IDs[i] = BoneID;
					Weights[i] = Weight;
					return;
				}
			}

			// should never get here - more bones than we have space for
			//assert(0);
		}

	};

	struct BoneInfo {
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;

		BoneInfo()
			: BoneOffset(0.0f),
			FinalTransformation(0.0f) {
			//ZERO_MEM(BoneOffset);
			//ZERO_MEM(FinalTransformation);
		}
	};

	struct FlagData {
		bool CalcTangentSpace;
		bool FindInstances;
		bool FindInvalidData;
		bool FlipUVs;
		bool GenSmoothNormals;
		bool GenUVCoords;
		bool ImproveCacheLocality;
		bool JoinIdenticalVertices;
		bool LimitBoneWeights;
		bool OptimizeMeshes;
		bool OptimizeGraph;
		bool PreTransformVertices;
		bool RemoveRedundantMaterials;
		bool SplitByBoneCount;
		bool Triangulate;
		bool TransformUVCoords;
		bool ValidateDataStructure;

		FlagData() {
			CalcTangentSpace = true;
			FindInstances = true;
			FindInvalidData = true;
			FlipUVs = true;
			GenSmoothNormals = true;
			GenUVCoords = true;
			ImproveCacheLocality = true;
			JoinIdenticalVertices = true;
			LimitBoneWeights = true;
			OptimizeMeshes = true;
			OptimizeGraph = true;
			PreTransformVertices = true;
			RemoveRedundantMaterials = true;
			SplitByBoneCount = true;
			Triangulate = true;
			TransformUVCoords = true;
			ValidateDataStructure = true;
		};
	};
}
#pragma endregion
