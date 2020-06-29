#include "macaroons_modbus_shim.hpp"

/* to test macaroons */
static std::string key = "a bad secret";
static std::string id = "id for a bad secret";
static std::string location = "https://www.modbus.com/macaroons/";
static std::string expected_signature = "27c9baef16ae041625139857bfca2cebebdcba4ce6637c59ea2693107cf053ce";
static std::string serialised;
static macaroons::Macaroon M;
static macaroons::Verifier V;

/* Turns ON or OFF a single bit of the remote device */
int
modbus_write_bit_macaroons(modbus_t *ctx, int addr, int status)
{
    int rc;

    std::cout << "in modbus_write_bit_macaroons" << std::endl;

    // initialise the Macaroon
    M = macaroons::Macaroon(location, key, id);

    // serialise the Macaroon and send it to the server
    serialised = M.serialize();
    rc = modbus_write_string(ctx, (uint8_t *)serialised.c_str(), (int)serialised.length());

    std::cout << "Macaroon sent." << std::endl;
    if(rc == (int)serialised.length()) {
        std::cout << "Macaroon response received. Calling modbus_write_bit()." << std::endl;
        return modbus_write_bit(ctx, addr, status);
    }

    std::cout << "Macaroon response failed." << std::endl;
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
        std::cout << M.inspect() << std::endl;
        for (std::string caveat : M.all_slices()) {
            std::cout << caveat << std::endl;
        }
        if(V.verify_unsafe(M, key)) {
            std::cout << "Verified!" << std::endl;
            return true;
        } else {
            std::cout << "Verification FAIL!" << std::endl;
        }
    } else {
        std::cout << "Not initalised" << std::endl;
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

    /* get the function from the request */
    rc = modbus_decompose_request(ctx, req, offset, slave_id, function, addr, nb);
    modbus_decompose_request_print(offset, slave_id, function, addr, nb);

    /**
     * If the function is WRITE_STRING then we short circuit a return to the server,
     * extract the Macaroon, reply to the caller, and wait for the following request.
     *
     * Upon receipt of the following request, we attempt to verify the Macaroon, and
     * if verification is successful, we process the request and return to the server.
     *
     * TODO:  The client should be adding the command to the Macaroon as a caveat to
     * protect it's integrity, and it needs to be added to the Verifier.  The command
     * will either come as a subsequent message, or we can just extract it from the
     * Macaroon itself.  Whichever we do needs to be coordinated with the client side
     * of the shim.
     * */
    if(*function == MODBUS_FC_WRITE_STRING) {
        rc = modbus_process_request(ctx, req, req_length, rsp, rsp_length, mb_mapping);
        if (rc == -1) {
            return rc;
        }


        rc = modbus_reply(ctx, rsp, *rsp_length);
        if (rc == -1) {
            return rc;
        }

        do {
            rc = modbus_receive(ctx, req);
            /* Filtered queries return 0 */
        } while (rc == 0);
    }

    if(process_macaroon(mb_mapping->tab_string)) {
        return modbus_process_request(ctx, req, rc, rsp, rsp_length, mb_mapping);
    }

    return -1;
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

