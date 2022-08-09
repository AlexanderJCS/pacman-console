#pragma once

#include <string>
#include <map>

#include "gameObject.h"

namespace LevelUtils
{
	std::vector<std::string> readLevel(std::string levelPath);
	std::map<char, std::vector<gameObject>> parseLevel(std::vector<std::string> levelContents);
}