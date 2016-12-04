#include "pidcontroller.hpp"
#include "car.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
//#include "../common/route.hpp"
//#include "../common/targetnodebase.hpp"
//#include "../common/tracktilebase.hpp"

#include <MCRandom>
#include <MCTrigonom>
#include <MCTypes>

PIDController::PIDController(Car& car, bool random):
CarController(car), m_data(random)
{
}

void PIDController::update(bool isRaceCompleted) {
	if(!m_track) throw std::runtime_error("Track must be set for the PIDController before calling update.");

	m_car.clearStatuses();
	m_data.updateErrors(m_car, m_track->trackData().route());

	// query the controllers
	float steerC = steerControl(isRaceCompleted);
	float speedC = speedControl(isRaceCompleted);

	// report the progress to listeners
	report(steerC, speedC, isRaceCompleted);
	// log the control signals in CarData
	m_data.updateControl(steerC, speedC);

	// steer according to steerC
	const MCFloat maxControl = 1.5;
	steerC = std::min(std::max(steerC, -maxControl), maxControl);

	if (steerC > 0) {
	    m_car.steer(Car::Steer::Right, std::abs(steerC));
	} else if (steerC < 0) {
	    m_car.steer(Car::Steer::Left, std::abs(steerC));
	} else {
		m_car.steer(Car::Steer::Neutral, 0);
	}

	// accelerate/brake according to speedC
	if(speedC < 0) m_car.brake();
	else if(speedC > m_car.speedInKmh()) m_car.accelerate();
}

float PIDController::steerControl(bool)
{
	return -(m_data.angularErrors.error * 0.025 + m_data.angularErrors.deltaError * 0.025);
}

float PIDController::speedControl(bool isRaceCompleted)
{
    // TODO: Maybe it'd be possible to adjust speed according to
    // the difference between current and target angles so that
    // computer hints wouldn't be needed anymore..?

	TrackTile      & currentTile = *m_track->trackTileAtLocation(
    m_car.location().i(), m_car.location().j());

	// Current speed of the car.
    const float absSpeed = m_car.speedInKmh();
	// The prescribed speed under the circumnstances.
	// By default, we accelerate slightly.
	float controlSpeed = absSpeed + 1;

    if (isRaceCompleted)
    {
        return 0;
    }
    else
    {
        // The following speed limits are experimentally defined.
        float scale = 8.1;
        if (currentTile.computerHint() == TrackTile::CH_BRAKE)
        {
            if (absSpeed > 14.0 * scale)
            {
                controlSpeed = -1; // we brake
            }
        }

        if (currentTile.computerHint() == TrackTile::CH_BRAKE_HARD)
        {
            if (absSpeed > 9.5 * scale)
            {
                controlSpeed = -1; // we brake
            }
        }

        if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_90)
        {

			if (absSpeed > 7.0 * scale)
            {
                controlSpeed = absSpeed; // we don't accelerate
            }
        }

        if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_LEFT ||
            currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_RIGHT)
        {
			if (absSpeed > 8.3 * scale) {
				controlSpeed = absSpeed; // we don't accelerate
			}
        }

        if (absSpeed < 3.6 * scale)
        {
			controlSpeed = absSpeed + 1; // we accelerate slightly
        }
    }

   return controlSpeed;
}

