#include "Application.h"
#include "ModuleFileSystem.h"

#include "ModuleScene.h"
#include "ModuleResource.h"
#include "ResourceShader.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceScene.h"
#include "ResourceModel.h"
#include "ResourceFolder.h"
#include "ResourceParticleSystem.h"

#include "ImporterMaterials.h"
#include "ImporterMesh.h"
#include "ImporterScene.h"
#include "ImporterTexture.h"
#include "ImporterShader.h"

#include "JsonConfig.h"
#include "PathNode.h"

#include "Dependencies/Assimp/include/scene.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Start()
{
	LoadAssets();
	
	return true;
}

bool ModuleResources::CleanUp()
{
	return false;
}

void ModuleResources::LoadAssets()
{
	std::vector<std::string> extension;
	extension.push_back("meta");
	extension.push_back("scene");
	uint32 modelFolder = 0;
	PathNode scenes = App->fileSystem->GetAllFiles("Assets/Scenes", nullptr, nullptr);
	PathNode models = App->fileSystem->GetAllFiles("Assets", nullptr, &extension);
	IterateAssets(models, modelFolder);
	IterateScenes(scenes);
	
}

bool ModuleResources::IterateAssets(PathNode node, uint32 ID)
{
	bool isNew = true;
	std::string metaPath = node.path + ".meta";
	if (App->fileSystem->Exists(metaPath.c_str()))
	{
		char* buffer = nullptr;
		uint size = App->fileSystem->Load(metaPath.c_str(), &buffer);
		if (size > 0)
		{
			JsonConfig jsonMeta(buffer);

			std::map<uint32, Resource*>::iterator iterator = importedResources.find(jsonMeta.GetNumber("UID"));
			if (iterator != importedResources.end())
			{
				ID = iterator->first;
				

				uint32 LastMod = App->fileSystem->GetLastModTime(node.path.c_str());
				uint32 configDate = jsonMeta.GetNumber("ModDate");
				if (LastMod != configDate)
					ImportFile(node.path.c_str());
			}
			else
			{
				string name = jsonMeta.GetString("Name");
				uint32 UID = jsonMeta.GetNumber("UID");
				ResourceType type = GetTypefromString(jsonMeta.GetString("Type"));
				Resource* resource = new Resource(type,node.path.c_str(), name.c_str(), UID);
				resource->libraryFile = jsonMeta.GetString("Library file");
				if (resource->type == ResourceType::Model) App->scene->sceneUID = resource->UID;
				if(resource->type == ResourceType::Model)
				{
					ArrayConfig resourceInModel = jsonMeta.GetArray("Resources in Models");
					for (uint i = 0; i < resourceInModel.GetSize(); i++)
					{
						JsonConfig metaInModel = resourceInModel.GetNode(i);
						resource->resourcesInModels.push_back(metaInModel.GetNumber("UID"));
						if (resource->type == ResourceType::Folder) break;
						string name = metaInModel.GetString("Name");
						uint32 UID = metaInModel.GetNumber("UID");
						ResourceType type = GetTypefromString(metaInModel.GetString("Type"));

						Resource* dataInModel = new Resource(type, node.path.c_str(), name.c_str(), UID);
						dataInModel->libraryFile = metaInModel.GetString("Library file");
						importedResources[dataInModel->UID] = dataInModel;
					}
				}

				importedResources[resource->UID] = resource;
				ID = resource->UID;

			}
			
		}
		RELEASE_ARRAY(buffer);
	}
	else
	{
		ID = ImportFile(node.path.c_str());
		if (ID != 0)
		{
			LOG("Succesfully imported from: %s", node.path.c_str());
		}
		else if (ID == 0)
		{
			LOG("ERROR: Couldn't import the file: %s", node.path.c_str());
		}
	}

	if (!node.isFile && !node.isLeaf)
	{
		std::vector<uint32> newChildren;
		for (uint i = 0; i < node.children.size(); i++)
		{
			uint32 childID = 0;
			if (IterateAssets(node.children[i], childID))
			{
				newChildren.push_back(childID);
			}
		}
	}

	return isNew;
}

