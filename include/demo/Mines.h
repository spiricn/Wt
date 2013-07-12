#ifndef WT_MINES_H
#define WT_MINES_H


#include "demo/ADemo.h"

#define TD_TRACE_TAG "Mines"

namespace wt{

struct MineCell{
	bool visible;
	bool hasMine;
	unsigned char numNeighborMines;
	void* userData;
	bool marked;
	Uint32 x, y;

	MineCell() : visible(false), hasMine(false), userData(NULL), numNeighborMines(0), marked(false){
	}
};

class CellRevealedEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	std::set<const MineCell*> cells;

	static const EvtType TYPE;

	CellRevealedEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </CellRevealedEvent>

class GameLostEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	GameLostEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </GameLostEvent>

const EvtType GameLostEvent::TYPE = "GameLost";

const EvtType CellRevealedEvent::TYPE = "CellRevealed";
	
class CellMarkedEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	MineCell* cell;
	CellMarkedEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </CellMarkedEvent>

const EvtType CellMarkedEvent::TYPE = "CellMarked";
class Minefield{
private:
	MineCell* mField;
	Uint32 mNumRows, mNumColumns, mNumMines;

public:
	Minefield() : mField(NULL){
	}


	Minefield(Uint32 numRows, Uint32 numColumns, Uint32 numMines) : mNumRows(numRows), mNumColumns(numColumns), mNumMines(numMines){
		mField = new MineCell[numRows*numColumns];

		restart();
	}

	~Minefield(){
		if(mField){
			delete[] mField;
		}
	}

	void restart(){
		for(Uint32 x=0; x<mNumColumns; x++){
			for(Uint32 y=0; y<mNumRows; y++){
				MineCell& cell = getCell(x, y);

				cell.marked = false;
				cell.visible= false;
				cell.numNeighborMines = 0;
				cell.hasMine = false;
			}
		}

		Uint32 numMines = mNumMines;
		while(numMines){
			MineCell& cell = getCell((Uint32)(math::random()*(mNumColumns-1)), (Uint32)(math::random()*(mNumRows-1)));
			if(!cell.hasMine){
				cell.hasMine = true;
				--numMines;
			}

		}

		for(Uint32 x=0; x<mNumColumns; x++){
			for(Uint32 y=0; y<mNumRows; y++){
				MineCell& cell = getCell(x, y);


				cell.x = x;
				cell.y = y;

				cell.visible = false;

				for(int dx=-1; dx<2; dx++){
					for(int dy=-1; dy<2; dy++){
						if(dx==0 && dy==0){
							continue;
						}

						if(inBounds(x+dx, y+dy) && getCell(x+dx, y+dy).hasMine){
							++cell.numNeighborMines;
						}
					}
				}
			}
		}
	}

	const MineCell& getCell(Int32 x, Int32 y) const{
		return getCell(x, y);
	}

	MineCell& getCell(Int32 x, Int32 y){
		WT_ASSERT(inBounds(x, y), "Coord out of bounds (%d, %d)", x, y);

		Int32 idx =y*mNumColumns + x;
		WT_ASSERT(idx >= 0 && idx < (Int32)(mNumRows*mNumColumns), "Invalid idx");

		return mField[idx];
	}

	bool inBounds(Int32 x, Int32 y) const{
		return x >= 0 && y >= 0 &&
			x < (Int32)mNumColumns && y < (Int32)mNumRows;
	}

	
	Uint32 getNumRows() const{
		return mNumRows;
	}

	Uint32 getNumColumn() const{
		return mNumColumns;
	}

};

class GameWonEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	GameWonEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </GameWonEvent>

const EvtType GameWonEvent::TYPE = "GameWon";

class Minesweeper{
public:
	enum GameState{
		eINACTIVE,
		ePLAYING,
		eWON,
		eLOST
	};

private:
	Minefield* mField;
	EventManager* mEventManager;	

	GameState mGameState;
public:
	Minesweeper() : mGameState(eINACTIVE), mField(NULL){
	}

	void createField(Uint32 numRows, Uint32 numColumns, Uint32 numMines){
		if(mField){
			delete mField;
			mField = NULL;
		}

		mField = new Minefield(numRows, numColumns, numMines);

		mGameState = ePLAYING;
	}

	GameState getState() const{
		return mGameState;
	}

	~Minesweeper(){
		delete mField;
	}

	void hook(EventManager* em){
		mEventManager = em;

		mEventManager->registerInternalEvent(CellRevealedEvent::TYPE);
		mEventManager->registerInternalEvent(GameLostEvent::TYPE);
		mEventManager->registerInternalEvent(GameWonEvent::TYPE);
		mEventManager->registerInternalEvent(CellMarkedEvent::TYPE);

	}

