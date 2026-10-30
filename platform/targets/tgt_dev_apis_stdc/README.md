# Linux Host Target

There are a couple of limitations to this target when it comes to a test which involves system reset or test process due to Watch Dog Timer (WDT) and NVMEM implementation.

- **WDT**:  Lacks functionality to recover after a hang as they just return failure or success.

- **NVMEM**: Stores data in an array in memory, which means NVMEM would be lost as it isn't a non-volatile implementation.

- If PSA crypto static library file is available, -DPSA_CRYPTO_LIB_FILENAME to be used to mention the absolute path to the file.

## License

The PSA API Compliance tests are distributed under the Apache License, Version 2.0.

--------------

*Copyright (c) 2018-2019, 2026, Arm Limited and Contributors. All rights reserved.*
