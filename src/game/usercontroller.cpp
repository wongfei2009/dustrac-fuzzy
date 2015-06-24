#include "usercontroller.hpp"
#include "inputhandler.hpp"
#include "car.hpp"

const float UserController::controlStep = 0.5;

UserController::UserController(Car& car, InputHandler& handler, unsigned int numPlayer):
	CarController(car), m_handler(handler), m_num_player(numPlayer) {}

float UserController::steerControl(bool isRaceCompleted) {
	if(isRaceCompleted) return 0;

	// Handle steering
	if (m_handler.getActionState(m_num_player, InputHandler::IA_LEFT))
	{
		return -controlStep;
	}
	else if (m_handler.getActionState(m_num_player, InputHandler::IA_RIGHT))
	{
		return controlStep;
	}

	return 0;
}

float UserController::speedControl(bool isRaceCompleted) {
	if(isRaceCompleted) return 0;

	// Handle braking
	if (m_handler.getActionState(m_num_player, InputHandler::IA_DOWN))
	{
		return -1;
	}

	// Handle accelerating
	if (m_handler.getActionState(m_num_player, InputHandler::IA_UP))
	{
		return m_car.speedInKmh() + 10;
	}

	return 0;
}

