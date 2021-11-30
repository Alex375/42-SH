import os.path
from argparse import ArgumentParser
from pathlib import Path
import subprocess as sp
from difflib import unified_diff
from dataclasses import dataclass, field
from contextlib import contextmanager
from os import listdir
from os.path import isfile, join
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
    input: str
    type: str = field(
        default_factory=lambda: "")
    checks: List[str] = field(
        default_factory=lambda: ["stdout", "stderr", "exitcode", "err_msg"])


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
        res += f"Stderr differ \n{diff(expected.stdout, actual.stderr)}\n"
    if len(res) > 0:
        res = res[:-1]
    return res


def run_shell(shell: str, stdin: str) -> sp.CompletedProcess:
    return sp.run([shell], input=stdin, capture_output=True, text=True)


def print_summary(passed: int, failed: int, start_time: float, end_time: float):
    if passed == 0:
        print(f"{termcolor.colored(f'[========SUMMARY ran {passed + failed} tests ', 'magenta')} | ", end='')
    elif failed == 0:
        print(f"{termcolor.colored(f'[========SUMMARY ran {passed + failed} tests ', 'green')} | ", end='')
    else:
        print(f"[========SUMMARY ran {passed + failed} tests ", end='')
    if passed == 0:
        print(f"{termcolor.colored(f'passed {passed}', 'magenta')} | ", end='')
    else:
        print(f"{termcolor.colored(f'passed {passed}', 'green')} | ", end='')
    if failed == 0:
        print(f"{termcolor.colored(f'failed {failed}', 'green')} | ", end='')
    else:
        if passed == 0:
            print(f"{termcolor.colored(f'failed {failed}', 'magenta')} | ", end='')
        else:
            print(f"{termcolor.colored(f'failed {failed}', 'red')} | ", end='')

    if failed == 0:
        print(f"{termcolor.colored(f' 100%', 'green')} ", end='')
    elif passed == 0:
        print(f"{termcolor.colored(f' 0%', 'magenta')} ", end='')
    elif passed / (failed + passed) <= 0.5:
        print(f"{termcolor.colored(f' {round((passed / (failed + passed)) * 100, 2)}%', 'red')} ", end='')
    else:
        print(f"{termcolor.colored(f' {round((passed / (failed + passed)) * 100, 2)}%', 'yellow')} ", end='')

    if passed == 0:
        print(f"{termcolor.colored(f'in {round(end_time - start_time, 2)} secs ========]', 'magenta')}", end='')
    elif failed == 0:
        print(f"{termcolor.colored(f'in {round(end_time - start_time, 2)} secs ========]', 'green')}", end='')
    else:
        print(f"in {round(end_time - start_time, 2)} secs ========]", end='')


if __name__ == "__main__":
    parser = ArgumentParser("Testsuite")
    parser.add_argument("--binary", required=True, type=Path)
    parser.add_argument("--category", required=False, type=str)
    parser.add_argument("--reference", required=False, type=str)
    args = parser.parse_args()

    binary_path = args.binary.absolute()
    categories = args.category
    ref = args.reference

    if os.path.exists(f"../../cmake-build-debug/{args.binary}"):
        shutil.copy(f"../../cmake-build-debug/{args.binary}", "./")
        if not os.path.exists(binary_path):
            raise FileNotFoundError(f"Tried to copy file but {binary_path} not found")

    if ref is None:
        ref = "dash"

    if categories is not None:
        categories = categories.split(' ')

    all_categories = [f[:-5] for f in listdir("./yaml_tests") if isfile(join("./yaml_tests", f)) and join("./yaml_tests", f).endswith(".yaml")]

    if categories is None:
        categories = all_categories

    for categ in categories:
        if categ not in all_categories:
            print(f"Unknown category : {categ}\nChoose in all categories : {all_categories}")
            raise SyntaxError("Bad category name")

    print(f"Categori(es) : {categories}")
    print(f"Testing {binary_path}")

    passed = 0
    failed = 0
    start_time = time.perf_counter()

    test_types = {
        "success": "stdout stderr exitcode",
        "failed": "stdout err_msg exitcode",
        "noerrcheck": "exitcode stdout"
    }

    for categ in categories:

        with open(f"yaml_tests/{categ}.yaml", "r") as file:
            testsuite = [TestCase(**testcase) for testcase in
                         list(yaml.safe_load(file))]
        if categ == "echo":
            ref = "bash"
        else:
            ref = args.reference
            if ref is None:
                ref = "dash"
        for testcase in testsuite:
            stdin = testcase.input
            name = testcase.name
            try:
                if testcase.type == "":
                    check = testcase.checks
                else:
                    check = test_types[testcase.type]
                with timeout(1):
                    dash_proc = run_shell(ref, stdin)
                    sh_proc = run_shell(binary_path, stdin)
                    test_repport = perform_checks(dash_proc, sh_proc,
                                                  check)
            except Exception as err:
                failed += 1
                if type(err) == TimeoutError and str(err) == "!!timeout!!":
                    print(f"{KO_TAG} {categ} - {name}\nTest timedout")
                elif type(err) == KeyError:
                    print(f"{KO_TAG} {categ} - {name}\nWrong test type")
                else:
                    print(f"{KO_TAG} {categ} - {name}\nWith arguments : '{stdin}'\n{err}\n")
            else:
                if len(test_repport) == 0:
                    passed += 1
                    print(f"{OK_TAG} {categ} - {name}")
                else:
                    failed += 1
                    print(
                        f"{KO_TAG} {categ} - {name}\nWith arguments : '{stdin}'\n{test_repport}\n")
    end_time = time.perf_counter()
    print_summary(passed, failed, start_time, end_time)
