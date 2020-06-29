#ifndef _CHERI_MACAROONS_MODBUS_SHIM_
#define _CHERI_MACAROONS_MODBUS_SHIM_

#include "macaroons_modbus_shim.hpp"

typedef enum {
    CHERI_SHIM,
    MACAROONS_SHIM,
    CHERI_MACAROONS_SHIM,
} shim_t;

/* Turns ON or OFF a single bit of the remote device */
int modbus_write_bit(modbus_t *ctx, int addr, int status, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_write_bit_macaroons(ctx, addr, status);
    }

    return -1;
}

/* Receive the request from a modbus master */
int modbus_receive(modbus_t *ctx, uint8_t *req, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_receive_macaroons(ctx, req);
    }

    return -1;
}

/**
 * Analyses the request and constructs a response.
 *
 * If an error occurs, this function construct the response
 * accordingly.
 * */
int modbus_process_request(modbus_t *ctx, uint8_t *req,
                           int req_length, uint8_t *rsp, int *rsp_length,
                           modbus_mapping_t *mb_mapping, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_process_request_macaroons(ctx, req, req_length,
            rsp, rsp_length, mb_mapping);
    }

    return -1;
}

#endif /* _CHERI_MACAROONS_MODBUS_SHIM_ */