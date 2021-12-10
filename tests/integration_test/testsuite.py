import dataclasses
import json
import os.path
import pathlib
from argparse import ArgumentParser
from pathlib import Path
import subprocess as sp
from difflib import unified_diff
from dataclasses import dataclass, field
from contextlib import contextmanager
from os import listdir
from os.path import isfile, join
import subprocess
import signal
import time
import shutil

import termcolor
import yaml
from typing import List

OK_TAG = f"[ {termcolor.colored('OK', 'green')} ]"
KO_TAG = f"[ {termcolor.colored('KO', 'red')} ]"


@dataclass
class TestCase:
    name: str
    input: str = field(default_factory=lambda: "")
    type: str = field(default_factory=lambda: "")
    checks: List[str] = field(
        default_factory=lambda: ["stdout", "stderr", "exitcode", "err_msg"])
    arguments: List[str] = field(default_factory=lambda: [])


@dataclass
class TestCategory:
    name: str
    reference: str
    file: str


def raise_timeout(signum, frame):
    raise TimeoutError()


@contextmanager
def timeout(time):
    signal.signal(signal.SIGALRM, raise_timeout)

    signal.alarm(time)

    try:
        yield
    except TimeoutError:
        raise TimeoutError("!!timeout!!")
    finally:
        signal.signal(signal.SIGALRM, signal.SIG_IGN)


def diff(expected: str, actual: str) -> str:
    expected_lines = expected.splitlines(keepends=True)
    actual_lines = actual.splitlines(keepends=True)
    return ''.join(
        unified_diff(expected_lines, actual_lines, fromfile='expected',
                     tofile='actual'))


def perform_checks(expected: sp.CompletedProcess, actual: sp.CompletedProcess,
                   checks):
    res = ""
    if "err_msg" in checks and actual.stderr == "":
        res += "Something was expected on stderr\n"
    if "exitcode" in checks and expected.returncode != actual.returncode:
        res += f"Exited with {actual.returncode} expected {expected.returncode}\n"
    if "stdout" in checks and expected.stdout != actual.stdout:
        res += f"Stdout differ \n{diff(expected.stdout, actual.stdout)}\n"
    if "stderr" in checks and expected.stderr != actual.stderr:
        res += f"Stderr differ \n{diff(expected.stderr, actual.stderr)}\n"
    if len(res) > 0:
        res = res[:-1]
    return res


def run_shell(shell: str, stdin: str, arguments: List[str]) -> sp.CompletedProcess:
    return sp.run([shell] + arguments, input=stdin, capture_output=True, text=True)


def print_summary(passed: int, failed: int, start_time: float, end_time: float):
    if passed == 0:
        print(
            f"{termcolor.colored(f'========[SUMMARY ran {passed + failed} tests ', 'magenta')} | ",
            end='')
    elif failed == 0:
        print(
            f"{termcolor.colored(f'========[SUMMARY ran {passed + failed} tests ', 'green')} | ",
            end='')
    else:
        print(f"========[SUMMARY ran {passed + failed} tests ", end='')
    if passed == 0:
        print(f"{termcolor.colored(f'passed {passed}', 'magenta')} | ", end='')
    else:
        print(f"{termcolor.colored(f'passed {passed}', 'green')} | ", end='')
    if failed == 0:
        print(f"{termcolor.colored(f'failed {failed}', 'green')} | ", end='')
    else:
        if passed == 0:
            print(f"{termcolor.colored(f'failed {failed}', 'magenta')} | ",
                  end='')
        else:
            print(f"{termcolor.colored(f'failed {failed}', 'red')} | ", end='')

    if failed == 0:
        print(f"{termcolor.colored(f' 100%', 'green')} ", end='')
    elif passed == 0:
        print(f"{termcolor.colored(f' 0%', 'magenta')} ", end='')
    elif passed / (failed + passed) <= 0.5:
        print(
            f"{termcolor.colored(f' {round((passed / (failed + passed)) * 100, 2)}%', 'red')} ",
            end='')
    else:
        print(
            f"{termcolor.colored(f' {round((passed / (failed + passed)) * 100, 2)}%', 'yellow')} ",
            end='')

    if passed == 0:
        print(
            f"{termcolor.colored(f'in {round(end_time - start_time, 2)} secs]========', 'magenta')}")
    elif failed == 0:
        print(
            f"{termcolor.colored(f'in {round(end_time - start_time, 2)} secs]========', 'green')}")
    else:
        print(f"in {round(end_time - start_time, 2)} secs]========")


