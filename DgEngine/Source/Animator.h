////////////////////////////////////////////////////////////////////////////////
// Filename: Animator.h
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


namespace dg {
	class Animator : public Component {

	public:
		static inline eComponentType ComponentType() {
			return eComponentType::ANIMATOR;
		}

	public:
		Animator(SceneObject*, const shared_ptr<Mesh>&);

		virtual ~Animator();

		virtual void DrawUI(TwBar*);

		void Play(int index);
		void Play(string name);
		void Stop();
		int GetNumberAnimations();
		const unsigned int& GetCurrentAnimations();
		
		void BoneTransform(vector<mat4>&);
		void ComputeInterpolatedScaling(aiVector3D&, float, const aiNodeAnim*);
		void ComputeInterpolatedRotation(aiQuaternion&, float, const aiNodeAnim*);
		void ComputeInterpolatedPosition(aiVector3D&, float, const aiNodeAnim*);
		unsigned int FindScaling(float, const aiNodeAnim*);
		unsigned int FindRotation(float, const aiNodeAnim*);
		unsigned int FindPosition(float, const aiNodeAnim*);
		const aiNodeAnim* FindNodeAnim(const aiAnimation*, const string);
		void ReadNodeHeirarchy(float, const aiNode*, const mat4&);

	private:
		shared_ptr<Mesh>	m_Mesh;
		unsigned int		m_IndexCurrentAnimation;
		long long			m_StartTime;
	};
}
