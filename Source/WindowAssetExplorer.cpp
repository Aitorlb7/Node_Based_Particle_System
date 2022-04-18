#include "WindowAssetExplorer.h"
#include "SceneWindow.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "JsonConfig.h"

#include "ImporterMaterials.h"
#include "ImporterTexture.h"

#include "ResourceTexture.h"
#include "ResourceMaterial.h"

#include "Dependencies/ImGui/imgui.h"

WindowAssetExplorer::WindowAssetExplorer(SceneWindow* parent, ImGuiWindowClass* windowClass, int ID, bool isActive): Window(parent, windowClass,ID,"Assets Explorer Window", isActive)
{
}

WindowAssetExplorer::~WindowAssetExplorer()
{
}

void WindowAssetExplorer::Start()
{
	LoadIcons();
	UpdateAssetExplorer();
	currentFolder = assetsFolder;
}

void WindowAssetExplorer::Draw()
{
	if (!isActive) return;

	AssetExplorerWindow();
}

void WindowAssetExplorer::CleanUp()
{
}


void WindowAssetExplorer::LoadIcons()
{
	modelIcon = new ResourceTexture();
	defaultIcon = new ResourceTexture();
	folderIcon = new ResourceTexture();
	returnIcon = new ResourceTexture();

	char* buffer = nullptr;
	uint size = App->fileSystem->Load("Assets/Icons/FBX_Icon_x4.png", &buffer);
	if (size > 0) Importer::TextureImporter::ImportTexture(modelIcon, buffer, size);
	RELEASE_ARRAY(buffer);

	size = 0;
	size = App->fileSystem->Load("Assets/Icons/ASE_Icon_x4.png", &buffer);
	if (size > 0) Importer::TextureImporter::ImportTexture(defaultIcon, buffer, size);
	RELEASE_ARRAY(buffer);

	size = 0;
	size = App->fileSystem->Load("Assets/Icons/Folder_Icon_x4.png", &buffer);
	if (size > 0) Importer::TextureImporter::ImportTexture(folderIcon, buffer, size);
	RELEASE_ARRAY(buffer);

	size = 0;
	size = App->fileSystem->Load("Assets/Icons/Return_Icon_x1.png", &buffer);
	if (size > 0) Importer::TextureImporter::ImportTexture(returnIcon, buffer, size);
	RELEASE_ARRAY(buffer);
}


void WindowAssetExplorer::UpdateAssetExplorer()
{
	std::vector<std::string> ignore_ext;
	ignore_ext.push_back("meta");
	assetsFolder = App->fileSystem->GetAllFiles("Assets", nullptr, &ignore_ext);

}

void WindowAssetExplorer::AssetExplorerWindow()
{
	Timer timer;
	timer.Start();
	if (updateTimer.ReadSec() > updateTime)
	{
		UpdateAssetExplorer();
		updateTimer.Start();
	}
	double time = timer.Read();

	ImGui::Begin("Assets Tree");
	AssetsTree(assetsFolder);
	ImGui::End();
	ImGui::Begin("Assets Explorer");
	AssetsExplorer(currentFolder);
	ImGui::End();
}

void WindowAssetExplorer::AssetsTree(PathNode& assetFolder)
{
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen
		| ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (assetFolder.IsLastFolder()) treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	if (currentFolder == assetFolder) treeFlags |= ImGuiTreeNodeFlags_Selected;

	if (!assetFolder.isFile) //This way we only show folders in the Tree
	{
		if (ImGui::TreeNodeEx(assetFolder.localPath.c_str(), treeFlags, assetFolder.localPath.c_str()))
		{
			if (ImGui::IsItemClicked())
			{
				nextFolder = previousFolder;
				currentFolder = assetFolder;
			}
			if (!assetFolder.IsLastFolder())
			{
				for (uint i = 0; i < assetFolder.children.size(); i++)
				{
					AssetsTree(assetFolder.children[i]);
				}
				ImGui::TreePop();
			}
		}
	}
}

