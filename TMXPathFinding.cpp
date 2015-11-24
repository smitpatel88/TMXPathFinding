/**
 *
 * Created by Smit Patel on 25/09/2015
 *
 * github: https://github.com/smitpatel88/TMXPathFinding
 *
 */

#include "TMXPathFinding.h"
#include <math.h>

TMXPathFinding::TMXPathFinding(TMXTiledMap *map, DIRECTION noOfDirection) {
	tileMap = map;
	dir = noOfDirection;
	switch (tileMap->getMapOrientation()) {
		case 0: // Orthogonal
			CCASSERT(dir != DIRECTION::SIX, "For Orthogonal Maps SIX direction is not possible");
			break;
		case 1: // Hexagonal
			CCASSERT(dir == DIRECTION::SIX, "For Hexagonal Maps only SIX direction is accepted");
			break;
		case 2: // Isometric
			CCASSERT(dir != DIRECTION::SIX, "For Isometric Maps SIX direction is not possible");
			break;
	}
	iCost = 0;
	min = 10000;
	inOpen = false;
	inClose = false;
	tileLayers.clear();
	setTileLayers( { } );

	extractNode = new TileNode();
}

TMXPathFinding::~TMXPathFinding() {
	for (auto o : open)
		delete o.first;
	for (auto c : close)
		delete c.first;
	open.clear();
	close.clear();
	tileLayers.clear();
}

void TMXPathFinding::setTileLayers(std::vector<std::string> layerNames) {
	tileLayers.clear();
	if (layerNames.size() == 0) {
		for (auto &layerNode : tileMap->getChildren()) {
			TMXLayer *layer = dynamic_cast<TMXLayer*>(layerNode);
			if (layer)
				tileLayers.push_back(layer);
		}
	}
	else {
		for (auto layerName : layerNames) {
			TMXLayer *layer = tileMap->getLayer(layerName);
			if (layer)
				tileLayers.push_back(layer);
		}
	}
}

std::vector<Vec2> TMXPathFinding::getPathUsingObstacles(Vec2 startPos, Vec2 endPos, std::vector<int> obstacleGIDs) {
	return getPath(startPos, endPos, {}, obstacleGIDs);
}

std::vector<Vec2> TMXPathFinding::getPathUsingWalkable(Vec2 startPos, Vec2 endPos, std::vector<int> walkableGIDs) {
	return getPath(startPos, endPos, walkableGIDs, {});
}

int TMXPathFinding::euclideanDistance(TileNode *start, TileNode *end) {
	int dx = end->getLocation().x - start->getLocation().x;
	int dy = end->getLocation().y - start->getLocation().y;
	int ans = sqrt(dx * dx + dy * dy);

	return ans;
}

