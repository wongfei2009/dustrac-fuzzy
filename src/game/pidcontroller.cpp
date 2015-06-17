#include <MCTrigonom>

#include "pidcontroller.hpp"
#include "car.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
#include "../common/route.hpp"
#include "../common/targetnodebase.hpp"
#include "../common/tracktilebase.hpp"

#include <MCRandom>
#include <MCTrigonom>
#include <MCTypes>

PIDController::PIDController(Car& car, bool random):
CarController(car), m_steeringData(),
m_speedData(), m_random(random),
m_lastTargetNodeIndex(0), m_randomTolerance()
{
}

void PIDController::update(bool isRaceCompleted) {
	if (m_track)
	{
		if (m_random && m_lastTargetNodeIndex != m_car.currentTargetNodeIndex())
        {
            setRandomTolerance();
        }

		m_car.clearStatuses();

		const Route    & route       = m_track->trackData().route();
		TargetNodeBase & tnode       = route.get(m_car.currentTargetNodeIndex());

		// Initial target coordinates
		MCVector3dF target(tnode.location().x(), tnode.location().y());
		target -= MCVector3dF(m_car.location() + MCVector3dF(m_randomTolerance));

		MCFloat angle = MCTrigonom::radToDeg(std::atan2(target.j(), target.i()));
		MCFloat cur   = static_cast<int>(m_car.angle()) % 360;
		MCFloat diff  = angle - cur;

		bool ok = false;
		while (!ok)
		{
		    if (diff > 180)
		    {
		        diff = diff - 360;
		        ok = false;
		    }
		    else if (diff < -180)
		    {
		        diff = diff + 360;
		        ok = false;
		    }
		    else
		    {
		        ok = true;
		    }
		}

		// update control data
		m_steeringData.updateError(diff);
		m_speedData.updateError(diff);

		// query the controllers
		float steerC = steerControl(isRaceCompleted);
		float speedC = speedControl(isRaceCompleted);

		// update control data
		m_steeringData.updateControl(steerC);
		m_speedData.updateControl(speedC);

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
		else if(speedC > m_car.absSpeed()) m_car.accelerate();

		// is this update to track progress??
		m_lastTargetNodeIndex = m_car.currentTargetNodeIndex();
	}
}

void PIDController::setRandomTolerance()
{
    m_randomTolerance = MCRandom::randomVector2d() * TrackTileBase::TILE_W / 8;
}

float PIDController::steerControl(bool) {
	return -(m_steeringData.error * 0.025 + m_steeringData.deltaError * 0.025);
}

float PIDController::speedControl(bool isRaceCompleted)
{
    // TODO: Maybe it'd be possible to adjust speed according to
    // the difference between current and target angles so that
    // computer hints wouldn't be needed anymore..?

	TrackTile      & currentTile = *m_track->trackTileAtLocation(
    m_car.location().i(), m_car.location().j());

	// Current speed of the car.
    const float absSpeed = m_car.absSpeed();
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
        float scale = 0.9;
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

