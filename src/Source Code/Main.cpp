#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include<ShObjIdl_core.h>


#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

#ifdef _DEBUG
	//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};



#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>

#include "sndfile/include/sndfile.h"

#include "OpenAL/include/AL/al.h"
#include "OpenAL/include/AL/alc.h"
#include "OpenAL/include/AL/alext.h"
#include "alhelpers.h"

#pragma comment (lib, "sndfile/lib/sndfile.lib")
#pragma comment (lib, "OpenAL/libs/Win32/OpenAL32.lib")
static ALuint LoadSound(const char* filename)
{
	ALenum err, format;
	ALuint buffer;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	/* Open the audio file and check that it's usable. */
	sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!sndfile)
	{
		fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(sndfile));
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
	{
		fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename, sfinfo.frames);
		sf_close(sndfile);
		return 0;
	}

	/* Get the sound format, and figure out the OpenAL format */
	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (sfinfo.channels == 4)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format)
	{
		fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
		sf_close(sndfile);
		return 0;
	}

	/* Decode the whole audio file to a buffer. */
	membuf = (short*)malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1)
	{
		free(membuf);
		sf_close(sndfile);
		fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename, num_frames);
		return 0;
	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	/* Buffer the audio data into a new buffer object, then free the data and
	 * close the file.
	 */
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndfile);

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	return buffer;
}

int main(int argc, char** argv)
{
	LOG(LogType::L_NORMAL, "Starting game '%s'...", TITLE);

#pragma region Audio test
	{

		ALuint source, buffer;
		ALfloat offset;
		ALenum state;

		/* Initialize OpenAL. */
		argv++; argc--;
		if (InitAL(&argv, &argc) != 0)
			return 1;

		/* Load the sound into a buffer. */
		buffer = LoadSound("test.wav");
		if (!buffer)
		{
			CloseAL();
			return 1;
		}

		/* Create the source to play the sound with. */
		source = 0;
		alGenSources(1, &source);
		alSourcei(source, AL_BUFFER, (ALint)buffer);
		assert(alGetError() == AL_NO_ERROR && "Failed to setup sound source");

		/* Play the sound until it finishes. */
		alSourcePlay(source);
		do {
			al_nssleep(10000000);
			alGetSourcei(source, AL_SOURCE_STATE, &state);

			/* Get the source offset. */
			alGetSourcef(source, AL_SEC_OFFSET, &offset);
			printf("\rOffset: %f  ", offset);
			fflush(stdout);
		} while (alGetError() == AL_NO_ERROR && state == AL_PLAYING);
		printf("\n");

		/* All done. Delete resources, and close down OpenAL. */
		alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);

		CloseAL();
	}
#pragma endregion



//This works BUT i dont like to hardcode the cmd.exe path, i think this will cause some issues with different OS or system paths
#pragma region CreateProcess
	//STARTUPINFO si;
	//SecureZeroMemory(&si, sizeof(STARTUPINFO));

	//si.cb = sizeof(STARTUPINFO);


	//PROCESS_INFORMATION pi;

	//BOOL result = CreateProcess(
	//	"c:\\windows\\system32\\cmd.exe",
	//	"/c dotnet build CSTest/ConsoleApp1.sln --configuration Debug",
	//	NULL,
	//	NULL,
	//	FALSE,
	//	0,
	//	NULL,
	//	NULL,
	//	&si,
	//	&pi);

	//if (result)
	//{
	//	WaitForSingleObject(pi.hProcess, INFINITE);
	//	printf("Process with ID: %i has exited.\n", GetProcessId(pi.hProcess));
	//	CloseHandle(pi.hProcess);
	//}
#pragma endregion

	
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	Application* App = NULL;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			LOG(LogType::L_NORMAL, "-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG(LogType::L_NORMAL, "-------------- Application Init --------------");
			if (App->Init() == false)
			{
				LOG(LogType::L_ERROR, "Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG(LogType::L_NORMAL, "-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG(LogType::L_ERROR, "Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP || App->GetExitState())
				state = MAIN_FINISH;
		}
		break;

		case MAIN_FINISH:

			LOG(LogType::L_NORMAL, "-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG(LogType::L_ERROR, "Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	EngineExternal = nullptr;
	RELEASE(App);

#ifdef _DEBUG
	int leaks = MAX(0, m_getMemoryStatistics().totalAllocUnitCount - 23);
	LOG(LogType::L_NORMAL, "With %d memory leaks!\n", (leaks > 0) ? leaks : 0);
#endif

	LOG(LogType::L_NORMAL, "Exiting game '%s'...\n", TITLE);
	return main_return;
}