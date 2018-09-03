#include <fuzzycontroller.hpp>
#include <car.hpp>

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
		m_fis->getInputVariable(0)->setValue(m_data.angularErrors.error);
		m_fis->getInputVariable(1)->setValue(m_data.angularErrors.deltaError);
		m_fis->getInputVariable(2)->setValue(m_car.speedInKmh());
	} else if(m_fis->numberOfInputVariables() >= 2) {
		m_fis->getInputVariable(0)->setValue(m_data.angularErrors.error);
		m_fis->getInputVariable(1)->setValue(m_data.angularErrors.deltaError);
	} else {
		m_fis->getInputVariable(0)->setValue(m_data.angularErrors.error);
	}

	// run the fuzzy controller
	m_fis->process();
	return m_fis->getOutputVariable(0)->getValue();
}

float FuzzyController::speedControl(bool isRaceCompleted) {
	// if the fuzzy controller has two outputs, use the second one for speed
	// control; otherwise fall back to the standard version
	if(m_fis->numberOfOutputVariables() >= 2) {
		return m_fis->getOutputVariable(1)->getValue();
	} else {
		return PIDController::speedControl(isRaceCompleted);
	}
}
