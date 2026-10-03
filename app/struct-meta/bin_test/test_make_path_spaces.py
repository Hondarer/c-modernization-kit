"""空白を含む生成器のパスと、生成カタログの make 設定を検証する。"""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


APP = Path(__file__).resolve().parents[1]
WORKSPACE = APP.parents[1]
HELPERS = WORKSPACE / "framework/makefw/makefiles/_path_functions.mk"


class MakePathSpacesTest(unittest.TestCase):
    def test_generator_and_catalog_detection(self):
        with tempfile.TemporaryDirectory(prefix="struct meta space ") as temp:
            root = Path(temp)
            generator = root / "tools space/generator.sh"
            generator.parent.mkdir()
            with open(generator, "w", encoding="utf-8", newline="\n") as handle:
                handle.write('''#!/bin/bash
    [[ "$#" -eq 4 && "$1" = --header && "$2" = sample_types.h && "$3" = --out ]] || exit 2
    printf 'int sample_catalog;\\n' > "$4"
    printf 'extern int sample_catalog;\\n' > "${4%.c}.h"
    ''')
            generator.chmod(0o755)
            (root / "sample_types.h").touch()
            include = lambda path: "include " + path.as_posix().replace(" ", "\\ ") + "\n"
            makefile = root / "makefile"
            with open(makefile, "w", encoding="utf-8", newline="\n") as handle:
                handle.write("SHELL := /bin/bash\nPLATFORM_WINDOWS := 1\n"
                    + include(HELPERS)
                    + include(APP / "prod/src/cmd/struct-meta-sample/makepart.mk")
                    + 'gen:\n\tmkdir -p "$@"\n')
            command = ["make", "--no-print-directory", "gen/sample_types_meta.h",
                       "STRUCT_META_GEN_BIN=" + generator.as_posix()]
            result = subprocess.run(command, cwd=root, stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT, text=True, encoding="utf-8", timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout)
            output = root / "gen/sample_types_meta.c"
            stamp = output.stat().st_mtime_ns
            result = subprocess.run(command, cwd=root, stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT, text=True, encoding="utf-8", timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout)
            self.assertEqual(output.stat().st_mtime_ns, stamp)
            catalog = root / "prod/src/cmd/struct-meta-sample/gen/sample_types_meta.c"
            catalog.parent.mkdir(parents=True)
            catalog.write_text("int sample_catalog;\n", encoding="utf-8")
            with open(makefile, "w", encoding="utf-8", newline="\n") as handle:
                handle.write(include(HELPERS)
                    + include(APP / "test/src/sampleTypesMetaTest/makepart.mk")
                    + 'inspect:\n\t@printf "%s\\n" "$(TEST_SRCS)"\n')
            result = subprocess.run(
                ["make", "--no-print-directory", "inspect", "MYAPP_DIR=" + root.as_posix()],
                cwd=root, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding="utf-8", timeout=30,
            )
            self.assertEqual(result.returncode, 0, result.stdout)
            self.assertEqual(result.stdout.strip(), catalog.as_posix())


if __name__ == "__main__":
    unittest.main()
