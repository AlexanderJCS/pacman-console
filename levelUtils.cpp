#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "gameObject.h"
#include "levelUtils.h"


std::vector<std::string> LevelUtils::readLevel(std::string levelPath)
{
	std::ifstream fileStream;
	fileStream.open(levelPath);

	if (!fileStream.is_open())
	{
		std::cout << "Could not open file: " << levelPath << std::endl;
		return {};
	}

	std::vector<std::string> contents;

	std::string line;
		
	while (std::getline(fileStream, line))
	{
		contents.push_back(line);
	}

	fileStream.close();

	return contents;
}


std::map<char, std::vector<gameObject>> LevelUtils::parseLevel(std::vector<std::string> levelContents)
{
	std::map<char, std::vector<gameObject>> levelMap;

	for (int i = 0; i < levelContents.size(); i++)
	{
		for (int j = 0; j < levelContents[i].size(); j++)
		{
			if (&levelContents[i][j] == " ")
			{
				continue;
			}
			
			gameObject object;
			object.x = j;
			object.y = i;

			if (levelMap.find(levelContents[i][j]) == levelMap.end())
			{
				levelMap.insert({ levelContents[i][j], { object } });
			}

			else
			{
				levelMap[levelContents[i][j]].push_back(object);
			}
		}
	}

	return levelMap;
}