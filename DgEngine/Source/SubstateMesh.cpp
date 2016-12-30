////////////////////////////////////////////////////////////////////////////////
// Filename: SubstateMesh.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SubstateMesh.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


SubstateMesh::SubstateMesh(const eDataType& dataType, const bool& instancedData)
	: Mesh(dataType) {
	m_InstancedData = instancedData;
}

SubstateMesh::~SubstateMesh() {}

void SubstateMesh::InitializeBuffers() {
	// Generate and populate the buffers with vertex attributes and the indices.
	m_Buffers.resize(eSubstateVertexBuffer::SUBSTATE_NUM_VBs);
	for(unsigned int i = 0; i<eSubstateVertexBuffer::SUBSTATE_NUM_VBs; i++) {
		glGenBuffers(1, &m_Buffers[i]);
	}

	GLenum bufferType = m_DataType==eDataType::STATIC_DATA ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;

	// Positions.
	if(m_Positions.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_POS_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_Positions.size(), glm::value_ptr(m_Positions.front()), bufferType);
		glEnableVertexAttribArray(SUBSTATE_POSITION_LOCATION);
		glVertexAttribPointer(SUBSTATE_POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Texture coordinates.
	if(m_TexCoords.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * m_TexCoords.size(), glm::value_ptr(m_TexCoords.front()), bufferType);
		glEnableVertexAttribArray(SUBSTATE_TEX_COORD_LOCATION);
		glVertexAttribPointer(SUBSTATE_TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Normals.
	if(m_Normals.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_Normals.size(), glm::value_ptr(m_Normals.front()), bufferType);
		glEnableVertexAttribArray(SUBSTATE_NORMAL_LOCATION);
		glVertexAttribPointer(SUBSTATE_NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Tangents.
	if(m_Tangents.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_TANGENT_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_Tangents.size(), glm::value_ptr(m_Tangents.front()), bufferType);
		glEnableVertexAttribArray(SUBSTATE_TANGENT_LOCATION);
		glVertexAttribPointer(SUBSTATE_TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Model matrix for instancing drawing.
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_MODEL_MATRIX_VB]);
	for(unsigned int i = 0; i<4; i++) {
		glEnableVertexAttribArray(SUBSTATE_MVP_LOCATION+i);
		glVertexAttribPointer(SUBSTATE_MVP_LOCATION+i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const GLvoid*) (sizeof(GLfloat) * i*4));
		glVertexAttribDivisor(SUBSTATE_MVP_LOCATION+i, 1);
	}

	// CALbyte buffer.
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_DATA_BYTE]);
	glEnableVertexAttribArray(SUBSTATE_BYTE_DATA_LOCATION);
	glVertexAttribIPointer(SUBSTATE_BYTE_DATA_LOCATION, 1, GL_BYTE, 0, 0);
	if(m_InstancedData) glVertexAttribDivisor(SUBSTATE_BYTE_DATA_LOCATION, 1);

	// CALint buffer.
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_DATA_INT]);
	glEnableVertexAttribArray(SUBSTATE_INT_DATA_LOCATION);
	glVertexAttribIPointer(SUBSTATE_INT_DATA_LOCATION, 1, GL_INT, 0, 0);
	if(m_InstancedData) glVertexAttribDivisor(SUBSTATE_INT_DATA_LOCATION, 1);

	// CALreal buffer.
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_DATA_REAL]);
	glEnableVertexAttribArray(SUBSTATE_REAL_DATA_LOCATION);
	glVertexAttribLPointer(SUBSTATE_REAL_DATA_LOCATION, 1, GL_DOUBLE, 0, (GLvoid*) 0);
	if(m_InstancedData) glVertexAttribDivisor(SUBSTATE_REAL_DATA_LOCATION, 1);

	// Indices
	if(m_Indices.size()>0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_INDEX_VB]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_Indices.size(), &m_Indices.front(), bufferType);
	}


	// Avoid problems with instaced buffers.
	mat4 matrix;
	vector<mat4> worldMatrices;
	worldMatrices.push_back(matrix);
	SetMMatrices(worldMatrices);
	UpdateMMatrices();

	CALbyte byteValue = CAL_FALSE;
	UpdateByteData(&byteValue, 1);

	CALint intValue = 0;
	UpdateIntData(&intValue, 1);

	CALreal realValue = 0.0;
	UpdateRealData(&realValue, 1);

	glBindVertexArray(m_VAO);
}

void SubstateMesh::SetMVPMatrices(vector<mat4>& vector) {}

void SubstateMesh::SetMMatrices(vector<mat4>& vector) {
	m_MMatrices = std::move(vector);
}

vector<mat4>& SubstateMesh::GetMMatrices() {
	return m_MMatrices;
}

void SubstateMesh::UpdatePositions() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_POS_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_Positions.size(), glm::value_ptr(m_Positions.front()));
	glBindVertexArray(0);
}

void SubstateMesh::UpdateTexCoords() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_TEXCOORD_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_TexCoords.size(), glm::value_ptr(m_TexCoords.front()));
	glBindVertexArray(0);
}

void SubstateMesh::UpdateNormals() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_NORMAL_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_Normals.size(), glm::value_ptr(m_Normals.front()));
	glBindVertexArray(0);
}

void SubstateMesh::UpdateTangents() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_TANGENT_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_Tangents.size(), glm::value_ptr(m_Tangents.front()));
	glBindVertexArray(0);
}

void SubstateMesh::UpdateIndices() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_INDEX_VB]);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_Indices.size(), &m_Indices.front());
	glBindVertexArray(0);
}

void SubstateMesh::UpdateMVPMatrices() {}

void SubstateMesh::UpdateMMatrices() {
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_MODEL_MATRIX_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m_MMatrices.size(), glm::value_ptr(m_MMatrices.front()), GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}

void SubstateMesh::UpdateBones() {}

void SubstateMesh::UpdateByteData(CALbyte* substateData, unsigned int dim) {
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_DATA_BYTE]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CALbyte) * dim, substateData, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}

void SubstateMesh::UpdateIntData(CALint* substateData, unsigned int dim) {
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_DATA_INT]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CALint) * dim, substateData, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}

void SubstateMesh::UpdateRealData(CALreal* substateData, unsigned int dim) {
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eSubstateVertexBuffer::SUBSTATE_DATA_REAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CALreal) * dim, substateData, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}
