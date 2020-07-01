#ifndef _SHIM_TYPES_HPP_
#define _SHIM_TYPES_HPP_

#include <iostream>

typedef enum {
    NO_SHIM,
    CHERI_SHIM,
    CHERI_SHIM_X, // used from within cheri_shim
    MACAROONS_SHIM,
    MACAROONS_SHIM_X, // used from within macaroons_shim
    CHERI_MACAROONS_SHIM,
} shim_t;

const std::string display_marker = "****************************************"
                                   "****************************************";

#endif /* _SHIM_TYPES_HPP_ */