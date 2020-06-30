#include "macaroons_modbus_shim.hpp"

/* to test macaroons */
static std::string key = "a bad secret";
static std::string id = "id for a bad secret";
static std::string location = "https://www.modbus.com/macaroons/";
static std::string expected_signature = "27c9baef16ae041625139857bfca2cebebdcba4ce6637c59ea2693107cf053ce";
static std::string serialised;
static macaroons::Macaroon M;
static macaroons::Verifier V;

bool
send_and_receive_macaroon(modbus_t *ctx, int command_code)
{
    int rc;
    std::string command;

    /**
     * initialise the Macaroon
     *
     * TODO: under normal circumstances, the client would already have the Macaroon
     * */
    M = macaroons::Macaroon(location, key, id);

    /* add the command as a caveat */
    command = std::to_string(1<<command_code);
    M = M.add_first_party_caveat(command);

    /* serialise the Macaroon and send it to the server */
    std::cout << "Sending Macaroon." << std::endl;
    serialised = M.serialize();
    rc = modbus_write_string(ctx, (uint8_t *)serialised.c_str(), (int)serialised.length());

    if(rc == (int)serialised.length()) {
        std::cout << "Macaroon response received." << std::endl;
        return true;
    } else {
        std::cout << "Macaroon response failed." << std::endl;
        return false;
    }
}

/**
 * Shim for modbus_read_bits()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_READ_COILS command
 * 2. Reads the boolean status of bits and sets the array elements
 *    in the destination to TRUE or FALSE (single bits)
 * */
int
modbus_read_bits_macaroons(modbus_t *ctx, int addr, int nb, uint8_t *dest)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_READ_COILS)) {
        std::cout << "Calling modbus_read_bits()." << std::endl;
        return modbus_read_bits(ctx, addr, nb, dest);
    }

    return -1;
}

/**
 * Shim for modbus_read_input_bits()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_READ_DISCRETE_INPUTS command
 * 2. Same as modbus_read_bits but reads the remote device input table
 * */
int
modbus_read_input_bits_macaroons(modbus_t *ctx, int addr, int nb, uint8_t *dest)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_READ_DISCRETE_INPUTS)) {
        std::cout << "Calling modbus_read_input_bits()." << std::endl;
        return modbus_read_input_bits(ctx, addr, nb, dest);
    }

    return -1;
}

/**
 * Shim for modbus_read_registers()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_READ_HOLDING_REGISTERS command
 * 2. Reads the holding registers of remote device and put the data into an
 *    array
 * */
int
modbus_read_registers_macaroons(modbus_t *ctx, int addr, int nb, uint16_t *dest)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_READ_HOLDING_REGISTERS)) {
        std::cout << "Calling modbus_read_registers()." << std::endl;
        return modbus_read_registers(ctx, addr, nb, dest);
    }

    return -1;
}

/**
 * Shim for modbus_read_input_registers()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_READ_INPUT_REGISTERS command
 * 2. Reads the holding registers of remote device and put the data into an
 *    array
 * */
int
modbus_read_input_registers_macaroons(modbus_t *ctx, int addr, int nb, uint16_t *dest)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_READ_INPUT_REGISTERS)) {
        std::cout << "Calling modbus_read_input_registers()." << std::endl;
        return modbus_read_input_registers(ctx, addr, nb, dest);
    }

    return -1;
}

/**
 * Shim for modbus_write_bit()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_WRITE_SINGLE_COIL command
 * 2. Turns ON or OFF a single bit of the remote device
 * */
int
modbus_write_bit_macaroons(modbus_t *ctx, int addr, int status)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_WRITE_SINGLE_COIL)) {
        std::cout << "Calling modbus_write_bit()." << std::endl;
        return modbus_write_bit(ctx, addr, status);
    }

    return -1;
}

/**
 * Shim for modbus_write_register()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_WRITE_SINGLE_REGISTER command
 * 2. Writes a value in one register of the remote device
 * */
