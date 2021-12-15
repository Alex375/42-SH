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
    input: str = field(default_factory=lambda: "")
    type: str = field(default_factory=lambda: "")
    checks: List[str] = field(
        default_factory=lambda: ["stdout", "stderr", "exitcode", "err_msg"])
    arguments: List[str] = field(default_factory=lambda: [])
    timeout: int = field(default_factory=lambda: 1)


@dataclass
class TestResult:
    passed: bool
    testcase: TestCase
    stdout: str
    stderr: str
    exitcode: int
    refstdout: str
    refstderr: str
    refexitcode: int
    category: str
    exception: Exception = None


@dataclass
class TestCategory:
    name: str
    reference: str
    file: str


def raise_timeout(_, __):
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


def get_testres(expected: sp.CompletedProcess, actual: sp.CompletedProcess,
                testcase: TestCase, category: str) -> TestResult:
    testres = TestResult(
        passed=True,
        testcase=testcase,
        stdout=actual.stdout,
        stderr=actual.stderr,
        exitcode=actual.returncode,
        refstdout=expected.stdout,
        refstderr=expected.stderr,
        refexitcode=expected.returncode,
        category=category
    )
    if "err_msg" in testres.testcase.checks and testres.stderr == "":
        testres.passed = False
    if "exitcode" in testres.testcase.checks and testres.refexitcode != testres.exitcode:
        testres.passed = False
    if "stdout" in testres.testcase.checks and testres.stdout != testres.refstdout:
        testres.passed = False
    if "stderr" in testres.testcase.checks and testres.stderr != testres.refstderr:
        testres.passed = False
    if testres.exception is not None:
        testres.passed = False
    return testres


def format_test(testres: TestResult) -> str:
    res = ""

    if testres.passed:
        res += f"{OK_TAG} {testres.category} - {testres.testcase.name}"
        return res
    if testres.exception is not None:
        if type(testres.exception) == TimeoutError and str(testres.exception) == "!!timeout!!":
            res += f"{KO_TAG} {testres.category} - {testres.testcase.name}\nTest timedout\n"
        elif type(testres.exception) == KeyError:
            res += f"{KO_TAG} {testres.category} - {testres.testcase.name}\nWrong test type"
        else:
            res += f"{KO_TAG} {testres.category} - {testres.testcase.name}\nWith:\nArguments : '{testres.testcase.arguments}'\nInput '{testres.testcase.input}'\n{testres.exception}\n"
        return res
    else:
        res += f"{KO_TAG} {testres.category} - {testres.testcase.name}\nWith:\nArguments: '{testres.testcase.arguments}'\nInput: '{testres.testcase.input}'\n"

    if "err_msg" in testres.testcase.checks and testres.stderr == "":
        res += "Something was expected on stderr\n"
    if "exitcode" in testres.testcase.checks and testres.refexitcode != testres.exitcode:
        res += f"Exited with {testres.exitcode} expected {testres.refexitcode}\n"
    if "stdout" in testres.testcase.checks and testres.stdout != testres.refstdout:
        res += f"Stdout differ \n{diff(testres.refstdout, testres.stdout)}\n"
    if "stderr" in testres.testcase.checks and testres.stderr != testres.refstderr:
        res += f"Stderr differ \n{diff(testres.refstderr, testres.stderr)}\n"
    if len(res) > 0:
        res = res[:-1]

    return res


def run_shell(shell: str, stdin: str,
              arguments: List[str]) -> sp.CompletedProcess:
    return sp.run([shell] + arguments, input=stdin, capture_output=True,
                  text=True)


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
        print(
            f"{termcolor.colored(f'Could found binary at {build_path}/{binary}', 'red')}")


def build_binary(binary: Path, build_path: Path):
    print(termcolor.colored("Building binary", 'blue'))
    os.system(f"cd {build_path} ; make {binary}")
    print(termcolor.colored("Done binary", 'blue'))

    print(termcolor.colored("Copying binary to test directory", 'blue'))


def main() -> int:
    parser = ArgumentParser("Testsuite")
    parser.add_argument("--binary", required=False, type=Path, default="42SH", help="Name of the binary to be tested")
    parser.add_argument("--category", required=False, type=str, help="categories to be tested")
    parser.add_argument("--reference", required=False, type=str, default="dash", help="reference for tests")
    parser.add_argument("--builddir", required=False, type=Path,
                        default="../../cmake-build-debug", help="directory of the build where binary is")
    parser.add_argument("--no_compile", required=False, action='store_true', help="doesn't compile the target")
    parser.add_argument("--clean", required=False, action='store_true', help="clean all temporary files after execution")
    parser.add_argument("--only_failed", required=False, action='store_true', help="only print failed tests")
    parser.add_argument("--raise_exception", required=False, action='store_true', help="raise exception if not all test passes")
    args = parser.parse_args()

    binary_path = args.binary.absolute()
    ref = args.reference
    build_dir = args.builddir
    no_compile = args.no_compile
    only_failed = args.only_failed
    raise_exception = args.raise_exception
    try:
        os.system(f"cat yaml_tests/sample_script/.hidden")
    except Exception:
        pass

    if not no_compile:
        build_binary(args.binary, build_dir)

    copy_binary(build_dir, args.binary)

    categories: List[TestCategory] = get_categories(args.category, ref)

    for c in categories:
        print(f"{c}")
    print(f"Testing {binary_path}")

    try:
        os.mkdir("trash")
    except FileExistsError:
        pass

    test_types = {
        "success": ["stdout", "stderr", "exitcode"],
        "failed": ["stdout", "err_msg", "exitcode"],
        "noerrcheck": ["exitcode", "stdout"]
    }

    passed = 0
    failed = 0
    start_time = time.perf_counter()
    test_results: List[TestResult] = []
    for categ in categories:

        print(f"{termcolor.colored(f'-----Testing {categ.name}-----', 'blue')}")

        with open(f"{categ.file}", "r") as file:
            testsuite = []
            if categ.file.endswith(".yaml"):
                testsuite = [TestCase(**testcase) for testcase in
                             list(yaml.safe_load(file))]
        for testcase in testsuite:
            stdin = testcase.input
            for key, value in test_types.items():
                if key == testcase.type:
                    testcase.checks = value
            try:
                with timeout(testcase.timeout):
                    dash_proc = run_shell(categ.reference, stdin,
                                          testcase.arguments)
                    sh_proc = run_shell(binary_path, stdin, testcase.arguments)
                    test_result = get_testres(dash_proc, sh_proc, testcase, categ.name)
                    test_results.append(test_result)
                    if not only_failed or not test_result.passed:
                        print(format_test(test_result))
            except Exception as err:
                test_result = TestResult(
                    False,
                    testcase,
                    None,
                    None,
                    None,
                    None,
                    None,
                    None,
                    categ.name,
                    err
                )
                test_results.append(test_result)
                print(format_test(test_result))
    end_time = time.perf_counter()
    for test in test_results:
        if test.passed:
            passed += 1
        else:
            failed += 1
    print_summary(passed, failed, start_time, end_time)

    if args.clean:
        try:
            shutil.rmtree("trash")
            os.remove(binary_path)
        except Exception:
            pass
    assert not raise_exception or failed == 0
    return int(failed != 0)


if __name__ == "__main__":
    exit(main())
