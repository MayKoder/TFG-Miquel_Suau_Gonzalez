#include "MO_Sound.h"

#include <stdlib.h>
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

M_Sound::M_Sound(Application* app, bool start_enabled) : Module(app, start_enabled), buffer(0), source(0), offset(0.0f), state(0)
{

}

M_Sound::~M_Sound()
{
}


#pragma region Audio test

//	/* Play the sound until it finishes. */
//	alSourcePlay(source);
//	do {
//		al_nssleep(10000000);
//		alGetSourcei(source, AL_SOURCE_STATE, &state);

//		/* Get the source offset. */
//		alGetSourcef(source, AL_SEC_OFFSET, &offset);
//		printf("\rOffset: %f  ", offset);
//		fflush(stdout);
//	} while (alGetError() == AL_NO_ERROR && state == AL_PLAYING);
//	printf("\n");



#pragma endregion
ALuint M_Sound::LoadSound(const char* filename)
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


bool M_Sound::Init()
{
	/* Initialize OpenAL. */
	if (InitAL() != 0)
		return false;

	/* Load the sound into a buffer. */
	//buffer = LoadSound("test.wav");
	//if (!buffer)
	//{
	//	CloseAL();
	//	return false;
	//}

	///* Create the source to play the sound with. */
	//alGenSources(1, &source);
	//alSourcei(source, AL_BUFFER, (ALint)buffer);
	//assert(alGetError() == AL_NO_ERROR && "Failed to setup sound source");

	//alSourcePlay(source);

	return true;
}

bool M_Sound::CleanUp()
{
	/* All done. Delete resources, and close down OpenAL. */
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);

	CloseAL();
	return true;
}
