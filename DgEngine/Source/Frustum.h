////////////////////////////////////////////////////////////////////////////////
// Filename: Frustum.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <memory>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"
#include "Mathematics.h"
#include "Plane.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Frustum {
	public:
		static const unique_ptr<Frustum>& Instance();
		static void Release();

	public:
		virtual ~Frustum();

		void ConstructFrustum();
		bool IsPointInFrustum(const glm::vec3& point);
		bool IsSphereInFrustum(const glm::vec3& center, const float radius);
		bool IsBoxInFrustum(const glm::vec3& min, const glm::vec3& max);
		void GetFrustumPlanes(glm::vec4 fp[6]);
		
		void SetupDebugFrustum();
		void Render();

	private:
		Frustum();

	private:
		static unique_ptr<Frustum>	m_Instance;
		Plane						m_Planes[6];
		glm::vec3					m_FarPoints[4];
		glm::vec3					m_NearPoints[4];

		// Debug render
		glm::vec3					frustum_vertices[8];
		GLuint						vaoFrustumID;
		GLuint						vboFrustumVerticesID;
		GLuint						vboFrustumIndicesID;
		bool						vaoInitialized;
	};
}
