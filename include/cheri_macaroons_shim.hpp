#ifndef _CHERI_MACAROONS_SHIM_
#define _CHERI_MACAROONS_SHIM_

/* For CHERI */
#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if !__has_feature(capabilities)
#error "<cheri_macaroons_shim.h> should only be included when CHERI is supported"
#endif

/* shim support */
#include "shim_types.hpp"
#include "macaroons_shim.hpp"
#include "cheri_shim.hpp"

/******************
 * CLIENT FUNCTIONS
 *****************/

/* Reads the boolean status of bits and sets the array elements
   in the destination to TRUE or FALSE (single bits). */
int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest, shim_t shim);

/* Same as modbus_read_bits but reads the remote device input table */
int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest, shim_t shim);

/* Reads the holding registers of remote device and put the data into an
   array */
int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest, shim_t shim);

/* Reads the input registers of remote device and put the data into an array */
int modbus_read_input_registers(modbus_t *ctx, int addr, int nb,
                                uint16_t *dest, shim_t shim);

/* Turns ON or OFF a single bit of the remote device */
int modbus_write_bit(modbus_t *ctx, int addr, int status, shim_t shim);

/* Writes a value in one register of the remote device */
int modbus_write_register(modbus_t *ctx, int addr,
                          const uint16_t value, shim_t shim);

/* Write the bits of the array in the remote device */
int modbus_write_bits(modbus_t *ctx, int addr, int nb,
                      const uint8_t *src, shim_t shim);

/* Write the values from the array to the registers of the remote device */
int modbus_write_registers(modbus_t *ctx, int addr, int nb,
                           const uint16_t *src, shim_t shim);

/* I'm not actually sure what this does...
   The unit test appears designed to fail */
int modbus_mask_write_register(modbus_t *ctx, int addr, uint16_t and_mask,
                               uint16_t or_mask, shim_t shim);

/* Write multiple registers from src array to remote device and read multiple
   registers from remote device to dest array. */
int modbus_write_and_read_registers(modbus_t *ctx,
                                    int write_addr, int write_nb,
                                    const uint16_t *src,
                                    int read_addr, int read_nb,
                                    uint16_t *dest, shim_t shim);

/* Send a request to get the slave ID of the device (only available in serial
   communication). */
int modbus_report_slave_id(modbus_t *ctx, int max_dest, uint8_t *dest, shim_t shim);

/******************
 * SERVER FUNCTIONS
 *****************/

/**
 *  Allocates 5 arrays to store bits, input bits, registers, inputs
 * registers, and a string. The pointers are stored in modbus_mapping structure.
 *
 * The modbus_mapping_new_start_address() function shall return the new allocated
 * structure if successful. Otherwise it shall return NULL and set errno to
 * ENOMEM.
 * */
modbus_mapping_t* modbus_mapping_new_start_address(
    unsigned int start_bits, unsigned int nb_bits,
    unsigned int start_input_bits, unsigned int nb_input_bits,
    unsigned int start_registers, unsigned int nb_registers,
    unsigned int start_input_registers, unsigned int nb_input_registers,
    shim_t shim);

/* Receive the request from a modbus master */
int modbus_receive(modbus_t *ctx, uint8_t *req, shim_t shim);

/**
 * Analyses the request and constructs a response.
 *
 * If an error occurs, this function construct the response
 * accordingly.
 * */
int modbus_process_request(modbus_t *ctx, uint8_t *req,
                           int req_length, uint8_t *rsp, int *rsp_length,
                           modbus_mapping_t *mb_mapping, shim_t shim);

#endif /* _CHERI_MACAROONS_SHIM_ */