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
#include "val_dispatcher.h"
#include "val_peripherals.h"
#include "pal_interfaces_ns.h"

extern val_api_t val_api;

/**
 *   @brief    - Returns the shared status buffer used by the current test run
 *   @param    - void
 *   @return   - Pointer to the shared status record
**/
static val_test_status_buffer_ts *val_status_buffer(void)
{
    return (val_test_status_buffer_ts *)PLATFORM_SHARED_REGION_BASE;
}

/**
 *   @brief    - Stores the packed test state and status code in shared memory
 *   @param    - status : Packed status field
 *   @return   - void
**/
void val_set_status(uint32_t status)
{
    val_test_status_buffer_ts *status_buffer = val_status_buffer();

    status_buffer->state = (uint8_t)((status >> TEST_STATE_SHIFT) & TEST_STATE_MASK);
    status_buffer->status_code = (uint8_t)(status & TEST_STATUS_CODE_MASK);
}

/**
 *   @brief    - Reads the packed test state and status code from shared memory
 *   @param    - void
 *   @return   - Packed status field
**/
uint32_t val_get_status(void)
{
    const val_test_status_buffer_ts *status_buffer = val_status_buffer();

    return (((uint32_t)status_buffer->state) << TEST_STATE_SHIFT) |
        (uint32_t)status_buffer->status_code;
}

/**
 *   @brief    - Prints the textual result line for a completed test
 *   @param    - state       : Encoded test state
 *             - status_code : Associated status or error code
 *   @return   - Normalized test state
**/
static uint32_t val_emit_status_line(uint32_t state, uint32_t status_code)
{
    switch (state)
    {
    case TEST_PASS:
        val_printf(ALWAYS, "Result=Passed\n");
        return TEST_PASS;
    case TEST_FAIL:
        val_printf(ALWAYS, "Result=Failed (Error code=%d)\n", status_code);
        return TEST_FAIL;
    case TEST_SKIP:
        val_printf(ALWAYS, "Result=Skipped (Skip code=%d)\n", status_code);
        return TEST_SKIP;
    case TEST_ERROR:
        val_printf(ALWAYS, "Result=Error (Error code=%d)\n", status_code);
        return TEST_ERROR;
    default:
        val_printf(ALWAYS, "Result=Failed (Error Code=%d)\n", status_code);
        return TEST_FAIL;
    }
}

/**
 *   @brief    - Reports the current test result and returns the normalized state
 *   @param    - void
 *   @return   - Final test state
**/
uint32_t val_report_status(void)
{
    uint32_t status = val_get_status();
    uint32_t state = (status >> TEST_STATE_SHIFT) & TEST_STATE_MASK;
    uint32_t status_code = status & TEST_STATUS_CODE_MASK;

    state = val_emit_status_line(state, status_code);
    val_printf(ALWAYS, "***********************************\n");
    return state;
}

