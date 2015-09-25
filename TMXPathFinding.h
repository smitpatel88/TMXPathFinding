/**
 *
 * Created by Smit Patel on 25/09/2015
 *
 * github: https://github.com/smitpatel88/TMXPathFinding
 *
 */

#ifndef _TMX_TILE_PATH_FINDING_H_
#define _TMX_TILE_PATH_FINDING_H_

#include "cocos2d.h"
USING_NS_CC;

/** @brief TMXPathFinding knows how to find path into TMX map.
 *
 * It uses TMX tiled map created by http://www.mapeditor.org
 * It supports orthogonal, hexagonal and isometric tile maps.
 * During Path finding, it takes current GID values from TMXLayers.
 * 
 * Features:
 * - Find Path between two Tile coordinates
 * - DIRECTION support : 
 *		1) DIRECTION::FOUR  - L, R, U, D
 *		2) DIRECTION::EIGHT - L, R, U, D, TL, BR, TR, BL
 * - You can add different Obstacle tiles.		e.g.- TILE_WALL, TILE_WATER
 * - You can add different Walkable tiles.		e.g.- TILE_ROAD, TILE_GRASS
 * - You can find path using only Obstacle tiles
 * - You can find path using only Walkable tiles
 * - You can find path for single or combined TMXLayers
 * 
 * Limitations:
 * - Multiple path finding is not possible
 * - Only one shortest path will return
 * 
 * How to use?
 * - You have to pass TMXTiledMap* and TMXPathFinding::DIRECTION to create TMXPathFinding object
 *		TMXTiledMap *tileMap = TMXTiledMap::create("level_1.tmx");
 *		TMXPathFinding *pathFind = new TMXPathFinding(tileMap, TMXPathFinding::DIRECTION::FOUR);
 * - Note : For Hexagonal you must pass DIRECTION::SIX
 *			For Orthogonal & Isometric DIRECTION::FOUR or DIRECTION::EIGHT
 * 
 * - You can set desired Layers list, [Note: If you don't call this, it will check for all layers in given TMXTiledMap]
 *		pathFind->setTileLayers({"Layer_1", "Layer_2", "Layer_3"});
 * - If you want to reset tile layers, means algo should check for all layers
 *		pathFind->setTileLayers( { } );
 * 
 * - To get path by using Walkable list
 *		Vec2 startPos(5, 5);
 *		Vec2 goalPos(10, 10);
 *		std::vector<Vec2> path = pathFind->getPathUsingWalkable(startPos, goalPos, { TILE_ROAD, TILE_GRASS });
 * 
 * - To get path by using Obstacle list
 *		std::vector<Vec2> path = pathFind->getPathUsingObstacles(startPos, goalPos, { TILE_WALL, TILE_WATER });
 * 
 * - Note : TILE_ROAD, TILE_GRASS, TILE_WALL, TILE_WATER will be GID as per your tileset define in TMX map.
 * - Remeber GID starts with 1 not 0.
 * 
 * - Once you got path
 * if (path.size() == 0)
 *		log("No Path is available...");
 * else {
 *		for (Vec2 tilePos : path)
 *			log("x: %f  y: %f", tilePos.x, tilePos.y);
 * }
 * 
 * - Once you finish Delete path finding object
 *		delete pathFind;
 * 
 */
class TMXPathFinding {
public:
	/**
	 * Directions for Path finding
	 */
	enum DIRECTION { FOUR = 4, SIX = 6, EIGHT = 8 };

	/** Creates a TMXPathFinding with TMX map.
	 *
	 * @param map A TMX tiled map obj.
	 * @param noOfDirection TMXPathFinding::DIRECTION::FOUR, SIX or EIGHT
     * @return An object.
	 */
	TMXPathFinding(TMXTiledMap *map, DIRECTION noOfDirection);
	~TMXPathFinding();

	/** Set Tile Layer names 
	 * 
	 * @param layerNames Required layer names while finding Path.
	 * example :
	 *		setTileLayers({"Layer_1", "Layer_2"});
	 *		setTileLayers( { } ); // Default, this will check for all layers inside TMXTiledMap
	 */
	void setTileLayers(std::vector<std::string> layerNames);

	/** Get shortest Path between Start & Goal Tile/cell avoiding Obstacles
	 * 
	 * @param startPos Starting Tile/cell pos
	 * @param endPos Goal Tile/cell pos
	 * @param obstacleGIDs Give all obstacle GIDs
	 * @return if succeed then route from startPos to endPos o/w empty vector
	 */
	std::vector<Vec2> getPathUsingObstacles(Vec2 startPos, Vec2 endPos, std::vector<int> obstacleGIDs);

	/** Get shortest Path between Start & Goal Tile/cell using Walkable
	 * 
	 * @param startPos Starting Tile/cell pos
	 * @param endPos Goal Tile/cell pos
	 * @param walkableGIDs Give all walkable GIDs
	 * @return if succeed then route from startPos to endPos o/w empty vector
	 */
	std::vector<Vec2> getPathUsingWalkable(Vec2 startPos, Vec2 endPos, std::vector<int> walkableGIDs);

private:
	/**
	* TileNode will be used internally.
	*/
	struct TileNode {
		Vec2 location;
		int costFromStart, costToGoal, totalCost;
		TileNode *parent;
		TileNode() {
			costFromStart = 0;
			costToGoal = 0;
			totalCost = 0;
			location = Vec2::ZERO;
		}
		inline Vec2 getLocation() { return location; }
		inline void setLocX(int X) { location.x += X; }
		inline void setLocY(int Y) { location.y += Y; }

		inline int getCostFromStart() { return costFromStart; }
		inline void setCostFromStart(int cost) { costFromStart += cost; }

		inline int getCostToGoal() { return costToGoal; }
		inline void setCostToGoal(int cost) { costToGoal = cost; }

		inline int getTotalCost() { return totalCost; }
		inline void setTotalCost() { totalCost = costFromStart + costToGoal; }
		inline void setTotalCost(int cost) { totalCost = cost; }

		inline TileNode* getParent() { return parent; }
		inline void setParent(TileNode *node) { parent = node; }
	};

	int euclideanDistance(TileNode *start, TileNode *end);
	std::vector<Vec2> getPath(Vec2 startPos, Vec2 endPos, std::vector<int> walkableGIDs, std::vector<int> obstacleGIDs);
	
	TMXTiledMap *tileMap;
	std::vector<TMXLayer*> tileLayers;

	TileNode *newNode, *modifiedNode, *extractNode, *startNode, *goalNode;
	int dir, iCost, min, costToOpen, costToClose;

	std::unordered_map<TileNode*, int> open, close;
	bool inOpen, inClose;

};

#endif // _TMX_TILE_PATH_FINDING_H_
