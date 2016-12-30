////////////////////////////////////////////////////////////////////////////////
// Filename: Gizmo.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <memory>
#include <string>
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Mesh.h"
#include "Common.h"
#include "Mathematics.h"
#include "Physics\PhysX.h"
#include "Shader.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace physx;


namespace dg {
	class Transform;

	class Gizmo {
	public:
		static const unique_ptr<Gizmo>& Instance();
		static void Release();
		static void SetGridSize(int);
		static void DrawGrid(const vec3& = vec_zero, const quat& = quaternion_zero, const vec3& = vec_one);
		static void DrawGizmo(eGizmo, const vec3& = vec_zero, const quat& = quaternion_zero, const vec3& = vec_one);
		static void DrawGizmo(eGizmo, Transform*, const vec3& = vec_one);

	public:
		virtual ~Gizmo();

	private:
		Gizmo();

		void InitializeGrid();

	private:
		static unique_ptr<Gizmo>	m_Instance;
		shared_ptr<Shader>			m_Shader;
		int							m_GridSize;
		vector<shared_ptr<Mesh>>	m_Meshes;
		vector<string>				m_MeshesPath;
		// Grid gizmo.
		shared_ptr<Mesh>			m_Grid_1;
		shared_ptr<Mesh>			m_Grid_2;
	};
}