bool ModuleResources::IterateScenes(PathNode node, uint32 ID)
{
	if (App->fileSystem->Exists(node.path.c_str()))
	{
		char* buffer = nullptr;
		uint size = App->fileSystem->Load(node.path.c_str(), &buffer);
		if (size > 0)
		{
			JsonConfig jsonMeta(buffer);

			std::map<uint32, Resource*>::iterator iterator = importedResources.find(jsonMeta.GetNumber("UID"));
			if (iterator != importedResources.end())
			{
				ID = iterator->first;


				uint32 LastMod = App->fileSystem->GetLastModTime(node.path.c_str());
				uint32 configDate = jsonMeta.GetNumber("ModDate");
				if (LastMod != configDate)
					ImportFile(node.path.c_str());
			}
			else
			{
				string name = jsonMeta.GetString("Name");
				uint32 UID = jsonMeta.GetNumber("UID");
				ResourceType type = GetTypefromString(jsonMeta.GetString("Type"));
				Resource* resource = new Resource(type, node.path.c_str(), name.c_str(), UID);
				resource->libraryFile = jsonMeta.GetString("Library file");
				importedResources[resource->UID] = resource;
			}

		}
		RELEASE_ARRAY(buffer);
	}
	
	if (!node.isFile && !node.isLeaf)
	{
		std::vector<uint32> newChildren;
		for (uint i = 0; i < node.children.size(); i++)
		{
			uint32 childID = 0;
			if (IterateScenes(node.children[i], childID))
			{
				newChildren.push_back(childID);
			}
		}
	}
	return true;
}



uint32 ModuleResources::Find(const char* file)
{
	return uint32();
}

uint32 ModuleResources::ImportFile(const char* assetsFile)
{
	ResourceType type = GetTypeFromFile(assetsFile);

	Resource* resource = new Resource();
	resource = 	CreateNewResource(assetsFile, type,"",resource->UID);
	char* buffer = nullptr;
	uint64 fileSize = 0;
	if (type != ResourceType::Folder)
		fileSize = App->fileSystem->Load(assetsFile, &buffer);

	switch (type)
	{
	case ResourceType::Texture: 

		Importer::TextureImporter::ImportTexture((ResourceTexture*)resource, buffer, fileSize);
		SaveResource((ResourceTexture*)resource);

		break;
	case ResourceType::Model:
		
		LoadModel(buffer, fileSize, (ResourceModel*)resource);
		SaveResource((ResourceModel*)resource);

		break;

	case ResourceType::Shader:
		
		LoadShader(buffer, fileSize, (ResourceShader*)resource);
		break;
	}
	

	RELEASE_ARRAY(buffer);

	return resource->GetUID();
}

void ModuleResources::LoadShader(const char* buffer, uint size, ResourceShader* shader)
{
	Importer::ShaderImporter::Import(shader->assetsFile.c_str(), shader);	
	SaveResource(shader);
}


void ModuleResources::LoadModel(const char* buffer, uint size, ResourceModel* model)
{
	const aiScene* aiScene = Importer::ModelImporter::ImportAssimpScene(buffer, size);
	Importer::ModelImporter::ImportScene(aiScene, model);

	std::vector<uint32> meshes;

	for (uint i = 0; i < aiScene->mNumMeshes; i++)
	{
		std::string name;
		for (uint h = 0; h < model->models.size(); h++)
		{
			if (model->models[h].meshID == i) name = model->models[h].name;
		}
		aiScene->mMeshes[i]->mName = name;
		ResourceMesh* resourceMesh = (ResourceMesh*)CreateNewResource(model->GetAssetsFile().c_str(), ResourceType::Mesh, aiScene->mMeshes[i]->mName.C_Str());
		Importer::MeshImporter::LoadMeshes(resourceMesh, aiScene->mMeshes[i]);
		SaveResource(resourceMesh);

		meshes.push_back(resourceMesh->GetUID());
		model->resourcesInModels.push_back(resourceMesh->GetUID());
	}
	std::vector<uint32> materials;
	for (uint i = 0; i < aiScene->mNumMaterials; i++)
	{
		std::string name;
		aiScene->mMaterials[i]->Get(AI_MATKEY_NAME, name);
			
		ResourceMaterial* resourceMaterial = (ResourceMaterial*)CreateNewResource(model->GetAssetsFile().c_str(), ResourceType::Material, aiScene->mMeshes[i]->mName.C_Str());


		Importer::MaterialsImporter::ImportMaterial(aiScene->mMaterials[i], resourceMaterial);
		SaveResource(resourceMaterial);
		materials.push_back(resourceMaterial->GetUID());
		model->resourcesInModels.push_back(resourceMaterial->GetUID());
	}

	for (uint i = 0; i < model->models.size(); i++)
	{
		if (model->models[i].meshID != -1)
		{
			model->models[i].meshID = meshes[model->models[i].meshID];
		}
		else
		{
			model->models[i].meshID = 0;
		}
		if (model->models[i].materialID != -1)
		{
			model->models[i].materialID = materials[model->models[i].materialID];
		}
		else
		{
			model->models[i].materialID = 0;
		}
	}

}

