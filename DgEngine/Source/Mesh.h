////////////////////////////////////////////////////////////////////////////////
// Filename: Mesh.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <map>
#include <vector>
#include <list>
#include <GL/glew.h>
#include <Importer.hpp>  // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"
#include "Texture.h"
#include "Material.h"
#include "Mathematics.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class MeshCollider;
	class Transform;
	class Animator;

	class Mesh {
		friend class MeshCollider;
		friend class Animator;
	public:
		Mesh(const eDataType& = eDataType::STATIC_DATA);
		virtual ~Mesh();

		virtual void Release();

		virtual bool LoadMesh(const string&, FlagData* = nullptr);
		virtual bool CreateMesh();
		virtual bool CreateMesh(const vector<vec3>&, 
			const vector<vec3>&, 
			const vector<vec2>&, 
			const vector<vec3>&, 
			const vector<unsigned int>&);
		virtual bool CreateBoundingBox();
		virtual bool CreateBoundingBox(vec3, vec3);

		virtual void Render(const GLuint&, const vector<shared_ptr<Material>>* = nullptr, GLenum = GL_TRIANGLES) const;
		virtual void RenderInstanced(const GLuint&, const vector<shared_ptr<Material>>* = nullptr, GLenum = GL_TRIANGLES) const;
		virtual void RenderBoundingBox(Transform*) const;
		virtual void OcclusionQuery(const GLuint&, const vector<mat4>&) const;

		const vector<shared_ptr<Material>>& GetDefaultMaterials() const;
		bool IsStatic() const;
		const eMeshType GetType() const;
		unsigned int NumBones() const;	// Animation data
		const vec3 GetMaxExtension() const;
		const vec3 GetMinExtension() const;

		virtual void SetPositions(vector<vec3>&);
		virtual vector<vec3>& GetPositions();
		virtual void UpdatePositions();
		virtual void SetNormals(vector<vec3>&);
		virtual vector<vec3>& GetNormals();
		virtual void UpdateNormals();
		virtual void SetTexCoords(vector<vec2>&);
		virtual vector<vec2>& GetTexCoords();
		virtual void UpdateTexCoords();
		virtual void SetTangents(vector<vec3>&);
		virtual vector<vec3>& GetTangents();
		virtual void UpdateTangents();
		virtual void SetMVPMatrices(vector<mat4>&);
		virtual vector<mat4>& GetMVPMatrices();
		virtual void UpdateMVPMatrices();
		virtual void SetMMatrices(vector<mat4>&);
		virtual vector<mat4>& GetMMatrices();
		virtual void UpdateMMatrices();
		virtual void SetIndices(vector<unsigned int>&);
		virtual vector<unsigned int>& GetIndices();
		virtual void UpdateIndices();
		virtual void UpdateBones();

	protected:
		virtual bool InitializeFromScene(const aiScene*, const string&);
		virtual void InitializeBuffers();
		virtual void InitializeMesh(unsigned int, const aiMesh*);
		virtual bool InitializeMaterials(const aiScene*, const string&);
		// Animation data
		void LoadBones(unsigned int, const aiMesh*);

	protected:
		eDataType						m_DataType;
		eMeshType						m_MeshType;
		Assimp::Importer				m_Importer;
		const aiScene*					m_Scene;
		GLuint							m_VAO;
		vector<GLuint>					m_Buffers;
		vector<MeshEntry>				m_Entries;
		vector<shared_ptr<Material>>	m_Materials;
		vector<vec3>					m_Positions;
		vector<vec3>					m_Normals;
		vector<vec2>					m_TexCoords;
		vector<vec3>					m_Tangents;
		vector<mat4>					m_MvpMatrices;
		vector<mat4>					m_MMatrices;
		vector<unsigned int>			m_Indices;
		Mesh*							m_BoundingBox;
		// Animation data
		map<string, unsigned int>		m_BoneMapping;
		unsigned int					m_NumBones;
		vector<BoneInfo>				m_BoneInfo;
		vector<VertexBoneData>			m_Bones;
		mat4							m_GlobalInverseTransform;
	};
}
