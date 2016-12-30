////////////////////////////////////////////////////////////////////////////////
// Filename: BoxCollider.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Collider.h"

////////////////
// NAMESPACES //
////////////////

namespace dg {
	class BoxCollider : public Collider {
	public:
		BoxCollider();
		virtual ~BoxCollider();

		virtual void Initialize(const ColliderData&);
		virtual string GetType();

	private:
		vec3	m_Size;
	};
}