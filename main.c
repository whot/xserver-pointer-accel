#include <assert.h>
#include <ptrveloc.h>

#define streq(a, b) (strcmp(a, b) == 0)

#define DEFAULT_PTR_NUMERATOR   2
#define DEFAULT_PTR_DENOMINATOR 1
#define DEFAULT_PTR_THRESHOLD   4

/**
 * These options just set the property, and then expect the property
 * handler to deal with the changes (if any?) from the defaults already
 * initialized.
 *
 * This code cuts that short and replicates the actual changes.
 */
static double ConstantDeceleration = 1.0; /* see ProcessVelocityConfiguration() */
static double AdaptiveDeceleration = 1.0; /* see ProcessVelocityConfiguration() */
/* AccelerationProfile, default to s->statistics.profile_number which
 * is AccelProfileClassic in InitVelocityData */
static int AccelerationProfile = AccelProfileClassic;
static int ExpectedRate = 0; /* see ProcessVelocityConfiguration() */
static int VelocityScale = 10; /* see InitVelocityData() */

void
SetConstantDeceleration(double v) {
    ConstantDeceleration = v;
}

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

/* This is basically what ApplyAccelerationSettings and
 * ProcessVelocityConfiguration() do, without the xorg.conf parsing */
static void
apply_acceleration_settings(DeviceIntPtr dev)
{
    DeviceVelocityPtr s;
    double v = 1.0;

    /* see ApplyAccelerationSettings which gets it from InitPtrFeedbackClassDeviceStruct */
    dev->ptrfeed->ctrl = defaultPointerControl;

    /* content from ProcessVelocityConfiguration */
    s = GetDevicePredictableAccelData(dev);

    v = ConstantDeceleration;
    if (v != 1.0)
        s->const_acceleration = 1/v;

    v = AdaptiveDeceleration;
    if (v >= 1.0f)
        s->min_acceleration = 1 / v;

    SetAccelerationProfile(s, AccelerationProfile);

    /* ExpectedRate, VelocityScale map to the same thing */
    v = ExpectedRate;
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
print_profile(DeviceIntPtr dev, int profile, double threshold, double accel)
{
    PointerAccelerationProfileFunc func;
    DeviceVelocityPtr vel = GetDevicePredictableAccelData(dev);
    double mmps;
    const double DPI = 1000;

    /* xserver code uses velocity in units/ms */

    /* This will segfault for AccelProfileDeviceSpecific */
    func = GetAccelerationProfile(vel, profile);

    printf("# data: velocity (mm/s) factor velocity(units/us) "
           "velocity(units/ms) factor(corr_mul applied on velocity)"
           "factor(const-decel applied) factor(corr_mul and const-decel applied)\n");

    for (mmps = 0.0; mmps < 1000; mmps += 1) {
        double factor, factor_cor_mul;

        double units_per_ms = mmps_to_upms(mmps, DPI);
        double units_per_us = units_per_us/1000;

        /* vel is units per ms */
        factor = func(dev, vel, units_per_ms, threshold, accel);
        factor_cor_mul = func(dev, vel, units_per_ms * vel->corr_mul, threshold, accel);
        double f_decel = factor * vel->const_acceleration;
        double fcm_decel = factor_cor_mul * vel->const_acceleration;
        printf("%.8f\t%.4f\t%.8f\t%.8f\t%.8f\t%.8f\t%.8f\n", mmps,
               factor,
               units_per_us,
               units_per_ms,
               factor_cor_mul,
               f_decel,
               fcm_decel);
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
        printf("If the profile is 'synaptics', the threshold is min-speed (deflt 0.2), "
               "the accel is max-speed (deflt 0.4)\n");
        return 1;
    }
    assert(argc == 4);

    if (streq(p, "simple"))
        profile = AccelProfileSimple;
    else if (streq(p, "classic"))
        profile = AccelProfileClassic;
    else if (streq(p, "smooth"))
        profile = AccelProfileSmoothLinear;
    else if (streq(p, "linear"))
        profile = AccelProfileLinear;
    else if (streq(p, "polynomial"))
        profile = AccelProfilePolynomial;
    else if (streq(p, "power"))
        profile = AccelProfilePower;
    else if (streq(p, "limited"))
        profile = AccelProfileSmoothLimited;
    else if (streq(p, "synaptics"))
        profile = AccelProfileDeviceSpecific;
    else {
        printf("Unsupported profile name\n");
        return 1;
    }

    threshold = strtod(argv[2], NULL);
    accel = strtod(argv[3], NULL);

    DeviceIntPtr dev = init_device();

    InitPredictableAccelerationScheme(dev, &pointerAccelerationScheme[1]);
    if (profile == AccelProfileDeviceSpecific) {
        double min_speed = threshold,
               max_speed = accel;
        SynapticsInit(dev, min_speed, max_speed);
    }

    apply_acceleration_settings(dev);

    /* This is the default when no configuration is applied */
    print_profile(dev, profile, threshold, accel);

    return 0;
}
