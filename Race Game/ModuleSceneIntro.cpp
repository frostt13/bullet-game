	#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModulePhysics3D.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	sensor_form = { 15,15,1 };
	floor_form = {1500,1,2000};
	ramp_form = {2500,250,1}; 
	landing_form = {2000,1,2000};
	

	
	//LOADS
	App->camera->Move(vec3(1.0f, 1.0f, 1.0f));
	App->camera->LookAt(vec3(0, 1, 0));
	CreateLinearCircuit(vec3(10, 0, 0));

	start = end = false;
	on_tunnel = false;
	fallen = false;
	landed = false;
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{

	for (int i = 0; i < CircuitPolygon.Count(); i++)
	{
		CircuitPolygon[i].Render();
	}
	for (int i = 0; i < CircuitCube.Count(); i++)
	{
		CircuitCube[i].Render();
	}

	for (p2List_item<PhysBody3D*>* item = Constraints.getFirst(); item; item = item->next) {
		item->data->GetTransform(CircuitPolygon[0].transform.M);
		CircuitPolygon[0].Render();
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	//the start and end have to be moved to the first sensor and the last one not in the cave one
	if (body1 == sensor_p3 && body2 == App->player->vehicle)
	{
		//entrance fo the cave
		on_tunnel = true;
		LOG("HIT!");
	}
	if (body1 == sensor_p2 && body2 == App->player->vehicle)
	{
		//out of the cave
		on_tunnel = false;
		LOG("HIT!");
	}
	if (body1 == sensor_start && body2 == App->player->vehicle)
	{
		//start music/game
		start = true;
		LOG("HIT!");
	}
	if (body1 == sensor_floor && body2 == App->player->vehicle)
	{
		//LOSE SOUND AND RESTART
		fallen = true;
		LOG("HIT!");
	}
	if (body1 == sensor_p5 && body2 == App->player->vehicle)
	{
		//LOSE SOUND AND RESTARt
		LOG("HIT!");
	}
	if (body1 == sensor_p6 && body2 == App->player->vehicle)
	{
		//land and restart
		landed = true;
		end = true;
		LOG("HIT!");
	}
}
void ModuleSceneIntro::CreateLinearCircuit(vec3 position)
{
	//----------SENSORS-----------

	sensor_floor = App->physics->AddBody(floor_form, 0.0f);
	sensor_floor->SetAsSensor(true);
	sensor_floor->SetPos(0, -20, 0);	//floor pos
	sensor_floor->GetTransform(&floor_form.transform);
	sensor_floor->collision_listeners.add(this);
	sensor_floor->SetId(-1);

	sensor_start = App->physics->AddBody(sensor_form, 0.0f);
	sensor_start->SetAsSensor(true);
	sensor_start->SetPos(-170, 47, -535);//beginning
	sensor_start->GetTransform(&sensor_form.transform);
	sensor_start->collision_listeners.add(this);
	sensor_start->SetId(0);

	sensor_p1 = App->physics->AddBody(sensor_form, 0.0f);
	sensor_p1->SetAsSensor(true);
	sensor_p1->SetPos(-170, 40, -500);//slope(to check if it moves downwards)
	sensor_p1->GetTransform(&sensor_form.transform);
	sensor_p1->collision_listeners.add(this);
	sensor_p1->SetId(1);

	sensor_p3 = App->physics->AddBody(sensor_form, 0.0f);
	sensor_p3->SetAsSensor(true);
	sensor_p3->SetPos(-179, 0, 40);//in tunel
	sensor_p3->GetTransform(&sensor_form.transform);
	sensor_p3->collision_listeners.add(this);
	sensor_p3->SetId(3);

	sensor_p2 = App->physics->AddBody(sensor_form, 0.0f);
	sensor_p2->SetAsSensor(true);
	sensor_p2->SetPos(5, 0, 30);//out tunel
	sensor_p2->GetTransform(&sensor_form.transform);
	sensor_p2->collision_listeners.add(this);
	sensor_p2->SetId(2);

	sensor_p4 = App->physics->AddBody(sensor_form, 0.0f);
	sensor_p4->SetAsSensor(true);
	sensor_p4->SetPos(-225,0,-210);//slalom
	sensor_p4->GetTransform(&sensor_form.transform);
	sensor_p4->collision_listeners.add(this);
	sensor_p4->SetId(4);

	sensor_p5 = App->physics->AddBody(ramp_form, 0.0f);
	sensor_p5->SetAsSensor(true);
	sensor_p5->SetPos(5,10,-50);//before jumping
	sensor_p5->GetTransform(&ramp_form.transform);
	sensor_p5->collision_listeners.add(this);
	sensor_p5->SetId(5);

	sensor_p6 = App->physics->AddBody(landing_form, 0.0f);
	sensor_p6->SetAsSensor(true);
	sensor_p6->SetPos(0,0,-185);//landing
	sensor_p6->GetTransform(&landing_form.transform);
	sensor_p6->collision_listeners.add(this);
	sensor_p6->SetId(6);

	int i = 0;
	int j = 0;

	PhysBody3D* rot_body = nullptr;
	PhysBody3D* fix_body = nullptr;

	CircuitPolygon.PushBack(Cube(20, 2, 1));
	CircuitPolygon[i].color = Blue;
	CircuitPolygon[i].SetPos(-200, 3, -90);
	rot_body = App->physics->AddBody(CircuitPolygon[i], 1500);
	i++;

	CircuitCube.PushBack(Cylinder(0.1, 0.1));
	CircuitCube[j].SetPos(-200, 3, -90);
	CircuitCube[j].color = Blue;
	fix_body = App->physics->AddBody(CircuitCube[j], 0);
	j++;

	btHingeConstraint* constraint = App->physics->AddConstraintHinge(*rot_body, *fix_body, vec3{ 0, 0, 0 }, vec3{ 0, 0, 0 }, vec3{ 0, 1, 0 }, vec3{ 0, 1, 0 }, true);

	constraint->enableAngularMotor(true, 4, 1000);
	constraint->setLimit(1, 0);

	Constraints.add(rot_body);

	CircuitPolygon.PushBack(Cube(5, 1, 25));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-179, 6.5, 15);
	CircuitPolygon[i].SetRotation(30, vec3(1, 0, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(25, 1, 5));
	CircuitPolygon[i].color = Red;
	CircuitPolygon[i].SetPos(-185, 13, 2);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(25, 1, 5));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-207, 13, -4);
	CircuitPolygon[i].SetRotation(-30, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(25, 1, 5));
	CircuitPolygon[i].color = White;
	CircuitPolygon[i].SetPos(-212, 13, -20);
	CircuitPolygon[i].SetRotation(70, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;


	CircuitPolygon.PushBack(Cube(15, 1, 5));
	CircuitPolygon[i].color = Green;
	CircuitPolygon[i].SetPos(-202, 13, -32);
	CircuitPolygon[i].SetRotation(15, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;


	CircuitPolygon.PushBack(Cube(5, 1, 40));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-192, 6.5, -52);
	CircuitPolygon[i].SetRotation(-30, vec3(1, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(20, 1, 95));
	CircuitPolygon[i].SetPos(-200, 1, -230);
	CircuitPolygon[i].SetRotation(-45, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = Red;
	i++;

	CircuitPolygon.PushBack(Cube(20, 1, 80));
	CircuitPolygon[i].SetPos(-170, 1, -295);
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = LightBlue;
	i++;

	CircuitPolygon.PushBack(Cube(20, 1, 95));
	CircuitPolygon[i].SetPos(-200.5, 1, -95);
	CircuitPolygon[i].SetRotation(45, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = Pink;
	i++;

	CircuitPolygon.PushBack(Cube(20, 1, 80));
	CircuitPolygon[i].SetPos(-230, 1, -160);
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = Orange;
	i++;
	//-----P1---------
	CircuitPolygon.PushBack(Cube(10, 1, 60));
	CircuitPolygon[i].color = Yellow;
	CircuitPolygon[i].SetPos(-170, 47, -531);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 60));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-170, 32, -476);
	CircuitPolygon[i].SetRotation(30, vec3(1, 0, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 60));
	CircuitPolygon[i].color = Pink;
	CircuitPolygon[i].SetPos(-170, 32, -476);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 60));
	CircuitPolygon[i].color = Yellow;
	CircuitPolygon[i].SetPos(-170, 17, -421);
	CircuitPolygon[i].SetRotation(30, vec3(1, 0, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 60));
	CircuitPolygon[i].color = Green;
	CircuitPolygon[i].SetPos(-170, 17, -421);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 60));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-170, 2, -366);
	CircuitPolygon[i].SetRotation(30, vec3(1, 0, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 60));
	CircuitPolygon[i].color = Orange;
	CircuitPolygon[i].SetPos(-170, 2, -366);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;


	//it's at home 
	CircuitPolygon.PushBack(Cube(5, 1, 25));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-179, 6.5, 15);
	CircuitPolygon[i].SetRotation(30, vec3(1, 0, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(25, 1, 6));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-185, 13, 2);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(25, 1, 6));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-207, 13, -4);
	CircuitPolygon[i].SetRotation(-30, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(25, 1, 6));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-212, 13, -20);
	CircuitPolygon[i].SetRotation(70, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(15, 1, 6));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-202, 13, -32);
	CircuitPolygon[i].SetRotation(15, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;
	//-----P2---------
	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(5, 0, 30);
	CircuitPolygon[i].color = LightBlue;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;
	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(10, 0, 30);
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;
	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(0, 0, 30);
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(2, 0, 55);
	CircuitPolygon[i].SetRotation(-15, { 0,1,0 });
	CircuitPolygon[i].color = LightBlue;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(7, 0, 55);
	CircuitPolygon[i].SetRotation(-15, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-3, 0, 55);
	CircuitPolygon[i].SetRotation(-15, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(2, 5, 55);
	CircuitPolygon[i].SetRotation(-15, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-9, 0, 80);
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetRotation(-30, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 25));
	CircuitPolygon[i].SetPos(-14, 0, 80);
	CircuitPolygon[i].SetRotation(-30, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 25));
	CircuitPolygon[i].SetPos(-4, 0, 80);
	CircuitPolygon[i].SetRotation(-30, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-9, 5, 80);
	CircuitPolygon[i].SetRotation(-30, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-25, 0, 100);
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetRotation(-45, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 25));
	CircuitPolygon[i].SetPos(-31, 0, 100);
	CircuitPolygon[i].SetRotation(-45, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 25));
	CircuitPolygon[i].SetPos(-19, 0, 100);
	CircuitPolygon[i].SetRotation(-45, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-25, 5, 100);
	CircuitPolygon[i].SetRotation(-45, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-45, 0, 115);
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetRotation(-60, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 25));
	CircuitPolygon[i].SetPos(-47, 0, 110);
	CircuitPolygon[i].SetRotation(-60, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 25));
	CircuitPolygon[i].SetPos(-40, 0, 118);
	CircuitPolygon[i].SetRotation(-60, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-45, 5, 115);
	CircuitPolygon[i].SetRotation(-60, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-70, 0, 125);
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetRotation(-75, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-75, 0, 121);
	CircuitPolygon[i].SetRotation(-75, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-70, 0, 130);
	CircuitPolygon[i].SetRotation(-75, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-70, 5, 125);
	CircuitPolygon[i].SetRotation(-75, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-97, 0, 129);
	CircuitPolygon[i].SetRotation(-90, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-101, 0, 134);
	CircuitPolygon[i].SetRotation(-90, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-105, 0, 124);
	CircuitPolygon[i].SetRotation(-90, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-97, 5, 129);
	CircuitPolygon[i].SetRotation(-90, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-125, 0, 125);
	CircuitPolygon[i].SetRotation(70, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-125, 0, 130);
	CircuitPolygon[i].SetRotation(70, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-125, 0, 120);
	CircuitPolygon[i].SetRotation(70, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-125, 5, 125);
	CircuitPolygon[i].SetRotation(70, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-148, 0, 110);
	CircuitPolygon[i].SetRotation(45, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-148, 0, 103);
	CircuitPolygon[i].SetRotation(45, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-148, 0, 117);
	CircuitPolygon[i].SetRotation(45, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-148, 5, 110);
	CircuitPolygon[i].SetRotation(45, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-163, 0, 90);
	CircuitPolygon[i].SetRotation(30, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-170, 0, 89);
	CircuitPolygon[i].SetRotation(30, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-163, 0, 80);
	CircuitPolygon[i].color = Green;
	CircuitPolygon[i].SetRotation(30, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-163, 5, 90);
	CircuitPolygon[i].color = Green;
	CircuitPolygon[i].SetRotation(30, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-175, 0, 63);
	CircuitPolygon[i].SetRotation(15, { 0,1,0 });
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-178, 0, 71);
	CircuitPolygon[i].SetRotation(15, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].SetPos(-170, 0, 60);
	CircuitPolygon[i].SetRotation(15, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].SetPos(-175, 5, 63);
	CircuitPolygon[i].SetRotation(15, { 0,1,0 });
	CircuitPolygon[i].color = Green;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;

	CircuitPolygon.PushBack(Cube(10, 1, 30));
	CircuitPolygon[i].color = LightBlue;
	CircuitPolygon[i].SetPos(-179, 0, 40);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;
	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].color = Green;
	CircuitPolygon[i].SetPos(-184, 0, 40);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;
	CircuitPolygon.PushBack(Cube(1, 10, 30));
	CircuitPolygon[i].color = Green;
	CircuitPolygon[i].SetPos(-174, 0, 40);
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;



	// ---- P5 & P6
	CircuitPolygon.PushBack(Cube(10, 1, 70));
	CircuitPolygon[i].SetPos(5, 0, 5);
	CircuitPolygon[i].color = LightBlue;
	App->physics->AddBody(CircuitPolygon[i], 0);
	i++;


	CircuitPolygon.PushBack(Cube(10, 1, 20));
	CircuitPolygon[i].SetPos(5, 5, -37);
	CircuitPolygon[i].SetRotation(30, vec3(1, 0, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = LightBlue;
	i++;

	CircuitPolygon.PushBack(Cube(30, 1, 30));
	CircuitPolygon[i].SetPos(0, 0, -170);
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = LightBlue;
	i++;

	CircuitPolygon.PushBack(Cube(30, 1, 30));
	CircuitPolygon[i].SetPos(15, 0, -176);
	CircuitPolygon[i].SetRotation(45, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = LightBlue;
	i++;

	CircuitPolygon.PushBack(Cube(30, 1, 30));
	CircuitPolygon[i].SetPos(-15, 0, -176);
	CircuitPolygon[i].SetRotation(-45, vec3(0, 1, 0));
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = LightBlue;
	i++;

	CircuitPolygon.PushBack(Cube(74, 1, 30));
	CircuitPolygon[i].SetPos(0, 0, -190);
	App->physics->AddBody(CircuitPolygon[i], 0);
	CircuitPolygon[i].color = LightBlue;

	
	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-170, 1, -295);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-175, 1, -290);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-165, 1, -290);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-161, 1, -286);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-166, 1, -286);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;


	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-170, 1, -270);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-175, 1, -265);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;


	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-165, 1, -265);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-177, 1, -245);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-190, 1, -245);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-190, 1, -237);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-199, 1, -235);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-167, 1, -260);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-208, 1, -220);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;



	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-220, 1, -223);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-195, 1, -223);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;


	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-208, 1, -215);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-215, 1, -215);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-220, 1, -210);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-224, 1, -215);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-230, 1, -195);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-225, 1, -190);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-225, 1, -190);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-235, 1, -180);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-225, 1, -185);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-230, 1, -180);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-230, 1, -180);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-230, 1, -165);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-233, 1, -160);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-239, 1, -160);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;


	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-221, 1, -160);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;

	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-232, 1, -150);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;


	CircuitCube.PushBack(Cylinder(1, 5));
	CircuitCube[j].SetPos(-227, 1, -148);
	CircuitCube[j].SetRotation(90, vec3(0, 0, 1));
	App->physics->AddBody(CircuitCube[j], 0);
	CircuitCube[j].color = Red;
	j++;
}