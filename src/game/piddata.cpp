#include "piddata.hpp"
#include "car.hpp"
#include "../common/route.hpp"
#include "../common/targetnodebase.hpp"
#include "../common/tracktilebase.hpp"

#include <MCRandom>
#include <MCTrigonom>

void DiffStore::update(float error_) {
	float tmpDelta = error_ - error;
	error = error_;
	deltaError2 = tmpDelta - deltaError;
	deltaError = tmpDelta;
}

PIDData::PIDData(bool random):
	angularErrors(),
	lateralErrors(),
	steerControl(0),
	speedControl(0),
	m_random(random),
	m_lastTargetNodeIndex(0),
	m_prevRandomTolerance(generateTolerance()),
	m_curRandomTolerance(generateTolerance())
{
}

MCVector2dF PIDData::generateTolerance() const {
	return m_random ? MCRandom::randomVector2d() * TrackTileBase::TILE_W / 8 : MCVector2dF();
}

void PIDData::updateRandomTolerance()
{
	m_prevRandomTolerance = m_curRandomTolerance;
    m_curRandomTolerance = generateTolerance();
}

void PIDData::updateErrors(const Car& car, const Route& route)
{
	unsigned int curNodeIndex = car.currentTargetNodeIndex();
	unsigned int prevNodeIndex = (curNodeIndex > 0) ?
		curNodeIndex - 1 : route.numNodes() - 1;

	if(m_lastTargetNodeIndex != curNodeIndex)
	{
		updateRandomTolerance();
		m_lastTargetNodeIndex = curNodeIndex;
	}

	TargetNodeBase & curNode     = route.get(curNodeIndex);
	TargetNodeBase & prevNode	 = route.get(prevNodeIndex);

	MCVector3dF curTarget(curNode.location().x(), curNode.location().y());
	MCVector3dF prevTarget(prevNode.location().x(), prevNode.location().y());

	// add random tolerance and make the targets relative to the current pos
	curTarget -= MCVector3dF(car.location() + MCVector3dF(m_curRandomTolerance));
	prevTarget -= MCVector3dF(car.location() + MCVector3dF(m_prevRandomTolerance));

	MCFloat targetAngle = MCTrigonom::radToDeg(std::atan2(curTarget.j(), curTarget.i()));
	MCFloat angleError = constrainAngle(targetAngle - car.angle());
	angularErrors.update(angleError);

	// point-line distance formula from http://geomalgorithms.com/a02-_lines.html
	MCVector3dF ul = (curTarget - prevTarget).normalized();
	MCVector3dF w = -prevTarget;
	MCFloat lateralError = (ul % w).length();
	lateralErrors.update(lateralError);
}

