/* A fake input.h */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "valuator-mask.h"

#undef _X_EXPORT
#define _X_EXPORT
#define _X_INTERNAL

#undef Bool
#define Bool int
#define BOOL int
#undef FALSE
#define FALSE 0
#undef TRUE
#define TRUE 1
#define CARD32 uint32_t
#define CARD16 uint16_t
#define CARD8 uint8_t
#define BYTE uint8_t
#define Atom uint32_t
#define None 0
#define X_INFO 0
#define Success 0
#define BadValue 1
#define BadMatch 2
#define BadLength 3
#define BadAlloc 4
#define XA_INTEGER 19

#define WindowPtr void*
#define ScrollInfo void*

/*int constants for pointer acceleration schemes*/
#define PtrAccelNoOp            0
#define PtrAccelPredictable     1
#define PtrAccelLightweight     2
#define PtrAccelDefault         PtrAccelPredictable

#define XIChangeDeviceProperty(...) /* */
#define LogMessageVerbSigSafe(type, level, ...) printf(__VA_ARGS__)
#define ErrorF(...) printf(__VA_ARGS__)
#define BUG_RETURN_VAL(...) /* */
#define BUG_WARN_MSG(...)

#define input_lock()
#define input_unlock()

#define ACCEL_PROP_PROFILE_NUMBER ""
#define ACCEL_PROP_CONSTANT_DECELERATION ""
#define ACCEL_PROP_VELOCITY_SCALING ""
#define ACCEL_PROP_ADAPTIVE_DECELERATION ""

#define XIDeleteDeviceProperty(...) /* */
#define XIGetKnownProperty(...) None
#define XIUnregisterPropertyHandler(...) /* */
#define XIRegisterPropertyHandler(...) Success
#define XISetDevicePropertyDeletable(...) /* */

#define BitIsOn(ptr, bit) (!!(((const BYTE *) (ptr))[(bit)>>3] & (1 << ((bit) & 7))))
#define SetBit(ptr, bit)  (((BYTE *) (ptr))[(bit)>>3] |= (1 << ((bit) & 7)))
#define ClearBit(ptr, bit) (((BYTE *)(ptr))[(bit)>>3] &= ~(1 << ((bit) & 7)))

static inline int
CountBits(const uint8_t * mask, int len)
{
    int i;
    int ret = 0;

    for (i = 0; i < len; i++)
        if (BitIsOn(mask, i))
            ret++;

    return ret;
}

struct _ValuatorAccelerationRec;
struct _DeviceIntRec;
typedef struct _DeviceIntRec DeviceIntRec;
typedef DeviceIntRec *DeviceIntPtr;

typedef void (*DeviceCallbackProc) (DeviceIntPtr /*pDev */ );

typedef void (*PointerAccelSchemeProc) (DeviceIntPtr /*device */ ,
                                        ValuatorMask * /*valuators */ ,
                                        CARD32 /*evtime */ );

typedef Bool (*PointerAccelSchemeInitProc) (DeviceIntPtr /*dev */ ,
                                            struct _ValuatorAccelerationRec *
                                            /*protoScheme */ );

typedef struct _ValuatorAccelerationRec {
    int number;
    PointerAccelSchemeProc AccelSchemeProc;
    void *accelData;            /* at disposal of AccelScheme */
    PointerAccelSchemeInitProc AccelInitProc;
    DeviceCallbackProc AccelCleanupProc;
} ValuatorAccelerationRec, *ValuatorAccelerationPtr;

typedef struct _AxisInfo {
    int resolution;
    int min_resolution;
    int max_resolution;
    int min_value;
    int max_value;
    Atom label;
    CARD8 mode;
    ScrollInfo scroll;
} AxisInfo, *AxisInfoPtr;

typedef struct _ValuatorClassRec {
    int sourceid;
    int numMotionEvents;
    int first_motion;
    int last_motion;
    void *motion;               /* motion history buffer. Different layout
                                   for MDs and SDs! */
    WindowPtr motionHintWindow;

    AxisInfoPtr axes;
    unsigned short numAxes;
    double *axisVal;            /* always absolute, but device-coord system */
    ValuatorAccelerationRec accelScheme;
    int h_scroll_axis;          /* horiz smooth-scrolling axis */
    int v_scroll_axis;          /* vert smooth-scrolling axis */
} ValuatorClassRec;

typedef struct _ValuatorClassRec *ValuatorClassPtr;

typedef struct _XIPropertyValue {
    Atom type;                  /* ignored by server */
    short format;               /* format of data for swapping - 8,16,32 */
    long size;                  /* size of data in (format/8) bytes */
    void *data;                 /* private to client */
} XIPropertyValueRec;

typedef XIPropertyValueRec *XIPropertyValuePtr;

typedef struct {
    int num, den, threshold;
    unsigned char id;
} PtrCtrl;

typedef struct _PtrFeedbackClassRec {
    // PtrCtrlProcPtr CtrlProc;
    PtrCtrl ctrl;
    // PtrFeedbackPtr next;
} PtrFeedbackClassRec;
typedef struct _PtrFeedbackClassRec *PtrFeedbackPtr;


struct _DeviceIntRec {
    ValuatorClassPtr valuator;
    PtrFeedbackPtr ptrfeed;
};


static inline int
XIPropToFloat(XIPropertyValuePtr val, int *nelem_return, float **buf_return)
{
    float *buf = calloc(val->size, sizeof *buf);

    for (int i = 0; i < val->size; i++) {
        buf[i] = ((float*)val->data)[i];
    }

    *nelem_return = val->size;
    *buf_return = buf;

    return Success;
}

static inline int
XIPropToInt(XIPropertyValuePtr val, int *nelem_return, int **buf_return)
{
    int i;
    int *buf;

    if (val->type != XA_INTEGER)
        return BadMatch;
    if (!*buf_return && *nelem_return)
        return BadLength;

    switch (val->format) {
    case 8:
    case 16:
    case 32:
        break;
    default:
        return BadValue;
    }

    buf = *buf_return;

    if (!buf && !(*nelem_return)) {
        buf = calloc(val->size, sizeof(int));
        if (!buf)
            return BadAlloc;
        *buf_return = buf;
        *nelem_return = val->size;
    }
    else if (val->size < *nelem_return)
        *nelem_return = val->size;

    for (i = 0; i < val->size && i < *nelem_return; i++) {
        switch (val->format) {
        case 8:
            buf[i] = ((CARD8 *) val->data)[i];
            break;
        case 16:
            buf[i] = ((CARD16 *) val->data)[i];
            break;
        case 32:
            buf[i] = ((CARD32 *) val->data)[i];
            break;
        }
    }

    return Success;
}

static inline void
accelPointer(DeviceIntPtr dev, ValuatorMask *valuators, CARD32 ms) {
	if (dev->valuator->accelScheme.AccelSchemeProc)
		dev->valuator->accelScheme.AccelSchemeProc(dev, valuators, ms);
}
