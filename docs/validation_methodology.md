# PSA API Compliance test validation methodology

The PSA API Compliance tests exercise specified API behavior using portable, self-checking C tests. They provide evidence about the API functions and scenarios exercised by a particular build. Passing the tests does not replace design verification or a security evaluation. PSA Certified currently reviews API compliance evidence and issues badges; see the [compliance FAQ](FAQ-compliance.md).

## Scope and repository layout

The repository contains tests for the PSA Crypto API, PSA Secure Storage API (ITS and PS), and PSA Initial Attestation API.

| Path | Contents |
|------|----------|
| `crypto/` | Crypto tests, test databases, and suite build rules |
| `storage/` | Shared ITS and PS tests, test databases, and suite build rules |
| `initial_attestation/` | Initial Attestation tests, test databases, and suite build rules |
| `val/inc/`, `val/src/` | Validation Abstraction Layer (VAL) interfaces and implementation |
| `platform/targets/common/` | Shared Platform Abstraction Layer (PAL) interfaces and PSA API wrappers |
| `platform/targets/<target>/nspe/` | Target configuration and platform-specific PAL implementation |
| `platform/drivers/` | Platform drivers used by the PAL |
| `tools/` | CMake support, test-list generation, and log analysis |
| `docs/` | Guides and test scenario checklists |

The source directories are at the repository root. The build still places test archives under `dev_apis/` in the build directory.

## Test framework

Tests call the VAL interfaces to invoke PSA APIs, report checks, and record results. The VAL uses the PAL for platform services, including console output, watchdog handling, persistent test progress, and access to the implementation under test. Tests and the VAL are portable; platform-specific adaptation belongs in the PAL and target configuration, as described in the [porting guide](porting_guide_dev_apis.md).

Each test directory contains a `test.cmake` file that selects its source files, a `test_entry_<id>.c` entry point, and the test implementation and data in `test_<id>.c` and associated headers. Test identifiers use `c` for Crypto, `s` for Storage, and `a` for Attestation. Both ITS and PS use the `storage/test_sNNN/` sources.

The dispatcher passes a `val_api_t` table of VAL function pointers to each test entry point. The entry point initializes the test, executes its checks through the VAL, performs any cleanup, and exits. The dispatcher waits for each test to finish before reporting its result and starting the next test.

Suite database files select the tests to compile. CMake runs `tools/scripts/gen_tests_list.py` to generate the selected test list and entry-point tables. The `SUITE`, `SPEC_VERSION`, `SUITE_TEST_RANGE`, `TESTS_COVERAGE`, and target configuration settings affect coverage. Crypto builds can also select a target-specific database with `BESPOKE_SUITE_TESTS`. Keep these settings with the test logs so the tested scope can be reproduced. A skipped test is not evidence that its scenario passed.

All tests selected for a build are combined into one test archive. If the target cannot accommodate the full suite, build and run smaller batches using `-DSUITE_TEST_RANGE="1;10"`, then `-DSUITE_TEST_RANGE="11;20"`, and so on until the required tests have been covered. These are inclusive test numbers within the selected suite, rather than the numeric IDs printed in the log. Keep the configuration and results for every batch. Commenting out a test entry in the suite database also excludes it from the build; record any such exclusions.

For the specification versions covered by this suite, see [Current release](../README.md#current-release). Specifications are published at [arm-software.github.io/psa-api](https://arm-software.github.io/psa-api/); sources and reference headers are maintained in [GlobalPlatform/psa-api](https://github.com/GlobalPlatform/psa-api).

## Integrating the test suite with the SUT

The System Under Test (SUT) must expose the selected PSA API implementation to the test application. Supply its headers through `PSA_INCLUDE_PATHS`, select a target and suite, and build using the [README instructions](../README.md#build-steps).

The build generates these static libraries:

| Library | Purpose |
|---------|---------|
| `<build_dir>/val/val_nspe.a` | VAL and test dispatcher |
| `<build_dir>/platform/pal_nspe.a` | PAL and selected platform drivers |
| `<build_dir>/dev_apis/<suite_dir>/test_combine.a` | Selected tests |

`<suite_dir>` is `crypto`, `storage`, or `initial_attestation`. All Storage suite selections use `storage`.

Integrate the libraries with an application that can call the implementation under test. For a system with separate Secure and Non-secure Processing Environments (SPE and NSPE), link them into the Non-secure application and provide the PSA service implementation in the appropriate environment. Load the application and initialize the platform, operating system if used, and API services before calling `int32_t val_entry(void)`. Host targets can access a software implementation directly; see their target configuration and README files.

The PAL must provide console output and any reset and persistent-state behavior needed by the selected tests. Review the host target limitations when a test requires watchdog recovery or persistence across reset. Initial Attestation builds also require QCBOR and access to the attestation verification key; see the [software requirements](sw_requirements.md) and [Attestation checklist](psa_attestation_testlist.md).

Console output normally uses a UART. If the target uses another transport, adapt the PAL output functions and arrange to capture the log on the host.

## Execution and results

`val_entry()` initializes platform services, obtains any saved test progress, and calls `val_dispatcher()` in `val/src/val_dispatcher.c`. The dispatcher executes selected tests sequentially, records their results, and prints the regression summary. On platforms supporting reset recovery, saved progress allows execution to resume after a test-triggered reset.

The normal per-test result lines and their summary counters are:

| Result line | Summary counter | Meaning |
|-------------|-----------------|---------|
| `Result=Passed` | `TOTAL PASSED` | The test completed its checks successfully |
| `Result=Failed (Error code=...)` | `TOTAL FAILED` | A check or required test operation failed |
| `Result=Skipped (Skip code=...)` | `TOTAL SKIPPED` | The test was skipped, for example because a required feature is unsupported |
| `Result=Error (Error code=...)` | `TOTAL SIM ERROR` | The test could not complete normally, for example because of an unexpected reset |

The regression report also gives `TOTAL TESTS`. Despite the historical `SIM ERROR` label, errors can occur on hardware as well as in simulation. A hang or watchdog timeout needs investigation even when the target cannot recover far enough to print a result or summary.

Capture the full console log, including the suite version, per-test results, and final summary. Record the test repository revision, target, build options, configuration headers, and implementation revision alongside it. Explain skipped or excluded tests when submitting evidence to PSA Certified.

For a failure, identify the test and checkpoint in the log, then inspect the corresponding test source. Numeric test IDs add a suite offset to the test number: Crypto uses 200, Storage uses 400, and Initial Attestation uses 600. For example, `TEST202` identifies `crypto/test_c002/`, `TEST401` identifies `storage/test_s001/`, and `TEST601` identifies `initial_attestation/test_a001/`.

A message such as `Failed at Checkpoint: 3` refers to the assertion using `TEST_CHECKPOINT_NUM(3)` in that test. Compare any actual and expected values with the API specification and implementation. VAL test result and error definitions are in `val/inc/val.h`; PSA API status values come from the implementation's PSA headers. For more diagnostic output, rebuild with `-DVERBOSITY=1` (INFO and above) or `-DVERBOSITY=2` (DEBUG and above), instead of the default level 3. The [failure analysis](test_failure_analysis.md) records observations for a specific historical implementation revision, rather than current results for all implementations.

The optional [log analysis tool](../tools/utils/README.md) displays test results in a tree view and supports both current and historical suite banners.

## License

The PSA API Compliance tests and this guide are distributed under the [Apache License, Version 2.0](../LICENSE.md).

--------------

*Copyright (c) 2026, Arm Limited and Contributors. All rights reserved.*
