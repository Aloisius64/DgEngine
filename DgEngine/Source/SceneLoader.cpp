////////////////////////////////////////////////////////////////////////////////
// Filename: SceneLoader.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <irrXML\irrXML.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SceneLoader.h"
#include "Animator.h"
#include "Behaviour.h"
#include "Camera.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Glossary.h"
#include "Light.h"
#include "Material.h"
#include "MeshRenderer.h"
//#include "Particles.h"
#include "Physics.h"
#include "Physics\CharacterController.h"
#include "Physics\PlaneCollider.h"
#include "Physics\BoxCollider.h"
#include "Physics\SphereCollider.h"
#include "Physics\CapsuleCollider.h"
#include "Physics\MeshCollider.h"
#include "Physics\TerrainCollider.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace irr;
using namespace io;
using namespace dg;

static IrrXMLReader* xml = nullptr;

void SceneLoader::LoadScene(string filePath) {
	if(filePath=="")
		return;

	xml = createIrrXMLReader(filePath.c_str());

	if(xml)
		ParseGraphScene(SceneObject::Root().get());

	FREE_PTR(xml);
}

void SceneLoader::ParseGraphScene(SceneObject* sceneObject) {
	bool exit = false;
	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())==ENVIRONMENT_STR) {
				ParseEnvironment();
			}
			if(string(xml->getNodeName())==SCENE_OBJECT_STR) {
				ParseSceneObject(sceneObject);
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==GRAPH_SCENE_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

void SceneLoader::ParseEnvironment() {
	bool exit = false;

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())==AMBIENT_OCCLUSION_STR) {
				int kernelSize = std::stoi(xml->getAttributeValue(KERNEL_SIZE_CSTR));
				if(!Environment::Instance()->InitializeAO(kernelSize)) {
					DebugConsoleMessage("Error to initialize Ambient Occlusion\n");
				}
			} else if(string(xml->getNodeName())==SKY_BOX_STR) {
				string xPos = string(xml->getAttributeValue(POS_X_CSTR));
				string xNeg = string(xml->getAttributeValue(NEG_X_CSTR));
				string yPos = string(xml->getAttributeValue(POS_Y_CSTR));
				string yNeg = string(xml->getAttributeValue(NEG_Y_CSTR));
				string zPos = string(xml->getAttributeValue(POS_Z_CSTR));
				string zNeg = string(xml->getAttributeValue(NEG_Z_CSTR));
				if(!Environment::Instance()->InitializeSkyBox(xPos, xNeg, yPos, yNeg, zPos, zNeg)) {
					DebugConsoleMessage("Error to initialize Skybox\n");
				}
			} else if(string(xml->getNodeName())==SHADOW_STR) {
				string enabled = xml->getAttributeValue(ENABLED_CSTR);
				if(enabled==TRUE_STR) {
					unsigned int mapSize = std::stoi(string(xml->getAttributeValue(MAP_SIZE_CSTR)));
					Environment::Instance()->InitializeShadows(mapSize);
				}
			} else if(string(xml->getNodeName())==HDR_STR) {
				Environment::Instance()->InitializeHdr();
				if(xml->getAttributeValue(EXPOSURE_CSTR)) {
					float exposure = std::stof(xml->getAttributeValue(EXPOSURE_CSTR));
					Environment::Instance()->SetExposure(exposure);
				}
			} else if(string(xml->getNodeName())==BLOOM_STR) {
				string enabled = xml->getAttributeValue(ENABLED_CSTR);
				if(enabled==TRUE_STR) {
					Environment::Instance()->InitializeBloom();
				}
			} else if(string(xml->getNodeName())==OIT_STR) {
				string enabled = xml->getAttributeValue(ENABLED_CSTR);
				//Environment::Instance()->SetOITActive(enabled==TRUE_STR);
			} else if(string(xml->getNodeName())==VR_STR) {
				string enabled = xml->getAttributeValue(ENABLED_CSTR);
				Environment::Instance()->SetOVRActive(enabled==TRUE_STR);
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==ENVIRONMENT_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

void SceneLoader::ParseSceneObject(SceneObject* sceneObject) {
	bool exit = false;
	string id = xml->getAttributeValue(ID_CSTR);
	string tag = xml->getAttributeValue(TAG_CSTR);
	string enabled = xml->getAttributeValue(ENABLED_CSTR);

	SceneObject* child = new SceneObject(id, tag);
	if(enabled==TRUE_STR) {
		child->Enable();
	} else {
		child->Disable();
	}
	sceneObject->AddChild(child);

	ParseComponents(child);
	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())==SCENE_OBJECT_STR) {
				ParseSceneObject(child);
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==SCENE_OBJECT_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

void SceneLoader::ParseComponents(SceneObject* sceneObject) {
	bool exit = false;
	xml->read();
	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())==TRANSFORM_COMPONENT_STR) {
				ParseTransform(sceneObject);
			} else if(string(xml->getNodeName())==CAMERA_COMPONENT_STR) {
				ParseCamera(sceneObject);
			} else if(string(xml->getNodeName())==MESH_COMPONENT_STR) {
				ParseMesh(sceneObject);
			} else if(string(xml->getNodeName())==PHYSICS_COMPONENT_STR) {
				ParsePhysics(sceneObject);
			} else if(string(xml->getNodeName())==AUDIO_COMPONENT_STR) {
				//ParseAudio(sceneObject);
			} else if(string(xml->getNodeName())==LIGHT_COMPONENT_STR) {
				ParseLight(sceneObject);
			} else if(string(xml->getNodeName())==BEHAVIOUR_COMPONENT_STR) {
				ParseBehaviour(sceneObject);
			} else if(string(xml->getNodeName())==PARTICLES_COMPONENT_STR) {
				//ParseParticles(sceneObject);
			}
		} break;
		case EXN_ELEMENT_END: {
			if(string(xml->getNodeName())==COMPONENTS_STR) {
				exit = true;
			}
		}break;
		default: break;
		}
	}
}

