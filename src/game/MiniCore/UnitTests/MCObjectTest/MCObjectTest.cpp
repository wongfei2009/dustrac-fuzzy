// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "MCObjectTest.hpp"
#include "../../Physics/mcforcegenerator.hh"
#include "../../Physics/mcforceregistry.hh"
#include "../../Physics/mcrectshape.hh"
#include "../../Core/mcworld.hh"
#include "../../Core/mcobject.hh"
#include "../../Core/mctimerevent.hh"
#include "../../Core/mctrigonom.hh"
#include "../../Core/mcvector3d.hh"

#include <cmath>

// Damping factor defined in MCObject
static const MCFloat DAMPING = 0.999;

class TestObject : public MCObject
{
public:

    TestObject()
    : MCObject("TEST_OBJECT")
    , m_timerEventReceived(false)
    {
    }

    virtual bool event(MCEvent & event)
    {
        if (event.instanceTypeID() == MCTimerEvent::typeID())
        {
            m_timerEventReceived = true;
            return true;
        }

        return false;
    }

    bool m_timerEventReceived;
};

static void vector3dCompare(MCVector3dF vector1, MCVector3dF vector2)
{
    QVERIFY(qFuzzyCompare(vector1.i(), vector2.i()));
    QVERIFY(qFuzzyCompare(vector1.j(), vector2.j()));
    QVERIFY(qFuzzyCompare(vector1.k(), vector2.k()));
}

MCObjectTest::MCObjectTest()
{
}

void MCObjectTest::testAddChildren()
{
    MCWorld world;
    MCObject root("root");
    MCObjectPtr child1(new MCObject("child1"));
    MCObjectPtr child2(new MCObject("child2"));

    QVERIFY(&root.parent() == &root);
    QVERIFY(&child1->parent() == child1.get());
    QVERIFY(&child2->parent() == child2.get());

    root.addChildObject(child1);
    root.addChildObject(child2);

    QVERIFY(&child1->parent() == &root);
    QVERIFY(&child2->parent() == &root);

    QVERIFY(root.index() == -1);
    QVERIFY(child1->index() == -1);
    QVERIFY(child2->index() == -1);

    world.addObject(root); // Currently world doesn't add children

    QVERIFY(root.index() >= 0);
    QVERIFY(child1->index() == -1);
    QVERIFY(child2->index() == -1);

    root.addToWorld(); // Adding via object adds also children

    QVERIFY(root.index() >= 0);
    QVERIFY(child1->index() >= 0);
    QVERIFY(child2->index() >= 0);
}

void MCObjectTest::testAddToWorld()
{
    MCWorld world;
    MCObject object("test");
    QVERIFY(object.index() == -1);
    world.addObject(object);
    QVERIFY(object.index() >= 0);

    world.removeObject(object); // Lazy removal
    QVERIFY(object.index() >= 0);
    world.stepTime(1);
    QVERIFY(object.index() == -1);

    object.addToWorld();
    QVERIFY(object.index() >= 0);

    world.removeObjectNow(object); // Immediate removal
    QVERIFY(object.index() == -1);
}

void MCObjectTest::testAngularVelocityAndSleep()
{
    MCWorld world;
    MCObject object("TestObject");
    object.addToWorld();

    QVERIFY(qFuzzyCompare(object.angularVelocity(), MCFloat(0)));

    world.stepTime(1);
    QVERIFY(object.sleeping() == true);
    QVERIFY(object.index() == -1); // Removed from integration

    object.setAngularVelocity(1);

    QVERIFY(object.sleeping() == false);
    QVERIFY(object.index() >= 0);
    QVERIFY(qFuzzyCompare(object.angularVelocity(), MCFloat(1)));

    object.setAngularVelocity(0);

    QVERIFY(qFuzzyCompare(object.angularVelocity(), MCFloat(0)));

    world.stepTime(1);
    QVERIFY(object.sleeping() == true);
    QVERIFY(object.index() == -1);
}

