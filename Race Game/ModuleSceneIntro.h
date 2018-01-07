#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "p2Point.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void CreateLinearCircuit(vec3 );

public:
	//-----VEHICLE-------
	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel; 

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;
	//-----CIRCUIT-------
	p2DynArray<Cube> CircuitPolygon;
	p2DynArray<Cylinder> CircuitCube;
	p2List<PhysBody3D*> Constraints;

	//------Sensor 2 ------
	Cube sensor_form;
	Cube floor_form;
	Cube ramp_form;
	Cube landing_form;

	PhysBody3D* sensor_start;
	PhysBody3D* sensor_p1;
	PhysBody3D* sensor_p2;
	PhysBody3D* sensor_p3;
	PhysBody3D* sensor_p4;
	PhysBody3D* sensor_p5;
	PhysBody3D* sensor_p6;
	PhysBody3D* sensor_end;
	PhysBody3D* sensor_floor;

	bool start,end;
	bool on_tunnel;
	bool fallen;
	bool landed;
};
