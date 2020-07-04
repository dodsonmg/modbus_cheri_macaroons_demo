#include "cheri_shim.hpp"

/******************
 * HELPER FUNCTIONS
 *****************/

/**
 * Shim function for libmodbus:modbus_mapping_new_start_address
 *
 * modbus_mapping_new_start_address allocates memory and establishes
 * the initial server state wrt coils and registers
 *
 * This shim reduces the permissions on the pointer to structure s.t.
 * pointer structure members can only load and store data, and the
 * pointer to the structure itself can only load and store the data
 * and pointers within the structure
 *
 * TODO:  Consider copying everything returned from libmodbus to a new
 * pointer and returning that to the caller to libmodbus cannot retain
 * a fully-privileged pointer to the structure
 * */
modbus_mapping_t* modbus_mapping_new_start_address_cheri(
    unsigned int start_bits, unsigned int nb_bits,
    unsigned int start_input_bits, unsigned int nb_input_bits,
    unsigned int start_registers, unsigned int nb_registers,
    unsigned int start_input_registers, unsigned int nb_input_registers)
{
    print_shim_info("cheri_shim", std::string(__FUNCTION__));

    modbus_mapping_t* mb_mapping;
    mb_mapping = modbus_mapping_new_start_address(
        start_bits, nb_bits,
        start_input_bits, nb_input_bits,
        start_registers, nb_registers,
        start_input_registers, nb_input_registers);

    // may need to be able to read and write to coils
    mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, CHERI_PERM_LOAD | CHERI_PERM_STORE);

    // generally, only need to read discrete inputs; however, need to write to initialise
    // TODO: remove CHERI_PERM_STORE after initialisation??
    mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, CHERI_PERM_LOAD | CHERI_PERM_STORE);

    // generally, only need to read input registers; however, need to write to initialise
    // TODO: remove CHERI_PERM_STORE after initialisation??
    mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, CHERI_PERM_LOAD | CHERI_PERM_STORE);

    // may need to read and write to holding registers
    mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, CHERI_PERM_LOAD | CHERI_PERM_STORE);

    // may need to read and write to the string (used for Macaroons)
    mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD | CHERI_PERM_STORE);

    print_mb_mapping(mb_mapping);

    std::cout << display_marker << std::endl;

    return mb_mapping;
}

/**
 * Shim function for libmodbus:modbus_process_request
 *
 * modbus_process_request processes a client request, modifies the server
 * state (if applicable) and sends a response to the client
 * */
