////////////////////////////////////////////////////////////////////////////////
// Filename: MeshRenderer.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Component.h"
#include "Mesh.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;

namespace dg {
	class SceneLoader;
	class UI;
	class Shader;

	class MeshRenderer : public Component {
		friend class SceneLoader;
		friend class UI;

	public:
		static inline eComponentType ComponentType() {
			return eComponentType::MESH_RENDERER;
		}

	public:
		MeshRenderer(SceneObject*);

		virtual ~MeshRenderer();

		virtual void Update();
		virtual void PostUpdate();
		virtual void Render(const shared_ptr<Shader>& = nullptr, bool = false, const GLuint& = NULL_QUERY);
		virtual void OcclusionRender(const shared_ptr<Shader>&, const GLuint&, const bool& = false);
		virtual void DrawUI(TwBar*);
		virtual void SetShaderParameters(const shared_ptr<Shader>& = nullptr, bool = false);

		void LoadMesh(const string&, FlagData* = nullptr, const vector<shared_ptr<Material>>* = nullptr);
		const shared_ptr<Mesh>& GetMesh();
		void RenderBoundingBox();
		void RenderNormals();
		void DrawNormals(const bool&);
		const bool& IsDrawNormals() const;
		void SetShadeless(const bool&);
		const bool& IsShadeless() const;
		void SetMaterials(const vector<shared_ptr<Material>>&);
		const vector<shared_ptr<Material>>& GetMaterials() const;

	protected:
		void Render_Standard(const shared_ptr<Shader>& = nullptr, const GLuint& = NULL_QUERY);
		void Render_Instanced(const shared_ptr<Shader>& = nullptr, const GLuint& = NULL_QUERY);

	protected:
		string							m_FilePath;
		shared_ptr<Mesh>				m_Mesh;
		vector<shared_ptr<Material>>	m_Materials;
		// Shadeless
		bool							m_Shadeless;	// No ligths computing.
		// Normals drawing.
		bool							m_DrawNormals;
		float							m_NormalMagnitude;
	};
}
