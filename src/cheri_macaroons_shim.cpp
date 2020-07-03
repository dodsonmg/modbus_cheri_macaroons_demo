#include "cheri_macaroons_shim.hpp"
#include "macaroons_shim.hpp"
#include "cheri_shim.hpp"

/******************
 * HELPER FUNCTIONS
 *****************/

void
print_shim_info(std::string file, std::string function)
{
    int length = file.length() + function.length() + 3;
    std::cout << display_marker << std::endl;
    std::cout << file << ":" << function << "()" << std::endl;
    std::cout << std::string(length, '-') << std::endl;
}

/**
 * Print the name of a requested function
 * */
void
print_modbus_function_name(int function)
{
    std::cout << std::showbase // show the 0x prefix
            << std::internal // fill between the prefix and the number
            << std::setfill('0') // fill with 0s
            << std::hex;
    std::cout << ">\tfunction: " << function << " (" <<
        get_modbus_function_name(function) << ")" << std::endl;
}

std::string
get_modbus_function_name(int function)
{
    switch(function) {
        case MODBUS_FC_READ_COILS:
            return "MODBUS_FC_READ_COILS";
        case MODBUS_FC_READ_DISCRETE_INPUTS:
            return "MODBUS_FC_READ_DISCRETE_INPUTS";
        case MODBUS_FC_READ_HOLDING_REGISTERS:
            return "MODBUS_FC_READ_HOLDING_REGISTERS";
        case MODBUS_FC_READ_INPUT_REGISTERS:
            return "MODBUS_FC_READ_INPUT_REGISTERS";
        case MODBUS_FC_WRITE_SINGLE_COIL:
            return "MODBUS_FC_WRITE_SINGLE_COIL";
        case MODBUS_FC_WRITE_SINGLE_REGISTER:
            return "MODBUS_FC_WRITE_SINGLE_REGISTER";
        case MODBUS_FC_WRITE_MULTIPLE_COILS:
            return "MODBUS_FC_WRITE_MULTIPLE_COILS";
        case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            return "MODBUS_FC_WRITE_MULTIPLE_REGISTERS";
        case MODBUS_FC_REPORT_SLAVE_ID:
            return "MODBUS_FC_REPORT_SLAVE_ID";
        case MODBUS_FC_READ_EXCEPTION_STATUS:
            return "MODBUS_FC_READ_EXCEPTION_STATUS";
        case MODBUS_FC_MASK_WRITE_REGISTER:
            return "MODBUS_FC_MASK_WRITE_REGISTER";
        case MODBUS_FC_WRITE_AND_READ_REGISTERS:
            return "MODBUS_FC_WRITE_AND_READ_REGISTERS";
        case MODBUS_FC_WRITE_STRING:
            return "MODBUS_FC_WRITE_STRING";
        default:
            return "ILLEGAL FUNCTION";
    }
}

/* Print CHERI capability details of mb_mapping pointers */
void
print_mb_mapping(modbus_mapping_t* mb_mapping)
{
    printf("mb_mapping:\t\t%#p\n", (void *)mb_mapping);
    printf("->tab_bits:\t\t%#p\n", (void *)mb_mapping->tab_bits);
    printf("->tab_input_bits:\t%#p\n", (void *)mb_mapping->tab_input_bits);
    printf("->tab_input_registers:\t%#p\n", (void *)mb_mapping->tab_input_registers);
    printf("->tab_registers:\t%#p\n", (void *)mb_mapping->tab_registers);
    printf("->tab_string:\t\t%#p\n", (void *)mb_mapping->tab_string);
}

/* Helper function to print the elements of a request. */
void
print_modbus_decompose_request(modbus_t *ctx, const uint8_t *req, int *offset,
                               int *slave_id, int *function, uint16_t *addr, int *nb,
                               uint16_t *addr_wr, int *nb_wr)
{
    modbus_decompose_request(ctx, req, offset, slave_id, function, addr, nb, addr_wr, nb_wr);

    std::cout << std::showbase // show the 0x prefix
              << std::internal // fill between the prefix and the number
              << std::setfill('0') // fill with 0s
              << std::hex;
    std::cout << "decompose request:" << std::endl;
    std::cout << "> " << "offset:\t\t" << *offset << std::endl;
    std::cout << "> " << "slave_id:\t\t" << *slave_id << std::endl;
    std::cout << "> " << "function:\t\t" << *function << " (" <<
        get_modbus_function_name(*function) << ")" << std::endl;
    std::cout << "> " << "addr:\t\t\t" << *addr << std::endl;
    std::cout << "> " << "nb:\t\t\t" << *nb << std::endl;
    std::cout << "> " << "addr_wr:\t\t" << *addr_wr << std::endl;
    std::cout << "> " << "nb_wr:\t\t" << *nb_wr << std::endl;
}

/******************
 * CLIENT FUNCTIONS
 *****************/

/* Reads the boolean status of bits and sets the array elements
   in the destination to TRUE or FALSE (single bits). */
int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_read_bits_macaroons(ctx, addr, nb, dest);
    } else {
        return modbus_read_bits(ctx, addr, nb, dest);
    }
}

/* Same as modbus_read_bits but reads the remote device input table */
int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_read_input_bits_macaroons(ctx, addr, nb, dest);
    } else {
        return modbus_read_input_bits(ctx, addr, nb, dest);
    }
}

/* Reads the holding registers of remote device and put the data into an
   array */
int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_read_registers_macaroons(ctx, addr, nb, dest);
    } else {
        return modbus_read_registers(ctx, addr, nb, dest);
    }
}

