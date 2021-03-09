#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Aircraft.hpp"

struct AircraftMover
{
	AircraftMover(float vx, float vy, float vz)
		: x(vx)
		, y(vy)
		, z(vz)
	{
	}

	void operator() (Aircraft& aircraft, const GameTimer& gt) const
	{
		aircraft.setVelocity(x, y, z);
	}

	float x; 
	float y;
	float z;
};



Player::Player()
{
}

void Player::handleEvent(CommandQueue& commands, const GameTimer& gt)
{
	// TO-DO: Handle One time Events
}

void Player::handleRealtimeInput(CommandQueue& commands, const GameTimer& gt)
{

	float playerSpeed = 1000.0f * gt.DeltaTime(); 
	
	if (d3dUtil::IsKeyDown('A'))
	{
		Command moveLeft;
		moveLeft.category = Category::PlayerAircraft;
		moveLeft.action = derivedAction<Aircraft>(AircraftMover(-playerSpeed, 0, 0));
		commands.push(moveLeft);
	}
	else if (d3dUtil::IsKeyDown('D'))
	{
		Command moveRight;
		moveRight.category = Category::PlayerAircraft;
		moveRight.action = derivedAction<Aircraft>(AircraftMover(playerSpeed, 0, 0));
		commands.push(moveRight);
	}
	else if (d3dUtil::IsKeyDown('W'))
	{
		Command moveUp;
		moveUp.category = Category::PlayerAircraft;
		moveUp.action = derivedAction<Aircraft>(AircraftMover(0, 0, playerSpeed));
		commands.push(moveUp);
	}
	else if (d3dUtil::IsKeyDown('S'))
	{
		Command moveDown;
		moveDown.category = Category::PlayerAircraft;
		moveDown.action = derivedAction<Aircraft>(AircraftMover(0, 0, -playerSpeed));
		commands.push(moveDown);
	}
	else 
	{
		Command stop;
		stop.category = Category::PlayerAircraft;
		stop.action = derivedAction<Aircraft>(AircraftMover(0, 0, 0));
		commands.push(stop);
	}
}
