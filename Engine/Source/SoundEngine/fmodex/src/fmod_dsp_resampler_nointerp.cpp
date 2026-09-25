#include "fmod_settings.h"

#include "fmod_dsp_resampler_nointerp.h"

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
// OFFSET: 0x00942FE0
void FMOD_Resampler_NoInterp(float *out, int outlength, void *src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P *position, FMOD_SINT64P *speed, int channels)
{
    float scale = 1.0f;
    
    switch (srcformat)
    {
        /*
            8 BIT
        */
        case FMOD_SOUND_FORMAT_PCM8:
        {
            signed char *inptr = (signed char *)src;

            scale /= (float)(1<<7);

            if (channels == 1)
            {
                float r1,r2,r3,r4;
                int len;

                len = outlength >> 2;
                while (len)
                {
                    r1 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r2 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r3 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r4 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;
                    len--;
                    out+=4;
                } 

                len = outlength & 3;
                while (len)
                {
                    *out++ = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    len--;
                } 
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        *out++ = (float)inptr[(position->mHi * channels) + count] * scale;
                    }
                    position->mValue += speed->mValue;
                    outlength--;
                } 
            }
            break;
        }

        /*
            16 BIT
        */
        case FMOD_SOUND_FORMAT_PCM16:
        {
            signed short *inptr = (signed short *)src;

            scale /= (float)(1<<15);

            if (channels == 1)
            {
                float r1,r2,r3,r4;
                int len;

                len = outlength >> 2;
                while (len)
                {
                    r1 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r2 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r3 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r4 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;
                    len--;
                    out+=4;
                } 

                len = outlength & 3;
                while (len)
                {
                    *out++ = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    len--;
                } 
            }
            else if (channels == 2)
            {
                float r1l,r2l,r3l,r4l;
                float r1r,r2r,r3r,r4r;
                int len;

                len = outlength >> 2;
                while (len)
                {
                    r1l = (float)inptr[(position->mHi<<1) + 0] * scale;
                    r1r = (float)inptr[(position->mHi<<1) + 1] * scale;
                    position->mValue += speed->mValue;
                    r2l = (float)inptr[(position->mHi<<1) + 0] * scale;
                    r2r = (float)inptr[(position->mHi<<1) + 1] * scale;
                    position->mValue += speed->mValue;
                    r3l = (float)inptr[(position->mHi<<1) + 0] * scale;
                    r3r = (float)inptr[(position->mHi<<1) + 1] * scale;
                    position->mValue += speed->mValue;
                    r4l = (float)inptr[(position->mHi<<1) + 0] * scale;
                    r4r = (float)inptr[(position->mHi<<1) + 1] * scale;
                    position->mValue += speed->mValue;

                    out[0] = r1l;
                    out[1] = r1r;
                    out[2] = r2l;
                    out[3] = r2r;
                    out[4] = r3l;
                    out[5] = r3r;
                    out[6] = r4l;
                    out[7] = r4r;
                    len--;
                    out+=8;
                } 

                len = outlength & 3;
                while (len)
                {
                    *out++ = (float)inptr[(position->mHi<<1) + 0] * scale;
                    *out++ = (float)inptr[(position->mHi<<1) + 1] * scale;
                    position->mValue += speed->mValue;
                    len--;
                } 
            }            
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        *out++ = (float)inptr[(position->mHi * channels) + count] * scale;
                    }
                    position->mValue += speed->mValue;
                    outlength--;
                } 
            }
            break;
        }

        /*
            24 BIT
        */
        case FMOD_SOUND_FORMAT_PCM24:
        {
            FMOD_INT24 *inptr = (FMOD_INT24 *)src;

            scale /= (float)(1<<23);

            if (channels == 1)
            {
                int len;
                
                len = outlength >> 2;
                while (len)
                {
                    FMOD_INT24 *s;
                    float p0, p1, p2, p3;

                    s = &inptr[position->mHi];
                    p0 = (float)((int)(((unsigned int)s->val[0] <<  8) | ((unsigned int)s->val[1] << 16) | ((unsigned int)s->val[2] << 24)) >> 8) * scale;
                    position->mValue += speed->mValue;
                    s = &inptr[position->mHi];
                    p1 = (float)((int)(((unsigned int)s->val[0] <<  8) | ((unsigned int)s->val[1] << 16) | ((unsigned int)s->val[2] << 24)) >> 8) * scale;
                    position->mValue += speed->mValue;
                    s = &inptr[position->mHi];
                    p2 = (float)((int)(((unsigned int)s->val[0] <<  8) | ((unsigned int)s->val[1] << 16) | ((unsigned int)s->val[2] << 24)) >> 8) * scale;
                    position->mValue += speed->mValue;
                    s = &inptr[position->mHi];
                    p3 = (float)((int)(((unsigned int)s->val[0] <<  8) | ((unsigned int)s->val[1] << 16) | ((unsigned int)s->val[2] << 24)) >> 8) * scale;
                    position->mValue += speed->mValue;

                    out[0] = p0;
                    out[1] = p1;
                    out[2] = p2;
                    out[3] = p3;
                    out+=4;
                    len--;
                } 

                len = outlength & 3;
                while (len)
                {
                    FMOD_INT24 *s0 = &inptr[position->mHi];
                    float  p0 = (float)((int)(((unsigned int)s0->val[0] <<  8) | ((unsigned int)s0->val[1] << 16) | ((unsigned int)s0->val[2] << 24)) >> 8) * scale;

                    *out++ = p0;
                    position->mValue += speed->mValue;
                    len--;
                } 
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        FMOD_INT24 *s0 = &inptr[(position->mHi * channels) + count];
                        float  p0 = (float)((int)(((unsigned int)s0->val[0] <<  8) | ((unsigned int)s0->val[1] << 16) | ((unsigned int)s0->val[2] << 24)) >> 8) * scale;

                        *out++ = p0;
                    }
                    position->mValue += speed->mValue;
                    outlength--;
                } 
            }
            break;
        }

        /*
            32 BIT
        */
        case FMOD_SOUND_FORMAT_PCM32:
        {
            signed int *inptr = (signed int *)src;

            scale /= (float)(1U<<31);

            if (channels == 1)
            {
                float r1,r2,r3,r4;
                int len;

                len = outlength >> 2;
                while (len)
                {
                    r1 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r2 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r3 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r4 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;
                    len--;
                    out+=4;
                } 

                len = outlength & 3;
                while (len)
                {
                    *out++ = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    len--;
                } 
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        *out++ = (float)inptr[(position->mHi * channels) + count] * scale;
                    }
                    position->mValue += speed->mValue;
                    outlength--;
                } 
            }
            break;
        }

        /*
            FLOATING POINT.
        */
        case FMOD_SOUND_FORMAT_PCMFLOAT:
        {
            float *inptr = (float *)src;

            if (channels == 1)
            {
                float r1,r2,r3,r4;
                int len;

                len = outlength >> 2;
                while (len)
                {
                    r1 = inptr[position->mHi];
                    position->mValue += speed->mValue;
                    r2 = inptr[position->mHi];
                    position->mValue += speed->mValue;
                    r3 = inptr[position->mHi];
                    position->mValue += speed->mValue;
                    r4 = inptr[position->mHi];
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;
                    len--;
                    out+=4;
                } 

                len = outlength & 3;
                while (len)
                {
                    *out++ = inptr[position->mHi];
                    position->mValue += speed->mValue;
                    len--;
                } 
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        *out++ = inptr[(position->mHi * channels) + count];
                    }
                    position->mValue += speed->mValue;
                    outlength--;
                } 
            }
            break;
        }
        default:
        {
            break;
        }
    };
}



