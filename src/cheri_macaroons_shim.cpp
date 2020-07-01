#include "cheri_macaroons_shim.hpp"

/******************
 * CLIENT FUNCTIONS
 *****************/

/* Reads the boolean status of bits and sets the array elements
   in the destination to TRUE or FALSE (single bits). */
int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_read_bits_macaroons(ctx, addr, nb, dest);
    } else {
        return modbus_read_bits(ctx, addr, nb, dest);
    }
}

/* Same as modbus_read_bits but reads the remote device input table */
int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_read_input_bits_macaroons(ctx, addr, nb, dest);
    } else {
        return modbus_read_input_bits(ctx, addr, nb, dest);
    }
}

/* Reads the holding registers of remote device and put the data into an
   array */
int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_read_registers_macaroons(ctx, addr, nb, dest);
    } else {
        return modbus_read_registers(ctx, addr, nb, dest);
    }
}

/* Reads the input registers of remote device and put the data into an array */
int modbus_read_input_registers(modbus_t *ctx, int addr, int nb,
                                uint16_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_read_input_registers_macaroons(ctx, addr, nb, dest);
    } else {
        return modbus_read_input_registers(ctx, addr, nb, dest);
    }
}

/* Turns ON or OFF a single bit of the remote device */
int modbus_write_bit(modbus_t *ctx, int addr, int status, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_write_bit_macaroons(ctx, addr, status);
    } else {
        return modbus_write_bit(ctx, addr, status);
    }
}

/* Writes a value in one register of the remote device */
int modbus_write_register(modbus_t *ctx, int addr,
                          const uint16_t value, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_write_register_macaroons(ctx, addr, value);
    } else {
        return modbus_write_register(ctx, addr, value);
    }
}

/* Write the bits of the array in the remote device */
int modbus_write_bits(modbus_t *ctx, int addr, int nb,
                      const uint8_t *src, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_write_bits_macaroons(ctx, addr, nb, src);
    } else {
        return modbus_write_bits(ctx, addr, nb, src);
    }
}

/* Write the values from the array to the registers of the remote device */
int modbus_write_registers(modbus_t *ctx, int addr, int nb,
                           const uint16_t *src, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_write_registers_macaroons(ctx, addr, nb, src);
    } else {
        return modbus_write_registers(ctx, addr, nb, src);
    }
}

/* I'm not actually sure what this does...
   The unit test appears designed to fail */
int modbus_mask_write_register(modbus_t *ctx, int addr, uint16_t and_mask,
                               uint16_t or_mask, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_mask_write_register_macaroons(ctx, addr,
                                                    and_mask, or_mask);
    } else {
        return modbus_mask_write_register(ctx, addr,
                                          and_mask, or_mask);
    }
}

/* Write multiple registers from src array to remote device and read multiple
   registers from remote device to dest array. */
int modbus_write_and_read_registers(modbus_t *ctx,
                                    int write_addr, int write_nb,
                                    const uint16_t *src,
                                    int read_addr, int read_nb,
                                    uint16_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_write_and_read_registers_macaroons(ctx, write_addr, write_nb, src,
                                                         read_addr, read_nb, dest);
    } else {
        return modbus_write_and_read_registers(ctx, write_addr, write_nb, src,
                                               read_addr, read_nb, dest);
    }
}

/* Send a request to get the slave ID of the device (only available in serial
   communication). */
int modbus_report_slave_id(modbus_t *ctx, int max_dest, uint8_t *dest, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_report_slave_id_macaroons(ctx, max_dest, dest);
    } else {
        return modbus_report_slave_id(ctx, max_dest, dest);
    }
}

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
    shim_t shim)
{
    if(shim == CHERI_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_mapping_new_start_address_cheri(
            start_bits, nb_bits,
            start_input_bits, nb_input_bits,
            start_registers, nb_registers,
            start_input_registers, nb_input_registers
        );
    } else {
        return modbus_mapping_new_start_address(
            start_bits, nb_bits,
            start_input_bits, nb_input_bits,
            start_registers, nb_registers,
            start_input_registers, nb_input_registers
        );
    }
}

/**
 * Receive the request from a modbus master
 * TODO:  ATM, this appears to be unnecessary.  Delete?
 * */
int modbus_receive(modbus_t *ctx, uint8_t *req, shim_t shim)
{
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_receive_macaroons(ctx, req);
    } else {
        return modbus_receive(ctx, req);
    }
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
    if(shim == MACAROONS_SHIM || shim == CHERI_MACAROONS_SHIM || shim == CHERI_SHIM_X) {
        return modbus_process_request_macaroons(ctx, req, req_length,
            rsp, rsp_length, mb_mapping);
    } else if(shim == CHERI_SHIM || shim == CHERI_MACAROONS_SHIM) {
        return modbus_process_request_cheri(ctx, req, req_length,
            rsp, rsp_length, mb_mapping, shim);
    } else {
        return modbus_process_request(ctx, req, req_length,
            rsp, rsp_length, mb_mapping);
    }
}