std::map<uint32, ResourceTexture*> ModuleResources::GetTexturesInMemory()
{
	ResourceTexture* tempTexture = new ResourceTexture();
	std::map<uint32, ResourceTexture*> texturesInMemory;
	std::map<uint32, Resource*>::iterator item;
	for (item = importedResources.begin(); item != importedResources.end(); item++)
	{
		if (item->second->type == ResourceType::Texture)
		{
			tempTexture = (ResourceTexture*)item->second;
			if (tempTexture->id > MAX_TEXTURES) tempTexture->id = 0;
			texturesInMemory[tempTexture->UID] = tempTexture;
		}

	}
	return texturesInMemory;
}


std::map<uint32, ResourceShader*> ModuleResources::GetShadersInMemory()
{

	ResourceShader* tempShader = new ResourceShader();
	std::map<uint32, ResourceShader*> shadersInMemory;
	std::map<uint32, Resource*>::iterator item;
	for (item = importedResources.begin(); item != importedResources.end(); item++)
	{
		if (item->second->type == ResourceType::Shader && item->second->name != "")
		{
			tempShader = (ResourceShader*)item->second;

			if (tempShader->shaderProgramID > MAX_SHADERS)
			{
				tempShader->shaderProgramID = 0;
				tempShader->fragmentID = 0;
				tempShader->vertexID = 0;
			}
			shadersInMemory[tempShader->UID] = tempShader;
		}
		
	}		
	return shadersInMemory;
}

void ModuleResources::UnloadResource(uint32 UID)
{
	std::map<uint32, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{
		RELEASE(it->second);
		resources.erase(it);
	}

}

void ModuleResources::DeleteResource(uint32 UID)
{
	UnloadResource(UID);

	std::map<uint32, Resource*>::iterator it = importedResources.find(UID);
	if (it != importedResources.end())
	{
		App->fileSystem->Remove(it->second->libraryFile.c_str());
		importedResources.erase(it);
	}

}


Resource* ModuleResources::CreateNewResource(const char* assetsFile, ResourceType type, const char* name, uint32 UID)
{
	Resource* resource = nullptr;
	
	if (name == nullptr)
	{
		LOG("Unknown resource Name");
	}
	switch (type)
	{
	case ResourceType::Texture: 
		resource = new ResourceTexture(assetsFile, TEXTURES_PATH, name, UID);
		break;
	case ResourceType::Mesh:

		resource = new ResourceMesh(assetsFile, MESHES_PATH, name, UID);
		break;
	case ResourceType::Scene:

		resource = new ResourceScene(assetsFile, SCENES_PATH, name, UID);
		break;
	case ResourceType::Material:

		resource = new ResourceMaterial(assetsFile, MATERIALS_PATH, name, UID);
		break;
	case ResourceType::Folder:
		resource = new ResourceFolder(assetsFile, FOLDERS_PATH, name, UID);
		
		break;
	case ResourceType::Model:
		resource = new ResourceModel(assetsFile, MODELS_PATH, name, UID);
		break;
	case ResourceType::Shader:
		resource = new ResourceShader(assetsFile, SHADERS_PATH, name, UID);
		break;
	case ResourceType::ParticleSystem:
		resource = new ResourceParticleSystem(assetsFile, PARTICLE_SYSTEM_PATH, name, UID);
		break;
	default:
		resource = new Resource();
		break;
	}

	if (resource->type != ResourceType::None)
	{
		resource->libraryFile.append(std::to_string(resource->GetUID()));
		if (resource->type != ResourceType::Scene) resource->libraryFile.append(ASE_EXTENSION);
		else resource->libraryFile.append(".scene");
		importedResources[resource->UID] = resource;


	}

	return resources[resource->UID] = resource;

}

