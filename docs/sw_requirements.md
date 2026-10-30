
# PSA API Compliance Test Suite Software Requirements

The following tool versions were used to validate the suite before its transfer: <br />

- Host Operating System     : Ubuntu 22.04, Windows 11
- Scripting tools           : Python 3.12.0
- Host Compiler toolchain   : GCC 5.4.0 (Linux Host) or MinGW 6.3.0 (Windows Host)
- Cross Compiler toolchain  : GNU Arm Embedded Toolchain 7.3.0 or Arm Compiler 6.18
- Build tools               : CMake 3.19
- Git version               : 2.17 or above

Use a host compiler for host builds or a cross compiler for embedded targets, together with Python, CMake, and Git. Select the compiler through the `TOOLCHAIN` option described in the [build instructions](../README.md#build-steps).

### Setting up a shell to enable compiler toolchain after installation

To import GNU Arm in your bash shell console, execute:
~~~
export PATH=<GNU_ARM_PATH>/bin:$PATH
~~~

To import Arm Compiler in your bash shell console, execute:
~~~
export PATH=<ARM_COMPILER_PATH>/bin:$PATH
~~~

## Download source

To download the main branch of the repository, type the following commands: <br />
~~~
git clone https://github.com/GlobalPlatform/psa-api-compliance.git
cd psa-api-compliance
~~~

## Cloning the external repository

To build Initial Attestation tests, the PSA API Compliance test suite clones the following
git repository. The cloning is automated as part of the test suite cmake build:

**Repo** <br />
Name      : https://github.com/laurencelundblade/QCBOR.git <br />
Commit id : 42272e466a8472948bf8fca076d113b81b99f0e0

## License
The PSA API Compliance tests are distributed under the Apache License, Version 2.0.

--------------

*Copyright (c) 2018-2026, Arm Limited and Contributors. All rights reserved.*
