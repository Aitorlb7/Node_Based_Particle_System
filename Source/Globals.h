#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI


typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 786
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define WIN_MAXIMIZED true
#define VSYNC false
#define TITLE "Node Based Particle System"

//File System

#define LIBRARY_PATH "Library/"
#define FOLDERS_PATH "Library/Folders/"
#define MESHES_PATH "Library/Meshes/"
#define MATERIALS_PATH "Library/Materials/"
#define TEXTURES_PATH "Library/Textures/"
#define SCENES_PATH "Library/Scenes/"
#define MODELS_PATH "Library/Models/"
#define SHADERS_PATH "Library/Shaders/"
#define PARTICLE_SYSTEM_PATH "Library/ParticleSystems/"

//#define ASSETS_PATH "Output/Assets"
#define ASSETS_FOLDER "Assets/"
#define MODELS_FOLDER "Assets/Models/"
#define TEXTURES_FOLDER "Assets/Textures/"
#define SCENES_FOLDER "Assets/Scenes/"
#define PARTICLE_SYSTEM_FOLDER "Assets/ParticleSystems/"
#define SKYBOX_FOLDER "Assets/Textures/Skybox/"


#define MESH_EXTENSION ".asemesh"
#define MATERIAL_EXTENSION	".asemat"
#define SCENE_EXTENSION	".asescene"
#define TEXTURE_EXTENSION ".asetex"
#define PARTICLE_SYSTEM_EXTENSION ".aseps"
#define ASE_EXTENSION ".ASE"

#define VERTEX_SHADER "__Vertex_Shader__"
#define FRAGMENT_SHADER "__Fragment_Shader__"

#define MAX_SHADERS 1000
#define MAX_TEXTURES 1000

// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }


#define GREEN ImVec4(0.1f, 1.0f, 0.0f, 1.0f)
#define YELLOW ImVec4(1.0f, 1.0f, 0.0f, 1.0f)

#define MAX_PARTICLES 2000

class Application;

extern Application* App;