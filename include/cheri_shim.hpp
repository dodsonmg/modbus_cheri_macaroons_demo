#ifndef _CHERI_SHIM_
#define _CHERI_SHIM_

#include <iostream>

/* For CHERI */
#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if !__has_feature(capabilities)
#error "<cheri_shim.h> should only be included when CHERI is supported"
#endif

#include "cheri_helper.h"

/* for modbus */
extern "C" {
    #include <modbus/modbus.h>
}


modbus_mapping_t* modbus_mapping_new_start_address_cap(
    unsigned int start_bits, unsigned int nb_bits,
    unsigned int start_input_bits, unsigned int nb_input_bits,
    unsigned int start_registers, unsigned int nb_registers,
    unsigned int start_input_registers, unsigned int nb_input_registers);

int modbus_process_request_cap(modbus_t *ctx, const uint8_t *req,
                               int req_length, uint8_t *rsp, int *rsp_length,
                               modbus_mapping_t *mb_mapping);

#endif /* _CHERI_SHIM_ */
