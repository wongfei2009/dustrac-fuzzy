#include "usercontroller.hpp"
#include "inputhandler.hpp"
#include "car.hpp"

UserController::UserController(Car& car, InputHandler& handler, unsigned int numPlayer):
	CarController(car), m_handler(handler), m_num_player(numPlayer) {}

void UserController::update(bool isRaceCompleted) {
	m_car.clearStatuses();

	// Handle accelerating / braking
	if (m_handler.getActionState(m_num_player, InputHandler::IA_DOWN))
	{
		if (!isRaceCompleted)
		{
			m_car.brake();
		}
	}
	else if (m_handler.getActionState(m_num_player, InputHandler::IA_UP))
	{
		if (!isRaceCompleted)
		{
			m_car.accelerate();
		}
	}

	// Handle turning
	if (m_handler.getActionState(m_num_player, InputHandler::IA_LEFT))
	{
		m_car.turnLeft();
	}
	else if (m_handler.getActionState(m_num_player, InputHandler::IA_RIGHT))
	{
		m_car.turnRight();
	}
	else
	{
		m_car.noSteering();
	}

}
