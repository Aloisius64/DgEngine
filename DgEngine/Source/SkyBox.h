////////////////////////////////////////////////////////////////////////////////
// Filename: SkyBox.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <memory>
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "CubemapTexture.h"
#include "Mesh.h"
#include "Shader.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;

namespace dg {
	class Mesh;
	class Transform;

	class SkyBox {
	public:
		SkyBox();
		~SkyBox();

		void Render();

		bool Initialize(const string&,
			const string&,
			const string&,
			const string&,
			const string&,
			const string&);

		const unique_ptr<CubemapTexture>& GetCubemapTex() const;

	private:
		unique_ptr<CubemapTexture>	m_CubemapTex;
		shared_ptr<Mesh>			m_Mesh;
		shared_ptr<Shader>			m_Shader;
		unique_ptr<Transform>		m_Transform;
	};
}
