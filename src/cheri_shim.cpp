#include "cheri_shim.hpp"

/**
 * Small helper to print the name of a requested function
 * */
void modbus_print_function(int function)
{
    switch(function) {
        case MODBUS_FC_READ_COILS:
            printf("MODBUS_FC_READ_COILS\n");
            break;
        case MODBUS_FC_READ_DISCRETE_INPUTS:
            printf("MODBUS_FC_READ_DISCRETE_INPUTS\n");
            break;
        case MODBUS_FC_READ_HOLDING_REGISTERS:
            printf("MODBUS_FC_READ_HOLDING_REGISTERS\n");
            break;
        case MODBUS_FC_READ_INPUT_REGISTERS:
            printf("MODBUS_FC_READ_INPUT_REGISTERS\n");
            break;
        case MODBUS_FC_WRITE_SINGLE_COIL:
            printf("MODBUS_FC_WRITE_SINGLE_COIL\n");
            break;
        case MODBUS_FC_WRITE_SINGLE_REGISTER:
            printf("MODBUS_FC_WRITE_SINGLE_REGISTER\n");
            break;
        case MODBUS_FC_WRITE_MULTIPLE_COILS:
            printf("MODBUS_FC_WRITE_MULTIPLE_COILS\n");
            break;
        case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            printf("MODBUS_FC_WRITE_MULTIPLE_REGISTERS\n");
            break;
        case MODBUS_FC_REPORT_SLAVE_ID:
            printf("MODBUS_FC_REPORT_SLAVE_ID\n");
            break;
        case MODBUS_FC_READ_EXCEPTION_STATUS:
            printf("MODBUS_FC_READ_EXCEPTION_STATUS\n");
            break;
        case MODBUS_FC_MASK_WRITE_REGISTER:
            printf("MODBUS_FC_MASK_WRITE_REGISTER\n");
            break;
        case MODBUS_FC_WRITE_AND_READ_REGISTERS:
            printf("MODBUS_FC_WRITE_AND_READ_REGISTERS\n");
            break;
        case MODBUS_FC_WRITE_STRING:
            printf("MODBUS_FC_WRITE_STRING\n");
            break;
        default:
            printf("ILLEGAL FUNCTION\n");
    }
}

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
modbus_mapping_t* modbus_mapping_new_start_address_cap(
    unsigned int start_bits, unsigned int nb_bits,
    unsigned int start_input_bits, unsigned int nb_input_bits,
    unsigned int start_registers, unsigned int nb_registers,
    unsigned int start_input_registers, unsigned int nb_input_registers)
{
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

    return mb_mapping;
}

/**
 * Shim function for libmodbus:modbus_reply
 *
 * modbus_reply processes a client request, modifies the server
 * state (if applicable) and sends a response to the client
 *
 * This shim [DOES WHAT?]
 * */
int modbus_process_request_cap(modbus_t *ctx, const uint8_t *req,
                              int req_length, uint8_t *rsp, int *rsp_length,
                              modbus_mapping_t *mb_mapping)
{
    int rc;
    int function;
    uint8_t *tab_bits_;
    uint8_t *tab_input_bits_;
    uint16_t *tab_input_registers_;
    uint16_t *tab_registers_;
    modbus_mapping_t *mb_mapping_;

    function = modbus_get_function(ctx, req);
    modbus_print_function(function);

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

    /* reduce mb_mapping capabilities based on the function in the request */
    switch(function) {
        case MODBUS_FC_READ_COILS:
            /* we only need to be able to read coil (tab_bits) values */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, CHERI_PERM_LOAD);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_READ_DISCRETE_INPUTS:
            /* we only need to be able to read discrete inputs (tab_input_bits) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, CHERI_PERM_LOAD);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_READ_HOLDING_REGISTERS:
            /* we only need to be able to read holding registers (tab_registers) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, CHERI_PERM_LOAD);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_READ_INPUT_REGISTERS:
            /* we only need to be able to read input registers (tab_input_registers) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, CHERI_PERM_LOAD);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

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

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        case MODBUS_FC_REPORT_SLAVE_ID:
        case MODBUS_FC_READ_EXCEPTION_STATUS:
            /* we shouldn't need to read or write coils or registers */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* structure pointer shouldn't need to be referenced at all */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, 0);
            break;

        case MODBUS_FC_MASK_WRITE_REGISTER:
        case MODBUS_FC_WRITE_AND_READ_REGISTERS:
            /* we only need to be able to read and write holding registers (tab_registers) */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, CHERI_PERM_LOAD | CHERI_PERM_STORE);

            /* structure pointer should now only need to load values and capabilities */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, CHERI_PERM_LOAD | CHERI_PERM_LOAD_CAP);
            break;

        default:
            /* we shouldn't need to read or write coils or registers */
            mb_mapping->tab_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_bits, 0);
            mb_mapping->tab_input_bits = (uint8_t *)cheri_perms_and(mb_mapping->tab_input_bits, 0);
            mb_mapping->tab_input_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_input_registers, 0);
            mb_mapping->tab_registers = (uint16_t *)cheri_perms_and(mb_mapping->tab_registers, 0);

            /* structure pointer shouldn't need to be referenced at all */
            mb_mapping = (modbus_mapping_t *)cheri_perms_and(mb_mapping, 0);
    }

    rc = modbus_process_request(ctx, req, req_length, rsp, rsp_length, mb_mapping);

    /* restore permissions of mb_mapping and member capabilities */
    mb_mapping = mb_mapping_;
    mb_mapping->tab_bits = tab_bits_;
    mb_mapping->tab_input_bits = tab_input_bits_;
    mb_mapping->tab_input_registers = tab_input_registers_;
    mb_mapping->tab_registers = tab_registers_;

    return rc;
}
