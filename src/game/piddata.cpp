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
	distanceErrors(),
	steerControl(0),
	speedControl(0),
	m_random(random),
	m_lastTargetNodeIndex(0),
	m_randomDisplacement(generateDisplacement())
{
}

inline MCVector2dF PIDData::generateDisplacement() const
{
	return m_random ? MCRandom::randomVector2d() *
		TrackTileBase::TILE_W / 8 : MCVector2dF();
}

void PIDData::updateDisplacement()
{
    m_randomDisplacement = generateDisplacement();
}

void PIDData::updateErrors(const Car& car, const Route& route)
{
	unsigned int targetNodeIndex = car.currentTargetNodeIndex();

	if(m_lastTargetNodeIndex != targetNodeIndex)
	{
		updateDisplacement();
		m_lastTargetNodeIndex = targetNodeIndex;
	}

	TargetNodeBase & targetNode     = route.get(targetNodeIndex);
	MCVector3dF target(targetNode.location().x(), targetNode.location().y());

	// add random tolerance and make the target relative to the current pos
	target -= MCVector3dF(car.location() + MCVector3dF(m_randomDisplacement));

	MCFloat targetAngle = MCTrigonom::radToDeg(std::atan2(target.j(), target.i()));
	MCFloat angleError = constrainAngle(targetAngle - car.angle());
	angularErrors.update(angleError);

	// current distance to the target location; note that target is already
	// relative to the car's present position
	distanceErrors.update(target.length());
}

