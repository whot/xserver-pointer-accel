#include <assert.h>
#include <ptrveloc.h>

#define streq(a, b) (strcmp(a, b) == 0)

#define DEFAULT_PTR_NUMERATOR   2
#define DEFAULT_PTR_DENOMINATOR 1
#define DEFAULT_PTR_THRESHOLD   4

PtrCtrl defaultPointerControl = {
    DEFAULT_PTR_NUMERATOR,
    DEFAULT_PTR_DENOMINATOR,
    DEFAULT_PTR_THRESHOLD,
    0
};

ValuatorAccelerationRec pointerAccelerationScheme[] = {
    {PtrAccelNoOp, NULL, NULL, NULL, NULL},
    {PtrAccelPredictable, acceleratePointerPredictable, NULL,
     InitPredictableAccelerationScheme, AccelerationDefaultCleanup},
    {PtrAccelLightweight, acceleratePointerLightweight, NULL, NULL, NULL},
    {-1, NULL, NULL, NULL, NULL}        /* terminator */
};

/* Just enough to not segfault during ptracceleration */
static DeviceIntPtr
init_device(void) {
    DeviceIntPtr dev = calloc(1, sizeof(*dev));

    dev->valuator = calloc(1, sizeof(*dev->valuator));
    dev->ptrfeed = calloc(1, sizeof(*dev->ptrfeed));

    return dev;
}

/* This is basically what ApplyAccelerationSettings does, without the
 * xorg.conf parsing */
static void
apply_acceleration_settings(DeviceIntPtr dev)
{
    DeviceVelocityPtr s;
    double v = 1.0;

    /* see ApplyAccelerationSettings which gets it from InitPtrFeedbackClassDeviceStruct */
    dev->ptrfeed->ctrl = defaultPointerControl;

    /* content from ProcessVelocityConfiguration */
    s = GetDevicePredictableAccelData(dev);

    /**
     * These options just set the property, and then expect the property
     * handler to deal with the changes (if any?) from the defaults already
     * initialized.
     *
     * This code cuts that short and replicates the actual changes.
     */

    /* ConstantDeceleration */
    v = 1.0; /* default from ApplyAccelerationSettings */
    if (v != 1.0)
        s->const_acceleration = 1/v;

    /* AdaptiveDeceleration */
    v = 1.0; /* default from ApplyAccelerationSettings */
    if (v >= 1.0f)
        s->min_acceleration = 1 / v;

     /* AccelerationProfile, default to s->statistics.profile_number which
      * is AccelProfileClassic in InitVelocityData */
    int profile = AccelProfileClassic;
    SetAccelerationProfile(s, profile);

    /* ExpectedRate, VelocityScale map to the same thing */
    v = 0.0; /* ExpectedRate default from ApplyAccelerationSettings */
    if (v > 0) {
        v = 1000.0/v;
    } else {
        v = s->corr_mul; /* VelocityScale default from ApplyAccelerationSettings */
    }
    /* now the bit from AccelSetScaleProperty */
    if (v > 0)
        s->corr_mul = v;

#if 0
    /* These are unchanged from the default in InitVelocityData */

    /* velocity tracker count */
     s->initial_range = s->initial_range;
     s->max_diff = s->max_diff;
     s->max_rel_diff = s->max_rel_diff;
     s->average_accel = s->average_accel;
     s->reset_time = s->reset_time;
#endif
     s->use_softening = s->const_acceleration == 1.0;
}

/* units/ms → mm/s */
static inline double
upms_to_mmps(double units, int dpi)
{
    return units/dpi * 25.4 * 1000;
}

/* mm/s → units/µs */
static inline double
mmps_to_upms(double mmps, int dpi)
{
        return mmps * (dpi/25.4) / 1000;
}

static inline void
print_profile(DeviceIntPtr dev, int profile, int threshold, int accel)
{
    PointerAccelerationProfileFunc func;
    DeviceVelocityPtr vel = GetDevicePredictableAccelData(dev);
    double mmps;
    const double DPI = 1000;

    /* xserver code uses velocity in units/ms */

    /* This will segfault for AccelProfileDeviceSpecific */
    func = GetAccelerationProfile(vel, profile);

    printf("# data: velocity (mm/s) factor velocity(units/us) "
           "velocity(units/ms) factor(corr_mul applied on velocity)\n");

    for (mmps = 0.0; mmps < 1000; mmps += 1) {
        double factor, factor_cor_mul;

        double units_per_ms = mmps_to_upms(mmps, DPI);
        double units_per_us = units_per_us/1000;

        /* vel is units per ms */
        factor = func(dev, vel, units_per_ms, threshold, accel);
        factor_cor_mul = func(dev, vel, units_per_ms * vel->corr_mul, threshold, accel);
        printf("%.8f\t%.4f\t%.8f\t%.8f\t%.8f\n", mmps,
               factor,
               units_per_us,
               units_per_ms,
               factor_cor_mul);
    }
}

/**
 * Constant stream of deltas at a given time interval, check what actually
 * happens and how they're accelerated.
 */
static inline void
print_deltas(DeviceIntPtr dev)
{
    ValuatorMask *mask = valuator_mask_new(2);
    CARD32 ms = 0;

    for (size_t i = 0; i < 20; i++, ms += 10) {
        valuator_mask_set(mask, 0, 10);
        valuator_mask_set(mask, 1, 0);

        double x = valuator_mask_get_double(mask, 0),
               y = valuator_mask_get_double(mask, 1);
        double ax, ay;

        accelPointer(dev, mask, ms);

        ax = valuator_mask_get_double(mask, 0);
        ay = valuator_mask_get_double(mask, 1);

        printf(":: mask: %.2f/%.2f -> %.2f/%.2f, delta %.2f/%.2f\n",
               x, y, ax, ay, (ax - x), (ay - y));
    }
}

int main(int argc, char **argv)
{
    double threshold, accel;
    int profile;
    const char *p = argv[1];

    if (argc != 4) {
        printf("Usage: %s <profile-name> <threshold> <accel>\n", argv[0]);
        printf("Defaults: classic 4 2\n");
        return 1;
    }
    assert(argc == 4);

    if (streq(p, "simple"))
        profile = AccelProfileSimple;
    else if (streq(p, "classic"))
        profile = AccelProfileSimple;
    else if (streq(p, "smooth"))
        profile = AccelProfileSmoothLinear;
    else if (streq(p, "linear"))
        profile = AccelProfileLinear;
    else {
        printf("Unsupported profile name\n");
        return 1;
    }

    threshold = strtod(argv[2], NULL);
    accel = strtod(argv[3], NULL);

    DeviceIntPtr dev = init_device();

    InitPredictableAccelerationScheme(dev, &pointerAccelerationScheme[1]);
    apply_acceleration_settings(dev);

    /* This is the default when no configuration is applied */
    print_profile(dev, profile, threshold, accel);

    return 0;
}
