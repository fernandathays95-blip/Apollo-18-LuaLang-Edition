/*
 * RADIO.c
 *
 * Module: RADIO_COMM
 * Purpose: Low-level radio communication handler
 *
 * Design rules:
 * - Deterministic execution
 * - No dynamic memory
 * - Fixed-size buffers
 * - Fail-safe defaults
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ===================== */
/* Configuration         */
/* ===================== */

#define RADIO_TX_BUFFER_SIZE 128
#define RADIO_RX_BUFFER_SIZE 128

/* ===================== */
/* Radio State           */
/* ===================== */

static volatile bool radio_initialized = false;
static volatile bool radio_link_ok      = false;

static uint8_t tx_buffer[RADIO_TX_BUFFER_SIZE];
static uint8_t rx_buffer[RADIO_RX_BUFFER_SIZE];

/* ===================== */
/* Hardware Hooks (stubs)*/
/* ===================== */
/* Implemented by platform-specific layer */

bool hw_radio_init(void);
bool hw_radio_send(const uint8_t *data, uint16_t length);
bool hw_radio_receive(uint8_t *data, uint16_t max_length, uint16_t *out_length);
bool hw_radio_link_status(void);

/* ===================== */
/* Internal Helpers      */
/* ===================== */

static void clear_buffers(void)
{
    memset(tx_buffer, 0, RADIO_TX_BUFFER_SIZE);
    memset(rx_buffer, 0, RADIO_RX_BUFFER_SIZE);
}

/* ===================== */
/* Public API            */
/* ===================== */

void radio_init(void)
{
    clear_buffers();

    radio_initialized = hw_radio_init();
    radio_link_ok     = false;
}

bool radio_is_ready(void)
{
    return radio_initialized;
}

bool radio_link_status(void)
{
    radio_link_ok = hw_radio_link_status();
    return radio_link_ok;
}

bool radio_send(const uint8_t *data, uint16_t length)
{
    if (!radio_initialized) {
        return false;
    }

    if (length == 0 || length > RADIO_TX_BUFFER_SIZE) {
        return false;
    }

    memcpy(tx_buffer, data, length);
    return hw_radio_send(tx_buffer, length);
}

bool radio_receive(uint16_t *out_length)
{
    if (!radio_initialized || out_length == NULL) {
        return false;
    }

    *out_length = 0;
    return hw_radio_receive(rx_buffer, RADIO_RX_BUFFER_SIZE, out_length);
}

const uint8_t *radio_rx_buffer(void)
{
    return rx_buffer;
}