void SceneLoader::ParseTransform(SceneObject* sceneObject) {
	bool exit = false;
	Transform* transform = new Transform(sceneObject);

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())==POSITION_STR) {
				float x, y, z;
				x = std::stof(xml->getAttributeValue(X_CSTR));
				y = std::stof(xml->getAttributeValue(Y_CSTR));
				z = std::stof(xml->getAttributeValue(Z_CSTR));
				transform->SetPosition(x, y, z);
			} else if(string(xml->getNodeName())==ROTATION_STR) {
				float x, y, z;
				x = std::stof(xml->getAttributeValue(X_CSTR));
				y = std::stof(xml->getAttributeValue(Y_CSTR));
				z = std::stof(xml->getAttributeValue(Z_CSTR));
				transform->SetRotation(x, y, z);
			} else if(string(xml->getNodeName())==SCALE_STR) {
				float x, y, z;
				x = std::stof(xml->getAttributeValue(X_CSTR));
				y = std::stof(xml->getAttributeValue(Y_CSTR));
				z = std::stof(xml->getAttributeValue(Z_CSTR));
				transform->SetScaling(x, y, z);
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==TRANSFORM_COMPONENT_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

void SceneLoader::ParseCamera(SceneObject* sceneObject) {
	bool exit = false;

	Camera* camera = new Camera(sceneObject);

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())==BACKGROUND_COLOR_STR) {
				camera->SetBackgroundColor(vec4(std::stof(xml->getAttributeValue(R_CSTR)),
					std::stof(xml->getAttributeValue(G_CSTR)),
					std::stof(xml->getAttributeValue(B_CSTR)),
					std::stof(xml->getAttributeValue(A_CSTR))));
			}
			if(string(xml->getNodeName())==FOVY_STR) {
				camera->SetFovy(std::stof(xml->getAttributeValue(VALUE_CSTR)));
			}
			if(string(xml->getNodeName())==Z_NEAR_STR) {
				camera->SetZNear(std::stof(xml->getAttributeValue(VALUE_CSTR)));
			}
			if(string(xml->getNodeName())==Z_FAR_STR) {
				camera->SetZFar(std::stof(xml->getAttributeValue(VALUE_CSTR)));
			}
			if(string(xml->getNodeName())==VIEWPORT_STR) {
				camera->SetViewportX(std::stof(xml->getAttributeValue(X_CSTR)));
				camera->SetViewportY(std::stof(xml->getAttributeValue(Y_CSTR)));
				camera->SetViewportWidth(std::stof(xml->getAttributeValue(WIDTH_CSTR)));
				camera->SetViewportHeight(std::stof(xml->getAttributeValue(HEIGHT_CSTR)));
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==CAMERA_COMPONENT_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

void SceneLoader::ParseMesh(SceneObject* sceneObject) {
	bool exit = false;
	string path = xml->getAttributeValue(PATH_CSTR);
	MeshRenderer* mesh = new MeshRenderer(sceneObject);
	FlagData flagData;
	vector<shared_ptr<Material>> materials;

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==FLAG_STR) {
				const char* value = xml->getAttributeValue(CALC_TANGENT_SPACE_CSTR);
				if(value) {
					flagData.CalcTangentSpace = value==TRUE_STR;
				}
				value = xml->getAttributeValue(FIND_INSTANCES_CSTR);
				if(value) {
					flagData.FindInstances = value==TRUE_STR;
				}
				value = xml->getAttributeValue(FIND_INVALID_DATA_CSTR);
				if(value) {
					flagData.FindInvalidData = value==TRUE_STR;
				}
				value = xml->getAttributeValue(FLIP_UV_CSTR);
				if(value) {
					flagData.FlipUVs = value==TRUE_STR;
				}
				value = xml->getAttributeValue(GEN_SMOOTH_NORMALS_CSTR);
				if(value) {
					flagData.GenSmoothNormals = value==TRUE_STR;
				}
				value = xml->getAttributeValue(GEN_UV_COORDS_CSTR);
				if(value) {
					flagData.GenUVCoords = value==TRUE_STR;
				}
				value = xml->getAttributeValue(IMPROVE_CACHE_LOCALITY_CSTR);
				if(value) {
					flagData.ImproveCacheLocality = value==TRUE_STR;
				}
				value = xml->getAttributeValue(JOIN_IDENTICAL_VERTICES_CSTR);
				if(value) {
					flagData.JoinIdenticalVertices = value==TRUE_STR;
				}
				value = xml->getAttributeValue(LIMIT_BONE_WEIGHTS_CSTR);
				if(value) {
					flagData.LimitBoneWeights = value==TRUE_STR;
				}
				value = xml->getAttributeValue(OPTIMIZE_MESHES_CSTR);
				if(value) {
					flagData.OptimizeMeshes = value==TRUE_STR;
				}
				value = xml->getAttributeValue(OPTIMIZE_GRAPH_CSTR);
				if(value) {
					flagData.OptimizeGraph = value==TRUE_STR;
				}
				value = xml->getAttributeValue(PRE_TRANSFORM_VERTICES_CSTR);
				if(value) {
					flagData.PreTransformVertices = value==TRUE_STR;
				}
				value = xml->getAttributeValue(REMOVE_REDUNDANT_MATERIALS_CSTR);
				if(value) {
					flagData.PreTransformVertices = value==TRUE_STR;
				}
				value = xml->getAttributeValue(SPLIT_BY_BONE_COUNT_CSTR);
				if(value) {
					flagData.SplitByBoneCount = value==TRUE_STR;
				}
				value = xml->getAttributeValue(TRIANGULATE_CSTR);
				if(value) {
					flagData.Triangulate = value==TRUE_STR;
				}
				value = xml->getAttributeValue(TRANSFORM_UV_COORDS_CSTR);
				if(value) {
					flagData.TransformUVCoords = value==TRUE_STR;
				}
				value = xml->getAttributeValue(VALIDATE_DATA_STRUCTURE_CSTR);
				if(value) {
					flagData.ValidateDataStructure = value==TRUE_STR;
				}
			} else if(string(xml->getNodeName())==MATERIALS_STR) {
				ParseMaterials(materials);
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==MESH_COMPONENT_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	if(!materials.empty())
		mesh->LoadMesh(path, &flagData, &materials);
	else
		mesh->LoadMesh(path, &flagData);

	mesh->GetMesh()->CreateBoundingBox();

	// If mesh is skinned add an Animator component.
	if(mesh->GetMesh()->GetType()==eMeshType::SKINNED_MESH) {
		new Animator(sceneObject, mesh->GetMesh());
	}
}

void SceneLoader::ParseMaterials(vector<shared_ptr<Material>>& materials) {
	bool exit = false;

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())==MATERIAL_STR) {
				shared_ptr<Material> material = ParseMaterial();
				materials.push_back(material);
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==MATERIALS_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

}

