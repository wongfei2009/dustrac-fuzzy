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

void CarController::setTrack(Track & track)
{
    m_track = &track;
    m_route = &track.trackData().route();
}

Car & CarController::car() const
{
    return m_car;
}

