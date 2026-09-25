#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_NONBLOCKING

#include "fmod_async.h"
#include "fmod_dspi.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"

namespace FMOD
{

LinkedListNode AsyncThread::gAsyncHead;


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// OFFSET: 0x00921FC0
void asyncThreadFunc(void *data)
{
    AsyncThread *asyncThread = (AsyncThread *)data;
    asyncThread->threadFunc();
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
// OFFSET: 0x00921FD0
AsyncThread::AsyncThread()
{
    mCrit         = 0;
    mThreadActive = false;
    mBusy         = false;
    mDone         = false;
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
// OFFSET: 0x00922040
FMOD_RESULT AsyncThread::shutDown()
{
    if (FMOD::gGlobal->gAsyncCrit)
    {
        AsyncThread *asyncthread;
        LinkedListNode *current, *next;

        FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);
        {
            current = gAsyncHead.getNext();
            while (current != &gAsyncHead)
            {
                next = current->getNext();

                asyncthread = SAFE_CAST(AsyncThread, current);
                asyncthread->reallyRelease();

                current = next;
            }
        }
        FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);
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
FMOD_RESULT AsyncThread::addCallback(FMOD_ASYNC_CALLBACK callback, AsyncThread **asyncthread)
{
    FMOD_RESULT result;

    if (asyncthread)
    {
        *asyncthread = 0;
    }

    LinkedListNode *node = (LinkedListNode *)FMOD_Object_Alloc(LinkedListNode);
    if (!node)
    {
        return FMOD_ERR_MEMORY;
    }

    node->setData((void *)callback);

    /*
        Make sure there's a thread for us
    */
    result = getAsyncThread(0);
    if (result != FMOD_OK)
    {
        return result;
    }

    LinkedListNode *threadnode = gAsyncHead.getNext();
    if (threadnode != &gAsyncHead)
    {
        AsyncThread *thread = SAFE_CAST(AsyncThread, threadnode);
        FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);
        {
            node->addBefore(&thread->mCallbackHead);
        }
        FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);

        if (asyncthread)
        {
            *asyncthread = thread;
        }
    }
    else
    {
        return FMOD_ERR_INTERNAL;
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
FMOD_RESULT AsyncThread::removeCallback(FMOD_ASYNC_CALLBACK callback)
{
    LinkedListNode *threadnode = gAsyncHead.getNext();

    if (threadnode != &gAsyncHead)
    {
        AsyncThread *asyncthread = SAFE_CAST(AsyncThread, threadnode);

        FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);

        LinkedListNode *callbacknode = asyncthread->mCallbackHead.getNext();
        for (; callbacknode != &asyncthread->mCallbackHead; callbacknode = callbacknode->getNext())
        {
            if (callbacknode->getData() == (void *)callback)
            {
                callbacknode->removeNode();
                FMOD_Memory_Free(callbacknode);
                break;
            }
        }

        FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);
    }
    else
    {
        return FMOD_ERR_INTERNAL;
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
// OFFSET: 0x009220A0
FMOD_RESULT AsyncThread::init(bool owned, SystemI *system)
{
    FMOD_RESULT result;

    mOwned = owned;

    result = FMOD_OS_CriticalSection_Create(&mCrit);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mThread.initThread("FMOD thread for FMOD_NONBLOCKING", asyncThreadFunc, this, ASYNC_THREADPRIORITY, 0, ASYNC_STACKSIZE, true, 0, system);
    if (result != FMOD_OK)
    {
        return result;
    }

    mThreadActive = true;

    FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);
    addBefore(&gAsyncHead);
    FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);

    FLOG((FMOD_DEBUG_USER_ANDREW, __FILE__, __LINE__, "AsyncThread::init", "created thread for %p %s\n", this, mOwned ? "(owned)" : ""));

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
FMOD_RESULT AsyncThread::release()
{
    if (mOwned)
    {
        if (mHead.getNext() != &mHead)
        {
            FLOG((FMOD_DEBUG_USER_ANDREW, __FILE__, __LINE__, "AsyncThread::release", "%p queue not empty\n", this));
        }

        mDone = true;
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
// OFFSET: 0x00921C60
FMOD_RESULT AsyncThread::reallyRelease()
{
    FMOD_OS_CriticalSection_Enter(mCrit);
    {
        if (mHead.getNext() != &mHead)
        {
            FLOG((FMOD_DEBUG_USER_ANDREW, __FILE__, __LINE__, "AsyncThread::reallyRelease", "%p queue not empty\n", this));
        }
        if (mBusy)
        {
            FLOG((FMOD_DEBUG_USER_ANDREW, __FILE__, __LINE__, "AsyncThread::reallyRelease", "%p still busy\n", this));
        }

        LinkedListNode *next, *current = mCallbackHead.getNext();
        while (current != &mCallbackHead)
        {
            next = current->getNext();
            current->removeNode();
            FMOD_Memory_Free(current);
            current = next;
        }
    }
    FMOD_OS_CriticalSection_Leave(mCrit);

    // FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);
    removeNode();
    // FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);

    mThreadActive = false;
    mThread.closeThread();

    if (mCrit)
    {
        FMOD_OS_CriticalSection_Free(mCrit);
    }

    FLOG((FMOD_DEBUG_USER_ANDREW, __FILE__, __LINE__, "AsyncThread::reallyRelease", "released thread for %p %s\n", this, mOwned ? "(owned)" : ""));

    FMOD_Memory_Free(this);

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
FMOD_RESULT AsyncThread::wakeupThread()
{
    return mThread.wakeupThread();
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
// OFFSET: 0x00921D30
FMOD_RESULT AsyncThread::update()
{
    if (FMOD::gGlobal->gAsyncCrit)
    {
        AsyncThread *asyncthread;
        LinkedListNode *current, *next;

        FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);
        {
            current = gAsyncHead.getNext();
            while (current != &gAsyncHead)
            {
                next = current->getNext();

                asyncthread = SAFE_CAST(AsyncThread, current);
                if (asyncthread->mDone)
                {
                    asyncthread->reallyRelease();
                }

                current = next;
            }
        }
        FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);
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
// OFFSET: 0x00921D80
FMOD_RESULT AsyncThread::threadFunc()
{
    FMOD_RESULT     result = FMOD_OK;
    SoundI         *sound = 0;
    SystemI        *system;
    LinkedListNode *current;

    if (mThreadActive)
    {
        FMOD_OS_CriticalSection_Enter(mCrit);
        {
            current = mHead.getNext();
            if (current != &mHead)
            {
                sound  = (SoundI *)current->getData();
                current->removeNode();
                mBusy = true;
            }
            else
            {
                current = 0;
            }
        }
        FMOD_OS_CriticalSection_Leave(mCrit);

        if (sound)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "AsyncThread::threadFunc", "Starting Asynchronous operation on sound %p\n", sound));
            FLOG_INDENT(4);

            system = sound->mSystem;

            if (sound->mOpenState == FMOD_OPENSTATE_LOADING)
            {
                if (sound->mMode & (FMOD_OPENMEMORY | FMOD_OPENMEMORY_POINT))
                {
                    result = system->createSoundInternal((const char *)sound->mAsyncData->mNameData, sound->mMode, sound->mAsyncData->mBufferSize, sound->mAsyncData->mBufferSizeType, sound->mAsyncData->mExInfoExists ? &sound->mAsyncData->mExInfo : 0, true, &sound);
                }
                else
                {
                    result = system->createSoundInternal(sound->mAsyncData->mName, sound->mMode, sound->mAsyncData->mBufferSize, sound->mAsyncData->mBufferSizeType, sound->mAsyncData->mExInfoExists ? &sound->mAsyncData->mExInfo : 0, true, &sound);
                }
            }
#ifdef FMOD_SUPPORT_STREAMING
#ifdef FMOD_SUPPORT_NONBLOCKSETPOS
            else if (sound->mOpenState == FMOD_OPENSTATE_SETPOSITION)
            {
                Stream *stream = SAFE_CAST(Stream, sound);

                /*
                    Block here and wait for stream thread to stop doing stuff.
                */
                while (!(stream->mFlags & (FMOD_SOUND_FLAG_SETPOS_SAFE | FMOD_SOUND_FLAG_THREADFINISHED)))
                {
                    FMOD_OS_Time_Sleep(10);
                }

                if (!(stream->mFlags & FMOD_SOUND_FLAG_THREADFINISHED))
                {
                    result = stream->mChannel->setPositionEx(sound->mAsyncData->mPosition, sound->mAsyncData->mPositionType, true);
                    if (result == FMOD_OK)
                    {
                        stream->mChannel->mFlags &= ~CHANNELREAL_FLAG_PAUSEDFORSETPOS;

                	    FMOD_OS_CriticalSection_Enter(sound->mSystem->mStreamRealchanCrit);
                        if (stream->mChannel->mRealChannel[0])
                        {
                            stream->mChannel->setPaused(stream->mChannel->mFlags & CHANNELREAL_FLAG_PAUSED ? true : false);
                        }
                	    FMOD_OS_CriticalSection_Leave(sound->mSystem->mStreamRealchanCrit);
                    }
                    else if (result == FMOD_ERR_INVALID_HANDLE)
                    {
                        result = FMOD_OK;
                    }
                }
            }
#endif
            else if (sound->mOpenState == FMOD_OPENSTATE_SEEKING)
            {
                Stream *stream = SAFE_CAST(Stream, sound);
                
                if (stream->mSubSoundList)
                {
                    result = FMOD_OK;
                }
                else
                {
                    result = stream->updateSubSound(stream->mSubSoundIndex, false);
                }
                
                if (result == FMOD_OK)
                {
                    result = stream->setPosition(0, FMOD_TIMEUNIT_PCM);                               
                    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "AsyncThread::threadFunc", "done setposition\n"));
                    if (result == FMOD_OK)
                    {
                        result = stream->flush();
                        if (result == FMOD_OK)
                        {
                            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "AsyncThread::threadFunc", "done flush\n"));
                        }
                        else
                        {
                            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "AsyncThread::threadFunc", "stream->flush returned %d\n", result));
                        }
                    }
                    else
                    {
                        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "AsyncThread::threadFunc", "stream->setPosition returned %d\n", result));
                    }
                }
            }
