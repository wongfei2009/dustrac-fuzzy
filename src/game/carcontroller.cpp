#include "carcontroller.hpp"

#include "car.hpp"
#include "track.hpp"
#include "trackdata.hpp"

CarController::CarController(Car& car)
: m_car(car)
, m_track(nullptr)
, m_route(nullptr)
{
}

void CarController::update(bool isRaceCompleted) {
	m_car.clearStatuses();

	// query the controllers
	float steerC = steerControl(isRaceCompleted);
	float speedC = speedControl(isRaceCompleted);

	// report the progress to listeners
	report(steerC, speedC, isRaceCompleted);

	// steer according to steerC
	const MCFloat maxControl = 1.5;
	steerC = std::min(std::max(steerC, -maxControl), maxControl);

	if (steerC >= 0) {
	    m_car.turnRight(std::abs(steerC));
	} else if (steerC < 0) {
	    m_car.turnLeft(std::abs(steerC));
	}

	// accelerate/brake according to speedC
	if(speedC < 0) m_car.brake();
	else if(speedC > m_car.speedInKmh()) m_car.accelerate();
}

void CarController::report(float steerControl, float speedControl, bool isRaceCompleted) {
	if(m_listeners) {
		for(auto& listener: *m_listeners) {
			listener->report(m_car, m_track, steerControl, speedControl, isRaceCompleted);
		}
	}
}

void CarController::setTrack(Track & track)
{
    m_track = &track;
    m_route = &track.trackData().route();
}

Car & CarController::car() const
{
    return m_car;
}

