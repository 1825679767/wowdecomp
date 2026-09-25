#include "fmod_settings.h"

#include "fmod_debug.h"
#include "fmod_file_user.h"
#include "fmod_systemi.h"

#include <stdio.h>

namespace FMOD
{

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008E8230
FMOD_RESULT UserFile::reallyOpen(const char *name_or_data, unsigned int *filesize)
{
    FMOD_RESULT result = FMOD_OK;

 	if (mOpenCallback)
    {
		result = mOpenCallback(name_or_data, mFlags & FMOD_FILE_UNICODE ? 1 : 0, filesize, &mHandle, &mUserData);
    }
 	else if (mSystem->mOpenCallback)
    {
		result = mSystem->mOpenCallback(name_or_data, mFlags & FMOD_FILE_UNICODE ? 1 : 0, filesize, &mHandle, &mUserData);
    }

    if (result == FMOD_OK)
    {
        result = reallySeek(0);
        if (result == FMOD_ERR_FILE_COULDNOTSEEK)
        {
            mFlags &= ~FMOD_FILE_SEEKABLE;
            result = FMOD_OK;
        }
    }
    	
    if (!mHandle)
	{
        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "UserFile::reallyOpen", "FAILED\n"));
	}

	return result;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008E8270
FMOD_RESULT UserFile::reallyClose()
{
	if (mCloseCallback)
    {
		mCloseCallback(mHandle, mUserData);
    }
	else if (mSystem->mCloseCallback)
    {
		mSystem->mCloseCallback(mHandle, mUserData);
    }
    else
    {
        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "UserFile::reallyClose", "FAILED\n"));
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008E82A0
FMOD_RESULT UserFile::reallyRead(void *buffer, unsigned int size, unsigned int *rd)
{
    FMOD_RESULT result = FMOD_OK;

	if (mReadCallback)
    {
		result = mReadCallback(mHandle, buffer, size, rd, mUserData);
    }
	else if (mSystem->mReadCallback)
    {
		result = mSystem->mReadCallback(mHandle, buffer, size, rd, mUserData);
    }
    else
    {
        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "UserFile::reallyRead", "FAILED\n"));
    }

    return result;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x008E82E0
FMOD_RESULT UserFile::reallySeek(unsigned int pos)
{
	if (mSeekCallback)
    {
        FMOD_RESULT result;

		result = mSeekCallback(mHandle, pos, mUserData);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
	else if (mSystem->mSeekCallback)
    {
        FMOD_RESULT result;

		result = mSystem->mSeekCallback(mHandle, pos, mUserData);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else
    {
        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "UserFile::reallyRead", "FAILED\n"));
    }

    return FMOD_OK;
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
#ifdef FMOD_SUPPORT_MEMORYTRACKER
FMOD_RESULT UserFile::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_FILE, sizeof(UserFile));

    return File::getMemoryUsedImpl(tracker);
}
#endif

}