void WindowAssetExplorer::AssetsExplorer(PathNode& assetFolder)
{
	//if(assetFolder.path == "Assets") previousFolder = assetFolder;

	nextFolder = previousFolder;

	uint row = 0;
	uint offset = 50;
	uint column = (float)(ImGui::GetWindowWidth() / (float)(iconSize + offset)); // Window.w / Item.w
	ImVec2 cursor = ImGui::GetCursorPos();
	ImVec2 flipV = ImVec2(0.0f, 1.0f);
	ImVec2 flipH = ImVec2(1.0f, 0.0f);

	ImGui::Text(assetFolder.localPath.c_str());

	ImGui::Separator();

	ImGui::BeginChild(1);


	for (uint i = 0; i < assetFolder.children.size(); i++)
	{
		ImGui::PushID(i);

		//Get Id from meta file and the Resource from that ID
		std::string meta = assetFolder.children[i].path + ".meta";
		char* buffer = nullptr;
		uint size = App->fileSystem->Load(meta.c_str(), &buffer);
		uint32 UID = 0;


		ImGui::SetCursorPosX((i - (row * column)) * (iconSize + offset) + offset);
		ImGui::SetCursorPosY(row * (iconSize + offset));

		if (size > 0)
		{
			UID = JsonConfig(buffer).GetNumber("UID");
			resource = App->resources->GetResourceInMemory(UID);

			textureIcon = (ResourceTexture*)resource;

			if (textureIcon->id > MAX_TEXTURES) textureIconLoaded = false;

			if (resource->type == ResourceType::Texture && !textureIconLoaded) App->resources->LoadResource(UID);

			switch (resource->type)
			{
			case ResourceType::Model:
				ImGui::ImageButton((ImTextureID)modelIcon->id, ImVec2(iconSize, iconSize), flipV, flipH, -1, ImVec4(0, 0, 0, 0), App->editor->ExplorerIconsTint);
				break;
			case ResourceType::Texture:
				ImGui::ImageButton((ImTextureID)textureIcon->id, ImVec2(iconSize, iconSize), flipV, flipH);
				break;
			case ResourceType::Folder:
				ImGui::ImageButton((ImTextureID)folderIcon->id, ImVec2(iconSize, iconSize), flipV, flipH, -1, ImVec4(0, 0, 0, 0), App->editor->ExplorerIconsTint);
				break;
			default:
				ImGui::ImageButton((ImTextureID)defaultIcon->id, ImVec2(iconSize, iconSize), flipV, flipH, -1, ImVec4(0, 0, 0, 0), App->editor->ExplorerIconsTint);
				break;
			}

			if (ImGui::IsItemHovered() && resource->type != ResourceType::Folder)
			{
				Hovered_UID = JsonConfig(buffer).GetNumber("UID");
			}
			RELEASE_ARRAY(buffer);
		}
		else
		{
			ImGui::ImageButton((ImTextureID)folderIcon->id, ImVec2(iconSize, iconSize), flipV, flipH, -1, ImVec4(0, 0, 0, 0), App->editor->ExplorerIconsTint);
		}

		if (ImGui::IsItemClicked() && !assetFolder.children[i].isFile)
		{
			nextFolder = assetFolder.children[i];
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) ImGui::OpenPopup("ImportPopUp");

		if (ImGui::BeginPopup("ImportPopUp"))
		{
			ImGui::IsItemHovered();
			if (ImGui::Selectable("Import Asset"))
			{
				App->resources->LoadResource(Hovered_UID);
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Asset", &UID, sizeof(uint32));

			switch (resource->type)
			{
			case ResourceType::Model:
				ImGui::Image((ImTextureID)modelIcon->id, ImVec2(iconSize, iconSize), flipV, flipH);
				break;
			case ResourceType::Texture:
				ImGui::Image((ImTextureID)textureIcon->id, ImVec2(iconSize, iconSize), flipV, flipH);
				break;
			case ResourceType::Shader:
				ImGui::Image((ImTextureID)folderIcon->id, ImVec2(iconSize, iconSize), flipV, flipH);
				break;
			}
			ImGui::EndDragDropSource();
		}

		ImGui::SetCursorPosX((i - (row * column)) * (iconSize + offset) + offset);
		ImGui::SetCursorPosY(row * (iconSize + offset) + iconSize + offset / 5);

		ImGui::Text(assetFolder.children[i].localPath.c_str());

		if(column > 0 && (i + 1) % column == 0) row++;

		ImGui::PopID();
	}

	ImGui::EndChild();

	if (nextFolder.path != "") assetFolder = nextFolder;
	else textureIconLoaded = true;
}


