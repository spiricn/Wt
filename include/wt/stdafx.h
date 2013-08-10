#ifdef WT_USE_STDAFX

#ifndef WT_STDAFX_H
#define WT_STDAFX_H


#pragma message("\nCreating wt precompiled header!\n")

// Tracedog
#include <td/td.h>

//SFML
#include <SFML/Audio.hpp>

// Lua
#define LUAPLUS_EXCEPTIONS 1
#include <LuaPlus.h>
#include <LuaFunction.h>

#include <physfs.h>

// PhysX
#include <PxPhysicsAPI.h> 
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxShapeExt.h>
#include <extensions/PxSimpleFactory.h>
#include <PxToolkit.h>
#include <extensions/PxVisualDebuggerExt.h>

// SDL
#include <SDL.h>

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H 

// Windows
#if defined(WIN32)
	#include <Windows.h>
	#include <WinCon.h>
	#include <conio.h>
	#include <direct.h>
	#include <WinSock.h>
#endif

// OpenGL
#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>

// STL
#include <time.h>
#include <stdio.h>
#include <queue>
#include <iostream>
#include <list>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <stack>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <stdint.h>
#include <stdarg.h> 

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>

// BOOST
//#include <boost/shared_ptr.hpp>
//#include <boost/functional/hash/hash.hpp>

// Devil
#include <IL/il.h>
#include <IL/ilu.h>

// wt

namespace wt{
	typedef std::string String;
	typedef std::vector<String> StringList;
}

#endif

#endif