	bool victory(){
		for(Uint32 x=0; x<mField->getNumColumn(); x++){
			for(Uint32 y=0; y<mField->getNumRows(); y++){
				MineCell& cell = mField->getCell(x, y);
				if(!cell.marked && !cell.visible
					|| (cell.marked && !cell.hasMine)){
					return false;
				}
			}
		}

		mEventManager->queueEvent( new GameWonEvent );

		mGameState = eWON;

		return true;
	}
	void setCellUserData(Uint32 x, Uint32 y, void* data){
		mField->getCell(x, y).userData = data;
	}

	const Minefield& getField() const{
		return *mField;
	}

	void restart(){
		mField->restart();
		mGameState = ePLAYING;
	}

	void mark(MineCell& cell){
		if(mGameState != ePLAYING){
			return;
		}

		if(!cell.visible){
			cell.marked = !cell.marked;

			CellMarkedEvent* e = new CellMarkedEvent;
			e->cell = &cell;

			mEventManager->queueEvent(e);
		}

		victory();
	}

	void mark(Uint32 x, Uint32 y){
		if(mGameState != ePLAYING){
			return;
		}

		if(mField->inBounds(x, y)){
			mark(mField->getCell(x, y));
		}
	}

	const  MineCell& getCell(Uint32 x, Uint32 y) const{
		return mField->getCell(x, y);
	}


	void reveal(std::set<MineCell*>& cells){
		if(cells.empty()){
			return;
		}

		CellRevealedEvent* e = new CellRevealedEvent;

		for(std::set<MineCell*>::iterator i=cells.begin(); i!=cells.end(); i++){
			(*i)->visible = true;
			e->cells.insert(*i);
		}

		mEventManager->queueEvent(e);
	}

	void revealNeighbours(Uint32 x, Uint32 y){
		if(mGameState != ePLAYING){
			return;
		}

		MineCell& cell = mField->getCell(x, y);

		if(!cell.visible){
			return;
		}

		Uint32 numMarked=0;
		for(int dx=-1; dx<2; dx++){
			for(int dy=-1; dy<2; dy++){
				if(dx==0 && dy==0){
					continue;
				}

				if(mField->inBounds(x+dx, y+dy) && mField->getCell(x+dx, y+dy).marked){
					numMarked++;
				}
			}
		}

		if(numMarked == cell.numNeighborMines){
			for(int dx=-1; dx<2; dx++){
				for(int dy=-1; dy<2; dy++){
					if(dx==0 && dy==0){
						continue;
					}

					if(mField->inBounds(x+dx, y+dy) && !mField->getCell(x+dx, y+dy).marked && !mField->getCell(x+dx, y+dy).visible){
						reveal(x+dx, y+dy);
					}
				}
			}
		}
	}

	void reveal(Uint32 x, Uint32 y){
		if(mGameState != ePLAYING){
			return;
		}

		MineCell& cell = mField->getCell(x, y);

		if(cell.marked){
			return;
		}

		if(cell.visible){
			return;
		}

		std::set<MineCell*> toReveal;

		if(!cell.hasMine){
			toReveal.insert(&cell);
			cell.visible = true;

			if(victory()){
				return;
			}

			if(cell.numNeighborMines != 0){
				reveal(toReveal);
				return;
			}

			std::vector<glm::vec2> unchecked;

			for(int dx=-1; dx<2; dx++){
				for(int dy=-1; dy<2; dy++){
					if(dx==0 && dy==0){
						continue;
					}

					if(mField->inBounds(x+dx, y+dy) && !mField->getCell(x+dx, y+dy).visible){
						unchecked.push_back(
							glm::vec2(x+dx, y+dy));
					}
				}
			}

			while(!unchecked.empty()){
				glm::vec2 cellCoords = unchecked.back();
				unchecked.pop_back();

				MineCell& neighbour = mField->getCell(cellCoords.x, cellCoords.y);

				neighbour.visible = true;
				toReveal.insert(&neighbour);

				if(victory()){
					return;
				}
				
				if(neighbour.numNeighborMines == 0){

					for(int dx=-1; dx<2; dx++){
						for(int dy=-1; dy<2; dy++){
							if(dx==0 && dy==0){
								continue;
							}

							if(mField->inBounds(cellCoords.x+dx, cellCoords.y+dy) && 
								!mField->getCell(cellCoords.x+dx, cellCoords.y+dy).visible){

								unchecked.push_back(
							glm::vec2(cellCoords.x+dx, cellCoords.y+dy));
							}
						}
					}
				}
			}
			reveal(toReveal);
		}
		else{
			mEventManager->queueEvent( new GameLostEvent );
			mGameState = eLOST;
		}

		
	}
};


}; // </wt>

#endif // </WT_MINESDEMO_H>
