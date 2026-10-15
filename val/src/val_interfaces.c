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


#include "val_framework.h"
#include "val_interfaces.h"
#include "val_peripherals.h"
#include "val_crypto.h"
#include "val_storage.h"
#include "val_attestation.h"

/*VAL APIs to be used by test */
const val_api_t val_api = {
    .print                     = val_print,
    .set_status                = val_set_status,
    .get_status                = val_get_status,
    .test_init                 = val_test_init,
    .test_exit                 = val_test_exit,
    .err_check_set             = val_err_check_set,
    .execute_non_secure_tests  = val_execute_non_secure_tests,
    .nvmem_read                = val_read_nvm,
    .nvmem_write               = val_write_nvm,
    .wd_timer_init             = val_wd_timer_init,
    .wd_timer_enable           = val_enable_watchdog,
    .wd_timer_disable          = val_disable_watchdog,
    .wd_reprogram_timer        = val_wd_reprogram_timer,
    .set_boot_flag             = val_set_boot_flag,
    .get_boot_flag             = val_get_boot_flag,
	.set_test_data             = val_set_test_data,
	.get_test_data             = val_get_test_data,
    .crypto_function           = val_crypto_function,
    .storage_function          = val_storage_function,
    .attestation_function      = val_attestation_function,
};
