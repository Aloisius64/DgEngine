////////////////////////////////////////////////////////////////////////////////
// Filename: Animator.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <list>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Animator.h"
#include "SceneObject.h"
#include "TimeClock.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region AntTweakBar
static void TW_CALL SetIndexCurrentAnimationCallback(const void *value, void *clientData) {
	static_cast<Animator *>(clientData)->Play(*static_cast<const unsigned int *>(value));
}
static void TW_CALL GetIndexCurrentAnimationCallback(void *value, void *clientData) {
	*static_cast<unsigned int *>(value) = static_cast<Animator*>(clientData)->GetCurrentAnimations();
}
#pragma endregion

Animator::Animator(SceneObject* sceneObject, const shared_ptr<Mesh>& mesh)
	: Component(eComponentType::ANIMATOR, sceneObject) {
	m_Mesh = mesh;
	m_IndexCurrentAnimation = 0;
	m_StartTime = TimeClock::CurrentTimeMillis();
}

Animator::~Animator() {
	m_Mesh = nullptr;
}

void Animator::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	TwAddButton(bar, "Animator", NULL, NULL, "");
	TwAddVarCB(bar, "IndexAnimation", TW_TYPE_UINT32, SetIndexCurrentAnimationCallback, GetIndexCurrentAnimationCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/IndexAnimation min=%d ", GetSceneObject()->Id().c_str(), 0);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/IndexAnimation step=%d ", GetSceneObject()->Id().c_str(), 1);
	TwDefine(parameter);
	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);
}

void Animator::Play(int index) {
	if(index>=0&&index<GetNumberAnimations()) {
		m_IndexCurrentAnimation = index;
	}
}

void Animator::Play(string name) {
	//mesh->playAnimation(name);
}

int Animator::GetNumberAnimations() {
	return m_Mesh->m_Scene->mNumAnimations;
}

const unsigned int& Animator::GetCurrentAnimations() {
	return m_IndexCurrentAnimation;
}

unsigned int Animator::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	for(unsigned int i = 0; i<pNodeAnim->mNumPositionKeys-1; i++) {
		if(AnimationTime<(float) pNodeAnim->mPositionKeys[i+1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned int Animator::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	assert(pNodeAnim->mNumRotationKeys>0);

	for(unsigned int i = 0; i<pNodeAnim->mNumRotationKeys-1; i++) {
		if(AnimationTime<(float) pNodeAnim->mRotationKeys[i+1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned int Animator::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	assert(pNodeAnim->mNumScalingKeys>0);

	for(unsigned int i = 0; i<pNodeAnim->mNumScalingKeys-1; i++) {
		if(AnimationTime<(float) pNodeAnim->mScalingKeys[i+1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

void Animator::ComputeInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	if(pNodeAnim->mNumPositionKeys==1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex+1);
	assert(NextPositionIndex<pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float) (pNodeAnim->mPositionKeys[NextPositionIndex].mTime-pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime-(float) pNodeAnim->mPositionKeys[PositionIndex].mTime)/DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End-Start;
	Out = Start+Factor * Delta;
}

void Animator::ComputeInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate...
	if(pNodeAnim->mNumRotationKeys==1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex+1);
	assert(NextRotationIndex<pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float) (pNodeAnim->mRotationKeys[NextRotationIndex].mTime-pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime-(float) pNodeAnim->mRotationKeys[RotationIndex].mTime)/DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void Animator::ComputeInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	if(pNodeAnim->mNumScalingKeys==1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex+1);
	assert(NextScalingIndex<pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float) (pNodeAnim->mScalingKeys[NextScalingIndex].mTime-pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime-(float) pNodeAnim->mScalingKeys[ScalingIndex].mTime)/DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End-Start;
	Out = Start+Factor * Delta;
}

void Animator::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const mat4& ParentTransform) {
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_Mesh->m_Scene->mAnimations[m_IndexCurrentAnimation];

	mat4 NodeTransformation = mat4(1.0f);
	memcpy(glm::value_ptr(NodeTransformation), &pNode->mTransformation, sizeof(float)*16);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if(pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		ComputeInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		mat4 ScalingM = glm::transpose(glm::scale(mat4(1.0f), vec3(Scaling.x, Scaling.y, Scaling.z)));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		ComputeInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		mat4 RotationM = glm::transpose(toMat4(quat(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z)));

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		ComputeInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		mat4 TranslationM = glm::transpose(glm::translate(mat4(1.0f), vec3(Translation.x, Translation.y, Translation.z)));

		// Combine the above transformations
		NodeTransformation = ScalingM * RotationM * TranslationM;
	}

	mat4 GlobalTransformation = NodeTransformation * ParentTransform;

	if(m_Mesh->m_BoneMapping.find(NodeName)!=m_Mesh->m_BoneMapping.end()) {
		unsigned int BoneIndex = m_Mesh->m_BoneMapping[NodeName];

		m_Mesh->m_BoneInfo[BoneIndex].FinalTransformation = m_Mesh->m_BoneInfo[BoneIndex].BoneOffset * GlobalTransformation * m_Mesh->m_GlobalInverseTransform;
	}

	for(unsigned int i = 0; i<pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

void Animator::BoneTransform(vector<mat4>& Transforms) {
	mat4 Identity = mat4(1.0f);

	float RunningTime = (float) (TimeClock::CurrentTimeMillis()-m_StartTime)/1000.0f;

	float TicksPerSecond = (float) (m_Mesh->m_Scene->mAnimations[m_IndexCurrentAnimation]->mTicksPerSecond!=0 ? m_Mesh->m_Scene->mAnimations[m_IndexCurrentAnimation]->mTicksPerSecond : 25.0f);
	float TimeInTicks = RunningTime * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float) m_Mesh->m_Scene->mAnimations[m_IndexCurrentAnimation]->mDuration);

	ReadNodeHeirarchy(AnimationTime, m_Mesh->m_Scene->mRootNode, Identity);

	Transforms.resize(m_Mesh->m_NumBones);

	for(unsigned int i = 0; i<m_Mesh->m_NumBones; i++) {
		Transforms[i] = m_Mesh->m_BoneInfo[i].FinalTransformation;
		Transforms[i] = glm::transpose(Transforms[i]);
	}
}

const aiNodeAnim* Animator::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName) {
	for(unsigned int i = 0; i<pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if(string(pNodeAnim->mNodeName.data)==NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