/* Reads the input registers of remote device and put the data into an array */
int modbus_read_input_registers(modbus_t *ctx, int addr, int nb,
                                uint16_t *dest, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_read_input_registers_macaroons(ctx, addr, nb, dest);
    } else {
        return modbus_read_input_registers(ctx, addr, nb, dest);
    }
}

/* Turns ON or OFF a single bit of the remote device */
int modbus_write_bit(modbus_t *ctx, int addr, int status, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_write_bit_macaroons(ctx, addr, status);
    } else {
        return modbus_write_bit(ctx, addr, status);
    }
}

/* Writes a value in one register of the remote device */
int modbus_write_register(modbus_t *ctx, int addr,
                          const uint16_t value, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_write_register_macaroons(ctx, addr, value);
    } else {
        return modbus_write_register(ctx, addr, value);
    }
}

/* Write the bits of the array in the remote device */
int modbus_write_bits(modbus_t *ctx, int addr, int nb,
                      const uint8_t *src, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_write_bits_macaroons(ctx, addr, nb, src);
    } else {
        return modbus_write_bits(ctx, addr, nb, src);
    }
}

/* Write the values from the array to the registers of the remote device */
int modbus_write_registers(modbus_t *ctx, int addr, int nb,
                           const uint16_t *src, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_write_registers_macaroons(ctx, addr, nb, src);
    } else {
        return modbus_write_registers(ctx, addr, nb, src);
    }
}

/* I'm not actually sure what this does...
   The unit test appears designed to fail */
int modbus_mask_write_register(modbus_t *ctx, int addr, uint16_t and_mask,
                               uint16_t or_mask, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
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
                                    uint16_t *dest, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_write_and_read_registers_macaroons(ctx, write_addr, write_nb, src,
                                                         read_addr, read_nb, dest);
    } else {
        return modbus_write_and_read_registers(ctx, write_addr, write_nb, src,
                                               read_addr, read_nb, dest);
    }
}

/* Send a request to get the slave ID of the device (only available in serial
   communication). */
int modbus_report_slave_id(modbus_t *ctx, int max_dest, uint8_t *dest, shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
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
    shim_t shim_type)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    /* If CHERI_X, then call the cheri_shim to restrict mb_mapping */
    if(shim_type == CHERI || shim_type == CHERI_MACAROONS) {
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
int modbus_receive(modbus_t *ctx, uint8_t *req, shim_t shim_type)
{
    print_shim_info("macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == MACAROONS || shim_type == CHERI_MACAROONS) {
        return modbus_receive_macaroons(ctx, req);
    } else {
        return modbus_receive(ctx, req);
    }
}

/**
 * Analyses the request and constructs a response.
 *
 * This is the most important function in the shim, and the ordering is important
 *
 * If shim_type == NONE, then this simply calls libmodbus:modbus_process_request
 *
 * If shim_type == CHERI, this will call the function in cheri_shim, which
 * restricts permissions on mb_mapping, then calls this function again.  This
 * function will call libmodbus:modbus_process_request and then
 * return to cheri_shim to restore permissions on mb_mapping (and then return here...)
 *
 * If shim_type == MACAROONS, this will call the function in macaroon_shim, which
 * 1. For WRITE_STRING:  Reset tab_string and call this function again with state MODBUS_X
 * 2. For all other functions:  Verify the Macaroon (in tab_string) and
 *    call this function again with state MODBUS_X
 *
 * If shim_type == CHERI_MACAROON_SHIM, this will:
 * 1. call the function in macaroons_shim to either reset tab_string or
 *    verify the Macaroon (state -> MACAROON_X)
 * 2. call the function in cheri_shim to restrict permissions on mb_mapping (state -> CHERI_X)
 * 3. call libmodbus:modbus_process_request
 *
 * TODO:  Modify macaroons_shim to read any restrictions in the Macaroon and modify
 * the request before eventually sending it to cheri_shim
 *
 * TODO:  Modify cheri_shim to restrict mb_mapping based on the function, address, and nb in the request
 * */
int modbus_process_request(modbus_t *ctx, uint8_t *req,
                           int req_length, uint8_t *rsp, int *rsp_length,
                           modbus_mapping_t *mb_mapping,
                           shim_t shim_type, shim_s shim_state)
{
    print_shim_info("cheri_macaroons_shim", std::string(__FUNCTION__));

    if(shim_type == CHERI && shim_state == INIT) {
        return modbus_process_request_cheri(ctx, req, req_length,
            rsp, rsp_length, mb_mapping, shim_type, shim_state);
    } else if(shim_type == MACAROONS && shim_state == INIT) {
        return modbus_process_request_macaroons(ctx, req, req_length,
            rsp, rsp_length, mb_mapping, shim_type, shim_state);
    } else if(shim_type == CHERI_MACAROONS && shim_state == INIT) {
        return modbus_process_request_macaroons(ctx, req, req_length,
            rsp, rsp_length, mb_mapping, shim_type, shim_state);
    } else if(shim_type == CHERI_MACAROONS && shim_state == MACAROONS_X) {
        return modbus_process_request_cheri(ctx, req, req_length,
            rsp, rsp_length, mb_mapping, shim_type, shim_state);
    } else if( (shim_type == CHERI && shim_state == CHERI_X) ||
               (shim_type == MACAROONS && shim_state == MACAROONS_X) ||
               (shim_type == CHERI_MACAROONS && shim_state == CHERI_X) ||
               (shim_type == NONE) ) {
        std::cout << "> " << "calling modbus_process_request()" << std::endl;
        std::cout << display_marker << std::endl;
        return modbus_process_request(ctx, req, req_length,
            rsp, rsp_length, mb_mapping);
    } else {
        return -1;
    }
}