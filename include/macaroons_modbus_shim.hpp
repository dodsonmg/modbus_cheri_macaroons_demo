#ifndef _MACAROONS_MODBUS_SHIM_
#define _MACAROONS_MODBUS_SHIM_

#include <random>
#include <iostream>
#include <cassert>

/* for modbus */
#include <modbus/modbus.h>

/* macaroons */
#include "macaroons/macaroons.hpp"

int modbus_write_bit_macaroons(modbus_t *ctx, int addr, int status);
int modbus_receive_macaroons(modbus_t *ctx, uint8_t *req);
int modbus_process_request_macaroons(modbus_t *ctx, uint8_t *req,
                                     int req_length, uint8_t *rsp, int *rsp_length,
                                     modbus_mapping_t *mb_mapping);

#endif /* _MACAROONS_MODBUS_SHIM_ */
