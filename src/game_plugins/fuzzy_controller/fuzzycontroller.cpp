#include <fuzzycontroller.hpp>
#include <car.hpp>
#include <track.hpp>
#include <trackdata.hpp>
#include <tracktile.hpp>
#include <../common/route.hpp>
#include <../common/targetnodebase.hpp>
#include <../common/tracktilebase.hpp>

#include <MCTrigonom>

FuzzyController::FuzzyController(Car& car, fl::Engine* engine):
	CarController(car), m_fis(engine) {}

FuzzyController::FuzzyController(Car& car, const std::string& fis_filename):
	CarController(car), m_fis(loadFis(fis_filename)) {}

FuzzyController::FuzzyController(Car& car, std::istream& fis_file):
	CarController(car), m_fis(loadFis(fis_file)) {}

fl::Engine* FuzzyController::loadFis(std::istream& fis_file) {
	std::string strfis{
		std::istreambuf_iterator<char>(fis_file),
		std::istreambuf_iterator<char>()
	};

	fl::FisImporter importer;
	fl::Engine* fis = importer.fromString(strfis);

	std::string status;
	if (not fis->isReady(&status))
		throw fl::Exception("Engine not ready. "
			"The following errors were encountered:\n" + status, FL_AT);

	return fis;
}

fl::Engine* FuzzyController::loadFis(const std::string& fis_filename) {
	std::ifstream file(fis_filename);
	return loadFis(file);
}

void FuzzyController::update(bool isRaceCompleted) {
	if (m_track) {
		m_car.clearStatuses();

		const Route    & route       = m_track->trackData().route();
		TargetNodeBase & tnode       = route.get(m_car.currentTargetNodeIndex());
		TrackTile      & currentTile = *m_track->trackTileAtLocation(
			m_car.location().i(), m_car.location().j());

		steerControl(tnode);
		speedControl(currentTile, isRaceCompleted);
	}
}

void FuzzyController::steerControl(TargetNodeBase& tnode) {
	// Initial target coordinates
	MCVector3dF target(tnode.location().x(), tnode.location().y());
	target -= MCVector3dF(m_car.location());

	MCFloat angle = MCTrigonom::radToDeg(std::atan2(target.j(), target.i()));
	MCFloat cur   = static_cast<int>(m_car.angle()) % 360;
	MCFloat diff  = angle - cur;
	MCFloat absSpeed = m_car.absSpeed();

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

	// if there is only one input variable, run this as a P controller,
	// if two, run as a PD
	if(m_fis->numberOfInputVariables() >= 3) {
		m_fis->getInputVariable(0)->setInputValue(diff);
		m_fis->getInputVariable(1)->setInputValue(diff - m_lastDiff);
		m_fis->getInputVariable(2)->setInputValue(absSpeed);
	} else if(m_fis->numberOfInputVariables() >= 2) {
		m_fis->getInputVariable(0)->setInputValue(diff);
		m_fis->getInputVariable(1)->setInputValue(diff - m_lastDiff);
	} else {
		m_fis->getInputVariable(0)->setInputValue(diff);
	}

	// run the fuzzy controller
	m_fis->process();
	auto control = m_fis->getOutputVariable(0)->defuzzify();

	const MCFloat maxControl = 1.5;
	control = control < 0 ? -control : control;
	control = control > maxControl ? maxControl : control;

	const MCFloat maxDelta = 3.0;
	if (diff < -maxDelta)
	{
		m_car.turnRight(control);
	}
	else if (diff > maxDelta)
	{
		m_car.turnLeft(control);
	}

	// Store the last difference
	m_lastDiff = diff;
}

void FuzzyController::speedControl(TrackTile& currentTile, bool isRaceCompleted) {
	const float absSpeed = m_car.absSpeed();

	// if the fuzzy controller has two outputs, use the second one for speed
	// control; otherwise fall back to the standard version
	if(m_fis->numberOfOutputVariables() >= 2) {
		auto controllerSpeed = m_fis->getOutputVariable(1)->defuzzify() / 10;

		// if controllerSpeed > absSpeed, accelerate
		// if controllerSpeed < 0, brake
		if(controllerSpeed > absSpeed) {
			m_car.accelerate();
		} else if(controllerSpeed < 0) {
			m_car.brake();
		}

	} else {

		// TODO: Maybe it'd be possible to adjust speed according to
		// the difference between current and target angles so that
		// computer hints wouldn't be needed anymore..?

		// Braking / acceleration logic
		bool accelerate = true;
		bool brake      = false;

		if (isRaceCompleted)
		{
			accelerate = false;
		}
		else
		{
			// The following speed limits are experimentally defined.
			float scale = 0.9;
			if (currentTile.computerHint() == TrackTile::CH_BRAKE)
			{
				if (absSpeed > 14.0 * scale)
				{
					brake = true;
				}
			}

			if (currentTile.computerHint() == TrackTile::CH_BRAKE_HARD)
			{
				if (absSpeed > 9.5 * scale)
				{
					brake = true;
				}
			}

			if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_90)
			{
				if (absSpeed > 7.0 * scale)
				{
					accelerate = false;
				}
			}

			if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_LEFT ||
				currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_RIGHT)
			{
				if (absSpeed > 8.3 * scale)
				{
					accelerate = false;
				}
			}

			if (absSpeed < 3.6 * scale)
			{
				accelerate = true;
				brake = false;
			}
		}

		if (brake)
		{
			m_car.brake();
		}
		else if (accelerate)
		{
			m_car.accelerate();
		}
	}
}
