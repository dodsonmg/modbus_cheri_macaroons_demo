#ifndef _MACAROONS_SHIM_
#define _MACAROONS_SHIM_

#include <iostream>
#include <random>
#include <cassert>

/* for Modbus */
extern "C" {
    #include <modbus/modbus.h>
}

/* Macaroons */
#include "macaroons/macaroons.hpp"

/* shim support */
#include "cheri_macaroons_shim.hpp"

/******************
 * SERVER FUNCTIONS
 *****************/

int modbus_receive_macaroons(modbus_t *ctx, uint8_t *req);
int modbus_process_request_macaroons(modbus_t *ctx, uint8_t *req,
                                     int req_length, uint8_t *rsp, int *rsp_length,
                                     modbus_mapping_t *mb_mapping,
                                     shim_t shim_type, shim_s shim_state);

/******************
 * CLIENT FUNCTIONS
 *****************/

/**
 * no function required for read/write token
 * since no additional macaroon is sent with that request
 * */
int modbus_read_bits_macaroons(modbus_t *ctx, int addr, int nb, uint8_t *dest);
int modbus_read_input_bits_macaroons(modbus_t *ctx, int addr, int nb, uint8_t *dest);
int modbus_read_registers_macaroons(modbus_t *ctx, int addr, int nb, uint16_t *dest);
int modbus_read_input_registers_macaroons(modbus_t *ctx, int addr, int nb, uint16_t *dest);
int modbus_write_bit_macaroons(modbus_t *ctx, int addr, int status);
int modbus_write_register_macaroons(modbus_t *ctx, int addr, const uint16_t value);
int modbus_write_bits_macaroons(modbus_t *ctx, int addr, int nb, const uint8_t *src);
int modbus_write_registers_macaroons(modbus_t *ctx, int addr, int nb, const uint16_t *data);
int modbus_mask_write_register_macaroons(modbus_t *ctx, int addr, uint16_t and_mask, uint16_t or_mask);
int modbus_write_and_read_registers_macaroons(modbus_t *ctx, int write_addr, int write_nb,
                                               const uint16_t *src, int read_addr, int read_nb,
                                               uint16_t *dest);
int modbus_report_slave_id_macaroons(modbus_t *ctx, int max_dest, uint8_t *dest);

#endif /* _MACAROONS_SHIM_ */