std::vector<Vec2> TMXPathFinding::getPath(Vec2 startPos, Vec2 endPos, std::vector<int> walkableGIDs, std::vector<int> obstacleGIDs) {
	open.clear();
	close.clear();

	// inizialize a goal node
	goalNode = new TileNode();
	goalNode->setLocX(endPos.x);
	goalNode->setLocY(endPos.y);

	// inizialize a start node
	startNode = new TileNode();
	startNode->setLocX(startPos.x);
	startNode->setLocY(startPos.y);
	startNode->setCostFromStart(0);
	startNode->setParent(nullptr);

	int cost = euclideanDistance(startNode, goalNode);

	startNode->setCostToGoal(cost);
	startNode->setTotalCost();

	open.emplace(startNode, startNode->getTotalCost());

	while (open.size() != 0) {
		// Fix a Cost to check the values
		min = 32767; // std::numeric_limits<int>::max()
		TileNode *minNode;

		// Find minNode from open QUEUE
		for (auto kv : open) {
			extractNode = kv.first;
			iCost = kv.second;
			if (iCost < min) {
				min = iCost;    // Change min to the New Cost got from the open QUEUE
				minNode = extractNode;
			}
		}
		extractNode = minNode;
		open.erase(minNode); // pop node from open

		// if it's a goal, we're done
		if (extractNode->getLocation() == goalNode->getLocation()) {
			// 1- retrieve all extractNode's parents
			// 2- save into Vec2 vector
			// 3- reverse Vec2 vector
			// 4- return Vec2 vector
			std::vector<Vec2> points;
			points.push_back(extractNode->getLocation());

			int size = extractNode->getCostFromStart();
			for (int i = 0; i < size; i++) {
				points.push_back(Vec2(extractNode->getParent()->getLocation().x, extractNode->getParent()->getLocation().y));
				extractNode = extractNode->getParent();
			}
			std::reverse(points.begin(), points.end());

			return points;
		}
		else {
			for (int i = 0; i < dir; i++) {
				costToOpen = 0;
				costToClose = 0;
				inOpen = false;
				inClose = false;
				newNode = new TileNode();
				newNode->setLocX(extractNode->getLocation().x);
				newNode->setLocY(extractNode->getLocation().y);

				switch (i) {
					case 0: // left
						newNode->setLocX(-1);
						newNode->setLocY(0);
						break;
					case 1: // right
						newNode->setLocX(1);
						newNode->setLocY(0);
						break;
					case 2: // up
						newNode->setLocX(0);
						newNode->setLocY(1);
						break;
					case 3: // down
						newNode->setLocX(0);
						newNode->setLocY(-1);
						break;
					case 4: // top-left
						newNode->setLocX(-1);
						newNode->setLocY(1);
						break;
					case 5: // bottom-left
						newNode->setLocX(-1);
						newNode->setLocY(-1);
						break;
					case 6: // bottom-right
						newNode->setLocX(1);
						newNode->setLocY(-1);
						break;
					case 7: // top-right
						newNode->setLocX(1);
						newNode->setLocY(1);
						break;

				}

				if (newNode->getLocation() != goalNode->getLocation()) {
					if (newNode->getLocation().x < 0 || newNode->getLocation().y < 0 ||
						newNode->getLocation().x >= tileMap->getMapSize().width ||
						newNode->getLocation().y >= tileMap->getMapSize().height) {
						// newNode is invalid, outside tileMap so ignore
						continue;
					}

					bool isNeedToContinue = false;
					// check newNode in given/all layers
					for (auto layer : tileLayers) {
						for (int gid : obstacleGIDs) {
							if (layer->getTileGIDAt(newNode->getLocation()) == gid) {
								// newNode is obstacle so ignore
								isNeedToContinue = true;
								break;
							}
						}
						if (isNeedToContinue)
							break;
						if (walkableGIDs.size() > 0) {
							isNeedToContinue = true;
							for (int gid : walkableGIDs) {
								if (layer->getTileGIDAt(newNode->getLocation()) == gid) {
									// newNode is walkable
									isNeedToContinue = false;
									break;
								}
							}
							if (isNeedToContinue) // newNode is not walkable so ignore
								break;
						}
					}
					if (isNeedToContinue)
						continue;
				}

				cost = euclideanDistance(newNode, goalNode);
				newNode->setCostFromStart(extractNode->getCostFromStart() + 1);
				newNode->setCostToGoal(cost);
				newNode->setParent(extractNode);
				newNode->setTotalCost();
				inOpen = false;
				inClose = false;

				for (auto kv : open) {
					TileNode *node = kv.first;
					if (newNode->getLocation() == node->getLocation()) {
						costToOpen = node->getTotalCost();
						inOpen = true;
						break;
					}
				}
				for (auto kv : close) {
					TileNode *node = kv.first;
					if (newNode->getLocation() == node->getLocation()) {
						costToClose = node->getTotalCost();
						inClose = true;
						break;
					}
				}

				if ((inOpen && (newNode->getTotalCost() >= costToOpen)) || (inClose && (newNode->getTotalCost() >= costToClose))) {
					// newNode is already in open or close QUEUE with lower cost so ignore
					continue;
				}
				else {
					if (inClose) {
						close.erase(newNode);
						// reinsert newNode in open
						open.emplace(newNode, newNode->getTotalCost());
					}

					if (inOpen) {
						// adjust newNode's location in Open
						iCost = costToOpen;
						modifiedNode = newNode;
						// remove from open
						open.erase(newNode);

						modifiedNode->setTotalCost(newNode->getTotalCost());
						// updated node reinsert in open
						open.emplace(modifiedNode, modifiedNode->getTotalCost());
					}
					else {
						// if its neither in OPEN nor in CLOSE insert it in OPEN
						open.emplace(newNode, newNode->getTotalCost());
					}
				}
			}
		}
		close.emplace(extractNode, extractNode->getTotalCost());
	}

	std::vector<Vec2> dummy;
	return dummy;
}
