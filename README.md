# TMXPathFinding
TMXPathFinding for Cocos2d-x 3.x

TMXPathFinding knows how to find path into TMX map.

It uses TMX tiled map created by http://www.mapeditor.org
It supports orthogonal, hexagonal and isometric tile maps.
During Path finding, it takes current GID values from TMXLayers.
 
Features:
 - Find Path between two Tile coordinates
 - DIRECTION support : 
		1) DIRECTION::FOUR  - L, R, U, D
		2) DIRECTION::EIGHT - L, R, U, D, TL, BR, TR, BL
 - You can add different Obstacle tiles.		e.g.- TILE_WALL, TILE_WATER
 - You can add different Walkable tiles.		e.g.- TILE_ROAD, TILE_GRASS
 - You can find path using only Obstacle tiles
 - You can find path using only Walkable tiles
 - You can find path for single or combined TMXLayers
 
Limitations:
 - Multiple path finding is not possible
 - Only one shortest path will return
 
How to use?
 - You have to pass TMXTiledMap* and TMXPathFinding::DIRECTION to create TMXPathFinding object
		TMXTiledMap *tileMap = TMXTiledMap::create("level_1.tmx");
		TMXPathFinding *pathFind = new TMXPathFinding(tileMap, TMXPathFinding::DIRECTION::FOUR);
 - Note : For Hexagonal you must pass DIRECTION::SIX
			For Orthogonal & Isometric DIRECTION::FOUR or DIRECTION::EIGHT
 
 - You can set desired Layers list, [Note: If you don't call this, it will check for all layers in given TMXTiledMap]
		pathFind->setTileLayers({"Layer_1", "Layer_2", "Layer_3"});
 - If you want to reset tile layers, means algo should check for all layers
		pathFind->setTileLayers( { } );
 
 - To get path by using Walkable list
		Vec2 startPos(5, 5);
		Vec2 goalPos(10, 10);
		std::vector<Vec2> path = pathFind->getPathUsingWalkable(startPos, goalPos, { TILE_ROAD, TILE_GRASS });
 
 - To get path by using Obstacle list
		std::vector<Vec2> path = pathFind->getPathUsingObstacles(startPos, goalPos, { TILE_WALL, TILE_WATER });
 
 - Note : TILE_ROAD, TILE_GRASS, TILE_WALL, TILE_WATER will be GID as per your tileset define in TMX map.
 - Remeber GID starts with 1 not 0.
 
 - Once you got path
 if (path.size() == 0)
		log("No Path is available...");
 else {
		for (Vec2 tilePos : path)
			log("x: %f  y: %f", tilePos.x, tilePos.y);
 }
 
 - Once you finish Delete path finding object
		delete pathFind;
 