shared_ptr<Material> SceneLoader::ParseMaterial() {
	MaterialDesc materialDesc;

	bool exit = false;

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT:{
			if(string(xml->getNodeName())==SHADER_NAME_STR) {
				materialDesc.shaderName = xml->getAttributeValue(VALUE_CSTR);
			} else if(string(xml->getNodeName())==MATERIAL_NAME_STR) {
				materialDesc.materialName = xml->getAttributeValue(VALUE_CSTR);
			} else if(string(xml->getNodeName())==WIREFRAME_STR) {
				string enabled = xml->getAttributeValue(VALUE_CSTR);
				materialDesc.wireframe = enabled==TRUE_STR;
			} else if(string(xml->getNodeName())==OPACITY_STR) {
				materialDesc.opacity = std::stof(xml->getAttributeValue(VALUE_CSTR));
			} else if(string(xml->getNodeName())==BUMP_SCALING_STR) {
				materialDesc.bumpScaling = std::stof(xml->getAttributeValue(VALUE_CSTR));
			} else if(string(xml->getNodeName())==SHININESS_STR) {
				materialDesc.shininess = std::stof(xml->getAttributeValue(VALUE_CSTR));
			} else if(string(xml->getNodeName())==REFLECTIVITY_STR) {
				materialDesc.reflectivity = std::stof(xml->getAttributeValue(VALUE_CSTR));
			} else if(string(xml->getNodeName())==REFRACTION_STR) {
				materialDesc.refraction = std::stof(xml->getAttributeValue(VALUE_CSTR));
			} else if(string(xml->getNodeName())==COLOR_ALBEDO_STR) {
				float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
				r = std::stof(xml->getAttributeValue(R_CSTR));
				g = std::stof(xml->getAttributeValue(G_CSTR));
				b = std::stof(xml->getAttributeValue(B_CSTR));
				a = std::stof(xml->getAttributeValue(A_CSTR));
				materialDesc.colorDiffuse = vec4(r, g, b, a);
			} else if(string(xml->getNodeName())==COLOR_AMBIENT_STR) {
				float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
				r = std::stof(xml->getAttributeValue(R_CSTR));
				g = std::stof(xml->getAttributeValue(G_CSTR));
				b = std::stof(xml->getAttributeValue(B_CSTR));
				a = std::stof(xml->getAttributeValue(A_CSTR));
				materialDesc.colorAmbient = vec4(r, g, b, a);
			} else if(string(xml->getNodeName())==COLOR_SPECULAR_STR) {
				float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
				r = std::stof(xml->getAttributeValue(R_CSTR));
				g = std::stof(xml->getAttributeValue(G_CSTR));
				b = std::stof(xml->getAttributeValue(B_CSTR));
				a = std::stof(xml->getAttributeValue(A_CSTR));
				materialDesc.colorSpecular = vec4(r, g, b, a);
			} else if(string(xml->getNodeName())==COLOR_EMISSIVE_STR) {
				float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
				r = std::stof(xml->getAttributeValue(R_CSTR));
				g = std::stof(xml->getAttributeValue(G_CSTR));
				b = std::stof(xml->getAttributeValue(B_CSTR));
				a = std::stof(xml->getAttributeValue(A_CSTR));
				materialDesc.colorEmissive = vec4(r, g, b, a);
			} else if(string(xml->getNodeName())==TC_SCALE_STR) {
				float x, y;
				x = std::stof(xml->getAttributeValue(X_CSTR));
				y = std::stof(xml->getAttributeValue(Y_CSTR));
				materialDesc.tc_scale = vec2(x, y);
			} else if(string(xml->getNodeName())==TC_OFFSET_STR) {
				float x, y;
				x = std::stof(xml->getAttributeValue(X_CSTR));
				y = std::stof(xml->getAttributeValue(Y_CSTR));
				materialDesc.tc_offset = vec2(x, y);
			} else if(string(xml->getNodeName())==TEXTURE_STR) {
				eTextureType textureType = ConvertToTextureType(xml->getAttributeValue(TYPE_CSTR));
				materialDesc.textureDesc[textureType].filePath = xml->getAttributeValue(PATH_CSTR);
				if(xml->getAttributeValue(MIP_MAP_CSTR))
					materialDesc.textureDesc[textureType].mipMap = xml->getAttributeValue(MIP_MAP_CSTR)==TRUE_STR;
				if(xml->getAttributeValue(WRAP_S_CSTR))
					materialDesc.textureDesc[textureType].wrapS = ConvertToTextureWrap(xml->getAttributeValue(WRAP_S_CSTR));
				if(xml->getAttributeValue(WRAP_T_CSTR))
					materialDesc.textureDesc[textureType].wrapT = ConvertToTextureWrap(xml->getAttributeValue(WRAP_T_CSTR));
				if(xml->getAttributeValue(MAG_FILTER_CSTR))
					materialDesc.textureDesc[textureType].magFilter = ConvertToTextureFilter(xml->getAttributeValue(MAG_FILTER_CSTR));
				if(xml->getAttributeValue(MIN_FILTER_CSTR))
					materialDesc.textureDesc[textureType].minFilter = ConvertToTextureFilter(xml->getAttributeValue(MIN_FILTER_CSTR));
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==MATERIAL_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	return DataManager::TryGetMaterial(materialDesc);
}

void SceneLoader::ParseLight(SceneObject* sceneObject) {
	bool exit = false;
	string type = xml->getAttributeValue(TYPE_CSTR);

	type = type==EMPTY_CSTR ? DIRECTIONAL_STR : type;

	//	Create and set the new light
	Light* light = new Light(sceneObject, Light::ConvertToTypeLight(type));

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==COLOR_STR) {
				light->SetColor(std::stof(xml->getAttributeValue(R_CSTR)),
					std::stof(xml->getAttributeValue(G_CSTR)),
					std::stof(xml->getAttributeValue(B_CSTR)));
			} else if(string(xml->getNodeName())==AMBIENT_INTENSITY_STR) {
				light->SetAmbientIntensity(std::stof(xml->getAttributeValue(VALUE_CSTR)));
			} else if(string(xml->getNodeName())==DIFFUSE_INTENSITY_STR) {
				light->SetDiffuseIntensity(std::stof(xml->getAttributeValue(VALUE_CSTR)));
			} else if(string(xml->getNodeName())==ATTENUATION_STR) {
				light->SetConstantAttenuation(std::stof(xml->getAttributeValue(CONSTANT_CSTR)));
				light->SetLinearAttenuation(std::stof(xml->getAttributeValue(LINEAR_CSTR)));
				light->SetExponentialAttenuation(std::stof(xml->getAttributeValue(EXP_CSTR)));
			} else if(string(xml->getNodeName())==CUTOFF_STR) {
				light->SetCutoff(std::stof(xml->getAttributeValue(VALUE_CSTR)));
			} else if(string(xml->getNodeName())==SHADOW_TYPE_STR) {
				light->SetShadowType(Light::ConvertToShadowLight(xml->getAttributeValue(VALUE_CSTR)));
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==LIGHT_COMPONENT_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

void SceneLoader::ParseBehaviour(SceneObject* sceneObject) {
	bool exit = false;

	//	Create and set the new audio component
	Behaviour* behaviour = new Behaviour(sceneObject);

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==BEHAVIOUR_COMPONENT_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

void SceneLoader::ParsePhysics(SceneObject* sceneObject) {
	bool exit = false;

	PhysX::Instance();

	//	Create and set the new physics component.
	RigidBody* rigidBody = new RigidBody();
	Physics* physics = new Physics(sceneObject, rigidBody);
	bool isStatic = false;
	bool initializedRigidBody = false;

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==RIGIDBODY_STR) {
				isStatic = (string(xml->getAttributeValue(STATIC_CSTR))==TRUE_STR);
				rigidBody->SetMass(std::stof(xml->getAttributeValue(MASS_CSTR)));
				rigidBody->SetDrag(std::stof(xml->getAttributeValue(DRAG_CSTR)));
				rigidBody->SetAngularDrag(std::stof(xml->getAttributeValue(ANGULAR_DRAG_CSTR)));
				rigidBody->SetGravity(string(xml->getAttributeValue(GRAVITY_CSTR))==TRUE_STR);
				rigidBody->SetKinematic(string(xml->getAttributeValue(KINEMATIC_CSTR))==TRUE_STR);

				string collisionDetection = xml->getAttributeValue(COLLISION_DETECTION_CSTR);
				if(collisionDetection==CONTINUOUS_STR) {
					rigidBody->SetCollisionDetection(eCollisionDetection::CONTINUOUS);
				} else if(collisionDetection==CONTINUOUS_DYNAMIC_STR) {
					rigidBody->SetCollisionDetection(eCollisionDetection::CONTINOUOUS_DYNAMIC);
				} else {
					rigidBody->SetCollisionDetection(eCollisionDetection::DISCRETE);
				}
			}
			if(string(xml->getNodeName())==FREEZE_POSITION_STR) {
				rigidBody->FreezePositionX((string(xml->getAttributeValue(X_CSTR))==TRUE_STR));
				rigidBody->FreezePositionY((string(xml->getAttributeValue(Y_CSTR))==TRUE_STR));
				rigidBody->FreezePositionZ((string(xml->getAttributeValue(Z_CSTR))==TRUE_STR));
			} else if(string(xml->getNodeName())==FREEZE_ROTATION_STR) {
				rigidBody->FreezeRotationX((string(xml->getAttributeValue(X_CSTR))==TRUE_STR));
				rigidBody->FreezeRotationY((string(xml->getAttributeValue(Y_CSTR))==TRUE_STR));
				rigidBody->FreezeRotationZ((string(xml->getAttributeValue(Z_CSTR))==TRUE_STR));
			}
			if(string(xml->getNodeName())==COLLIDERS_STR) {
				ParseColliders(sceneObject, rigidBody);
			}
			if(string(xml->getNodeName())==CHARACTER_CONTROLLER_STR) {
				ParseCharacterController(sceneObject);
			}
		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==RIGIDBODY_STR) {
				rigidBody->Initialize(sceneObject, isStatic);
				initializedRigidBody = true;
			}
			if(string(xml->getNodeName())==PHYSICS_COMPONENT_STR) {
				if(initializedRigidBody) {
					PhysX::GetActiveScene()->addActor(*rigidBody->GetActor());
				}
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

void SceneLoader::ParseColliders(SceneObject* sceneObject, RigidBody* rigidBody) {
	bool exit = false;

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==PLANE_COLLIDER_STR) {
				ParsePlaneCollider(sceneObject, rigidBody);
			} else if(string(xml->getNodeName())==BOX_COLLIDER_STR) {
				ParseBoxCollider(sceneObject, rigidBody);
			} else if(string(xml->getNodeName())==SPHERE_COLLIDER_STR) {
				ParseSphereCollider(sceneObject, rigidBody);
			} else if(string(xml->getNodeName())==CAPSULE_COLLIDER_STR) {
				ParseCapsuleCollider(sceneObject, rigidBody);
			} else if(string(xml->getNodeName())==MESH_COLLIDER_STR) {
				ParseMeshCollider(sceneObject, rigidBody);
			} else if(string(xml->getNodeName())==TERRAIN_COLLIDER_STR) {
				ParseTerrainCollider(sceneObject, rigidBody);
			}
		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==COLLIDERS_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}
}

PhysicsMaterial* SceneLoader::ParsePhysicsMaterial() {
	bool exit = false;

	// Check first in the map
	string nameMaterial = xml->getAttributeValue(NAME_CSTR);
	PhysicsMaterial* material = PhysX::GetMaterial(nameMaterial);

	if(material==nullptr) {
		material = new PhysicsMaterial();
		material->SetName(nameMaterial);
		material->SetDynamicFriction(std::stof(xml->getAttributeValue(DYNAMIC_FRICTION_CSTR)));
		material->SetStaticFriction(std::stof(xml->getAttributeValue(STATIC_FRICTION_CSTR)));
		material->SetRestitution(std::stof(xml->getAttributeValue(RESTITUTION_CSTR)));
		// Add to map
		PhysX::AddMaterialToMap(material);
	}

	const PhysicsMaterialMap& map = PhysX::GetMaterialMap();
	map.find(nameMaterial);

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {

		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==PHYSIC_MATERIAL_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	return material;
}

void SceneLoader::ParseCharacterController(SceneObject* sceneObject) {
	bool exit = false;

	CharacterController* characterController = new CharacterController();
	PhysicsMaterial* material = nullptr;
	CharacterControllerData characterData;

	if(xml->getAttributeValue(SHAPE_TYPE_CSTR)) {
		if(string(xml->getAttributeValue(SHAPE_TYPE_CSTR))==BOX_CSTR) {
			characterData.shapeType = PxControllerShapeType::eBOX;
		} else {
			characterData.shapeType = PxControllerShapeType::eCAPSULE;
		}
	} else {
		characterData.shapeType = PxControllerShapeType::eCAPSULE;
	}

	if(xml->getAttributeValue(UP_DIRECTION_X_CSTR)
		&&xml->getAttributeValue(UP_DIRECTION_Y_CSTR)
		&&xml->getAttributeValue(UP_DIRECTION_Z_CSTR)) {
		string stringUpDirectionX = xml->getAttributeValue(UP_DIRECTION_X_CSTR);
		string stringUpDirectionY = xml->getAttributeValue(UP_DIRECTION_Y_CSTR);
		string stringUpDirectionZ = xml->getAttributeValue(UP_DIRECTION_Z_CSTR);
		if(stringUpDirectionX!=EMPTY_CSTR &&
			stringUpDirectionY!=EMPTY_CSTR &&
			stringUpDirectionZ!=EMPTY_CSTR) {
			characterData.upDirection = PxVec3(std::stof(stringUpDirectionX), std::stof(stringUpDirectionY), std::stof(stringUpDirectionZ));
		}
	}

	//	slopeLimit = "45.0"
	if(xml->getAttributeValue(SLOPE_LIMIT_CSTR)) {
		characterData.slopeLimit = std::stof(xml->getAttributeValue(SLOPE_LIMIT_CSTR));
	}
	//	invisibleWallHeight = "0.0"
	if(xml->getAttributeValue(INVISIBLE_WALL_HEIGHT_CSTR)) {
		characterData.invisibleWallHeight = std::stof(xml->getAttributeValue(INVISIBLE_WALL_HEIGHT_CSTR));
	}
	//	maxJumpHeight = "0.4"
	if(xml->getAttributeValue(MAX_JUMP_HEIGHT_CSTR)) {
		characterData.maxJumpHeight = std::stof(xml->getAttributeValue(MAX_JUMP_HEIGHT_CSTR));
	}
	//	contactOffset = "0.0"
	if(xml->getAttributeValue(CONTACT_OFFSET_CSTR)) {
		characterData.contactOffset = std::stof(xml->getAttributeValue(CONTACT_OFFSET_CSTR));
	}
	//	stepOffset = "0.3"
	if(xml->getAttributeValue(STEP_OFFSET_CSTR)) {
		characterData.stepOffset = std::stof(xml->getAttributeValue(STEP_OFFSET_CSTR));
	}
	//	density = "0.0"
	if(xml->getAttributeValue(DENSITY_CSTR)) {
		characterData.density = std::stof(xml->getAttributeValue(DENSITY_CSTR));
	}
	//	scaleCoeff = "0.0"
	if(xml->getAttributeValue(SCALE_COEFF_CSTR)) {
		characterData.scaleCoeff = std::stof(xml->getAttributeValue(SCALE_COEFF_CSTR));
	}
	//	volumeGrowth = "0.0"
	if(xml->getAttributeValue(VOLUME_GROWTH_CSTR)) {
		characterData.volumeGrowth = std::stof(xml->getAttributeValue(VOLUME_GROWTH_CSTR));
	}
	//	halfHeight = "0.5"
	if(xml->getAttributeValue(HALF_HEIGHT_CSTR)) {
		characterData.halfHeight = std::stof(xml->getAttributeValue(HALF_HEIGHT_CSTR));
	}
	//	halfSideExtent = "0.5"
	if(xml->getAttributeValue(HALF_SIDE_EXTENT_CSTR)) {
		characterData.halfSideExtent = std::stof(xml->getAttributeValue(HALF_SIDE_EXTENT_CSTR));
	}
	//	halfForwardExtent = "0.5"
	if(xml->getAttributeValue(HALF_FORWARD_EXTENT_CSTR)) {
		characterData.halfForwardExtent = std::stof(xml->getAttributeValue(HALF_FORWARD_EXTENT_CSTR));
	}
	//	radius = "0.5"
	if(xml->getAttributeValue(RADIUS_CSTR)) {
		characterData.radius = std::stof(xml->getAttributeValue(RADIUS_CSTR));
	}
	//	height = "2.0"
	if(xml->getAttributeValue(HEIGHT_CSTR)) {
		characterData.height = std::stof(xml->getAttributeValue(HEIGHT_CSTR));
	}

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==PHYSIC_MATERIAL_STR) {
				material = ParsePhysicsMaterial();
				characterData.material = material;
			}
		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==CHARACTER_CONTROLLER_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	characterData.userData = sceneObject;
	characterController->Initialize(sceneObject, &characterData);
	Physics* physicsComponent = GetComponentType(sceneObject, Physics);
	physicsComponent->m_CharacterController = characterController;
}

void SceneLoader::ParsePlaneCollider(SceneObject* sceneObject, RigidBody* rigidBody) {
	bool exit = false;

	//	<PlaneCollider isTrigger = "false"
	//	direction = "Y-Axis">
	//	<!--direction = "[X-Axis | Y-Axis | Z-Axis]"-->
	//	<PhysicsMaterial></PhysicsMaterial>
	//	<Center x = "0.0" y = "0.0" z = "0.0"/>
	//	</PlaneCollider>

	PlaneCollider* collider = new PlaneCollider();
	ColliderData colliderData;
	colliderData.trigger = (string(xml->getAttributeValue(IS_TRIGGER_CSTR))==TRUE_STR);
	if(xml->getAttributeValue(DIRECTION_CSTR)==X_AXIS_CSTR) {
		colliderData.direction = eGeometryDirection::X_AXIS;
	} else if(xml->getAttributeValue(DIRECTION_CSTR)==Z_AXIS_CSTR) {
		colliderData.direction = eGeometryDirection::Z_AXIS;
	} else {
		colliderData.direction = eGeometryDirection::Y_AXIS;
	}

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==PHYSIC_MATERIAL_STR) {
				colliderData.material = ParsePhysicsMaterial();
			}
			if(string(xml->getNodeName())==CENTER_CSTR) {
				colliderData.center[0] = std::stof(xml->getAttributeValue(X_CSTR));
				colliderData.center[1] = std::stof(xml->getAttributeValue(Y_CSTR));
				colliderData.center[2] = std::stof(xml->getAttributeValue(Z_CSTR));
			}
		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==PLANE_COLLIDER_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	colliderData.parentActor = rigidBody->GetActor();
	collider->Initialize(colliderData);
	Physics* physicsComponent = GetComponentType(sceneObject, Physics);
	physicsComponent->AddCollider(collider);
}

void SceneLoader::ParseBoxCollider(SceneObject* sceneObject, RigidBody* rigidBody) {
	bool exit = false;

	//	<BoxCollider isTrigger = "false">
	//	<PhysicsMaterial name = "nameMaterial"
	//	dynamicFriction = "0.6"
	//	staticFriction = "0.6"
	//	restitution = "0.0">
	//	</PhysicsMaterial>
	//	<Center x = "0.0" y = "0.0" z = "0.0"/>
	//	<Size x = "1.0" y = "1.0" z = "1.0"/>
	//	</BoxCollider>

	BoxCollider* collider = new BoxCollider();
	ColliderData colliderData;
	colliderData.trigger = (string(xml->getAttributeValue(IS_TRIGGER_CSTR))==TRUE_STR);

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==PHYSIC_MATERIAL_STR) {
				colliderData.material = ParsePhysicsMaterial();
			}
			if(string(xml->getNodeName())==CENTER_CSTR) {
				colliderData.center[0] = std::stof(xml->getAttributeValue(X_CSTR));
				colliderData.center[1] = std::stof(xml->getAttributeValue(Y_CSTR));
				colliderData.center[2] = std::stof(xml->getAttributeValue(Z_CSTR));
			}
			if(string(xml->getNodeName())=="Size") {
				colliderData.sizeCollider[0] = std::stof(xml->getAttributeValue(X_CSTR));
				colliderData.sizeCollider[1] = std::stof(xml->getAttributeValue(Y_CSTR));
				colliderData.sizeCollider[2] = std::stof(xml->getAttributeValue(Z_CSTR));
			}
		} break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==BOX_COLLIDER_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	colliderData.parentActor = rigidBody->GetActor();
	collider->Initialize(colliderData);
	Physics* physicsComponent = GetComponentType(sceneObject, Physics);
	physicsComponent->AddCollider(collider);
}

