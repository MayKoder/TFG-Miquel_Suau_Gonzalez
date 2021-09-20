#pragma once


#include"Module.h"


typedef unsigned int ALuint;
typedef int ALenum;
typedef float ALfloat;

class M_Sound : public Module
{
public:

	M_Sound(Application* app, bool start_enabled = true);
	virtual ~M_Sound();

	ALuint LoadSound(const char* filename);

	bool Init() override;
	//update_status Update(float dt) override;
	bool CleanUp() override;

	

	ALuint source, buffer;
	ALfloat offset;
	ALenum state;

};