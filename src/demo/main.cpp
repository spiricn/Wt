#include "demo/stdafx.h"

#if 1
#include "demo/DemoManager.h"

#include "demo/GodRayDemo.h"
#include "demo/ParticleDemo.h"
#include "demo/DayNightDemo.h"
#include "demo/SoundDemo.h"
#include "demo/PhysicsDemo.h"
#include "demo/BasicDemo.h"
#include "demo/AnimationDemo.h"
#include "demo/LightDemo.h"
#include "demo/FogDemo.h"
#include "demo/OffscreenDemo.h"
#include "demo/MotionBlurDemo.h"
#include "demo/GuiDemo.h"
#include "demo/MinesDemo.h"
#include "demo/GoldDiggersDemo.h"
#include "demo/PathFindingDemo.h"
#include "demo/TestDemo.h"
#include "wt/FileIOStream.h"

using namespace wt;

#if defined(WT_DEMO_NO_CONSOLE) && defined(WIN32)
	#pragma comment(linker, "/SUBSYSTEM:windows")
#endif

#ifndef WT_DEMO_NO_CONSOLE
int main(){
#else
#if defined(WIN32)
	int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow){
#else
	#error Not implemented on this platform
#endif
#endif
	DemoManager::getSingleton().initialize();

	WT_REGISTER_DEMO(GodRayDemo);

	WT_REGISTER_DEMO(ParticleDemo);

	WT_REGISTER_DEMO(LightDemo);

	WT_REGISTER_DEMO(DayNightDemo);

	WT_REGISTER_DEMO(PhysicsDemo);

	WT_REGISTER_DEMO(MotionBlurDemo);

	WT_REGISTER_DEMO(SoundDemo);

	WT_REGISTER_DEMO(BasicDemo);

	WT_REGISTER_DEMO(AnimationDemo);

	WT_REGISTER_DEMO(GuiDemo);

	WT_REGISTER_DEMO(FogDemo);

	WT_REGISTER_DEMO(OffscreenDemo);
	
	WT_REGISTER_DEMO(MinesDemo);

	WT_REGISTER_DEMO(GoldDiggersDemo);

	WT_REGISTER_DEMO(PathFindingDemo);

	WT_REGISTER_DEMO(TestDemo);

	DemoManager::getSingleton().start();

	return 0;
}

#else


#include "wt/exception.h"

#include <wt/FileIOStream.h>
#include <wt/Sp.h>

#include <wt/ZipFileSystem.h>
#include <wt/LocalFileSystem.h>


int main(){
	wt::AFileSystem* fileSystem = new wt::ZipFileSystem;

	wt::Sp<wt::AIOStream> stream = fileSystem->open("test_dir/test.txt", wt::AIOStream::eMODE_READ);

	WT_ASSERT(stream->isReadable(), "Stream not readable");

	char* contents = new char[stream->getSize()+1];
	memset(contents, 0x00, stream->getSize()+1);
	stream->read(contents, stream->getSize());

	LOGI("Read contents: \"%s\"", contents);
//
//	FileIOStream out("out.txt", AIOStream::eMODE_WRITE);
//	WT_ASSERT(out.isWritable(), "File not writable");
//
//	char bfr[] = "12345";
//	out.write((uint8_t*)bfr, strlen(bfr));
//	out.close();
//
//	FileIOStream in("out.txt", AIOStream::eMODE_READ);
//	WT_ASSERT(in.isReadable(), "File not readable");
//	memset(bfr, 0x00, sizeof(bfr));
//
//	LOG("read %ld bytes", in.read((uint8_t*)bfr, 5));
//
//	LOG("read %s", bfr);
//
//	LOG("File size %d", in.getSize());
//
//	PHYSFS_init(NULL);
//
//	//PHYSFS_new
//
//	PHYSFS_addToSearchPath("d:\\Documents\\prog\\c++\\workspace\\Wt\\vfs\\dir.zip", 0);
//	//PHYSFS_addToSearchPath("d:\\Documents\\prog\\c++\\workspace\\Wt\\vfs\\", 1);
//
//	LOG("%d", PHYSFS_exists("test.txt"));
//
//	//PHYSFS_setWriteDir("dir.zip");
//
//	//PHYSFS_file* file = PHYSFS_openRead("test.txt");
//	//char bfr[256];
//	//memset(bfr, 0x00, 256);
//	//LOG("%ld read", PHYSFS_read(file, bfr, 256, 1));
//
//
//	//LOG("read: \"%s\"", bfr);
//
//	PHYSFS_setWriteDir("d:\\Documents\\prog\\c++\\workspace\\Wt\\vfs\\dir.zip");
//
//	//PHYSFS_
//	PHYSFS_file* file = PHYSFS_openWrite("test.txt");
//	WT_ASSERT(file, "Invalid file");
//	PHYSFS_write(file, "asdf", 4, 1);
//	LOG("%p", file);
//
//	PHYSFS_close(file);
//	PHYSFS_deinit();

}

#endif