int modbus_process_request_cheri(modbus_t *ctx, uint8_t *req,
                                int req_length, uint8_t *rsp, int *rsp_length,
                                modbus_mapping_t *mb_mapping,
                                shim_t shim_type, shim_s shim_state)
{
    int rc;
    uint8_t *tab_bits_;
    uint8_t *tab_input_bits_;
    uint16_t *tab_input_registers_;
    uint16_t *tab_registers_;
    uint8_t *tab_string_;
    modbus_mapping_t *mb_mapping_;

    int *offset = (int *)malloc(sizeof(int));
    int *slave_id = (int *)malloc(sizeof(int));
    int *function = (int *)malloc(sizeof(int));
    uint16_t *addr = (uint16_t *)malloc(sizeof(uint16_t));
    int *nb = (int *)malloc(sizeof(int));
    uint16_t *addr_wr = (uint16_t *)malloc(sizeof(uint16_t));
    int *nb_wr = (int *)malloc(sizeof(int));

    print_shim_info("cheri_shim", std::string(__FUNCTION__));

    modbus_decompose_request(ctx, req, offset, slave_id, function, addr, nb, addr_wr, nb_wr);

    /**
     * Create copies of pointers to mb_mapping and members for restoration
     * after calling libmodbus:modbus_process_request.
     *
     * This allows reducing permissions to the structure and members before sending
     * them to libmodbus:modbus_process_request.
     *
     * TODO:  This should probably only be done once upon initialisation and the calling
     * application should only have the structure with reduced permissions.
     * */
    mb_mapping_ = mb_mapping;
    tab_bits_ = mb_mapping->tab_bits;
    tab_input_bits_ = mb_mapping->tab_input_bits;
    tab_input_registers_ = mb_mapping->tab_input_registers;
    tab_registers_ = mb_mapping->tab_registers;
    tab_string_ = mb_mapping->tab_string;

    /* reduce mb_mapping capabilities based on the function in the request */
    switch(*function) {
        case MODBUS_FC_READ_COILS:
            /* we only need to be able to read coil (tab_bits) values */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, CHERI_PERM_LOAD);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* we need to read the serialised Macaroon (tab_string) */
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_READ_DISCRETE_INPUTS:
            /* we only need to be able to read discrete inputs (tab_input_bits) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, CHERI_PERM_LOAD);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* we need to read the serialised Macaroon (tab_string) */
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_READ_HOLDING_REGISTERS:
            /* we only need to be able to read holding registers (tab_registers) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, CHERI_PERM_LOAD);

            /* we need to read the serialised Macaroon (tab_string) */
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_READ_INPUT_REGISTERS:
            /* we only need to be able to read input registers (tab_input_registers) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, CHERI_PERM_LOAD);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* we need to read the serialised Macaroon (tab_string) */
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_WRITE_SINGLE_COIL:
        case MODBUS_FC_WRITE_MULTIPLE_COILS:
            /* we only need to be able to store coil (tab_bits) values */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, CHERI_PERM_STORE);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* we need to read the serialised Macaroon (tab_string) */
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_WRITE_SINGLE_REGISTER:
        case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            /* we only need to be able to write holding registers (tab_registers) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, CHERI_PERM_STORE);

            /* we need to read the serialised Macaroon (tab_string) */
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_REPORT_SLAVE_ID:
            /* we shouldn't need to read or write coils or registers */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* we need to read the serialised Macaroon (tab_string) */
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_MASK_WRITE_REGISTER:
        case MODBUS_FC_WRITE_AND_READ_REGISTERS:
            /* we only need to be able to read and write holding registers (tab_registers) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, CHERI_PERM_LOAD | CHERI_PERM_STORE);

            /* we need to read the serialised Macaroon (tab_string) */
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_WRITE_STRING:
            /* we only need to be able to read and write the string (tab_string) used for Macaroons */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD | CHERI_PERM_STORE);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_READ_STRING:
            /* we only need to be able to read and write the string (tab_string) used for Macaroons */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        default:
            /* we shouldn't need to read or write coils or registers */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);
            mb_mapping->tab_string = (uint8_t *)cheri_perms_and(mb_mapping->tab_string, 0);

            /* structure pointer shouldn't need to be referenced at all */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, 0);
    }

    /**
     * Print the decomposed request and the resulting mb_mapping pointers
     * */
    print_modbus_decompose_request(ctx, req, offset, slave_id, function, addr, nb, addr_wr, nb_wr);
    std::cout << std::endl;
    print_mb_mapping(mb_mapping);

    /**
     * Set state to CHERI_X (completed work within cheri_shim)
     * Return to cheri_macaroons_shim to call libmodbus:modbus_process_request()
     * */
    shim_state = CHERI_X;
    rc = modbus_process_request(ctx, req, req_length, rsp, rsp_length, mb_mapping,
                                shim_type, shim_state);

    /* restore permissions of mb_mapping and member capabilities */
    mb_mapping = mb_mapping_;
    mb_mapping->tab_bits = tab_bits_;
    mb_mapping->tab_input_bits = tab_input_bits_;
    mb_mapping->tab_input_registers = tab_input_registers_;
    mb_mapping->tab_registers = tab_registers_;
    mb_mapping->tab_string = tab_string_;

    return rc;
}