void MCObjectTest::testAngularVelocityIntegration()
{
    MCWorld world;
    MCObject object("TestObject");
    object.addToWorld();

    QVERIFY(qFuzzyCompare(object.angularVelocity(), MCFloat(0)));
    QVERIFY(qFuzzyCompare(object.angle(), MCFloat(0)));

    const MCFloat step = 1;
    MCFloat aVel = 1;

    object.setAngularVelocity(aVel);
    world.stepTime(step);

    // Nothing should happen without shape
    QVERIFY(qFuzzyCompare(object.angle(), 0));

    MCShapePtr shape(new MCRectShape(nullptr, 10, 10));
    object.setShape(shape);
    object.setMomentOfInertia(1); // Needs to be > 0.1 to enable angular integration
    object.setAngularVelocity(aVel);

    world.stepTime(step);

    QVERIFY(qFuzzyCompare(object.angle(), MCTrigonom::radToDeg(aVel * step * DAMPING)));
    const MCFloat angle = object.angle();

    world.stepTime(step);
    aVel *= DAMPING;
    QVERIFY(qFuzzyCompare(object.angle(), angle + MCTrigonom::radToDeg(aVel * step * DAMPING)));
}

void MCObjectTest::testDefaultFlags()
{
    MCObject object("TestObject");
    QVERIFY(!object.bypassCollisions());
    QVERIFY(object.isRenderable());
    QVERIFY(!object.isParticle());
    QVERIFY(object.isPhysicsObject());
    QVERIFY(!object.isTriggerObject());

    object.setBypassCollisions(true);
    QVERIFY(object.bypassCollisions());

    object.setIsRenderable(false);
    QVERIFY(!object.isRenderable());

    object.setIsParticle(true);
    QVERIFY(object.isParticle());

    object.setIsPhysicsObject(false);
    QVERIFY(!object.isPhysicsObject());

    object.setIsTriggerObject(true);
    QVERIFY(object.isTriggerObject());
}

void MCObjectTest::testChildRotate()
{
    MCWorld world;
    world.setDimensions(0, 1024, 0, 768, 0, 100, 1);

    MCObject root("root");
    MCObjectPtr child1(new MCObject("child1"));
    MCObjectPtr child2(new MCObject("child2"));

    float child1Angle = 45;
    root.addChildObject(child1, MCVector3dF(1, 1, 1), child1Angle);

    const float child2Angle = 90;
    root.addChildObject(child2, MCVector3dF(2, 2, 2), 90);

    root.addToWorld();

    // Root at (0, 0, 0)

    const float rootAngle = 69;
    root.rotate(rootAngle); // Rotate children only when root rotates

    QVERIFY(root.angle() == rootAngle);
    QVERIFY(child1->angle() == child1Angle + rootAngle);
    QVERIFY(child2->angle() == child2Angle + rootAngle);

    vector3dCompare(child1->location(),
        MCVector3dF(MCTrigonom::rotatedVector(MCVector2dF(1.0, 1.0), rootAngle), 1.0f));

    vector3dCompare(child2->location(),
        MCVector3dF(MCTrigonom::rotatedVector(MCVector2dF(2.0, 2.0), rootAngle), 2.0f));

    // Now move the root

    const MCVector3dF rootLocation(3, 4, 5);
    root.translate(rootLocation);

    QVERIFY(root.angle() == rootAngle);
    QVERIFY(child1->angle() == child1Angle + rootAngle);
    QVERIFY(child2->angle() == child2Angle + rootAngle);

    vector3dCompare(child1->location(),
        rootLocation + MCVector3dF(MCTrigonom::rotatedVector(MCVector2dF(1.0, 1.0), rootAngle), 1.0f));

    vector3dCompare(child2->location(),
        rootLocation + MCVector3dF(MCTrigonom::rotatedVector(MCVector2dF(2.0, 2.0), rootAngle), 2.0f));

    // Rotate child relatively

    child1Angle = 47;
    child1->rotateRelative(child1Angle);
    root.translate(rootLocation);

    vector3dCompare(child1->location(),
        rootLocation + MCVector3dF(MCTrigonom::rotatedVector(MCVector2dF(1.0, 1.0), rootAngle), 1.0f));

    QVERIFY(child1->angle() == rootAngle + child1Angle);
}

