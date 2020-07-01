#ifndef _SHIM_TYPES_HPP_
#define _SHIM_TYPES_HPP_

typedef enum {
    CHERI_SHIM,
    CHERI_SHIM_X, // used from within cheri_shim
    MACAROONS_SHIM,
    MACAROONS_SHIM_X, // used from within macaroons_shim
    CHERI_MACAROONS_SHIM,
} shim_t;

#endif /* _SHIM_TYPES_HPP_ */