#ifndef _CHERI_MACAROONS_SHIM_
#define _CHERI_MACAROONS_SHIM_

/* For CHERI */
#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if !__has_feature(capabilities)
#error "<cheri_macaroons_shim.h> should only be included when CHERI is supported"
#endif

#include "macaroons/macaroons.hpp"
#include "macaroons_shim.hpp"
#include "cheri_shim.hpp"

typedef enum {
    CHERI_SHIM,
    MACAROONS_SHIM,
    CHERI_MACAROONS_SHIM,
} shim_t;

/* Reads the boolean status of bits and sets the array elements
   in the destination to TRUE or FALSE (single bits). */
int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_read_bits_macaroons(ctx, addr, nb, dest);
    }

    return -1;
}

/* Same as modbus_read_bits but reads the remote device input table */
int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_read_input_bits_macaroons(ctx, addr, nb, dest);
    }

    return -1;
}

/* Reads the holding registers of remote device and put the data into an
   array */
int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_read_registers_macaroons(ctx, addr, nb, dest);
    }

    return -1;
}

/* Reads the input registers of remote device and put the data into an array */
int modbus_read_input_registers(modbus_t *ctx, int addr, int nb,
                                uint16_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_read_input_registers_macaroons(ctx, addr, nb, dest);
    }

    return -1;
}

/* Turns ON or OFF a single bit of the remote device */
int modbus_write_bit(modbus_t *ctx, int addr, int status, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_write_bit_macaroons(ctx, addr, status);
    }

    return -1;
}

/* Writes a value in one register of the remote device */
int modbus_write_register(modbus_t *ctx, int addr,
                          const uint16_t value, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_write_register_macaroons(ctx, addr, value);
    }

    return -1;
}

/* Write the bits of the array in the remote device */
int modbus_write_bits(modbus_t *ctx, int addr, int nb,
                      const uint8_t *src, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_write_bits_macaroons(ctx, addr, nb, src);
    }

    return -1;
}

/* Write the values from the array to the registers of the remote device */
int modbus_write_registers(modbus_t *ctx, int addr, int nb,
                           const uint16_t *src, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_write_registers_macaroons(ctx, addr, nb, src);
    }

    return -1;
}

/* I'm not actually sure what this does...
   The unit test appears designed to fail */
int modbus_mask_write_register(modbus_t *ctx, int addr, uint16_t and_mask,
                               uint16_t or_mask, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_mask_write_register_macaroons(ctx, addr,
                                                    and_mask, or_mask);
    }

    return -1;
}

/* Write multiple registers from src array to remote device and read multiple
   registers from remote device to dest array. */
int modbus_write_and_read_registers(modbus_t *ctx,
                                    int write_addr, int write_nb,
                                    const uint16_t *src,
                                    int read_addr, int read_nb,
                                    uint16_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_write_and_read_registers_macaroons(ctx, write_addr, write_nb, src,
                                                         read_addr, read_nb, dest);
    }

    return -1;
}

/* Send a request to get the slave ID of the device (only available in serial
   communication). */
int modbus_report_slave_id(modbus_t *ctx, int max_dest, uint8_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM) {
        return modbus_report_slave_id_macaroons(ctx, max_dest, dest);
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

#endif /* _CHERI_MACAROONS_SHIM_ */