void ModuleResources::SaveMeta(Resource* resource)
{
	string name;
	string extension;
	App->fileSystem->SplitFilePath(resource->GetAssetsFile().c_str(), nullptr, &name, &extension);
	resource->name = name;
	JsonConfig jsonConfig;
	jsonConfig.SetNumber("UID", resource->GetUID());
	jsonConfig.SetString("Name", resource->name.c_str());
	jsonConfig.SetString("Extension", extension);
	jsonConfig.SetString("Library file", resource->GetLibraryFile());
	jsonConfig.SetString("Type", GetStringFromResource(resource).c_str());

	uint64 modDate = App->fileSystem->GetLastModTime(resource->assetsFile.c_str());
	jsonConfig.SetNumber("ModDate", modDate);

	std::string path;
	if (resource->type != ResourceType::Scene) path = resource->GetAssetsFile().append(".meta");
	else path = resource->GetAssetsFile();

	if (resource->type == ResourceType::Model)
	{
		
		ArrayConfig resourceInModel = jsonConfig.SetArray("Resources in Models");
		for (uint i = 0; i < resource->resourcesInModels.size(); ++i)
		{
			Resource* newresource = importedResources[resource->resourcesInModels[i]];
			JsonConfig& node = resourceInModel.AddNode();
			node.SetNumber("UID", newresource->GetUID());
			node.SetString("Name", newresource->name.c_str());
			node.SetString("Library file", newresource->GetLibraryFile());
			node.SetString("Type", GetStringFromResource(newresource).c_str());
		}
	}

	char* buffer;
	uint size = jsonConfig.SerializeConfig(&buffer);
	if (size > 0)
	{
		App->fileSystem->Save(path.c_str(), buffer, size);
	}
}

void ModuleResources::SaveResource(Resource* resource)
{
	char* buffer = nullptr;
	uint size = 0;

	switch (resource->GetType())
	{
	//case(ResourceType::FOLDER): { size = Importer::Folders::Save((R_Folder*)resource, &buffer); break; }
	case(ResourceType::Mesh): { size = Importer::MeshImporter::Save((ResourceMesh*)resource, &buffer); break; }
	case(ResourceType::Texture): { size = Importer::TextureImporter::Save((ResourceTexture*)resource, &buffer); break; }
	case(ResourceType::Material): { size = Importer::MaterialsImporter::Save((ResourceMaterial*)resource, &buffer); break; }
	case(ResourceType::Scene): { size = Importer::SceneImporter::Save((ResourceScene*)resource, &buffer);break; }
	case(ResourceType::Model):{ size = Importer::ModelImporter::Save((ResourceModel*)resource, & buffer); break; }
	case(ResourceType::Shader): {size = Importer::ShaderImporter::Save((ResourceShader*)resource, &buffer); break; }
	}

	if (size > 0)
	{
		if(resource->type != ResourceType::Mesh && resource->type != ResourceType::Material) 
			SaveMeta(resource);
		App->fileSystem->Save(resource->GetLibraryFile().c_str(), buffer, size);
	}
}

Resource* ModuleResources::LoadResource(uint32 UID)
{
	
	Resource* resource = nullptr;
	resource = GetResource(UID);

	if (!resource) 
		resource = GetResourceInMemory(UID);

	if (!resource)
	{
		LOG("ERROR The resource doen't have a library file");
		return nullptr;
	}

	Resource* tempResource = CreateNewResource(resource->assetsFile.c_str(),resource->GetType(),resource->name.c_str(),resource->UID);
	char* buffer = nullptr;
	uint size = App->fileSystem->Load(resource->GetLibraryFile().c_str(), &buffer);
	if (size == 0)
	{
		LOG("ERROR The resource doen't have a library file");
		//RELEASE(resource);
		return nullptr;
	}
	switch (resource->GetType())
	{
	case (ResourceType::Mesh):
		Importer::MeshImporter::Load((ResourceMesh*)tempResource, buffer);
		break;
	case (ResourceType::Material):
		Importer::MaterialsImporter::Load((ResourceMaterial*)tempResource, buffer);
		break;
	case (ResourceType::Model):
		Importer::ModelImporter::Load((ResourceModel*)tempResource, buffer);
		break;
	case(ResourceType::Scene):
		 Importer::SceneImporter::Load((ResourceScene*)tempResource, buffer);
		break;
	case(ResourceType::Texture):
		Importer::TextureImporter::Load((ResourceTexture*)tempResource, buffer,size);
		break;
	case ResourceType::Shader:
		Importer::ShaderImporter::Load((ResourceShader*)tempResource, buffer, size);
		break;
	}



	return tempResource;
}