int
modbus_write_register_macaroons(modbus_t *ctx, int addr, const uint16_t value)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_WRITE_SINGLE_REGISTER)) {
        std::cout << "Calling modbus_write_register()." << std::endl;
        return modbus_write_register(ctx, addr, value);
    }

    return -1;
}

/**
 * Shim for modbus_write_bits()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_WRITE_SINGLE_COIL command
 * 2. Write the bits of the array in the remote device
 * */
int
modbus_write_bits_macaroons(modbus_t *ctx, int addr, int nb, const uint8_t *src)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_WRITE_MULTIPLE_COILS)) {
        std::cout << "Calling modbus_write_bits()." << std::endl;
        return modbus_write_bits(ctx, addr, nb, src);
    }

    return -1;
}

/**
 * Shim for modbus_write_registers()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_WRITE_MULTIPLE_REGISTERS command
 * 2. Write the values from the array to the registers of the remote device
 * */
int
modbus_write_registers_macaroons(modbus_t *ctx, int addr, int nb, const uint16_t *data)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_WRITE_MULTIPLE_REGISTERS)) {
        std::cout << "Calling modbus_write_registers()." << std::endl;
        return modbus_write_registers(ctx, addr, nb, data);
    }

    return -1;
}

/**
 * Shim for modbus_mask_write_register()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_MASK_WRITE_REGISTER command
 * 2. I'm not actually sure what this does...
 *    The unit test appears designed to fail
 * */
int
modbus_mask_write_register_macaroons(modbus_t *ctx, int addr,
                                     uint16_t and_mask, uint16_t or_mask)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_MASK_WRITE_REGISTER)) {
        std::cout << "Calling modbus_mask_write_register()." << std::endl;
        return modbus_mask_write_register(ctx, addr, and_mask, or_mask);
    }

    return -1;
}

/**
 * Shim for modbus_write_and_read_registers()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_WRITE_AND_READ_REGISTERS command
 * 2. Write multiple registers from src array to remote device and read multiple
 *    registers from remote device to dest array
 * */
int
modbus_write_and_read_registers_macaroons(modbus_t *ctx, int write_addr,
                                          int write_nb, const uint16_t *src,
                                          int read_addr, int read_nb,
                                          uint16_t *dest)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_WRITE_AND_READ_REGISTERS)) {
        std::cout << "Calling modbus_write_and_read_registers()." << std::endl;
        return modbus_write_and_read_registers(ctx, write_addr, write_nb, src,
                                               read_addr, read_nb, dest);
    }

    return -1;
}

/**
 * Shim for modbus_report_slave_id()
 *
 * 1. Sends a Macaroon with the MODBUS_FC_REPORT_SLAVE_ID command
 * 2. Send a request to get the slave ID of the device (only available in
 *    serial communication)
 * */
int
modbus_report_slave_id_macaroons(modbus_t *ctx, int max_dest,
                                           uint8_t *dest)
{
    std::string command;

    std::cout << "in " << __FUNCTION__ << "()" << std::endl;

    if(send_and_receive_macaroon(ctx, MODBUS_FC_REPORT_SLAVE_ID)) {
        std::cout << "Calling modbus_report_slave_id()." << std::endl;
        return modbus_report_slave_id(ctx, max_dest, dest);
    }

    return -1;
}

/* Receive the request from a modbus master */
int
modbus_receive_macaroons(modbus_t *ctx, uint8_t *req)
{
    return modbus_receive(ctx, req);
}

bool
process_macaroon(uint8_t *tab_string)
{
    serialised = std::string((char *)tab_string);

    // try to deserialise the string into a Macaroon
    try {
        M = macaroons::Macaroon::deserialize(serialised);
    } catch(macaroons::exception::Invalid &e) {
        std::cout << e.what() << std::endl;
    }

    if(M.is_initialized()){
        if(V.verify_unsafe(M, key)) {
            std::cout << "VERIFICATION: Pass" << std::endl;
            return true;
        } else {
            std::cout << "VERIFICATION: Fail" << std::endl;
        }
    } else {
        std::cout << "MACAROON: Not initialised" << std::endl;
    }

    return false;
}

