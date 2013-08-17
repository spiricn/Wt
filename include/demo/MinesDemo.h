#ifndef WT_MINESDEMO_H
#define WT_MINESDEMO_H

#include "demo/ADemo.h"
#include "demo/Mines.h"

#include "wt/MusicPlayer.h"
#include "wt/Toast.h"

namespace wt{


//#define CURSOR_ENABLED
	

class MineActor{
public:
	const MineCell* cell;
	ModelledActor* sceneActor;

	void update(float dt){
	}
};

class MinesDemo : public ADemo, public EventListener{
private:
	Minesweeper mGameState;
	//ParticleEmitter mEmitter;
	ASoundSystem* mSoundSystem;
	MusicPlayer* mAmbientMusic;
	MusicPlayer* mDefeatMusic;
	ProcPtr mEndGameToast;

	typedef std::vector<MineActor*> ActorList;
	ActorList mActors;

	enum Difficulty{
		eBEGGINER,
		eINTERMEDIATE,
		eEXPERT
	};

	Difficulty mDifficulty;

#ifdef CURSOR_ENABLED
	ParticleEffect* mCursor;
#endif

public:
	

	class MineDetacher : public AProcess{
	private:
		struct DetachCell{
			const MineCell* cell;
			float waitTime;

		}; // </DetachCell>

		std::vector<DetachCell> mCells;
		Interpolator<float> mInterpolator;
		float mBaseHeight;
	public:
		MineDetacher(EvtPtr evt) : mInterpolator(0, -0.3, .1, false, Interpolator<float>::eEASE_IN_QUAD){
			CellRevealedEvent* e = (CellRevealedEvent*)evt.get();

			for(std::set<const MineCell*>::iterator i=e->cells.begin(); i!=e->cells.end(); i++){
				const MineCell* cell = *i;

				ModelledActor* actor = (ModelledActor*)cell->userData;

				mBaseHeight = actor->getTransform().getPosition().y;

				String skin;

				switch(cell->numNeighborMines){
					case 0:skin = "visible"; break;
					case 1:skin = "1"; break;
					case 2:skin = "2"; break;
					case 3:skin = "3"; break;
					case 4:skin = "4"; break;
					case 5:skin = "5"; break;
					case 6:skin = "6"; break;
					case 7:skin = "7"; break;
					case 8:skin = "8"; break;
				}

				actor->setSkin(skin);

				DetachCell detachCell;
				detachCell.cell = cell;

				
					

				if(cell->numNeighborMines > 0){
				}
				else{
					detachCell.waitTime = math::random();
				}

				mCells.push_back(detachCell);
			}
		}
		
		void onProcUpdate(float dt){
			mInterpolator.update(dt);

			for(std::vector<DetachCell>::iterator i=mCells.begin(); i!=mCells.end(); i++){
				i->waitTime -= dt;

				const MineCell* cell = i->cell;

				ModelledActor* actor = (ModelledActor*)cell->userData;

				if(i->cell->numNeighborMines == 0){
					if(i->waitTime <= 0.0f){
						// drop
						((PxRigidDynamic*)actor->getPhysicsActor()->getPxActor())->wakeUp();


						// remove from list
						i = mCells.erase(i);
						if(i == mCells.end()){
							break;
						}
					}
				}
				else{
					glm::vec3 pos = actor->getTransform().getPosition();
					pos.y = mBaseHeight + mInterpolator.getValue();

					actor->getTransform().setPosition(pos);

					if(mInterpolator.isFinished()){
						// remove from list
						i = mCells.erase(i);
						if(i == mCells.end()){
							break;
						}
					}
				}
			}

			if(mCells.empty()){
				killProcess();
			}
		}
	private:
	};

	MinesDemo() : mEndGameToast(NULL){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );

		/*mEmitter.render(&getScene()->getCamera(),
			&getRenderer()->getFrustum());*/
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);

		//mEmitter.update(dt);
		mSoundSystem->update(dt);
		