void SceneLoader::ParseSphereCollider(SceneObject* sceneObject, RigidBody* rigidBody) {
	bool exit = false;

	//	<SphereCollider isTrigger = "false"
	//	radius = "0.5">
	//	<PhysicsMaterial></PhysicsMaterial>
	//	<Center x = "0.0" y = "0.0" z = "0.0"/>
	//	</SphereCollider>

	SphereCollider* collider = new SphereCollider();
	ColliderData colliderData;
	colliderData.trigger = (string(xml->getAttributeValue(IS_TRIGGER_CSTR))==TRUE_STR);
	colliderData.radius = std::stof(xml->getAttributeValue(RADIUS_CSTR));

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==PHYSIC_MATERIAL_STR) {
				colliderData.material = ParsePhysicsMaterial();
			}
			if(string(xml->getNodeName())==CENTER_CSTR) {
				colliderData.center[0] = std::stof(xml->getAttributeValue(X_CSTR));
				colliderData.center[1] = std::stof(xml->getAttributeValue(Y_CSTR));
				colliderData.center[2] = std::stof(xml->getAttributeValue(Z_CSTR));
			}
		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==SPHERE_COLLIDER_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	colliderData.parentActor = rigidBody->GetActor();
	collider->Initialize(colliderData);
	Physics* physicsComponent = GetComponentType(sceneObject, Physics);
	physicsComponent->AddCollider(collider);
}

