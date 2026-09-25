#ifndef _FMOD_MEMORY_H
#define _FMOD_MEMORY_H

#include "fmod_settings.h"

#ifdef __cplusplus

#ifndef PLATFORM_PS3
extern "C++" /* in case it included inside an extern "C" */
{
#endif

#include "fmod.h"

#include <new>
#include "fmod_os_misc.h"
#include "fmod_debug.h"
#include "fmod_globals.h"

#define FMOD_MEMORY_THREADSAFE
#define FMOD_MEMORY_USEBITS

#ifdef FMOD_SUPPORT_DLMALLOC
    #include "../lib/dlmalloc/dlmalloc.h"
    #define FMOD_MEMORY_DEFAULTBLOCKSIZE  256
#else
    #if defined(FMOD_MEMORY_USEBITS)
        #ifdef PLATFORM_PS3
            #define FMOD_MEMORY_DEFAULTBLOCKSIZE 32     /* 16 is incredibly slow on the PS3 for some reason */
        #else
            #define FMOD_MEMORY_DEFAULTBLOCKSIZE 16
        #endif
    #else
        #define FMOD_MEMORY_DEFAULTBLOCKSIZE  256
    #endif
#endif

#define FMOD_MEMORY_MAXTHREADS 32

namespace FMOD
{
    typedef struct
    {
        int             mSize;
        int             mNumBlocks;
        int             mBlockOffset;
        unsigned int    mThreadID;
    } MemBlockHeader;

    /*
        MemoryPool class.  Does all the memory management work.
    */
    class MemPool
    {
        #if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_MEMORY)
        friend class ProfileMemory;
        #endif

      private:
        unsigned char               *mBitmap;
        unsigned char               *mData;
                                    
        bool                         mCustomPool;
        int                          mSizeBytes;
        int                          mSizeBlocks;
        int                          mNumBlocks;
        int                          mMaxBlocks;
        unsigned int                 mCurrentAllocated[FMOD_MEMORY_MAXTHREADS + 1];
        unsigned int                 mCurrentAllocatedThreadID[FMOD_MEMORY_MAXTHREADS]; /* id 0 means all threads. */
        unsigned int                 mMaxAllocated;
        unsigned int                 mMaxAllocatedSecondary;
        unsigned int                 mCurrentAllocatedSecondary;
        int                          mActualMaxBytes;
        int                          mWastage;
        int                          mFirstFreeBlock;
        FMOD_MEMORY_ALLOCCALLBACK    mAlloc;
        FMOD_MEMORY_REALLOCCALLBACK  mRealloc;
        FMOD_MEMORY_FREECALLBACK     mFree;

#ifdef FMOD_SUPPORT_DLMALLOC
        mspace                       mDLMallocSpace;
#endif

#ifdef FMOD_MEMORY_THREADSAFE
        FMOD_OS_CRITICALSECTION      *mCrit;
#endif
#ifdef FMOD_DEBUG
        int                          mAllocCount;
#endif
        inline int               findFreeBlocks(int offset, int to, int numblocks);

      public:

        void                     set(int blockoffset, int value, int numblocks);
        int                      mBlockSize;

        MemPool();
        ~MemPool();
     
        FMOD_RESULT     init      (void *poolmem, int poolsize, int blocksize);
        FMOD_RESULT     initCustom(void *poolmem, int poolsize, int blocksize);
        void            close();

        void           *alloc  (int len,             const char *file = "", const int line = 0, FMOD_MEMORY_TYPE type = FMOD_MEMORY_NORMAL, bool clear = false);
        void           *calloc (int len,             const char *file = "", const int line = 0, FMOD_MEMORY_TYPE type = FMOD_MEMORY_NORMAL);
        void           *realloc(void *ptr, int len,  const char *file = "", const int line = 0, FMOD_MEMORY_TYPE type = FMOD_MEMORY_NORMAL);
        void            free   (void *ptr,           const char *file = "", const int line = 0, FMOD_MEMORY_TYPE type = FMOD_MEMORY_NORMAL);
        int             getSize(void *ptr,           const char *file = "", const int line = 0);

        // OFFSET: 0x008D0520
        inline FMOD_RESULT setCallbacks(FMOD_MEMORY_ALLOCCALLBACK useralloc, FMOD_MEMORY_REALLOCCALLBACK userrealloc, FMOD_MEMORY_FREECALLBACK userfree)
        {
            mAlloc   = useralloc;
            mRealloc = userrealloc;
            mFree    = userfree;

            return FMOD_OK;
        }

        inline unsigned int getCurrentAllocated(unsigned int threadid = 0)   
        { 
            return mCurrentAllocated[threadid];
        }
        inline unsigned int getCurrentAllocatedSecondary()
        {
            return mCurrentAllocatedSecondary;
        }
        // OFFSET: 0x008CF9E0
        inline unsigned int getCurrentThreadID(FMOD_UINT_NATIVE threadid = 0)
        {
            unsigned int count;
            FMOD_UINT_NATIVE id = threadid;

            if (!id)
            {
                FMOD_OS_Thread_GetCurrentID(&id);
            }

            for (count = 1; count < FMOD_MEMORY_MAXTHREADS; count++)
            {
                if (id == mCurrentAllocatedThreadID[count])
                {
                    break;
                }
                else if (!mCurrentAllocatedThreadID[count])
                {
                    mCurrentAllocatedThreadID[count] = id;
                    mCurrentAllocated[count] = 0;
                    break;
                }
            }

            return count;
        }
        // OFFSET: 0x008E5150
        inline void clearThreadID(FMOD_UINT_NATIVE id)
        {
            int count;

            for (count = 1; count < FMOD_MEMORY_MAXTHREADS; count++)
            {
                if (id == mCurrentAllocatedThreadID[count])
                {
                    mCurrentAllocated[count] = 0;
                    mCurrentAllocatedThreadID[count] = 0;
                    break;
                }
            }
        }

        inline unsigned int     getMaxAllocated()          { return mMaxAllocated; }
        inline unsigned int     getMaxAllocatedSecondary() { return mMaxAllocatedSecondary; }
        inline unsigned char   *getData()                  { return mData; }
        inline int              getSizeBytes()             { return mSizeBytes; }       
    };

    /*
        Base class for Memory overriding to access the global memory pool.
    */
    class MemSingleton
    {
        void *mBuffer;
        int   mRefCount;
        
      public:
        
        MemSingleton() : mBuffer(0), mRefCount(0) {}
        ~MemSingleton() { free(); }

        void *alloc(int len, const char *file = "", const int line = 0);
        void  free(const char *file = "", const int line = 0);
        void *getData() { return mBuffer; }
    };

    void * F_CALLBACK Memory_DefaultMalloc(unsigned int size, FMOD_MEMORY_TYPE type);
    void * F_CALLBACK Memory_DefaultRealloc(void *data, unsigned int size, FMOD_MEMORY_TYPE type);
    void   F_CALLBACK Memory_DefaultFree(void *ptr, FMOD_MEMORY_TYPE type);

    /*
        Standard alloc functions.
    */
#ifndef PLATFORM_PS3_SPU

    #define FMOD_Memory_Alloc(_len)             FMOD::gGlobal->gSystemPool->alloc((_len), __FILE__, __LINE__, false)
    #define FMOD_Memory_Calloc(_len)            FMOD::gGlobal->gSystemPool->calloc((_len), __FILE__, __LINE__)
    #define FMOD_Memory_ReAlloc(_ptr, _len)     FMOD::gGlobal->gSystemPool->realloc((_ptr), _len, __FILE__, __LINE__)
    #define FMOD_Memory_Free(_ptr)              FMOD::gGlobal->gSystemPool->free((_ptr), __FILE__, __LINE__)

    #define FMOD_Memory_AllocType(_len, _type)         FMOD::gGlobal->gSystemPool->alloc((_len), __FILE__, __LINE__, _type)
    #define FMOD_Memory_CallocType(_len, _type)        FMOD::gGlobal->gSystemPool->calloc((_len), __FILE__, __LINE__, _type)
    #define FMOD_Memory_ReAllocType(_ptr, _len, _type) FMOD::gGlobal->gSystemPool->realloc((_ptr), _len, __FILE__, __LINE__, _type)
    #define FMOD_Memory_FreeType(_ptr, _type)          FMOD::gGlobal->gSystemPool->free((_ptr), __FILE__, __LINE__, _type)
    
    #define FMOD_Object_Alloc(_type)            new (FMOD::gGlobal->gSystemPool->alloc(sizeof(_type), __FILE__, __LINE__)) (_type)
    #define FMOD_Object_Calloc(_type)           new (FMOD::gGlobal->gSystemPool->calloc(sizeof(_type), __FILE__, __LINE__)) (_type)
    #define FMOD_Object_AllocSize(_type, _len)  new (FMOD::gGlobal->gSystemPool->alloc((_len) < sizeof(_type) ? sizeof(_type) : (_len), __FILE__, __LINE__)) (_type)
    #define FMOD_Object_CallocSize(_type, _len) new (FMOD::gGlobal->gSystemPool->calloc((_len) < sizeof(_type) ? sizeof(_type) : (_len), __FILE__, __LINE__)) (_type)
//    #define FMOD_Object_AllocArray(_type, _num) new (FMOD::gGlobal->gSystemPool->calloc(sizeof(_type) * (_num), __FILE__, __LINE__)) (_type[_num]);

#ifndef PLATFORM_PS3
} /* extern "C++" */
#endif

#else

    #define FMOD_Memory_Alloc(_len)             (0)
    #define FMOD_Memory_Calloc(_len)            (0)
    #define FMOD_Memory_ReAlloc(_ptr, _len)     (0)
    #define FMOD_Memory_Free(_ptr)              (0)
    #define FMOD_Memory_GetSize(_ptr)           (0)
    #define FMOD_Memory_AddCounter(_ptr)        (0)
    #define FMOD_Memory_RemoveCounter(_ptr)     (0)
    #define FMOD_Memory_PauseCounter(_ptr, _p)  (0)

    #define FMOD_Object_Alloc(_type)            (0)
    #define FMOD_Object_Calloc(_type)           (0)
    #define FMOD_Object_AllocSize(_type, _len)  (0)
    #define FMOD_Object_CallocSize(_type, _len) (0)
//    #define FMOD_Object_AllocArray(_type, _num)

#endif


}

#else   /* __cplusplus */

    void *FMOD_Memory_allocC  (int len,             const char *file, const int line);
    void *FMOD_Memory_callocC (int len,             const char *file, const int line);
    void *FMOD_Memory_reallocC(void *ptr, int len,  const char *file, const int line);
    void  FMOD_Memory_freeC   (void *ptr,           const char *file, const int line);

    #define FMOD_Memory_Alloc(_len)             FMOD_Memory_allocC((_len), __FILE__, __LINE__)
    #define FMOD_Memory_Calloc(_len)            FMOD_Memory_callocC((_len), __FILE__, __LINE__)
    #define FMOD_Memory_ReAlloc(_ptr, _len)     FMOD_Memory_reallocC((_ptr), _len, __FILE__, __LINE__)
    #define FMOD_Memory_Free(_ptr)              FMOD_Memory_freeC((_ptr), __FILE__, __LINE__)

#endif  /* __cplusplus */
    

#endif
