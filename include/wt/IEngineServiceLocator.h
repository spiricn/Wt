#ifndef WT_IENGINERSERVICELOCATOR_H
#define WT_IENGINERSERVICELOCATOR_H

namespace wt
{
	class AGameWindow;
	class AGameInput;
	class EventManager;
	class ProcessManager;
	class Scene;
	class Physics;
	class AResourceSystem;
	class Renderer;
} // </wt>

namespace lua
{
	class State;
} // </lua>

namespace wt
{

class IEngineServiceLocator{
public:
	virtual AGameWindow* getWindow() const = 0;

	virtual AGameInput* getInput() const = 0;

	virtual EventManager* getEventManager() const = 0;

	virtual ProcessManager* getProcessManager() const = 0;

	virtual lua::State* getLuaState() const = 0;

	virtual Scene* getScene() const = 0;
	
	virtual Physics* getPhysics() const = 0;

	virtual AResourceSystem* getAssets() const = 0;

	virtual Renderer* getRenderer() const = 0;

}; // </IEngineServiceLocator>

} // </wt>

#endif // </WT_IENGINERSERVICELOCATOR_H>