Resource* ModuleResources::GetResource(uint32 UID)
{
	std::map<uint32, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{
		return it->second;
	}
	return nullptr;
}




ResourceType ModuleResources::GetTypeFromFile(const char* path) const
{
	std::string extension;
	App->fileSystem->SplitFilePath(path, nullptr, nullptr, &extension);

	static_assert(static_cast<int>(ResourceType::None) == 8, "Code Needs Update");

	if (extension == "tga" || extension == "png" || extension == "jpg" || extension == "TGA" || extension == "PNG" || extension == "JPG")
		return ResourceType::Texture;
	if (extension == "scene")
		return ResourceType::Scene;
	if (extension == "FBX" || extension == "fbx")
		return ResourceType::Model;
	if (extension == "shader")
		return ResourceType::Shader;

	return App->fileSystem->IsDirectory(path) ? ResourceType::Folder : ResourceType::None;
}

ResourceType ModuleResources::GetTypefromString(std::string typeString) const
{
	if (typeString == "Folder") return ResourceType::Folder;
	else if (typeString == "Material") return ResourceType::Material;
	else if (typeString == "Mesh") return ResourceType::Mesh;
	else if (typeString == "Model") return ResourceType::Model;
	else if (typeString == "Scene") return ResourceType::Scene;
	else if (typeString == "Texture") return ResourceType::Texture;
	else if (typeString == "Shader") return ResourceType::Shader;
	return ResourceType::None;
}

std::string ModuleResources::GetStringFromResource(Resource* resource)
{
	string path;
	switch (resource->type)
	{
	case ResourceType::Folder:
		return "Folder";
		break;
	case ResourceType::Material:
		App->fileSystem->SplitFilePath(resource->libraryFile.c_str(), &path, nullptr, nullptr);
		if (path == TEXTURES_PATH)
		{
			return "Texture";
		}

		else return "Material";
		break;
	case ResourceType::Mesh:
		return "Mesh";
		break;
	case ResourceType::Model:
		return "Model";
		break;
	case ResourceType::Scene:
		return "Scene";	
		break;
	case ResourceType::Texture:
		return "Texture";
		break;
	case ResourceType::Shader:
		return "Shader";
		break;
	}
	
	return "None";
}

Resource* ModuleResources::GetResourceInMemory(uint32 UID)
{
	Resource* resource = nullptr;
	std::map<uint32, Resource*>::iterator item = importedResources.find(UID);
	if (item != importedResources.end()) resource = item->second;
	
	return resource;
}

ResourceShader* ModuleResources::GetShader(const char* name)
{

	ResourceShader* tempShader = new ResourceShader();
	std::map<uint32, Resource*>::iterator item;

	for (item = importedResources.begin(); item != importedResources.end(); item++)
	{
		if (item->second->type == ResourceType::Shader && item->second->name == name)
		{
			return (ResourceShader*)App->resources->LoadResource(item->second->UID);	
		}

	}

	LOG("ERROR The shader doesn't exist");
	return nullptr;
}


ResourceTexture* ModuleResources::GetTexture(const char* name)
{
	ResourceTexture* tempTexture = new ResourceTexture();
	std::map<uint32, Resource*>::iterator item;

	for (item = importedResources.begin(); item != importedResources.end(); item++)
	{
		if (item->second->type == ResourceType::Texture && item->second->name == name)
		{
			tempTexture = (ResourceTexture*)item->second;

			if (tempTexture->id > MAX_TEXTURES)
			{
				tempTexture = (ResourceTexture*)App->resources->LoadResource(tempTexture->UID);

				return tempTexture;
			}
			else
			{
				return tempTexture;
			}
		}

	}

	LOG("ERROR The shader doesn't exist");
	return nullptr;
}

