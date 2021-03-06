#include "../proj.win32/main.h"
#include "gameController.h"
#include "TetrisScene.h"

using namespace Tetris;

GameController::GameController() : _inputHandler(&_map, &_currentShape)
{
	//push drawable objects into vector
	_objectsToDraw.push_back(&_map);//objects are placed on the screen starting from first element of the vector
	_objectsToDraw.push_back(&_menuPanel);
	_objectsToDraw.push_back(&_currentShape);//last object is on top of every other

	_map.addObserver(&_menuPanel);
}

GameController::~GameController()
{

}

bool GameController::GameLoop()
{
	bool stateChanged = false;
	
	stateChanged = _inputHandler.updateInput(_time) || Update(_time);

	return stateChanged;
}

bool GameController::Update(std::chrono::time_point<std::chrono::system_clock>& previousTime)
{
	bool stateChanged = false;//to know if we need to redraw screen

	auto currentTime = std::chrono::system_clock::now();
	auto elapsedTime = (currentTime - previousTime);

	if (elapsedTime > _timePerFrame)//ensure same amount of time per frame
	{
		if (_currentShape.hasLanded())
		{
			_currentShape.LandShape(_map);

			_map.CheckForFilledRow();

			if (CheckIfGameIsOver())
			{
				NewGame(*_tetrisNode);
			}
			else
			{
				//block landed, so we make new one
				NewShape();
			}

		}
		else
		{
			//apply gravity
			_currentShape.Move(Point::DOWN, _map);
			previousTime = currentTime;//we only need to remember time when block moved down 1 tile
			stateChanged = true;
		}

	}

	return stateChanged;
}

void GameController::NewShape()
{
	ShapeType shapeType = _shapeGenerator.NewShape();

	_currentShape.SetShape(shapeType);
	_currentShape.SetPosition(_newShapePosition);

	_menuPanel.SetUpcomingShape(_shapeGenerator.GetNextShapeType());

	_inputHandler.reset();
}

bool GameController::CheckIfGameIsOver()
{
	bool gameIsOver = false;

	Point position;
	TileType tileType;

	position.y = _map.GetBorderWidth();;//game is considered over when top most row has at least 1 solid brick in it

	size_t left = _map.GetBorderWidth();
	size_t right = _map.GetWidth() - _map.GetBorderWidth();

	for (size_t x = left; x < right; ++x)
	{
		position.x = x;
		tileType = _map.GetValueAt(position);
		if (tileType == TileType::brick)
		{
			gameIsOver = true;
			break;
		}
	}

	return gameIsOver;
}

void GameController::NewGame(cocos2d::Node& node)
{
	_tetrisNode = &node;
	
	_map.Init();
	_menuPanel.Init(_map, *_tetrisNode);

	_time = std::chrono::system_clock::now();

	NewShape();
}

std::vector<IDrawable*>& Tetris::GameController::GetObjectsToDraw()
{
	return _objectsToDraw;
}
