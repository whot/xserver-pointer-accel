#pragma once

#include <stdint.h>

#define MAX_VALUATORS 36
#define _X_EXPORT
#define _X_HIDDEN
#define Bool int
//#define FALSE 0
//#define TRUE 1

typedef struct _ValuatorMask ValuatorMask;

struct _ValuatorMask {
    int8_t last_bit;            /* highest bit set in mask */
    int8_t has_unaccelerated;
    uint8_t mask[(MAX_VALUATORS + 7) / 8];
    double valuators[MAX_VALUATORS];    /* valuator data */
    double unaccelerated[MAX_VALUATORS];    /* valuator data */
};


extern _X_EXPORT ValuatorMask *valuator_mask_new(int num_valuators);
extern _X_EXPORT void valuator_mask_free(ValuatorMask **mask);
extern _X_EXPORT void valuator_mask_set_range(ValuatorMask *mask,
                                              int first_valuator,
                                              int num_valuators,
                                              const int *valuators);
extern _X_EXPORT void valuator_mask_set(ValuatorMask *mask, int valuator,
                                        int data);
extern _X_EXPORT void valuator_mask_set_double(ValuatorMask *mask, int valuator,
                                               double data);
extern _X_EXPORT void valuator_mask_zero(ValuatorMask *mask);
extern _X_EXPORT int valuator_mask_size(const ValuatorMask *mask);
extern _X_EXPORT int valuator_mask_isset(const ValuatorMask *mask, int bit);
extern _X_EXPORT void valuator_mask_unset(ValuatorMask *mask, int bit);
extern _X_EXPORT int valuator_mask_num_valuators(const ValuatorMask *mask);
extern _X_EXPORT void valuator_mask_copy(ValuatorMask *dest,
                                         const ValuatorMask *src);
extern _X_EXPORT int valuator_mask_get(const ValuatorMask *mask, int valnum);
extern _X_EXPORT double valuator_mask_get_double(const ValuatorMask *mask,
                                                 int valnum);
extern _X_EXPORT Bool valuator_mask_fetch(const ValuatorMask *mask,
                                          int valnum, int *val);
extern _X_EXPORT Bool valuator_mask_fetch_double(const ValuatorMask *mask,
                                                 int valnum, double *val);
extern _X_EXPORT Bool valuator_mask_has_unaccelerated(const ValuatorMask *mask);
extern _X_EXPORT void valuator_mask_set_unaccelerated(ValuatorMask *mask,
                                                      int valuator,
                                                      double accel,
                                                      double unaccel);
extern _X_EXPORT void valuator_mask_set_absolute_unaccelerated(ValuatorMask *mask,
                                                               int valuator,
                                                               int absolute,
                                                               double unaccel);
extern _X_EXPORT double valuator_mask_get_accelerated(const ValuatorMask *mask,
                                                      int valuator);
extern _X_EXPORT double valuator_mask_get_unaccelerated(const ValuatorMask *mask,
                                                        int valuator);
extern _X_EXPORT Bool valuator_mask_fetch_unaccelerated(const ValuatorMask *mask,
                                                        int valuator,
                                                        double *accel,
                                                        double *unaccel);
extern _X_HIDDEN void valuator_mask_drop_unaccelerated(ValuatorMask *mask);
