////////////////////////////////////////////////////////////////////////////////
// Filename: OIT.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "OIT.h"
#include "Configuration.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#define MAX_FRAMEBUFFER_WIDTH 2048
#define MAX_FRAMEBUFFER_HEIGHT 2048

OIT::OIT() {
	m_Head_pointer_texture = 0;
	m_Head_pointer_clear_buffer = 0;
	m_Atomic_counter_buffer = 0;
	m_Linked_list_buffer = 0;
	m_Linked_list_texture = 0;
}

OIT::~OIT() {
	DELETE_TEXTURE(m_Linked_list_texture);
	DELETE_BUFFER(m_Linked_list_buffer);
	DELETE_BUFFER(m_Atomic_counter_buffer);
	DELETE_BUFFER(m_Head_pointer_clear_buffer);
	DELETE_TEXTURE(m_Head_pointer_texture);
}

bool OIT::Initialize() {
		
	// Create head pointer texture.
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_Head_pointer_texture);
	glBindTexture(GL_TEXTURE_2D, m_Head_pointer_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, MAX_FRAMEBUFFER_WIDTH, MAX_FRAMEBUFFER_HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindImageTexture(0, m_Head_pointer_texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

	// Create buffer for clearing the head pointer texture.
	glGenBuffers(1, &m_Head_pointer_clear_buffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_Head_pointer_clear_buffer);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT * sizeof(GLuint), NULL, GL_STATIC_DRAW);
	m_Data = (GLuint *) glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	memset(m_Data, 0x00, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT * sizeof(GLuint));
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

	// Create the atomic counter buffer.
	glGenBuffers(1, &m_Atomic_counter_buffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_Atomic_counter_buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_COPY);

	// Create the linked list storage buffer.
	glGenBuffers(1, &m_Linked_list_buffer);
	glBindBuffer(GL_TEXTURE_BUFFER, m_Linked_list_buffer);
	glBufferData(GL_TEXTURE_BUFFER, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT*3*sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Bind it to a texture (for use as a TBO)
	glGenTextures(1, &m_Linked_list_texture);
	glBindTexture(GL_TEXTURE_BUFFER, m_Linked_list_texture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32UI, m_Linked_list_buffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glBindImageTexture(1, m_Linked_list_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI);


	return dg::CheckGLError();
}

void OIT::ClearBuffers() {
	// Reset atomic counter
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_Atomic_counter_buffer);
	m_Data = (GLuint *) glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_WRITE_ONLY);
	m_Data[0] = 0;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	// Clear head-pointer image
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_Head_pointer_clear_buffer);
	glBindTexture(GL_TEXTURE_2D, m_Head_pointer_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Configuration::Instance()->GetScreenWidth(), Configuration::Instance()->GetScreenHeigth(), GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OIT::BindBuffers() {
	// Bind head-pointer image for read-write
	glBindImageTexture(0, m_Head_pointer_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	// Bind linked-list buffer for write
	glBindImageTexture(1, m_Linked_list_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI);
}

bool OIT::IsInitialized() const {
	return 	m_Head_pointer_texture!=0&&
		m_Head_pointer_clear_buffer!=0&&
		m_Atomic_counter_buffer!=0&&
		m_Linked_list_buffer!=0&&
		m_Linked_list_texture!=0;
}