void MCObjectTest::testChildTranslate()
{
    MCWorld world;
    world.setDimensions(0, 1024, 0, 768, 0, 100, 1);

    MCObject root("root");
    MCObjectPtr child1(new MCObject("child1"));
    MCObjectPtr child2(new MCObject("child2"));

    root.addChildObject(child1, MCVector3dF(1, 1, 1));
    root.addChildObject(child2, MCVector3dF(2, 2, 2));

    root.addToWorld();

    // Root at (0, 0, 0)

    vector3dCompare(root.location(), MCVector3dF(0, 0, 0));
    vector3dCompare(child1->location(), MCVector3dF(0, 0, 0));
    vector3dCompare(child2->location(), MCVector3dF(0, 0, 0));

    // Now move the root

    root.translate(MCVector3dF(1, 2, 3)); // Translate children only when root translates

    vector3dCompare(root.location(), MCVector3dF(1, 2, 3));
    vector3dCompare(child1->location(), MCVector3dF(2, 3, 4));
    vector3dCompare(child2->location(), MCVector3dF(3, 4, 5));
}

void MCObjectTest::testCollisionLayer()
{
    MCWorld world;
    world.setDimensions(0, 1024, 0, 768, 0, 100, 1);

    MCObject root("root");
    MCObjectPtr child1(new MCObject("child1"));
    MCObjectPtr child2(new MCObject("child2"));

    root.addChildObject(child1);
    root.addChildObject(child2);

    root.addToWorld();

    QVERIFY(root.collisionLayer() == 0);
    QVERIFY(child1->collisionLayer() == 0);
    QVERIFY(child2->collisionLayer() == 0);

    root.setCollisionLayer(1);

    QVERIFY(root.collisionLayer() == 1);
    QVERIFY(child1->collisionLayer() == 0);
    QVERIFY(child2->collisionLayer() == 0);

    child1->setCollisionLayer(2);

    QVERIFY(root.collisionLayer() == 1);
    QVERIFY(child1->collisionLayer() == 2);
    QVERIFY(child2->collisionLayer() == 0);
}

void MCObjectTest::testInitialAngle()
{
    MCObject object("TestObject");
    QVERIFY(object.initialAngle() == 0);
    object.setInitialAngle(1);
    QVERIFY(object.initialAngle() == 1);
}

void MCObjectTest::testInitialLocation()
{
    MCObject object("TestObject");
    vector3dCompare(object.initialLocation(), MCVector3dF(0, 0, 0));
    const MCVector3dF initialLocation(1, 2, 3);
    object.setInitialLocation(initialLocation);
    vector3dCompare(object.initialLocation(), initialLocation);
}

void MCObjectTest::testMass()
{
    MCObject object("TestObject");
    QVERIFY(qFuzzyCompare(object.mass(), MCFloat(0.0)));

    object.setMass(MCFloat(1000.0));
    QVERIFY(qFuzzyCompare(object.mass(), MCFloat(1000.0)));
    QVERIFY(qFuzzyCompare(object.invMass(), MCFloat(1.0 / 1000.0)));

    object.setMass(1, true);
    QVERIFY(object.stationary());
    QVERIFY(object.sleeping());
    QVERIFY(qFuzzyCompare(object.invMass(), MCFloat(0.0)));
}

void MCObjectTest::testRenderLayer()
{
    MCWorld world;
    world.setDimensions(0, 1024, 0, 768, 0, 100, 1);

    MCObject root("root");
    MCObjectPtr child1(new MCObject("child1"));
    MCObjectPtr child2(new MCObject("child2"));

    root.addChildObject(child1);
    root.addChildObject(child2);

    root.addToWorld();

    QVERIFY(root.renderLayer() == 0);
    QVERIFY(child1->renderLayer() == 0);
    QVERIFY(child2->renderLayer() == 0);

    root.setRenderLayer(1);

    QVERIFY(root.renderLayer() == 1);
    QVERIFY(child1->renderLayer() == 0);
    QVERIFY(child2->renderLayer() == 0);

    child1->setRenderLayer(2);

    QVERIFY(root.renderLayer() == 1);
    QVERIFY(child1->renderLayer() == 2);
    QVERIFY(child2->renderLayer() == 0);
}