/**
    @brief    - This function executes given list of tests from non-secure sequentially
                This executes the non-secure API test path
    @param    - tests_list : list of tests to be executed
    @return   - val_status_t
**/
val_status_t val_execute_non_secure_tests(const client_test_t *tests_list)
{
    val_status_t          status = VAL_STATUS_SUCCESS;
    val_status_t          test_status = VAL_STATUS_SUCCESS;
    boot_t                boot;
    uint32_t              i = 1;

    status = val_get_boot_flag(&boot.state);
    if (VAL_IS_ERROR(status))
    {
       val_set_status(RESULT_FAIL(status));
       return status;
    }

    if (boot.state == BOOT_NOT_EXPECTED || boot.state == BOOT_EXPECTED_REENTER_TEST
        || boot.state == BOOT_EXPECTED_CONT_TEST_EXEC
        || boot.state == BOOT_EXPECTED_ON_SECOND_CHECK)
    {
        while (tests_list[i] != NULL)
        {
            /*
             * Reboot have been expected by test in previous ns run,
             * consider previous run pass and jump to second test function
             * of the same test if available.
             */

            if ((boot.state ==  BOOT_EXPECTED_REENTER_TEST) && (i == 1))
            {
                val_print(DBG, "[Check 1] PASSED\n", 0);
                i++;
                continue;
            }

            if ((boot.state ==  BOOT_EXPECTED_ON_SECOND_CHECK) && (i == 1))
            {
                val_print(DBG, "[Check 2] PASSED\n", 0);
                i = i + 2 ;
                continue;
            }

            if (boot.state != BOOT_EXPECTED_CONT_TEST_EXEC)
            {
                status = val_set_boot_flag(BOOT_NOT_EXPECTED);
                if (VAL_IS_ERROR(status))
                {
                    return status;
                }
            }

            if (i == 1)
                val_print(TEST, "[Info] Executing tests from non-secure\n", 0);
            /* keep track of the test block numbers, helps when the panic happened */
            status = val_set_test_data(NVM_TEST_DATA2, i);
            /* Execute client tests */
            test_status = tests_list[i](CALLER_NONSECURE);
            status = test_status ? test_status:status;
            if (IS_TEST_SKIP(status))
            {
                val_set_status(status);
                return status;
            }
            else if (VAL_IS_ERROR(status))
            {
                val_set_status(RESULT_FAIL(status));
                return status;
            }

            i++;
        }
   }
   else
   {
       /* If we are here means, we are in second run of this test */
       status = VAL_STATUS_SUCCESS;
       if (boot.state != BOOT_EXPECTED_S)
       {
            val_print(DBG, "[Check 1] PASSED\n", 0);
       }
   }
   return status;
}

/**
 *   @brief    Parses input status for a given test and
 *             outputs appropriate information on the console
 *   @return   Test state
**/
uint32_t val_report_status_bespoke(void)
{
    uint32_t status, state, status_code;

    status = val_get_status();

    state = (status >> TEST_STATE_SHIFT) & TEST_STATE_MASK;
    status_code = status & TEST_STATUS_CODE_MASK;

    switch (state)
    {
        case TEST_START:
            state = TEST_FAIL;
            val_print(ALWAYS, "\nResult=Failed (Error Code=0x%x)\n",
                                            VAL_STATUS_INIT_FAILED);
            break;

        case TEST_END:
        case TEST_PASS:
            state = TEST_PASS;
            val_print(ALWAYS, "\nResult=Passed\n", 0);
            break;

        case TEST_FAIL:
            state = TEST_FAIL;
            val_print(ALWAYS, "\nResult=Failed (Error code=0x%x)\n",
                status_code);
            break;

        case TEST_SKIP:
            state = TEST_SKIP;
            val_print(ALWAYS, "\nResult=Skipped (Skip code=0x%x)\n",
                status_code);
            break;

        case TEST_ERROR:
            state = TEST_ERROR;
            val_print(ALWAYS, "\nResult=Sim Error (Error code=0x%x)\n",
                status_code);
            break;

        default:
            state = TEST_FAIL;
            val_print(ALWAYS, "\nResult=Failed (Error Code=0x%x)\n",
                status_code);
            break;
    }

    val_print(ALWAYS, "***********************************\n", 0);
    return state;
}

/*
    @brief           - This function checks if the input status argument is an error.
                       On error, we print the checkpoint value and set the status.
    @param           - checkpoint     : Test debug checkpoint
                     - val_status_t   : Test status
    @return          - returns the input status back to the program.
*/

val_status_t val_err_check_set(uint32_t checkpoint, val_status_t status)
{
    if (VAL_IS_ERROR(status))
    {
        val_print(ERROR, "\tCheckpoint %d : ", checkpoint);
        val_print(ERROR, "Error Code=0x%x \n", status);
        val_set_status(RESULT_FAIL(status));
    }
    else
    {
        status = (val_get_status() & TEST_STATUS_CODE_MASK);
        if (VAL_IS_ERROR(status))
        {
            val_print(ERROR, "\tCheckpoint %d : ", checkpoint);
            val_print(ERROR, "Error Code=0x%x \n", status);
        }
        else
        {
            val_print(DBG, "\tCheckpoint %d \n", checkpoint);
        }
    }
    return status;
}

/**
  @brief  This API prints the test number, description and
          sets the test state to TEST_START on successful execution.
  @param  test_num       :unique number identifying this test
  @param  desc           :brief description of the test
  @param  test_bitfield  :Addition test info such as
                           - test isolation level requirement
                           - Watchdog timeout type
  @return void
**/

