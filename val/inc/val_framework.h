/** @file
 * Copyright (c) 2018-2026, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/


#ifndef _VAL_FRAMEWORK_H_
#define _VAL_FRAMEWORK_H_

#include "val.h"
#include "val_interfaces.h"

/* Struture to capture test state */
typedef struct {
    uint16_t reserved;
    uint8_t  state;
    uint8_t  status_code;
} val_test_status_buffer_ts;

/* prototypes */
void val_set_status(uint32_t status);
uint32_t val_get_status(void);
uint32_t val_report_status(void);

uint32_t     val_report_status_bespoke(void);
val_status_t val_err_check_set(uint32_t checkpoint, val_status_t status);
void         val_test_init(uint32_t test_num, uint32_t comp_num, char8_t *desc,
                                              uint32_t test_bitfield);
void         val_test_exit(void);
val_status_t val_get_last_run_test_id(test_id_t *test_id);
val_status_t val_execute_non_secure_tests(const client_test_t *tests_list);
val_status_t val_set_boot_flag(boot_state_t state);
val_status_t val_get_boot_flag(boot_state_t *state);
val_status_t val_set_test_data(int32_t nvm_index, int32_t test_data);
val_status_t val_get_test_data(int32_t nvm_index, int32_t *test_data);

#endif