void SceneLoader::ParseCapsuleCollider(SceneObject* sceneObject, RigidBody* rigidBody) {
	bool exit = false;

	//	<CapsuleCollider isTrigger = "false"
	//	radius = "0.5"
	//	heght = "1.0"
	//	direction = "Y-Axis">
	//	<!--direction = "[X-Axis | Y-Axis | Z-Axis]"-->
	//	<PhysicsMaterial></PhysicsMaterial>
	//	<Center x = "0.0" y = "0.0" z = "0.0"/>
	//	</CapsuleCollider>

	CapsuleCollider* collider = new CapsuleCollider();
	ColliderData colliderData;
	colliderData.trigger = (string(xml->getAttributeValue(IS_TRIGGER_CSTR))==TRUE_STR);
	colliderData.radius = std::stof(xml->getAttributeValue(RADIUS_CSTR));
	colliderData.height = std::stof(xml->getAttributeValue(HEIGHT_CSTR));

	if(xml->getAttributeValue(DIRECTION_CSTR)==X_AXIS_CSTR) {
		colliderData.direction = eGeometryDirection::X_AXIS;
	} else if(xml->getAttributeValue(DIRECTION_CSTR)==Z_AXIS_CSTR) {
		colliderData.direction = eGeometryDirection::Z_AXIS;
	} else {
		colliderData.direction = eGeometryDirection::Y_AXIS;
	}

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==PHYSIC_MATERIAL_STR) {
				colliderData.material = ParsePhysicsMaterial();
			}
			if(string(xml->getNodeName())==CENTER_CSTR) {
				colliderData.center[0] = std::stof(xml->getAttributeValue(X_CSTR));
				colliderData.center[1] = std::stof(xml->getAttributeValue(Y_CSTR));
				colliderData.center[2] = std::stof(xml->getAttributeValue(Z_CSTR));
			}
		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==CAPSULE_COLLIDER_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	colliderData.parentActor = rigidBody->GetActor();
	collider->Initialize(colliderData);
	Physics* physicsComponent = GetComponentType(sceneObject, Physics);
	physicsComponent->AddCollider(collider);
}