void val_test_init(uint32_t test_num, uint32_t comp_num, char8_t *desc, uint32_t test_bitfield)
{
   val_status_t         status = VAL_STATUS_SUCCESS;

   val_test_status_buffer_ts *status_buffer =
                              (val_test_status_buffer_ts *)PLATFORM_SHARED_REGION_BASE;
   char *suite;

   switch (comp_num)
    {
        case VAL_CRYPTO_BASE:
            suite = "Crypto";
            break;
        case VAL_STORAGE_BASE:
            suite = "Storage";
            break;
        case VAL_INITIAL_ATTESTATION_BASE:
            suite = "Attestation";
            break;
        default:
            suite = "Unknown";
            break;
    }

   /*global init*/
   status_buffer->state        = TEST_FAIL;
   status_buffer->status_code  = VAL_STATUS_INVALID;

   val_print(ALWAYS, "\rSuite=", 0);
   val_print(ALWAYS, suite, 0);
   val_print(ALWAYS, " : Test=", 0);
   val_print(ALWAYS, desc, 0);
   val_print(ALWAYS, "Num=%d\n", test_num);

   /* common skip logic */
   if (PLATFORM_PSA_ISOLATION_LEVEL < GET_TEST_ISOLATION_LEVEL(test_bitfield))
   {
       val_set_status(RESULT_SKIP(VAL_STATUS_ISOLATION_LEVEL_NOT_SUPP));
       val_print(ALWAYS, "\tSkipping test. Required isolation level is not supported\n", 0);
       return;
   }

#ifdef WATCHDOG_AVAILABLE
   /* Initialise watchdog */
   status = val_wd_timer_init(GET_WD_TIMOUT_TYPE(test_bitfield));
   if (VAL_IS_ERROR(status))
   {
       val_print(ERROR, "\tval_wd_timer_init failed Error=0x%x\n", status);
       return;
   }

   /* Enable watchdog Timer */
   status = val_enable_watchdog();
   if (VAL_IS_ERROR(status))
   {
       val_print(ERROR, "\tval_wd_timer_enable failed Error=0x%x\n", status);
       return;
   }
#endif

   val_set_status(RESULT_START(status));
   return;
}

/**
  @brief  This API sets the test state to TEST_PASS if test is successfuly passed.
  @param none
  @return none
**/

void val_test_exit(void)
{
    val_status_t         status = VAL_STATUS_SUCCESS;

#ifdef WATCHDOG_AVAILABLE
    status = val_disable_watchdog();
    if (VAL_IS_ERROR(status))
    {
       val_print(ERROR, "\tval_wd_timer_disable failed Error=0x%x\n", status);
       val_set_status(RESULT_FAIL(status));
       return;
    }
#endif

    status = val_get_status();

    /* return if test skipped or failed */
    if (IS_TEST_FAIL(status) || IS_TEST_SKIP(status))
    {
        return;
    }
    else
    {
        val_set_status(RESULT_PASS(VAL_STATUS_SUCCESS));
    }
}