#endif
            else
            {
                FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "AsyncThread::threadFunc", "AsyncThread::threadFunc: unexpected mOpenState (%d).  Result = %d\n", sound->mOpenState, result));
            }

            sound->mAsyncData->mResult = result;
            sound->mFlags             |= FMOD_SOUND_FLAG_DONOTRELEASE;                                               /* Dissalow release... */
            sound->mOpenState          = (result == FMOD_OK) ? FMOD_OPENSTATE_READY : FMOD_OPENSTATE_ERROR;     /* .. but allow everything else. */
            if (sound->mSubSoundParent)
            {
                sound->mSubSoundParent->mOpenState = sound->mOpenState;
            }
            if (sound->mSubSoundShared)
            {
                sound->mSubSoundShared->mOpenState = sound->mOpenState;
            }
            mBusy                      = false;

            if (sound->mAsyncData->mExInfoExists)
            {
                if (sound->mAsyncData->mExInfo.nonblockcallback)
                {
                    sound->mUserData = sound->mAsyncData->mExInfo.userdata;

                    sound->mAsyncData->mExInfo.nonblockcallback((FMOD_SOUND *)sound, result);
                }
            }

            sound->mFlags &= ~FMOD_SOUND_FLAG_DONOTRELEASE;

            /*
                Release this thread if it's owned
            */
            release();

            FLOG_INDENT(-4);
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "AsyncThread::threadFunc", "Finished Asynchronous operation on sound %p\n", sound));
        }

        /*
            Call any callbacks that have been added
        */
        LinkedListNode *node;

        FMOD_OS_CriticalSection_Enter(mCrit);
        {
            node = mCallbackHead.getNext();
        }
        FMOD_OS_CriticalSection_Leave(mCrit);

        while (node != &mCallbackHead)
        {
            FMOD_ASYNC_CALLBACK callback = (FMOD_ASYNC_CALLBACK)node->getData();

            result = callback();
            if (result != FMOD_OK)
            {
                return result;
            }

            FMOD_OS_CriticalSection_Enter(mCrit);
            {
                node = node->getNext();
            }
            FMOD_OS_CriticalSection_Leave(mCrit);
        }

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
// OFFSET: 0x00922130
FMOD_RESULT AsyncThread::getAsyncThread(SoundI *sound)
{
    FMOD_RESULT     result;
    LinkedListNode *current;
    bool            found, owned;
    AsyncThread    *asyncthread = 0;

    owned = false;
    found = false;

    FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);
    {
        current = gAsyncHead.getNext();
        if (current != &gAsyncHead)
        {
            asyncthread = SAFE_CAST(AsyncThread, current);

            FMOD_OS_CriticalSection_Enter(asyncthread->mCrit);
            {
                if (0) //(asyncthread->mHead.getNext() != &asyncthread->mHead) || asyncthread->mBusy || asyncthread->mDone)
                {
                    owned = true;       /* The first asyncthread is or will be busy so make a new one */
                }
                else
                {
                    found = true;       /* The first asyncthread isn't doing anything so use it */
                }
            }
            FMOD_OS_CriticalSection_Leave(asyncthread->mCrit);
        }
    }
    FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);

    if (!found)
    {
        asyncthread = FMOD_Object_Alloc(AsyncThread);
        if (!asyncthread)
        {
            return FMOD_ERR_MEMORY;
        }

        result = asyncthread->init(owned, sound ? sound->mSystem : 0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (sound)
    {
        if (sound->mAsyncData->mThread)
        {
            FLOG((FMOD_DEBUG_USER_ANDREW, __FILE__, __LINE__, "AsyncThread::getAsyncThread", "sound->mAsyncData->Thread not null == %p\n", sound->mAsyncData->mThread));
        }

        sound->mAsyncData->mThread = asyncthread;
    }

    return FMOD_OK;
}


}

#endif
