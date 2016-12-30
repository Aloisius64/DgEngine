////////////////////////////////////////////////////////////////////////////////
// Filename: Frustum.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Frustum.h"
#include "Camera.h"
#include "Environment.h"
#include "Gizmo.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


//#define DRAW_DEBUG_FRUSTUM

unique_ptr<Frustum> Frustum::m_Instance = nullptr;

const unique_ptr<Frustum>& Frustum::Instance() {
	if (!m_Instance) {
		m_Instance = unique_ptr<Frustum>(new Frustum());
	}

	return m_Instance;
}

void Frustum::Release() {
	if (m_Instance == nullptr)
		return;

	m_Instance = nullptr;
}

Frustum::Frustum() {
	vaoInitialized = false;
	vaoFrustumID = 0;
	vboFrustumVerticesID = 0;
	vboFrustumIndicesID = 0;
}

Frustum::~Frustum() {
	DELETE_BUFFER(vboFrustumVerticesID);
	DELETE_BUFFER(vboFrustumIndicesID);
	DELETE_VERTEX_ARRAY(vaoFrustumID);
}

void Frustum::ConstructFrustum() {
	Camera* camera = Camera::ActiveCamera();
	glm::vec3 position = camera->GetPosition();
	glm::vec3 look = camera->GetTarget();
	glm::vec3 up = camera->GetUp();
	glm::vec3 right = camera->GetRight();

	glm::vec3 cN = position + look*camera->GetZNear();
	glm::vec3 cF = position + look*camera->GetZFar();

	float Hnear = 2.0f * tan(glm::radians(camera->GetFovy() / 2.0f)) * camera->GetZFar();
	float Wnear = Hnear * camera->m_Aspect;
	float Hfar = 2.0f * tan(glm::radians(camera->GetFovy() / 2.0f)) * camera->GetZNear();
	float Wfar = Hfar * camera->m_Aspect;
	float hHnear = Hnear / 2.0f;
	float hWnear = Wnear / 2.0f;
	float hHfar = Hfar / 2.0f;
	float hWfar = Wfar / 2.0f;

	m_FarPoints[0] = cN + up*hHfar - right*hWfar;
	m_FarPoints[1] = cN - up*hHfar - right*hWfar;
	m_FarPoints[2] = cN - up*hHfar + right*hWfar;
	m_FarPoints[3] = cN + up*hHfar + right*hWfar;

	m_NearPoints[0] = cF + up*hHnear - right*hWnear;
	m_NearPoints[1] = cF - up*hHnear - right*hWnear;
	m_NearPoints[2] = cF - up*hHnear + right*hWnear;
	m_NearPoints[3] = cF + up*hHnear + right*hWnear;

	m_Planes[0] = Plane::FromPoints(m_NearPoints[3], m_NearPoints[0], m_FarPoints[0]);
	m_Planes[1] = Plane::FromPoints(m_NearPoints[1], m_NearPoints[2], m_FarPoints[2]);
	m_Planes[2] = Plane::FromPoints(m_NearPoints[0], m_NearPoints[1], m_FarPoints[1]);
	m_Planes[3] = Plane::FromPoints(m_NearPoints[2], m_NearPoints[3], m_FarPoints[2]);
	m_Planes[4] = Plane::FromPoints(m_NearPoints[0], m_NearPoints[3], m_NearPoints[2]);
	m_Planes[5] = Plane::FromPoints(m_FarPoints[3], m_FarPoints[0], m_FarPoints[1]);

#if defined(_DEBUG) && defined(DRAW_DEBUG_FRUSTUM)
	if (!vaoInitialized) {
		SetupDebugFrustum();
	}

	//store the view frustum vertices
	frustum_vertices[0] = m_FarPoints[0];
	frustum_vertices[1] = m_FarPoints[1];
	frustum_vertices[2] = m_FarPoints[2];
	frustum_vertices[3] = m_FarPoints[3];

	frustum_vertices[4] = m_NearPoints[0];
	frustum_vertices[5] = m_NearPoints[1];
	frustum_vertices[6] = m_NearPoints[2];
	frustum_vertices[7] = m_NearPoints[3];

	//update the frustum vertices on the GPU
	glBindVertexArray(vaoFrustumID);
	glBindBuffer(GL_ARRAY_BUFFER, vboFrustumVerticesID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(frustum_vertices), &frustum_vertices[0]);
	glBindVertexArray(0);
#endif // DEBUG
}

