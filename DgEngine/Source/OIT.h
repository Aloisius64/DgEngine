////////////////////////////////////////////////////////////////////////////////
// Filename: OIT.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace dg {
	class OIT {
	public:
		OIT();
		virtual ~OIT();

		bool Initialize();
		void ClearBuffers();
		void BindBuffers();
		bool IsInitialized() const;

	private:
		GLuint* m_Data;
		// Head pointer image and PBO for clearing it
		GLuint  m_Head_pointer_texture;
		GLuint  m_Head_pointer_clear_buffer;
		// Atomic counter buffer
		GLuint  m_Atomic_counter_buffer;
		// Linked list buffer
		GLuint  m_Linked_list_buffer;
		GLuint  m_Linked_list_texture;
	};
}
