#ifndef WT_PATHFINDINGDEMO_H
#define WT_PATHFINDINGDEMO_H

#include "demo/ADemo.h"

#define TD_TRACE_TAG "PathFindingDemo"

namespace wt{

	
struct PathNode;

typedef std::vector<PathNode*> NodeList;

typedef std::set<PathNode*> NodeSet;

struct PathNode{
private:
	NodeList mConnections;

	glm::vec3 mPosition;

public:
	PathNode(const glm::vec3& pos) : mPosition(pos){
	}

	const NodeList& getConnections() const{
		return mConnections;
	}

	void connect(PathNode* node){
		mConnections.push_back(node);
		node->mConnections.push_back(this);
	}

	bool isConnected(PathNode* node) const{
		return std::find(mConnections.begin(), mConnections.end(), node) != mConnections.end();
	}

	const glm::vec3& getPosition() const{
		return mPosition;
	}
};


struct Path{
	NodeList mNodes;

	void insertNode(PathNode* node){
		mNodes.push_back(node);
	}

	PathNode* findClosestNode(const glm::vec3& position){
		PathNode* closestNode = NULL;
		float closestDistance = 0;

		for(NodeList::iterator i=mNodes.begin(); i!=mNodes.end(); i++){
			float distance = glm::length((*i)->getPosition() - position);

			if(closestNode == NULL || distance <  closestDistance){
				closestNode = *i;
				closestDistance = distance;
			}
		}

		return closestNode;
	}

	bool findPath(PathNode* parentNode, PathNode* startNode, PathNode* endNode, NodeList& traversed, NodeList& result){
		if(startNode->isConnected(endNode)){
			// Done
			result.insert(result.begin(), endNode);
			result.insert(result.begin(), startNode);
			return true;
		}

		traversed.push_back(startNode);

		for(NodeList::const_iterator i=startNode->getConnections().begin(); i!=startNode->getConnections().end(); i++){
			if(std::find(traversed.begin(), traversed.end(), *i) != traversed.end()){
				continue;
			}

			if(findPath(startNode, *i, endNode, traversed, result)){
				result.insert(result.begin(), startNode);
				return true;
			}
		}

		return false;
	}

	bool findPath(const glm::vec3& startPos, const glm::vec3& destination, NodeList& path){
		PathNode* startNode = findClosestNode(startPos);
		PathNode* endNode = findClosestNode(destination);

		NodeList traversed;
		return findPath(NULL, startNode, endNode, traversed, path);
	}
};

class PathFindingDemo : public ADemo{
public:

	PathFindingDemo(){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());
	}

	String getConfigFile() const{
		return "";
	}

	String getLevelFile() const{
		return "";
	}

}; // </PathFindingDemo>

WT_DECLARE_DEMO(PathFindingDemo)

}; // </wt>

#endif // </WT_PATHFINDINGDEMO_H>
