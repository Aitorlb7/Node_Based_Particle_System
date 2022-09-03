#include "Window.h"
#include "PathNode.h"
#include "Timer.h"


class ResourceTexture;
class Resource;

class WindowAssetExplorer : public Window
{
public:

	WindowAssetExplorer(bool isActive);
	~WindowAssetExplorer();

	void Start() override;

	void Draw() override;

	void CleanUp() override;

	void LoadIcons();

	void UpdateAssetExplorer();

	void AssetExplorerWindow();

	void AssetsTree(PathNode& assetFolder);

	void AssetsExplorer(PathNode& assetFolder);

	ResourceTexture* modelIcon = nullptr;

private:
	ResourceTexture* defaultIcon = nullptr;
	ResourceTexture* folderIcon = nullptr;
	ResourceTexture* returnIcon = nullptr;

	ResourceTexture* textureIcon = nullptr;
	Resource* resource = nullptr;

	uint updateTime = 5;
	Timer updateTimer;

	PathNode assetsFolder;
	PathNode currentFolder;
	PathNode nextFolder;
	PathNode previousFolder;
	uint32 Hovered_UID = 0;

	uint iconSize = 80;

	bool textureIconLoaded = false;


};