////////////////////////////////////////////////////////////////////////////////
// Filename: SubstateMesh.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Mesh.h"
#include "OpenCALFiles.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class SubstateMesh : public Mesh {
	public:
		SubstateMesh(const eDataType& = eDataType::STATIC_DATA, const bool& = true);
		virtual ~SubstateMesh();
		
		virtual void UpdatePositions();
		virtual void UpdateNormals();
		virtual void UpdateTexCoords();
		virtual void UpdateTangents();
		virtual void SetMVPMatrices(vector<mat4>&);
		virtual void UpdateMVPMatrices();
		virtual void SetMMatrices(vector<mat4>&);
		virtual vector<mat4>& GetMMatrices();
		virtual void UpdateMMatrices();
		virtual void UpdateIndices();
		virtual void UpdateBones();
		void UpdateByteData(CALbyte*, unsigned int);
		void UpdateIntData(CALint*, unsigned int);
		void UpdateRealData(CALreal*, unsigned int);

	protected:
		virtual void InitializeBuffers();

	private:
		bool	m_InstancedData;
	};
}