/**
    @brief    - This function returns the test ID of the last test that was run
    @param    - test_id address
    @return   - val_status_t
**/
val_status_t val_get_last_run_test_id(test_id_t *test_id)
{
    val_status_t    status;
    regre_report_t  test_count;
    boot_t          boot;
    int             intermediate_boot = 0;
    uint8_t         boot_state[] = {BOOT_NOT_EXPECTED,
                                    BOOT_EXPECTED_NS,
                                    BOOT_EXPECTED_S,
                                    BOOT_EXPECTED_BUT_FAILED,
                                    BOOT_EXPECTED_REENTER_TEST,
                                    BOOT_EXPECTED_CONT_TEST_EXEC,
                                    BOOT_EXPECTED_ON_SECOND_CHECK
                                    };

    status = val_get_boot_flag(&boot.state);
    if (VAL_IS_ERROR(status))
    {
        return status;
    }

    val_print(INFO, "\n\tboot.state=0x%x", boot.state);

    intermediate_boot = is_reboot_run(boot.state, boot_state,
                                    sizeof(boot_state)/sizeof(boot_state[0]));

    if (!intermediate_boot)
    {
         /* First boot. Initiliase necessary data structure */
         status = val_set_boot_flag(BOOT_UNKNOWN);
         if (VAL_IS_ERROR(status))
         {
             return status;
         }

         *test_id = VAL_INVALID_TEST_ID;
         status = val_write_nvm(VAL_NVM_OFFSET(NVM_PREVIOUS_TEST_ID),
                                  test_id, sizeof(test_id_t));
         if (VAL_IS_ERROR(status))
         {
             val_print(ALWAYS, "\n\tNVMEM write error", 0);
             return status;
         }

         val_reset_regression_report(&test_count);
         status = (val_write_nvm(VAL_NVM_OFFSET(NVM_TOTAL_PASS_INDEX),
                        &test_count.total_pass, sizeof(uint32_t)) ||
                   val_write_nvm(VAL_NVM_OFFSET(NVM_TOTAL_FAIL_INDEX),
                        &test_count.total_fail, sizeof(uint32_t))  ||
                   val_write_nvm(VAL_NVM_OFFSET(NVM_TOTAL_SKIP_INDEX),
                        &test_count.total_skip, sizeof(uint32_t))  ||
                   val_write_nvm(VAL_NVM_OFFSET(NVM_TOTAL_ERROR_INDEX),
                        &test_count.total_error, sizeof(uint32_t)));

         if (VAL_IS_ERROR(status))
         {
             val_print(ERROR, "\n\tNVMEM write error", 0);
             return status;
         }
    }

    status = val_read_nvm(VAL_NVM_OFFSET(NVM_PREVIOUS_TEST_ID), test_id, sizeof(test_id_t));
    if (VAL_IS_ERROR(status))
    {
        val_print(ERROR, "\n\tNVMEM read error", 0);
    }

    val_print(INFO, "In val_get_last_run_test_id, test_id=%x\n", *test_id);
    return status;
}

/**
    @brief    - This function sets the given boot.state value to corresponding
                boot NVMEM location
    @param    - state: boot_state_t
    @return   - val_status_t
**/
val_status_t val_set_boot_flag(boot_state_t state)
{
   boot_t           boot;
   val_status_t     status;

   boot.state = state;
   status = val_write_nvm(VAL_NVM_OFFSET(NVM_BOOT), &boot, sizeof(boot_t));
   if (VAL_IS_ERROR(status))
   {
       val_print(ERROR, "\tval_nvmem_write failed. Error=0x%x\n", status);
       return status;
   }
   return status;
}

/**
    @brief    - This function returns boot.state value available in boot NVMEM location
    @param    - state address
    @return   - val_status_t
**/
val_status_t val_get_boot_flag(boot_state_t *state)
{
   boot_t           boot;
   val_status_t     status;

   status = val_read_nvm(VAL_NVM_OFFSET(NVM_BOOT), &boot, sizeof(boot_t));
   if (VAL_IS_ERROR(status))
   {
       val_print(ERROR, "\tval_nvmem_read failed. Error=0x%x\n", status);
       return status;
   }
   *state = boot.state;
   return status;
}

/**
    @brief    - This function sets the test specific data
                NVMEM location
    @param    - nvm index
    @param    - nvm testdata
    @return   - val_status_t
**/
val_status_t val_set_test_data(int32_t nvm_index, int32_t test_data)
{
   val_status_t     status;

   status = val_write_nvm(VAL_NVM_OFFSET(nvm_index), &test_data, sizeof(int32_t));
   if (VAL_IS_ERROR(status))
   {
       val_print(ERROR, "\tval_nvmem_write failed for test data. Error=0x%x\n", status);
       return status;
   }
   return status;
}

/**
    @brief    - This function gets the test specific data
                NVMEM location
    @param    - nvm index
    @param    - nvm testdata
    @return   - val_status_t
**/
val_status_t val_get_test_data(int32_t nvm_index, int32_t *test_data)
{
   val_status_t     status;

   status = val_read_nvm(VAL_NVM_OFFSET(nvm_index), test_data, sizeof(int32_t));
   if (VAL_IS_ERROR(status))
   {
       val_print(ERROR, "\tval_nvmem_read failed for test data. Error=0x%x\n", status);
       return status;
   }
   return status;
}
