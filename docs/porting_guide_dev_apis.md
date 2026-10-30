
# Porting Guide: PSA API Compliance tests
-----------------------------------------------------------

## Introduction
The PSA API Compliance test suite contains the *Platform Abstraction Layer* (PAL) which abstracts platform-specific information from the tests. You must implement and port the PAL interface functions to your target platform. Create and update the target configuration file to match the details of this target platform. All paths in this guide are relative to the repository root.

This document provides the porting steps and the list of PAL APIs.

## Porting steps

### Target configuration

You must populate your system configuration and provide it as an input to the test suite. The system configuration is captured in an input configuration header file called **pal_config.h**, available at **platform/targets/<platform_name>/nspe/**. <br />

An example input configuration header is as shown:

    // UART device info
    #define UART_NUM              1
    #define UART_0_BASE           0x40004000
    #define UART_0_SIZE           0xFFF
    #define UART_0_INTR_ID        0xFF
    #define UART_0_PERMISSION     TYPE_READ_WRITE

    // Watchdog device info
    #define WATCHDOG_NUM          1
    #define WATCHDOG_0_BASE       0x40008000
    #define WATCHDOG_0_SIZE       0xFFF
    #define WATCHDOG_0_INTR_ID    0xFF
    #define WATCHDOG_0_PERMISSION TYPE_READ_WRITE

  For details on the structure of the input, refer to **platform/targets/tgt_dev_apis_tfm_an521/nspe/pal_config.h**.

### Adding a new target

  1. Create a new directory in **platform/targets/<platform_name>**. For reference, see the existing platform **tgt_dev_apis_tfm_an521** directory.
  2. Execute `cp -rf platform/targets/tgt_dev_apis_tfm_an521/ platform/targets/<platform_name>/`.
  3. Update **platform/targets/<platform_name>/nspe/pal_config.h** with your platform details. Refer to **platform/targets/tgt_dev_apis_tfm_an521/nspe/pal_config.h** for structure details.
  4. Update **platform/targets/<platform_name>/target.cmake** appropriately to select the correct instances of PAL files for compilation.
  5. Refer to the **List of PAL APIs** section to view the PAL APIs needed by your selected suite. Shared PSA API wrappers are in `platform/targets/common/<suite_name>/pal_*_intf.c`; platform-specific driver functions are in `platform/targets/<platform_name>/nspe/pal_driver_intf.c` and the driver sources selected by `target.cmake`. Reuse these implementations where suitable, and port the platform-specific functions as required.
  6. Update `platform/targets/<platform_name>/nspe/pal_crypto_config.h` to enable or disable Crypto features selectively for the Crypto test suite.

**Note**:
The test suite requires access to the following peripherals:
  - One UART to print Test NSPE messages
  - One Watchdog timer to help recover from any fatal error conditions
  - Non-volatile memory support to preserve test status over watchdog timer reset. Each byte of this region must be initialised to FF at power on reset.
  - If your implementation lacks the corresponding features, use `DISABLE_CRYPTO_PAKE`, `DISABLE_CRYPTO_1_1_0`, and `DISABLE_KEY_AGREEMENT` as appropriate. These macros are enabled in the example `platform/targets/tgt_dev_apis_tfm_an521/nspe/pal_crypto_config.h`; review them for your implementation before running compliance tests.


## List of PAL APIs
Since the test suite is agnostic to various system targets, you must port the following PAL NSPE APIs before building the tests. Implement these functions for your target platform. <br />

| No | Prototype                                                                                                                   | Description                                                            | Parameters                                               |
|----|-----------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------|----------------------------------------------------------|
| 01 | int pal_uart_init_ns(void);                                                                                                 | Initializes the UART                                     | None<br/>                                                              |
| 02 | int pal_print(uint8_t c);                                                                                                   | Sends a character to the UART TX FIFO                     | c  :  Input character<br/>                                             |
| 03 | int pal_print_ns(const char *str, int32_t data);                                                                            | Parses the input string and writes bytes into UART TX FIFO| str      : Input String<br/>data     : Value for format specifier<br/>                             |
| 04 | int pal_wd_timer_init_ns(uint32_t time_us, uint32_t timer_tick_us);                                                         | Initializes a hardware watchdog timer                                 | time_us         : Time in micro seconds<br/>timer_tick_us   : Number of ticks per micro second<br/>|
| 05 | int pal_watchdog_enable(void);                                                                                              | Enables a hardware watchdog timer                                      | None<br/>|
| 06 | int pal_watchdog_disable(void);                                                                                             | Disables a hardware watchdog timer                                     | None<br/>    |
| 07 | int pal_nvm_read(uint32_t offset, void *buffer, size_t size);                                                               | Reads from the given non-volatile address.                                 | offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 08 | int pal_nvm_write(uint32_t offset, void *buffer, size_t size);                                                              | Writes into given non-volatile address.                                | offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 09 | int32_t pal_crypto_function(int type, va_list valist);                                                                     | Calls the requested Crypto function                       | type    : Function code<br/>valist  : variable argument list<br/>                             |
| 10 | uint32_t pal_its_function(int type, va_list valist);                                                                     | Calls the requested Internal Trusted Storage  function                       | type    : Function code<br/>valist  : Variable argument list<br/>                             |
| 11 | uint32_t pal_ps_function(int type, va_list valist);                                                                     | Calls the requested Protected Storage  function                       | type    : Function code<br/>valist  : Variable argument list<br/>                             |
| 12 | int32_t pal_attestation_function(int type, va_list valist);                                                                | Calls the requested Initial Attestation  function                       | type    : Function code<br/>valist  : Variable argument list<br/>                             |
| 13 | uint32_t pal_compute_hash(int32_t cose_alg_id, struct q_useful_buf buffer_for_hash, struct q_useful_buf_c *hash, struct q_useful_buf_c protected_headers, struct q_useful_buf_c payload);                                                                | Computes hash for the requested data                       | cose_alg_id    : Algorithm ID<br/>buffer_for_hash  : Temp buffer for calculating hash<br/><br/>hash  : Pointer to store the hash<br/> buffer_for_hash  : Temp buffer for calculating hash<br/>protected_headers : data to be hashed<br/>payload  : Payload data<br/>                             |
| 14 | uint32_t pal_crypto_pub_key_verify(int32_t cose_algorithm_id, struct q_useful_buf_c token_hash, struct q_useful_buf_c signature);                                                                | Function call to verify the signature using the public key              | cose_algorithm_id    : Algorithm ID<br/>token_hash  : Data that needs to be verified<br/>signature  : Signature to be verified against<br/>                             |
| 15 | int pal_system_reset(void) | Resets the system | None |
| 16 | void pal_set_custom_test_list(char *custom_test_list); | Sets the custom test list buffer | custom_test_list : Custom test list buffer<br/>                             |
| 17 | bool_t pal_is_test_enabled(test_id_t test_id); | Tells if a test is enabled on platform | test_id : Test ID<br/>                             |
| 18 | unsigned int pal_platform_init(void); | Optional api which can be used for initializing a platform specific operations/informations | None<br/>                             |

## License
The PSA API Compliance tests are distributed under the Apache License, Version 2.0.

--------------

*Copyright (c) 2019-2026, Arm Limited and Contributors. All rights reserved.*
