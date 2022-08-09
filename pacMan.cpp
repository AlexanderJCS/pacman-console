#include <windows.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <io.h>
#include <fcntl.h>

#include "gameObject.h"
#include "levelUtils.h"


const int REFRESH = 50;
const std::string LEVELNAME = "level.txt";
const short GHOST_CHANGE = 10;  // a 1/n change to change a ghost's mode every frame

const std::map<char, std::wstring> UNICODE_MAP = {
	{ 'X', L"\u001b[34;1m\u2588\u001b[0m" },
	{ 'G', L"\u2593" },
	{ 'O', L"\u001b[31;1m\u15e3\u001b[0m" },
	{ 'P', L"\u001b[33;1m\u15e4\u001b[0m" },
	{ '*', L"●"}
};


void removeElement(std::vector<std::vector<int>>& untrimmed, std::vector<int> element)
{
	untrimmed.erase(std::remove(untrimmed.begin(), untrimmed.end(), element), untrimmed.end());
}


void showConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}


size_t maxWidth(std::vector<std::string> lines)
{
	size_t maxSize = 0;

	for (const auto& line : lines)
	{
		if (line.size() > maxSize)
		{
			maxSize = line.size();
		}
	}

	return maxSize - 1;
}


class PacMan
{
	short lives = 3;
	
	std::map<char, std::vector<gameObject>> levelMap;
	size_t levelWidth;
	size_t levelHeight;

	void draw()
	{
		// Create the screen string
		std::vector<std::string> screen;

		for (int i = 0; i < levelHeight; i++)
		{
			screen.push_back("  ");
			
			for (int j = 0; j < levelWidth; j++)
			{
				screen[i] += "  ";
			}
		}
		
		// Draw level
		for (const auto& it : levelMap)
		{
			for (const auto& obj : it.second)
			{	
				screen[obj.y][obj.x * 2] = it.first;

				if (it.first != 'P' && it.first != 'O' && it.first != '*')
				{
					screen[obj.y][obj.x * 2 + 1] = it.first;
				}
				
				else
				{
					screen[obj.y][obj.x * 2 + 1] = ' ';
				}
			}
		}

		std::wcout << "\033[1;1H";  // Move cursor to top left

		// Output the screen
		for (const auto& line : screen)
		{
			for (const auto& ch : line)
			{
				if (UNICODE_MAP.find(ch) != UNICODE_MAP.end())
				{
					std::wcout << UNICODE_MAP.at(ch);
				}
				
				else
				{
					std::wcout << ch;
				}
			}
			std::wcout << "\n";
		}

		// Output lives
		std::wcout << "Lives: ";

		for (short i = 0; i < lives; i++)
		{
			std::wcout << L"\u001b[33;1m\u15e4\u001b[0m";
		}
	}

	bool validMove(int x, int y, std::vector<gameObject> colliders)
	{
		for (const auto& collider : colliders)
		{
			if (collider.x == x && collider.y == y)
			{
				return false;
			}
		}
		
		return true;
	}

	void movePacMan()
	{
		for (auto& pacman : levelMap['P'])
		{
			short queueX = DIR_TO_COORDS[pacman.moveQueue][0] + pacman.x;
			short queueY = DIR_TO_COORDS[pacman.moveQueue][1] + pacman.y;

			if (validMove(queueX, queueY, levelMap['X']) && pacman.moveQueue != STOP)
			{
				pacman.x = queueX;
				pacman.y = queueY;
				pacman.dir = pacman.moveQueue;
				pacman.moveQueue = STOP;
				continue;
			}
			
			short moveX = DIR_TO_COORDS[pacman.dir][0] + pacman.x;
			short moveY = DIR_TO_COORDS[pacman.dir][1] + pacman.y;

			if (validMove(moveX, moveY, levelMap['X']))
			{
				pacman.x = moveX;
				pacman.y = moveY;
				continue;
			}

			pacman.dir = STOP;
			pacman.moveQueue = STOP;
		}
	}

	void applyPacManVelocity(short xVel, short yVel)
	{
		for (auto& pacman : levelMap['P'])
		{
			if (xVel)
			{
				pacman.x = xVel;
			}
			
			if (yVel)
			{
				pacman.y = yVel;
			}
		}
	}

	/*
	The next move pacman will execute when it is valid
	*/

	void setMoveQueue(direction dir)
	{
		for (auto& pacMan : levelMap['P'])
		{
			pacMan.moveQueue = dir;
		}
	}

	void getInput()
	{
		if (GetKeyState('W') & 0x8000)
		{
			setMoveQueue(UP);
		}

		else if (GetKeyState('S') & 0x8000)
		{
			setMoveQueue(DOWN);
		}

		else if (GetKeyState('A') & 0x8000)
		{
			setMoveQueue(LEFT);
		}

		else if (GetKeyState('D') & 0x8000)
		{
			setMoveQueue(RIGHT);
		}
	}

	std::vector<std::vector<int>> possibleDirs(direction dir, short x, short y)
	{
		std::vector<std::vector<int>> ghostMoves = {
			{ 1, 0 },
			{ 0, 1 },
			{ -1, 0 },
			{ 0, -1 }
		};

		removeElement(ghostMoves, OPP_DIRECTIONS[dir]);

		// Find elements to remove
		std::vector<std::vector<int>> elementsToRemove;

		for (auto ghostMove : ghostMoves)
		{
			std::vector<gameObject> nonCollideables = levelMap['X'];
			nonCollideables.insert(nonCollideables.end(), levelMap['O'].begin(), levelMap['O'].end());
			
			if (!validMove(ghostMove[0] + x, ghostMove[1] + y, nonCollideables))
			{
				elementsToRemove.push_back(ghostMove);
			}
		}

		// Remove elements
		for (auto element : elementsToRemove)
		{
			removeElement(ghostMoves, element);
		}

		return ghostMoves;
	}

