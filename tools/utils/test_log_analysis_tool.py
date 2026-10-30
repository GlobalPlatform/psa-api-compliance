#/** @file
# * Copyright (c) 2026, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
#**/

"""Tests for current and historical log banner recognition."""

from pathlib import Path
from tempfile import TemporaryDirectory
from types import SimpleNamespace
import unittest
from unittest.mock import Mock, patch

import log_analysis_tool


class LogBannerTests(unittest.TestCase):
    """Verify banner recognition without constructing the GUI."""

    def test_supported_banners(self):
        """Accept the current banner and historical suite banners."""
        for suite_name, version in (
            ("PSA API Compliance tests", "1.8"),
            ("PSA Architecture Test Suite", "0.8"),
            ("PSA Architecture Test Suite", "0.9"),
        ):
            with self.subTest(suite_name=suite_name, version=version):
                view = SimpleNamespace(
                    Scrolledtreeview1=Mock(), Scrolledtext1=Mock()
                )
                with TemporaryDirectory() as directory:
                    log = Path(directory) / "crypto.txt"
                    log.write_text(
                        f"***** {suite_name} - Version {version} *****\n"
                        "Running.. Crypto Suite\n"
                    )
                    with log.open("r+") as stream:
                        open_patch = patch.object(
                            log_analysis_tool, "open", return_value=stream,
                            create=True
                        )
                        with open_patch, \
                             patch.object(log_analysis_tool, "gtoplevel", view), \
                             patch.object(log_analysis_tool, "testsuite_list", []), \
                             patch.object(log_analysis_tool,
                                          "cryptoDataExtraction") as extract:
                            log_analysis_tool.findTestSuite(str(log))
                            extract.assert_called_once_with(str(log), "Crypto")
                view.Scrolledtreeview1.heading.assert_called_once_with(
                    "#0", text=f"PSA API Compliance tests {version}"
                )
                view.Scrolledtext1.insert.assert_not_called()


if __name__ == "__main__":
    unittest.main()
