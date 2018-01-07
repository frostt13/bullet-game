#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define NITRO_ACCELERATION 40000.0f
#define MAX_ACCELERATION 10000.0f
#define TURN_DEGREES 12.0f * DEGTORAD
#define BRAKE_POWER 1000.0f
#define MAX_FRAMES_TURBO 20

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	vec3 initial_pos;
	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;
	uint nitro;
	Timer timer;
	uint fast_lap;
	bool first_lap;
};