bool Frustum::IsPointInFrustum(const glm::vec3& point) {
	for (int i = 0; i < 6; i++) {
		if (m_Planes[i].GetDistance(point) < 0)
			return false;
	}
	return true;
}

bool Frustum::IsSphereInFrustum(const glm::vec3& center, const float radius) {
	for (int i = 0; i < 6; i++) {
		float d = m_Planes[i].GetDistance(center);
		if (d < -radius)
			return false;
	}
	return true;
}

bool Frustum::IsBoxInFrustum(const glm::vec3& min, const glm::vec3& max) {
	for (int i = 0; i < 6; i++) {
		glm::vec3 p = min, n = max;
		glm::vec3 N = m_Planes[i].N;
		if (N.x >= 0) {
			p.x = max.x;
			n.x = min.x;
		}
		if (N.y >= 0) {
			p.y = max.y;
			n.y = min.y;
		}
		if (N.z >= 0) {
			p.z = max.z;
			n.z = min.z;
		}

		if (m_Planes[i].GetDistance(p) < 0) {
			return false;
		}
	}
	return true;
}

void Frustum::GetFrustumPlanes(glm::vec4 fp[6]) {
	for (int i = 0; i < 6; i++)
		fp[i] = glm::vec4(m_Planes[i].N, m_Planes[i].d);
}

void Frustum::SetupDebugFrustum() {
	//setup Frustum geometry
	glGenVertexArrays(1, &vaoFrustumID);
	glGenBuffers(1, &vboFrustumVerticesID);
	glGenBuffers(1, &vboFrustumIndicesID);

	//store the view frustum vertices
	frustum_vertices[0] = m_FarPoints[0];
	frustum_vertices[1] = m_FarPoints[1];
	frustum_vertices[2] = m_FarPoints[2];
	frustum_vertices[3] = m_FarPoints[3];

	frustum_vertices[4] = m_NearPoints[0];
	frustum_vertices[5] = m_NearPoints[1];
	frustum_vertices[6] = m_NearPoints[2];
	frustum_vertices[7] = m_NearPoints[3];

	GLushort frustum_indices[36] = {
		0,4,3,3,4,7, //top
		6,5,1,6,1,2, //bottom
		0,1,4,4,1,5, //left
		7,6,3,3,6,2, //right
		4,5,6,4,6,7, //near
		3,2,0,0,2,1, //far
	};
	glBindVertexArray(vaoFrustumID);

	glBindBuffer(GL_ARRAY_BUFFER, vboFrustumVerticesID);
	//pass frustum vertices to buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(frustum_vertices), &frustum_vertices[0], GL_DYNAMIC_DRAW);

	dg::CheckGLError();

	//enable vertex attribute array for position
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	dg::CheckGLError();

	//pass indices to element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFrustumIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frustum_indices), &frustum_indices[0], GL_STATIC_DRAW);

	dg::CheckGLError();

	glBindVertexArray(0);
}

void Frustum::Render() {
#if defined(_DEBUG) && defined(DRAW_DEBUG_FRUSTUM)	
	shared_ptr<Shader> shader = DataManager::GetShader(SHADER_GIZMO);

	shader->SetActive();
	shader->SetInt("AlbedoMap", COLOR_TEXTURE_UNIT_INDEX);
	shader->SetInt("AlbedoMap_bool", false);
	shader->SetInt("ActiveOIT", Environment::Instance()->IsOITActive());

	Transform transform(NULL);
	transform.SetScaling(vec_one*0.25f);

	shader->SetFloatMatrix4("MVPMatrix", transform.GetWorldViewProjectionMatrix());

	shader->SetFloatVector4("Color", glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));

	//bind frustum vertex array object
	glBindVertexArray(vaoFrustumID);

	//enable alpha blending
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	////set polygon mode as fill
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//reset the line drawing mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//draw triangles
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	//disable blending
	//glDisable(GL_BLEND);
#endif
}
