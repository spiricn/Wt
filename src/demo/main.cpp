#include "demo/stdafx.h"


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

#if 0

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

#elif 1

#include <wt/net/TCPServer.h>
#include <wt/Thread.h>

#include "wt/RemoteFileSystemServer.h"
#include "wt/RemoteFileSystemClient.h"

class ServerTest : public Thread{
	void run(){
		RemoteFileSystemServer server;
		server.startServer("D:/documents", 8420);

		utils::pause();
	}
};

#include <wt/Timer.h>

class ClientTest : public Thread{

public:
	void run(){
		RemoteFileSystemClient client;
		StreamPtr stream = client.open("test.wts", AIOStream::eMODE_READ);

		FILE* local = fopen(
			"C:/users/nikola/desktop/dl.wts"
			, "wb");
		
		LOGD("Starting transfer");

		Timer t;
		
		uint64_t totalRead = 0;

		while(true){
			static const int kBUFFER_SIZE = 512;

			char bfr[kBUFFER_SIZE ];
			int64_t bytesRead = stream->read(bfr, kBUFFER_SIZE );

			fwrite(bfr, 1, bytesRead, local);

			totalRead += bytesRead;

			if(bytesRead != kBUFFER_SIZE ){
				break;
			}
		}

		LOGD("%ld", totalRead);

		fclose(local);

		LOGD("done %s / s", utils::formatSize((float)totalRead/t.getSeconds()).c_str());
		utils::pause();	
	}
};

int main(){
	net::Socket::initializeLib();

	ClientTest client;
	//ServerTest server;

	//server.start();
	client.start();

	//server.wait();
	client.wait();

	return 0;
}
#else

#include <wt/net/TCPServer.h>
#include <wt/Thread.h>
#include "wt/RemoteFileSystemServer.h"
#include "wt/RemoteFileSystemClient.h"

#define TD_TRACE_TAG "RemoteFileServer"

int main(int argc, char* argv[]){
	if(argc != 3){
		LOGI("Usage: rfs <root> <port>");
		return 0;
	}

	LOGD("Creating RFS; root=\"%s\"; port=%d", argv[1], atoi(argv[2]));

	net::Socket::initializeLib();

	RemoteFileSystemServer server;
	server.startServer(argv[1], atoi(argv[2]));

	LOGI("Server running.");
	server.wait();
}

#endif