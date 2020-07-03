/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#ifdef _WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif

/* For MinGW */
#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
#endif

extern "C" {
    #include <modbus/modbus.h>
    #include "unit-test.h"
}

/* CHERI Macaroons */
#include "cheri_macaroons_shim.hpp"

enum {
    TCP,
    TCP_PI,
    RTU
};

int main(int argc, char *argv[])
{
    int s = -1;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    int rc;
    int i;
    uint8_t *query = NULL;
    uint8_t *rsp = NULL;
    int rsp_length = 0;
    int header_length;

    shim_t shim_type;
    shim_s shim_state;

    /* identify the shim type.  default = CHERI_MACAROONS. */
    if (argc > 1) {
        if (strcmp(argv[1], "NONE") == 0) {
            shim_type = NONE;
        } else if (strcmp(argv[1], "CHERI") == 0) {
            shim_type = CHERI;
        } else if (strcmp(argv[1], "MACAROONS") == 0) {
            shim_type = MACAROONS;
        } else if (strcmp(argv[1], "CHERI_MACAROONS") == 0) {
            shim_type = CHERI_MACAROONS;
        } else {
            std::cout << "usage: cheri_macaroons_server [NONE|CHERI|MACAROONS|CHERI_MACAROONS]" << std::endl;
            return -1;
        }
    } else {
        shim_type = CHERI_MACAROONS;
    }

    ctx = modbus_new_tcp("127.0.0.1", 1502);
    query = (uint8_t *)malloc(MODBUS_MAX_STRING_LENGTH * sizeof(uint8_t));
    rsp = (uint8_t *)malloc(MODBUS_TCP_MAX_ADU_LENGTH * sizeof(uint8_t));

    header_length = modbus_get_header_length(ctx);

    modbus_set_debug(ctx, TRUE);

    mb_mapping = modbus_mapping_new_start_address(
        UT_BITS_ADDRESS, UT_BITS_NB,
        UT_INPUT_BITS_ADDRESS, UT_INPUT_BITS_NB,
        UT_REGISTERS_ADDRESS, UT_REGISTERS_NB_MAX,
        UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB,
        shim_type);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    /* Examples from PI_MODBUS_300.pdf.
       Only the read-only input values are assigned. */

    /* Initialize input values that's can be only done server side. */
    modbus_set_bits_from_bytes(mb_mapping->tab_input_bits, 0, UT_INPUT_BITS_NB,
                               UT_INPUT_BITS_TAB);

    /* Initialize values of INPUT REGISTERS */
    for (i=0; i < UT_INPUT_REGISTERS_NB; i++) {
        mb_mapping->tab_input_registers[i] = UT_INPUT_REGISTERS_TAB[i];;
    }

    s = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &s);

    for (;;) {
        do {
            rc = modbus_receive(ctx, query);
            /* Filtered queries return 0 */
        } while (rc == 0);

        /* The connection is not closed on errors which require on reply such as
           bad CRC in RTU. */
        if (rc == -1 && errno != EMBBADCRC) {
            /* Quit */
            break;
        }

        shim_state = INIT;
        rc = modbus_process_request(ctx, query, rc, rsp, &rsp_length, mb_mapping,
                                    shim_type, shim_state);
        if (rc == -1) {
            break;
        }

        rc = modbus_reply(ctx, rsp, rsp_length);
        if (rc == -1) {
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    if (s != -1) {
        close(s);
    }

    modbus_mapping_free(mb_mapping);
    free(query);

    return 0;
}