void SceneLoader::ParseMeshCollider(SceneObject* sceneObject, RigidBody* rigidBody) {
	bool exit = false;

	//	<MeshCollider convex = "false"
	//	isTrigger = "false">
	//	<PhysicsMaterial></PhysicsMaterial>
	//	<!--Pointer to : <Mesh></Mesh>-->
	//	<Mesh path = "./Resources/Giant/"></Mesh>
	//	</MeshCollider>

	MeshCollider* collider = new MeshCollider();
	ColliderData colliderData;

	colliderData.convex = (string(xml->getAttributeValue(CONVEX_CSTR))==TRUE_STR);

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==PHYSIC_MATERIAL_STR) {
				colliderData.material = ParsePhysicsMaterial();
			}
			if(string(xml->getNodeName())==MESH_COMPONENT_STR) {
				colliderData.meshPath = xml->getAttributeValue(PATH_CSTR);
			}
		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==MESH_COLLIDER_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	colliderData.parentActor = rigidBody->GetActor();
	collider->Initialize(sceneObject, colliderData);
	Physics* physicsComponent = GetComponentType(sceneObject, Physics);
	physicsComponent->AddCollider(collider);
}

void SceneLoader::ParseTerrainCollider(SceneObject* sceneObject, RigidBody* rigidBody) {
	bool exit = false;

	//	<TerrainCollider>
	//	<PhysicsMaterial></PhysicsMaterial>
	//	<!--Read Data directly from this file-->
	//	<HeightFieldData></HeightFieldData>
	//	<!--Read Data from another file-->
	//	<HeightFieldFile path = EMPTY></HeightFieldFile>
	//	</TerrainCollider>

	TerrainCollider* collider = new TerrainCollider();
	ColliderData colliderData;

	while(!exit) {
		xml->read();
		switch(xml->getNodeType()) {
		case EXN_ELEMENT: {
			if(string(xml->getNodeName())==PHYSIC_MATERIAL_STR) {
				colliderData.material = ParsePhysicsMaterial();
			}
			if(string(xml->getNodeName())==HEIGHT_FIELD_DATA_CSTR) {
				// TODO
			}
			if(string(xml->getNodeName())==HEIGHT_FIELD_FILE_CSTR) {
				// TODO
			}
		}break;
		case EXN_ELEMENT_END:{
			if(string(xml->getNodeName())==TERRAIN_COLLIDER_STR) {
				exit = true;
			}
		} break;
		default: break;
		}
	}

	colliderData.parentActor = rigidBody->GetActor();
	collider->Initialize(colliderData);
	Physics* physicsComponent = GetComponentType(sceneObject, Physics);
	physicsComponent->AddCollider(collider);
}