		getScene()->getSkyBox()->getTransform().rotate(1, 1, 1, 3*dt);

	}

	void onMouseMotion(const MouseMotionEvent* evt){
		RaycastHitEvent hit;

		
		if(getScene()->getPhysics()->pick(getScene()->getCamera(),
			glm::vec2(evt->mX, evt->mY),
			glm::vec2(getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight()), hit)){

			//PointLight light;
			//getScene()->getPointLight(0, light);

			//light.position = hit.mImpact + glm::vec3(0, 3, 0);
			//getScene()->setPointLight(0, light);


#ifdef CURSOR_ENABLED
				mCursor->setParam("uPosition", hit.mImpact + glm::vec3(0, 3, 0));
#endif
		}
	}

	bool handleEvent(const Sp<Event> e){
		if(e->getType() == CellRevealedEvent::TYPE){
			CellRevealedEvent* evt = (CellRevealedEvent*)e.get();

			if(evt->cells.size() > 1){
				/*mSoundSystem->playSound("$ROOT/misc/magicclick");*/
			}
			getProcManager().attach( new MineDetacher(e) );

		}
		else if(e->getType() == CellMarkedEvent::TYPE){
			CellMarkedEvent* evt = (CellMarkedEvent*)e.get();

			ModelledActor* actor = (ModelledActor*)evt->cell->userData;

			actor->setSkin(
				evt->cell->marked ? "marked" : "hidden");

		}
		else if(e->getType() == GameLostEvent::TYPE){
			Toast* toast = new Toast(&getUi(),
				glm::vec2(0, 0), 
				glm::vec2(getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight()),
				getAssets()->getTextureManager()->find("defeat"));
			toast->setLinger(true)->setFadeOutValue(0.3f);

			getProcManager().attach(
				mEndGameToast = toast
			);			

			LOG("Game lost!");
			for(Uint32 i=0; i<mGameState.getField().getNumColumn(); i++){
				for(Uint32 j=0; j<mGameState.getField().getNumRows(); j++){
					ModelledActor* a = (ModelledActor*)mGameState.getCell(i, j).userData;

					PxVec3 force;
					pxConvert(glm::vec3(-5 + math::random()*10, math::random()*20, -5 + math::random()*10), force);
					
					((PxRigidDynamic*)a->getPhysicsActor()->getPxActor())->setLinearVelocity(force);
				}
			}

			mAmbientMusic->play("assets/demo/MinesDemo/-1 - AngryMod.ogg");
			
			getPhysics()->getScene()->setGravity(PxVec3(0, -9.81f, 0));
		}
		else if(e->getType() == GameWonEvent::TYPE){
			LOG("Game won!");

			Toast* toast = new Toast(&getUi(),
					glm::vec2(0, 0), 
					glm::vec2(getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight()),
					getAssets()->getTextureManager()->find("victory"));
			toast->setLinger(true)->setFadeOutValue(0.3);

			getProcManager().attach( mEndGameToast = toast );


			mAmbientMusic->play("assets/demo/MinesDemo/-1 - 1990.ogg");

			getPhysics()->getScene()->setGravity(PxVec3(0, 3.81f, 0));
			for(Uint32 i=0; i<mGameState.getField().getNumColumn(); i++){
				for(Uint32 j=0; j<mGameState.getField().getNumRows(); j++){
					ModelledActor* a = (ModelledActor*)mGameState.getCell(i, j).userData;

					PxVec3 force;
					pxConvert(glm::vec3(-5 + math::random()*10, math::random()*20, -5 + math::random()*10), force);
					
					((PxRigidDynamic*)a->getPhysicsActor()->getPxActor())->setLinearVelocity(force);
				}
			}
		}
		return true;
	}

	void onMouseDown(float x, float y, MouseButton btn){
		RaycastHitEvent hit;

		if(mGameState.getState() != Minesweeper::ePLAYING){
			restart(mDifficulty);
		}
		else{
			if(getScene()->getPhysics()->pick(getScene()->getCamera(),
				glm::vec2(x, y),
				glm::vec2(getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight()), hit)){
					MineActor* ma = (MineActor*)hit.mPickedActor->getSceneActor()->getUserData();

					if(getManager()->getInput()->isMouseButtonDown(BTN_LEFT) && getManager()->getInput()->isMouseButtonDown(BTN_RIGHT)){
						mGameState.revealNeighbours(ma->cell->x, ma->cell->y);
					}
					else if(btn == BTN_LEFT){
						mGameState.reveal(ma->cell->x, ma->cell->y);
					
					}
					else if(btn == BTN_RIGHT){
						mGameState.mark(ma->cell->x, ma->cell->y);
					}

			}
		}
	}

	void restart(Difficulty difficulty){
		LOG("restart");

		Uint32 numRows, numColumns, numMines;
		glm::quat cameraRotation;
		glm::vec3 cameraPosition;

		if(difficulty == eEXPERT){
			numRows = 16;
			numColumns = 30;
			numMines = 99;
			cameraRotation =  glm::quat(0.524339, 0.472943, 0.525832, -0.474297);
			cameraPosition  = glm::vec3(-5.062535, 26.684690, -1.298899);
		}
		else if(difficulty == eINTERMEDIATE){
			numRows = 16;
			numColumns = 16;
			numMines = 40;
			cameraRotation = glm::quat(0.554675, 0.442495, 0.550975, -0.439430);
			cameraPosition = glm::vec3(-8.682729, 22.547667, -1.188196);
		}
		else{
			cameraPosition = glm::vec3(-6.661929, 12.316175, -0.978690);
			cameraRotation = glm::quat(0.568863, 0.418802, 0.570079, -0.419707);
			numRows = 9;
			numColumns = 9;
			numMines = 10;
		}

		mDifficulty = difficulty;

		// Create new field
		mGameState.createField( numColumns, numRows, numMines );

		getScene()->getCamera().setPosition(cameraPosition);
		getScene()->getCamera().setRotation(cameraRotation);

		// Remove all the actors from the scene
		for(ActorList::iterator i=mActors.begin(); i!=mActors.end(); i++){
			getScene()->deleteActor((*i)->sceneActor);
			delete *i;
		}

		mActors.clear();
		
		// Handle toast left from the previous game defeat/victory
		if(mEndGameToast){
			Toast* toast = (Toast*)mEndGameToast.get();
			if(toast->isAlive()){
				toast->setFadeOutValue(0)->setFadeOutTime(0.3)->fadeOutNow();
				mEndGameToast = NULL;
			}
		}


		mAmbientMusic->play("assets/demo/MinesDemo/-1 - 2ndBallad.ogg");

		getPhysics()->getScene()->setGravity(PxVec3(0, -9.81f, 0));


		for(Uint32 x=0; x<mGameState.getField().getNumColumn(); x++){
			for(Uint32 y=0; y<mGameState.getField().getNumRows(); y++){
				// Create scene actor
				ModelledActor* actor = getScene()->createModelledActor();
				actor->setModel(
					getAssets()->getModelManager()->find("cube"), "hidden");

				mGameState.setCellUserData(x, y, actor);

				// Initial actor position
				actor->getTransform().setPosition(
					(- (mGameState.getField().getNumColumn() * 2.1f)/2) + x * (2.1),
					0,
					(- (mGameState.getField().getNumRows() * 2.1f)/2) + y * (2.1));

				actor->getTransform().setRotation(
					0, 1, 0, -90);

				// Create game actor
				MineActor* ma = new MineActor;
				ma->sceneActor = actor;
				ma->cell = &mGameState.getCell(x, y);

				mActors.push_back(ma);
				actor->setUserData(ma);

				// Create physics actor
				PhysicsActor::Desc desc;

				desc.geometryType = PhysicsActor::eBOX_GEOMETRY;
				desc.controlMode = PhysicsActor::ePHYSICS_MODE;
				desc.type = PhysicsActor::eDYNAMIC_ACTOR;
				desc.geometryDesc.boxGeometry.hx = 1.0f;
				desc.geometryDesc.boxGeometry.hy = 1.0f;
				desc.geometryDesc.boxGeometry.hz = 1.0f;
				desc.pose = actor->getTransform();

				PhysicsActor* pActor = getPhysics()->createActor(actor, desc);
				((PxRigidDynamic*)pActor->getPxActor())->putToSleep();
			}
		}

	}

	void onKeyDown(VirtualKey c){
		if(c == KEY_r){
			restart(mDifficulty);
		}
		else if(c == KEY_d){
			changeDifficulty(mDifficulty == eBEGGINER ? eINTERMEDIATE : mDifficulty == eINTERMEDIATE ? eEXPERT : eBEGGINER);
		}

		ADemo::onKeyDown(c);
	}

	void changeDifficulty(Difficulty diff){
		if(mDifficulty == diff){
			return;
		}

		Texture2D* texture = getAssets()->getTextureManager()->find(
			diff == eBEGGINER ? "begginer" : diff == eINTERMEDIATE ? "intermediate" : "expert");

		Toast* toast = new Toast(&getUi(),
			glm::vec2(0, 0), getUi().getCanvas().getSize()/7.0f, texture);

		toast->setDuration(0.5f)->setFadeInTime(0.1f)->setFadeOutTime(0.1f);

		getProcManager().attach(toast);

		restart(diff);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());
		
		mGameState.hook(getEventManager());

		getEventManager()->registerListener(this, CellRevealedEvent::TYPE);
		
		getEventManager()->registerListener(this, GameLostEvent::TYPE);

		getEventManager()->registerListener(this, CellMarkedEvent::TYPE);

		getEventManager()->registerListener(this, GameWonEvent::TYPE);

		getManager()->getInput()->setMouseGrabbed(false);


	/*	PointLight l;
		l.enabled = true;
		l.position = glm::vec3(210, 10, 210);

		l.color =  Color::Green();
		l.ambientIntensity = 0;
		l.diffuseIntensity = 0.1;

		l.attenuation.constant = 0;
		l.attenuation.linear = 0;
		l.attenuation.quadratic = 0.01;
		getScene()->addPointLight(l);*/

		getScene()->getSkyBox()->getTransform().rotate(1, 0, 0, 180);


		mSoundSystem = new SFSoundSystem((SFSoundManager*)getAssets()->getSoundManager());

		
		getProcManager().attach(mAmbientMusic = new MusicPlayer(mSoundSystem));

		bool soundEnabled = true;
		lua::luaConv(config.Get("soundEnabled"), soundEnabled);
		if(!soundEnabled){
			mSoundSystem->setGlobalVolume(0);
		}

		mAmbientMusic->play("assets/demo/MinesDemo/-1 - 2ndBallad.ogg");
		

#ifdef CURSOR_ENABLED
		mCursor = mEmitter.create(5,
			getAssets()->getTextureManager()->getFromPath("$ROOT/brushes/circle_soft"), "shaders/force.vp")
			// Field center
			->setParam("uPosition", glm::vec3(202, 10, 269))
			->setParam("uSize", glm::vec3(0.5, 0.5, 0.5));
#endif

		getScene()->getFog().color = Color(1, 201/255.0f, 14/255.0f);
		getScene()->getFog().density += 0.007f;
	
		restart(eEXPERT);
	}

	String getConfigFile() const{
		return "assets/MinesDemoConfig.lua";
	}

	String getLevelFile() const{
		return "assets/demo/MinesDemo/level.lua";
	}

}; // </MinesDemo>

WT_DECLARE_DEMO(MinesDemo)

}; // </wt>

#endif // </WT_MINESDEMO_H>