	void applyGhostDirChase(gameObject& ghost)
	{
		std::vector<std::vector<int>> ghostMoves = possibleDirs(ghost.dir, ghost.x, ghost.y);	
			
		// Determine the direction
		if (ghostMoves.size() == 0)
		{
			ghost.dir = STOP;
			return;
		}
			
		std::vector<int> minDistDir = { 0, 0 };
		float minDist = 999;

		for (auto ghostMove : ghostMoves)
		{
			float dist = pow(ghost.x - levelMap['P'][0].x + ghostMove[0], 2) +
					        pow(ghost.y - levelMap['P'][0].y + ghostMove[1], 2);	
				
			if (dist < minDist)
			{
				minDist = dist;
				minDistDir = ghostMove;
			}
		}

		ghost.dir = COORDS_TO_DIR[minDistDir];
	}

	void applyGhostDirScatter(gameObject& ghost)
	{
		std::vector<std::vector<int>> ghostMoves = possibleDirs(ghost.dir, ghost.x, ghost.y);

		// Determine the direction
		if (ghostMoves.size() == 0)
		{
			ghost.dir = STOP;
			return;
		}

		std::vector<int> maxDistDir = { 0, 0 };
		float maxDist = 0;

		for (auto ghostMove : ghostMoves)
		{
			float dist = pow(ghost.x - levelMap['P'][0].x + ghostMove[0], 2) +
				pow(ghost.y - levelMap['P'][0].y + ghostMove[1], 2);

			if (dist > maxDist)
			{
				maxDist = dist;
				maxDistDir = ghostMove;
			}
		}

		ghost.dir = COORDS_TO_DIR[maxDistDir];
	}

	void applyGhostDirs()
	{
		for (auto& ghost : levelMap['O'])
		{
			switch (ghost.mode)
			{
			case CHASE:
				applyGhostDirChase(ghost);
				break;
			
			case SCATTER:
				applyGhostDirScatter(ghost);
			}
		}
	}

	void moveGhosts()
	{
		for (auto& ghost : levelMap['O'])
		{
			ghost.x += DIR_TO_COORDS[ghost.dir][0];
			ghost.y += DIR_TO_COORDS[ghost.dir][1];
		}
	}

	void teleportOtherSide()
	{
		for (auto& object : levelMap['P'])
		{
			if (object.x > levelWidth && object.dir != LEFT)
			{
				object.x = 0;
			}
			
			if (object.x == -1)
			{
				object.x = levelWidth;
			}
		}

		for (auto& object : levelMap['O'])
		{
			if (object.x > levelWidth && object.dir != LEFT)
			{
				object.x = 0;
			}

			if (object.x == -1)
			{
				object.x = levelWidth;
			}
		}
	}

	void removeGameObject(short x, short y, std::vector<gameObject>& container)
	{
		for (int i = 0; i < container.size(); i++)
		{
			if (container[i].x == x && container[i].y == y)
			{
				container.erase(container.begin() + i);
				break;
			}
		}
	}
	
	void collectPellets()
	{
		for (auto& pacman : levelMap['P'])
		{
			if (!validMove(pacman.x, pacman.y, levelMap['*']))
			{
				removeGameObject(pacman.x, pacman.y, levelMap['*']);
			}
		}
	}

	void changeGhostMode()
	{
		if (rand() % GHOST_CHANGE == 0)
		{
			int ghostIndex = rand() % levelMap['O'].size();
			
			if (levelMap['O'][ghostIndex].mode == CHASE)
			{
				levelMap['O'][ghostIndex].mode = SCATTER;
			}
			
			else if (levelMap['O'][ghostIndex].mode == SCATTER)
			{
				levelMap['O'][ghostIndex].mode = CHASE;
			}
		}
	}
	
	bool won()
	{
		if (levelMap['*'].size() != 0)
		{
			return false;
		}

		for (int i = 0; i < 3; i++)
		{
			std::wcout << "You won!\n";
		}
		
		Sleep(3000);

		return true;
	}

public:
	PacMan()
	{
		auto level = LevelUtils::readLevel(LEVELNAME);
		
		levelHeight = level.size();
		levelWidth = maxWidth(level);

		levelMap = LevelUtils::parseLevel(level);
	}

	void run()
	{
		short counter = 0;
		
		while (true)
		{
			counter++;
			
			teleportOtherSide();
			draw();
			collectPellets();
			
			if (won())
			{
				break;
			}
			
			getInput();
			movePacMan();
			
			if (counter == 2 || counter == 3)
			{
				applyGhostDirs();
				moveGhosts();
				
				if (counter == 3)
				{
					counter = 0;
				}
			}

			changeGhostMode();
			Sleep(REFRESH);
		}
	}
};


int main()
{
	srand(time(0));
	_setmode(_fileno(stdout), _O_U16TEXT);
	
	showConsoleCursor(false);
	system("cls");

	PacMan pm;
	pm.run();
}