def get_categories(category: str, reference: str) -> List[TestCategory]:
    categlist: List[TestCategory] = []
    all_categories = [f[:-5] for f in listdir("./yaml_tests") if
                      isfile(join("./yaml_tests", f)) and (
                              join("./yaml_tests", f).endswith(
                                  ".yaml") or join("./yaml_tests",
                                                   f).endswith(".json"))]

    if category is not None:
        category = category.split(' ')
    else:
        category = all_categories

    for catego in category:
        if catego not in all_categories:
            print(
                f"Unknown category : {catego}\nChoose in all categories : {all_categories}")
            raise SyntaxError("Bad category name")
        ext = ".yaml"
        if not isfile(join("./yaml_tests", catego + ext)):
            ext = ".json"
        refer = reference
        if catego == "echo":
            refer = "bash"
        categlist.append(
            TestCategory(catego, refer, join("./yaml_tests", catego + ext)))
    return categlist


def copy_binary(build_path: Path, binary: Path):
    if os.path.exists(f"{build_path}/{binary}"):
        shutil.copy(f"{build_path}/{binary}", "./")
        if not os.path.exists(binary):
            raise FileNotFoundError(
                f"Tried to copy file but {binary.absolute()} not found")
    else:
        print(f"{termcolor.colored(f'Could found binary at {build_path}/{binary}', 'red')}")


def build_binary(binary: Path, build_path: Path):
    print(termcolor.colored("Building binary", 'blue'))
    os.system(f"cd {build_path} ; make {binary}")
    print(termcolor.colored("Done binary", 'blue'))

    print(termcolor.colored("Copying binary to test directory", 'blue'))


def main() -> int:
    parser = ArgumentParser("Testsuite")
    parser.add_argument("--binary", required=False, type=Path, default="42SH")
    parser.add_argument("--category", required=False, type=str)
    parser.add_argument("--reference", required=False, type=str, default="dash")
    parser.add_argument("--builddir", required=False, type=Path,
                        default="../../cmake-build-debug")
    parser.add_argument("--no_compile", required=False, action='store_true')
    parser.add_argument("--clean", required=False, action='store_true')
    args = parser.parse_args()

    binary_path = args.binary.absolute()
    ref = args.reference
    build_dir = args.builddir
    no_compile = args.no_compile
    if not no_compile:
        build_binary(args.binary, build_dir)

    copy_binary(build_dir, args.binary)

    categories: List[TestCategory] = get_categories(args.category, ref)

    print(f"Test categori(es) : {categories}")
    print(f"Testing {binary_path}")

    try:
        os.mkdir("trash")
    except FileExistsError:
        pass

    test_types = {
        "success": "stdout stderr exitcode",
        "failed": "stdout err_msg exitcode",
        "noerrcheck": "exitcode stdout"
    }

    passed = 0
    failed = 0
    start_time = time.perf_counter()
    for categ in categories:

        with open(f"{categ.file}", "r") as file:
            testsuite = []
            if categ.file.endswith(".yaml"):
                testsuite = [TestCase(**testcase) for testcase in
                             list(yaml.safe_load(file))]
        for testcase in testsuite:
            stdin = testcase.input
            name = testcase.name
            try:
                if testcase.type == "":
                    check = testcase.checks
                else:
                    check = test_types[testcase.type]
                with timeout(1):
                    dash_proc = run_shell(categ.reference, stdin, testcase.arguments)
                    sh_proc = run_shell(binary_path, stdin, testcase.arguments)
                    test_repport = perform_checks(dash_proc, sh_proc,
                                                  check)
            except Exception as err:
                failed += 1
                if type(err) == TimeoutError and str(err) == "!!timeout!!":
                    print(f"{KO_TAG} {categ.name} - {name}\nTest timedout\n")
                elif type(err) == KeyError:
                    print(f"{KO_TAG} {categ.name} - {name}\nWrong test type")
                else:
                    print(
                        f"{KO_TAG} {categ.name} - {name}\nWith:\nArguments : '{testcase.arguments}'\nInput '{stdin}'\n{err}\n")
            else:
                if len(test_repport) == 0:
                    passed += 1
                    print(f"{OK_TAG} {categ.name} - {name}")
                else:
                    failed += 1
                    print(
                        f"{KO_TAG} {categ.name} - {name}\nWith:\nArguments: '{testcase.arguments}'\nInput: '{stdin}'\n{test_repport}\n")
    end_time = time.perf_counter()
    print_summary(passed, failed, start_time, end_time)

    if args.clean:
        try:
            shutil.rmtree("trash")
            os.remove(binary_path)
        except Exception:
            pass
    return int(failed != 0)


if __name__ == "__main__":
    exit(main())
