
#ifndef __MAP_PATH_H__
#define __MAP_PATH_H__

#include "stlastar.h"
#include "cugl/cugl.h"

class MapPath {
protected:
	int *_map;
	int _width;
	int _height;



public:
	int GetMap(int x, int y);
	std::vector<cugl::Vec2> doSearch(int *_map, int width, int height, cugl::Vec2 botLeftPos, float gridScale, int startX, int startY, int targX, int targY, int *isVali);

};




class MapSearchNode
{
public:
	int x;	 // the (x,y) positions of the node
	int y;
	MapPath *_mapPath; // ptr to parent

	MapSearchNode() : x(0), y(0), _mapPath(nullptr) {} //hope never called!
	MapSearchNode(MapPath *parent) : x(0), y(0), _mapPath(parent) {}
	MapSearchNode(int px, int py, MapPath *parent) : x(px), y(py), _mapPath(parent) {}

	float GoalDistanceEstimate(MapSearchNode &nodeGoal);
	bool IsGoal(MapSearchNode &nodeGoal);
	bool GetSuccessors(AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node);
	float GetCost(MapSearchNode &successor);
	bool IsSameState(MapSearchNode &rhs);

	void PrintNodeInfo();


};






#endif