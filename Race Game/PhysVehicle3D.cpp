#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	/*if(wheels != NULL)
		delete wheels;*/
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{

	Cylinder wheel_front(2.0, 0.3);
	wheel_front.color = Black;

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
			vehicle->updateWheelTransform(i);
			vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel_front.transform);

			wheel_front.Render();
		
	}
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();


	chassis.Render();

	Cube stick(info.stick_size.x, info.stick_size.y, info.stick_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&stick.transform);
	btVector3 stickoffset(info.stick_offset.x, info.stick_offset.y, info.stick_offset.z);
	stickoffset = offset.rotate(q.getAxis(), q.getAngle());

	stick.transform.M[12] += stickoffset.getX();
	stick.transform.M[13] += stickoffset.getY();
	stick.transform.M[14] += stickoffset.getZ();


	stick.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}
// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}