void MCObjectTest::testRotate()
{
    MCObject object("TestObject");
    QVERIFY(qFuzzyCompare(object.angle(), MCFloat(0.0)));
    object.rotate(90);
    QVERIFY(qFuzzyCompare(object.angle(), MCFloat(90)));

    MCWorld world;
    MCShapePtr shape(new MCRectShape(nullptr, 10, 10));
    object.setShape(shape);
    object.rotate(45);
    QVERIFY(qFuzzyCompare(object.angle(), MCFloat(45)));
    QVERIFY(qFuzzyCompare(shape->angle(), MCFloat(45)));

    object.addToWorld();
    object.rotate(22);
    QVERIFY(qFuzzyCompare(object.angle(), MCFloat(22)));
    QVERIFY(qFuzzyCompare(shape->angle(), MCFloat(22)));
}

void MCObjectTest::testTimerEvent()
{
    TestObject testObject1, testObject2;
    QVERIFY(!testObject1.m_timerEventReceived);
    QVERIFY(!testObject2.m_timerEventReceived);

    testObject1.m_timerEventReceived = false;
    testObject2.m_timerEventReceived = false;
    MCTimerEvent timerEvent(100);
    MCObject::sendTimerEvent(timerEvent);
    QVERIFY(!testObject1.m_timerEventReceived);
    QVERIFY(!testObject2.m_timerEventReceived);

    testObject1.m_timerEventReceived = false;
    testObject2.m_timerEventReceived = false;
    MCObject::subscribeTimerEvent(testObject1);
    MCObject::subscribeTimerEvent(testObject2);
    MCObject::sendTimerEvent(timerEvent);
    QVERIFY(testObject1.m_timerEventReceived);
    QVERIFY(testObject2.m_timerEventReceived);

    testObject1.m_timerEventReceived = false;
    testObject2.m_timerEventReceived = false;
    MCObject::unsubscribeTimerEvent(testObject1);
    MCObject::unsubscribeTimerEvent(testObject2);
    MCObject::sendTimerEvent(timerEvent);
    QVERIFY(!testObject1.m_timerEventReceived);
    QVERIFY(!testObject2.m_timerEventReceived);
}

void MCObjectTest::testTranslate()
{
    MCWorld world;
    MCObject object("TestObject");
    object.addToWorld();

    vector3dCompare(object.location(), MCVector3dF(0, 0, 0));

    object.translate(MCVector3dF(1, 2, 3));

    vector3dCompare(object.location(), MCVector3dF(1, 2, 3));
}

void MCObjectTest::testVelocityAndSleep()
{
    MCWorld world;
    MCObject object("TestObject");
    object.addToWorld();

    vector3dCompare(object.velocity(), MCVector3dF(0, 0, 0));

    world.stepTime(1);
    QVERIFY(object.sleeping() == true);
    QVERIFY(object.index() == -1); // Removed from integration

    object.setVelocity(MCVector3dF(1, 1, 1));

    QVERIFY(object.sleeping() == false);
    QVERIFY(object.index() >= 0);
    vector3dCompare(object.velocity(), MCVector3dF(1, 1, 1));

    object.setVelocity(MCVector3dF(0, 0, 0));

    vector3dCompare(object.velocity(), MCVector3dF(0, 0, 0));

    world.stepTime(1);
    QVERIFY(object.sleeping() == true);
    QVERIFY(object.index() == -1);
}

void MCObjectTest::testVelocityIntegration()
{
    MCWorld world;
    world.setDimensions(0, 1024, 0, 768, 0, 100, 1);
    MCObject object("TestObject");
    object.addToWorld();

    vector3dCompare(object.location(), MCVector3dF(0, 0, 0));
    vector3dCompare(object.velocity(), MCVector3dF(0, 0, 0));

    MCVector3dF velocity(1, 2, 3);
    object.setVelocity(velocity);

    world.stepTime(1);

    vector3dCompare(object.location(), velocity * DAMPING);
    const MCVector3dF location = object.location();

    world.stepTime(1);
    velocity *= DAMPING;

    vector3dCompare(object.location(), location + velocity * DAMPING);
}

QTEST_MAIN(MCObjectTest)
