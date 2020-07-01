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

int main(int argc, char*argv[])
{
    int s = -1;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    int rc;
    int i;
    int use_backend;
    uint8_t *query = NULL;
    uint8_t *rsp = NULL;
    int rsp_length = 0;
    int header_length;

    shim_t shim = CHERI_MACAROONS_SHIM;

    /* RTU configuration doesn't support Macaroons, so we drop it */
    if (argc > 1) {
        if (strcmp(argv[1], "tcp") == 0) {
            use_backend = TCP;
        } else if (strcmp(argv[1], "tcppi") == 0) {
            use_backend = TCP_PI;
        } else if (strcmp(argv[1], "rtu") == 0) {
            // use_backend = RTU;
            printf("RTU configuration is not supported\n");
            return -1;
        } else {
            printf("Usage:\n  %s [tcp|tcppi] - Modbus server for unit testing\n\n", argv[0]);
            return -1;
        }
    } else {
        /* By default */
        use_backend = TCP;
    }

    if (use_backend == TCP) {
        ctx = modbus_new_tcp("127.0.0.1", 1502);
        query = (uint8_t *)malloc(MODBUS_MAX_STRING_LENGTH * sizeof(uint8_t));
        rsp = (uint8_t *)malloc(MODBUS_TCP_MAX_ADU_LENGTH * sizeof(uint8_t));
    } else {
        ctx = modbus_new_tcp_pi("::0", "1502");
        query = (uint8_t *)malloc(MODBUS_MAX_STRING_LENGTH * sizeof(uint8_t));
        rsp = (uint8_t *)malloc(MODBUS_TCP_MAX_ADU_LENGTH * sizeof(uint8_t));
    }
    header_length = modbus_get_header_length(ctx);

    modbus_set_debug(ctx, TRUE);

    mb_mapping = modbus_mapping_new_start_address(
        UT_BITS_ADDRESS, UT_BITS_NB,
        UT_INPUT_BITS_ADDRESS, UT_INPUT_BITS_NB,
        UT_REGISTERS_ADDRESS, UT_REGISTERS_NB_MAX,
        UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB,
        shim);
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

    if (use_backend == TCP) {
        s = modbus_tcp_listen(ctx, 1);
        modbus_tcp_accept(ctx, &s);
    } else {
        s = modbus_tcp_pi_listen(ctx, 1);
        modbus_tcp_pi_accept(ctx, &s);
    }

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

        rc = modbus_process_request(ctx, query, rc, rsp, &rsp_length, mb_mapping, shim);
        if (rc == -1) {
            break;
        }

        rc = modbus_reply(ctx, rsp, rsp_length);
        if (rc == -1) {
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    if (use_backend == TCP) {
        if (s != -1) {
            close(s);
        }
    }
    modbus_mapping_free(mb_mapping);
    free(query);

    return 0;
}
