#ifndef _H_QT_PRECOMPILED_HDR
#define _H_QT_PRECOMPILED_HDR

#pragma message("\n< Recopmiling QT STDAFX.h >\n")

#define NOMINMAX 


//SFML
#include <SFML/Audio.hpp>

// QT
#include <gl/glew.h>
#include <QtGui/QDesktopServices>
#include <QtGui/QApplication>
#include <QGLWidget>
#include <QUrl>
#include <QMouseEvent>
#include <QtGui/QMainWindow>
#include <QtGui/qinputdialog.h>
#include <QtGui/QTreeWidget.h>
#include <QtGui/QFileDialog.h>

// LuaPlus
#include <LuaPlus.h>



// Lua
#define LUAPLUS_EXCEPTIONS 1
#include <LuaPlus.h>
#include <LuaFunction.h>

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
#undef main

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H 

// Windows
#ifdef WIN32
	//#include <Windows.h>
	#include <conio.h>
	#include <direct.h>
#endif

// OpenGL
#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>

// STL
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>

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

// Devil
#include <IL/il.h>
#include <IL/ilu.h>

#include "wt/Types.h"

namespace wt{
	typedef std::string String;
	typedef std::vector<String> StringList;
}

#endif