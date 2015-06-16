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
	PIDController(car, false), m_fis(engine) {}

FuzzyController::FuzzyController(Car& car, const std::string& fis_filename):
	PIDController(car, false), m_fis(loadFis(fis_filename)) {}

FuzzyController::FuzzyController(Car& car, std::istream& fis_file):
	PIDController(car, false), m_fis(loadFis(fis_file)) {}

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

float FuzzyController::steerControl(bool) {
	// if there is only one input variable, run this as a P controller,
	// if two, run as a PD
	if(m_fis->numberOfInputVariables() >= 3) {
		m_fis->getInputVariable(0)->setInputValue(m_steeringData.error);
		m_fis->getInputVariable(1)->setInputValue(m_steeringData.deltaError);
		m_fis->getInputVariable(2)->setInputValue(m_car.absSpeed());
	} else if(m_fis->numberOfInputVariables() >= 2) {
		m_fis->getInputVariable(0)->setInputValue(m_steeringData.error);
		m_fis->getInputVariable(1)->setInputValue(m_steeringData.deltaError);
	} else {
		m_fis->getInputVariable(0)->setInputValue(m_steeringData.error);
	}

	// run the fuzzy controller
	m_fis->process();
	return m_fis->getOutputVariable(0)->defuzzify();
}

float FuzzyController::speedControl(bool isRaceCompleted) {
	// if the fuzzy controller has two outputs, use the second one for speed
	// control; otherwise fall back to the standard version
	if(m_fis->numberOfOutputVariables() >= 2) {
		return m_fis->getOutputVariable(1)->defuzzify() / 10;
	} else {
		return PIDController::speedControl(isRaceCompleted);
	}
}