void
modbus_decompose_request_print(int *offset, int *slave_id, int *function, uint16_t *addr, int *nb)
{
    std::cout << "offset:\t\t" << *offset << std::endl;
    std::cout << "slave_id:\t" << *slave_id << std::endl;
    std::cout << std::showbase // show the 0x prefix
              << std::internal // fill between the prefix and the number
              << std::setfill('0') // fill with 0s
              << std::hex;
    std::cout << "function:\t" << *function << std::endl;
    std::cout << "addr:\t\t" << *addr << std::endl;
    std::cout << "nb:\t\t" << *nb << std::endl;
}

/**
 * Analyses the request and constructs a response.
 *
 * If an error occurs, this function construct the response
 * accordingly.
 * */
int
modbus_process_request_macaroons(modbus_t *ctx, uint8_t *req,
                                 int req_length, uint8_t *rsp, int *rsp_length,
                                 modbus_mapping_t *mb_mapping)
{
    int rc;
    int *offset = (int *)malloc(sizeof(int));
    int *slave_id = (int *)malloc(sizeof(int));
    int *function = (int *)malloc(sizeof(int));
    uint16_t *addr = (uint16_t *)malloc(sizeof(uint16_t));
    int *nb = (int *)malloc(sizeof(int));

    std::string command;

    /* get the function from the request */
    modbus_decompose_request(ctx, req, offset, slave_id, function, addr, nb);
    // modbus_decompose_request_print(offset, slave_id, function, addr, nb);

    /**
     * If the function is WRITE_STRING then we short circuit a return to the server,
     * extract the Macaroon, reply to the caller, and wait for the following request.
     *
     * Upon receipt of the following request, we attempt to verify the Macaroon, and
     * if verification is successful, we process the request and return to the server.
     * */
    while(*function == MODBUS_FC_WRITE_STRING) {
        // zero out the state variable where the Macaroon string is stored
        memset(mb_mapping->tab_string, 0, MODBUS_MAX_STRING_LENGTH * sizeof(uint8_t));

        rc = modbus_process_request(ctx, req, req_length, rsp, rsp_length, mb_mapping);
        if (rc == -1) {
            return rc;
        }

        std::cout << mb_mapping->tab_string << std::endl;

        rc = modbus_reply(ctx, rsp, *rsp_length);
        if (rc == -1) {
            return rc;
        }

        do {
            rc = modbus_receive(ctx, req);
            /* Filtered queries return 0 */
        } while (rc == 0);

        /* get the function from the request */
        modbus_decompose_request(ctx, req, offset, slave_id, function, addr, nb);
        // modbus_decompose_request_print(offset, slave_id, function, addr, nb);
    }

    // add the command in the subsequent request to the temporary Verifier
    command = std::to_string(1<<*function);
    V.satisfy_exact(command);

    // Extract the Macaroon.  If verification is successful, process the request.
    if(process_macaroon(mb_mapping->tab_string)) {
        std::cout << "Processing request:" << std::endl;
        modbus_decompose_request_print(offset, slave_id, function, addr, nb);
        rc = modbus_process_request(ctx, req, rc, rsp, rsp_length, mb_mapping);
    } else {
        rc = -1;
    }

    // clear the Macaroon, Verifier, and tab_string
    M = macaroons::Macaroon();
    V = macaroons::Verifier();
    memset(mb_mapping->tab_string, 0, MODBUS_MAX_STRING_LENGTH * sizeof(uint8_t));

    return rc;
}

/*
DANGER:  This came from GitHub...
https://github.com/InversePalindrome/Blog/tree/master/RandomString
*/
std::string
generate_key(std::size_t length)
{
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string random_string;

    for (std::size_t i = 0; i < length; ++i)
    {
        random_string += characters[distribution(generator)];
    }

    return random_string;
}

