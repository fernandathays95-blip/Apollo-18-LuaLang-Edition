/*
 * ALERT.c
 *
 * Module: ALERT_SYSTEM
 * Purpose: Centralized alert and warning handler
 *
 * Design rules:
 * - Deterministic behavior
 * - No dynamic allocation
 * - Fail-safe defaults
 * - Small and predictable
 */

#include <stdint.h>
#include <stdbool.h>

/* ===================== */
/* Alert Levels          */
/* ===================== */

typedef enum {
    ALERT_INFO = 0,
    ALERT_WARNING = 1,
    ALERT_CRITICAL = 2
} alert_level_t;

/* ===================== */
/* Alert Codes           */
/* ===================== */

typedef enum {
    ALERT_NONE = 0,
    ALERT_SENSOR_FAIL,
    ALERT_OVER_TEMPERATURE,
    ALERT_OVER_PRESSURE,
    ALERT_ENGINE_FAULT,
    ALERT_COMMUNICATION_LOSS
} alert_code_t;

/* ===================== */
/* Alert State           */
/* ===================== */

static volatile alert_level_t current_level = ALERT_INFO;
static volatile alert_code_t  current_code  = ALERT_NONE;

/* ===================== */
/* Hardware Hooks (stubs)*/
/* ===================== */

/* These functions are expected to be implemented
 * by the hardware / platform layer.
 */

void hw_led_info(bool on);
void hw_led_warning(bool on);
void hw_led_critical(bool on);

void telemetry_send_alert(alert_level_t level, alert_code_t code);

/* ===================== */
/* Internal Helpers      */
/* ===================== */

static void alert_clear_outputs(void)
{
    hw_led_info(false);
    hw_led_warning(false);
    hw_led_critical(false);
}

static void alert_set_outputs(alert_level_t level)
{
    alert_clear_outputs();

    if (level == ALERT_INFO) {
        hw_led_info(true);
    } else if (level == ALERT_WARNING) {
        hw_led_warning(true);
    } else {
        hw_led_critical(true);
    }
}

/* ===================== */
/* Public API            */
/* ===================== */

void alert_init(void)
{
    current_level = ALERT_INFO;
    current_code  = ALERT_NONE;

    alert_set_outputs(ALERT_INFO);
}

void alert_raise(alert_level_t level, alert_code_t code)
{
    /* Fail-safe: critical always overrides */
    if (level >= current_level) {
        current_level = level;
        current_code  = code;

        alert_set_outputs(level);
        telemetry_send_alert(level, code);
    }
}

void alert_clear(void)
{
    current_level = ALERT_INFO;
    current_code  = ALERT_NONE;

    alert_set_outputs(ALERT_INFO);
}

alert_level_t alert_get_level(void)
{
    return current_level;
}

alert_code_t alert_get_code(void)
{
    return current_code;
}