//void SceneLoader::ParseAudio(SceneObject* sceneObject) {
//	bool exit = false;
//
//	bool isLooped = string(xml->getAttributeValue("isLooped"))==TRUE_STRING;
//	bool is3D = string(xml->getAttributeValue("is3D"))==TRUE_STRING;
//	string path = string(xml->getAttributeValue(PATH_CSTR));
//
//	//	Create and set the new audio component
//	Audio* audio = new Audio(path, sceneObject, is3D, isLooped);
//	sceneObject->SetComponent(eComponentType::AUDIO_COMPONENT, audio);
//
//	while(!exit) {
//		xml->read();
//		switch(xml->getNodeType()) {
//		case EXN_ELEMENT: break;
//		case EXN_ELEMENT_END:{
//			if(string(xml->getNodeName())=="Audio") {
//				exit = true;
//			}
//		} break;
//		default: break;
//		}
//	}
//}

//void SceneLoader::ParseParticles(SceneObject* sceneObject) {
//	bool exit = false;
//
//	//	Create and set the new audio component
//	Particles* particles = new Particles(sceneObject);
//	sceneObject->SetComponent(eComponentType::PARTICLES, particles);
//	DgParticleData particlesData;
//	
//	// FluidInit
//	//particlesData.particleCount = 1000;
//	//particlesData.emitterWidth = 1.5f;
//	//particlesData.emitterVelocity = 9.0f;
//	//particlesData.emitterSpacing); // 0.81f
//	//particlesData.randomAngle //0.0f
//	//particlesData.randomPos //PxVec3(0.0f, 0.0f, 0.0f)
//
//	while(!exit) {
//		xml->read();
//		switch(xml->getNodeType()) {
//		case EXN_ELEMENT: {
//			if(string(xml->getNodeName())=="ParticleSystemType") {
//				string value = xml->getAttributeValue(VALUE);
//				if(value=="PARTICLE") {
//					particlesData.systemType = ParticleSystemType::PARTICLE_SYSTEM;
//				} else if(value=="FLUID") {
//					particlesData.systemType = ParticleSystemType::FLUID_SYSTEM;
//				} else if(value=="DRAIN") {
//					particlesData.systemType = ParticleSystemType::DRAIN_SYSTEM;
//				} else {
//					particlesData.systemType = ParticleSystemType::NULL_SYSTEM;
//				}
//			}
//			if(string(xml->getNodeName())=="ParticleCount") {
//				int value = std::stoi(xml->getAttributeValue(VALUE));
//				particlesData.particleCount = value;
//			}
//			if(string(xml->getNodeName())=="UseLifetime") {
//				bool value = string(xml->getAttributeValue(VALUE))==TRUE_STRING;
//				particlesData.useLifetime = value;
//			}
//			if(string(xml->getNodeName())=="Lifetime") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.lifetime = value;
//			}
//			if(string(xml->getNodeName())=="MaxParticles") {
//				int value = std::stoi(xml->getAttributeValue(VALUE));
//				particlesData.maxParticles = value;
//			}
//			if(string(xml->getNodeName())=="Restitution") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.restitution = value;
//			}
//			if(string(xml->getNodeName())=="Viscosity") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.viscosity = value;
//			}
//			if(string(xml->getNodeName())=="Stiffness") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.stiffness = value;
//			}
//			if(string(xml->getNodeName())=="DynamicFriction") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.dynamicFriction = value;
//			}
//			if(string(xml->getNodeName())=="ParticleDistance") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.particleDistance = value;
//			}
//			if(string(xml->getNodeName())=="GridSize") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.gridSize = value;
//			}
//			if(string(xml->getNodeName())=="RestOffset") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.restOffset = value;
//			}
//			if(string(xml->getNodeName())=="ContactOffset") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.contactOffset = value;
//			}
//			if(string(xml->getNodeName())=="MaxMotionDistance") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.maxMotionDistance = value;
//			}
//			if(string(xml->getNodeName())=="Damping") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.damping = value;
//			}
//			if(string(xml->getNodeName())=="ExternalAcceleration") {
//				particlesData.externalAcceleration = PxVec3(
//					std::stof(xml->getAttributeValue(X)),
//					std::stof(xml->getAttributeValue(Y)),
//					std::stof(xml->getAttributeValue(Z)));
//			}
//			if(string(xml->getNodeName())=="Emitter") {
//				// This is for custom emitter
//				//DgParticleEmitter* emitter;
//				//string value = xml->getAttributeValue(VALUE);
//				//particlesData. = value;
//			}
//			if(string(xml->getNodeName())=="EmitterShape") {
//				string value = xml->getAttributeValue(VALUE);
//				if(value == "ELLIPSE") {
//					particlesData.emitterShape = DgParticleEmitter::Shape::eELLIPSE;
//				} else {
//					particlesData.emitterShape = DgParticleEmitter::Shape::eRECTANGLE;
//				}
//			}
//			if(string(xml->getNodeName())=="EmitterTransform") {
//				// This is null for now, retrieve it from Transform
//				//PxVec3 translation(
//				//	std::stof(xml->getAttributeValue("xT")),
//				//	std::stof(xml->getAttributeValue("yT")),
//				//	std::stof(xml->getAttributeValue("zT")));
//				//PxQuat rotationX(ToRadian(std::stof(xml->getAttributeValue("xR"))), PxVec3(1.0f, 0.0f, 0.0f);
//				//PxQuat rotationY(ToRadian(std::stof(xml->getAttributeValue("yR"))), PxVec3(1.0f, 0.0f, 0.0f);
//				//PxQuat rotationZ(ToRadian(std::stof(xml->getAttributeValue("zR"))), PxVec3(1.0f, 0.0f, 0.0f);
//				//particlesData.emitterTransform = PxTransform(translation, rotationX*rotationY*rotationZ);
//			}
//			if(string(xml->getNodeName())=="EmitterExtentX") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.emitterExtentX = value;
//			}
//			if(string(xml->getNodeName())=="EmitterExtentY") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.emitterExtentY = value;
//			}
//			if(string(xml->getNodeName())=="EmitterSpacing") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.emitterSpacing = value;
//			}
//			if(string(xml->getNodeName())=="EmitterRandomAngle") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.emitterRandomAngle = value;
//			}
//			if(string(xml->getNodeName())=="EmitterRandomPos") {
//				particlesData.emitterRandomPos = PxVec3(
//					std::stof(xml->getAttributeValue(X)),
//					std::stof(xml->getAttributeValue(Y)),
//					std::stof(xml->getAttributeValue(Z)));
//			}
//			if(string(xml->getNodeName())=="EmitterRate") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.emitterRate = value;
//			}
//			if(string(xml->getNodeName())=="EmitterMaxRate") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.emitterMaxRate = value;
//			}
//			if(string(xml->getNodeName())=="EmitterWidth") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.emitterWidth = value;
//			}
//			if(string(xml->getNodeName())=="EmitterVelocity") {
//				float value = std::stof(xml->getAttributeValue(VALUE));
//				particlesData.emitterVelocity = value;
//			}
//			if(string(xml->getNodeName())=="BoxCollider") {
//				particlesData.drainType = DrainType::BOX_DRAIN;
//				particlesData.boxHalfExtensions = PxVec3(
//					std::stof(xml->getAttributeValue("hx")),
//					std::stof(xml->getAttributeValue("hy")),
//					std::stof(xml->getAttributeValue("hz")));
//			}
//			if(string(xml->getNodeName())=="SphereCollider") {
//				particlesData.drainType = DrainType::SPHERE_DRAIN;
//				particlesData.sphereRadius = std::stof(xml->getAttributeValue(RADIUS_CSTR));
//			}
//			if(string(xml->getNodeName())=="PlaneCollider") {
//				particlesData.drainType = DrainType::PLANE_DRAIN;
//				if(xml->getAttributeValue(DIRECTION_CSTR)==X_AXIS_CSTR) {
//					particlesData.planeDirection = DgGeometryDirection::X_AXIS;
//				} else if(xml->getAttributeValue(DIRECTION_CSTR)==Z_AXIS_CSTR) {
//					particlesData.planeDirection = DgGeometryDirection::Z_AXIS;
//				} else {
//					particlesData.planeDirection = DgGeometryDirection::Y_AXIS;
//				}
//			}
//			if(string(xml->getNodeName())=="ParticleSystemRenderType") {
//				string value = xml->getAttributeValue(VALUE);
//				if(value=="BILLBOARD") {
//					particlesData.renderType = ParticleSystemRenderType::BILLBOARD_RENDER;
//				} else if(value=="MESH") {
//					particlesData.renderType = ParticleSystemRenderType::MESH_RENDER;
//				} else {
//					particlesData.renderType = ParticleSystemRenderType::NULL_RENDER;
//				}
//			}
//			if(string(xml->getNodeName())=="PathBillboardTexture") {
//				string value = xml->getAttributeValue(VALUE);
//				particlesData.pathBillboardTexture = value;
//			}
//			if(string(xml->getNodeName())=="PathMesh") {
//				string value = xml->getAttributeValue(VALUE);
//				particlesData.pathMesh = value;
//			}
//		}break;
//		case EXN_ELEMENT_END:{
//			if(string(xml->getNodeName())=="Particles") {
//				exit = true;
//			}
//		} break;
//		default: break;
//		}
//	}
//
//	//particlesData.systemType = ParticleSystemType::PARTICLE_SYSTEM;
//	//particlesData.renderType = ParticleSystemRenderType::BILLBOARD_RENDER;
//	particles->initParticles(particlesData);
//}
