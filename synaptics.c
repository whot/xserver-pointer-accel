#include "input.h"
#include "ptrveloc.h"
#include <math.h>

typedef struct {
    double accl;
    double max_speed;
    double min_speed;
    int press_motion_min_z;
    int press_motion_max_z;
    double press_motion_min_factor;
    double press_motion_max_factor;
} SynapticsParameters;

typedef struct {
    int z;
} SynapticsHwState;

typedef struct {
    SynapticsParameters synpara;
    int moving_state;

    SynapticsHwState *hwState;

    int minp, maxp;
} SynapticsPrivate;

#define MS_TOUCHPAD_RELATIVE 1 /* doesn't matter */

static InputInfoRec pInfo;
static SynapticsPrivate priv;
static SynapticsHwState hwState;

double
SynapticsAccelerationProfile(DeviceIntPtr dev,
                             DeviceVelocityPtr vel,
                             double velocity, double thr, double acc)
{
    InputInfoPtr pInfo = dev->public.devicePrivate;
    SynapticsPrivate *priv = (SynapticsPrivate *) (pInfo->private);
    SynapticsParameters *para = &priv->synpara;

    double accelfct;

    /*
     * synaptics accel was originally base on device coordinate based
     * velocity, which we recover this way so para->accl retains its scale.
     */
    velocity /= vel->const_acceleration;

    /* speed up linear with finger velocity */
    accelfct = velocity * para->accl;

    /* clip acceleration factor */
    if (accelfct > para->max_speed * acc)
        accelfct = para->max_speed * acc;
    else if (accelfct < para->min_speed)
        accelfct = para->min_speed;

    /* modify speed according to pressure */
    if (priv->moving_state == MS_TOUCHPAD_RELATIVE) {
        int minZ = para->press_motion_min_z;
        int maxZ = para->press_motion_max_z;
        double minFctr = para->press_motion_min_factor;
        double maxFctr = para->press_motion_max_factor;

        if (priv->hwState->z <= minZ) {
            accelfct *= minFctr;
        }
        else if (priv->hwState->z >= maxZ) {
            accelfct *= maxFctr;
        }
        else {
            accelfct *=
                minFctr + (priv->hwState->z - minZ) * (maxFctr -
                                                       minFctr) / (maxZ - minZ);
        }
    }

    return accelfct;
}

void
SynapticsInit(DeviceIntPtr dev, double min_speed, double max_speed)
{
    float tmpf;

    dev->public.devicePrivate = &pInfo;
    pInfo.private = &priv;

    priv.synpara.min_speed = min_speed; /* 0.4 */
    priv.synpara.max_speed = max_speed; /* 0.7 */

    /* This is taken from the synaptics driver, but we hardcode the pressure
     * min/max factor to 1 anyway, so all this doesn't matter */
    priv.minp = 0;
    priv.maxp = 255; /* ABS_PRESSURE range */
    double range = priv.maxp - priv.minp + 1;
    double pressureMotionMinZ = priv.minp + range * (30.0 / 256);
    double pressureMotionMaxZ = priv.minp + range * (160.0 / 256);
    priv.synpara.press_motion_min_z = pressureMotionMinZ;
    priv.synpara.press_motion_max_z = pressureMotionMaxZ;

    /* Defaults from the driver are 1.0, and this way pressure the pressure
     * motion factor is always 1.0 as well so it doesn't matter what we set
     * as z value */
    priv.synpara.press_motion_min_factor = 1.0;
    priv.synpara.press_motion_max_factor = 1.0;

    /* see set_default_parameters() */
    double diag;
    const char *env;
    if ((env = getenv("SYNAPTICS_DIAGONAL")) == NULL)
        hypot(5112 - 1024, 4832 - 2024); /* T440s over PS/2 */
    else
        diag = atoi(env);
    double accelFactor = 200.0 / diag;
    priv.synpara.accl = accelFactor;

    priv.moving_state = MS_TOUCHPAD_RELATIVE;
    priv.hwState = &hwState;
    /* this needs to be within that range, otherwise we end up scaling */
    priv.hwState->z = pressureMotionMinZ;

    DeviceVelocityPtr vel = GetDevicePredictableAccelData(dev);
    SetDeviceSpecificAccelerationProfile(vel,
                                         SynapticsAccelerationProfile);

    /* see DeviceInit() in the real synaptics.c */
    tmpf = 1.0/priv.synpara.min_speed;
    SetConstantDeceleration(tmpf);

    priv.synpara.max_speed /= priv.synpara.min_speed;
    priv.synpara.min_speed = 1.0;

    vel->corr_mul = 12.5f;
}
