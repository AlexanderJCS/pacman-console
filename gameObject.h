#pragma once

#include <vector>
#include <map>

enum direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	STOP
};

enum movementMode {
	SCATTER,
	CHASE,
	FRIGHTENED
};

static std::map<std::vector<int>, direction> COORDS_TO_DIR = {
	{ { 0, -1 }, UP},
	{ { 0, 1 }, DOWN},
	{ { -1, 0 }, LEFT},
	{ { 1, 0 }, RIGHT},
	{ { 0, 0 }, STOP}
};

static std::map<direction, std::vector<int>> DIR_TO_COORDS = {
	{ UP, { 0, -1 } },
	{ DOWN, { 0, 1 } },
	{ LEFT, { -1, 0 } },
	{ RIGHT, { 1, 0 } },
	{ STOP, { 0, 0 } }
};

static std::map<direction, std::vector<int>> OPP_DIRECTIONS = {
	{ UP, { 0, 1 } },
	{ DOWN, { 0, -1 } },
	{ LEFT, { 1, 0 } },
	{ RIGHT, { -1, 0 } },
	{ STOP, { 0, 0 } }
};

struct gameObject
{
	short x;
	short y;
	direction dir = STOP;  // used for player and ghost movement
	direction moveQueue = STOP;  // used for player movement
	movementMode mode = SCATTER;
};