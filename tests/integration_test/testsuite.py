from argparse import ArgumentParser
from pathlib import Path
import subprocess as sp
from difflib import unified_diff
from dataclasses import dataclass, field
from contextlib import contextmanager
import signal

import termcolor
import yaml
from typing import List

OK_TAG = f"[ {termcolor.colored('OK', 'green')} ]"
KO_TAG = f"[ {termcolor.colored('ko', 'red')} ]"


@dataclass
class TestCase:
    name: str
    input: str
    checks: List[str] = field(default_factory=lambda: ["stdout", "stderr", "exitcode", "err_msg"])


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
    return ''.join(unified_diff(expected_lines, actual_lines, fromfile='excpected', tofile='actual'))


def perform_checks(excpected: sp.CompletedProcess, actual: sp.CompletedProcess, checks):
    res = ""
    if "err_msg" in checks and actual.stderr == "":
        res += "Something was expected on stderr\n"
    if "exitcode" in checks and excpected.returncode != actual.returncode:
        res += f"Exited with {actual.returncode} expected {excpected.returncode}\n"
    if "stdout" in checks and excpected.stdout != actual.stdout:
        res += f"Stdout differ \n{diff(excpected.stdout, actual.stdout)}\n"
    if "stderr" in checks and excpected.stderr != actual.stderr:
        res += f"Stderr differ \n{diff(excpected.stdout, actual.stderr)}\n"
    if len(res) > 0:
        res = res[:-1]
    return res


def run_shell(shell: str, stdin: str) -> sp.CompletedProcess:
    return sp.run([shell], input=stdin, capture_output=True, text=True)


if __name__ == "__main__":
    parser = ArgumentParser("Testsuite")
    parser.add_argument("--binary", required=True, type=Path)
    args = parser.parse_args()

    binary_path = args.binary.absolute()
    print(f"Testing {binary_path}")

    with open("yaml_tests/test.yaml", "r") as file:
        testsuite = [TestCase(**testcase) for testcase in list(yaml.safe_load(file))]

    for testcase in testsuite:
        stdin = testcase.input
        name = testcase.name

        try:
            with timeout(1):
                dash_proc = run_shell("dash", stdin)
                sh_proc = run_shell(binary_path, stdin)
                test_repport = perform_checks(dash_proc, sh_proc, testcase.checks)
        except Exception as err:
            if type(err) == TimeoutError and str(err) == "!!timeout!!":
                print(f"{KO_TAG} {name}\nTest timedout")
            else:
                print(f"{KO_TAG} {name} with arguments : {stdin}\n{err}")
        else:
            if len(test_repport) == 0:
                print(f"{OK_TAG} {name}")
            else:
                print(f"{KO_TAG} {name} with arguments : {stdin}\n{test_repport}")
