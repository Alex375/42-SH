# 42-SH


## Git usage 
When we need to merge branches we merge into the `dev-merge` branch which is subsequently pull into master.


# Tests
Integration test file is `tests/integration_test/testsuite.py`.

### Environment setup
- Generate en virtual environment named `env` (outside of AFS on Nuc)
```sh
$ python3 -m venv env
```
- Activate the virtual environment
```sh
$ source env/bin/activate
````
- Install the requiered package 
  - Go in your project directory
  - Then run :
```sh
$ cd tests/integration_test
$ pip install -r requirements.txt 
```
Your virtual environment is now setup to run the testsuite.

### Run the test suite
Test suite argument :
- `--binary` (required): binary file to test, if the binary exists in `cmake-build-debug` it will copy it
- `--reference` (optional) : reference program (`dash` by default)
- `--category` (optional) : categories of test to be run categories are files stored in`yaml_tests` (by default all categories are run)
Exemple :
```shell
$ python testsuite.py --binary 42SH
$
$ python testsuite.py --binary 42SH --category "echo sample"
```

### Add your test
If you want to add a category create a `yaml` file in `yaml_tests`.

Open the desired yaml file to add your test.

Tests are composed of 2 required fields:
- `name` : the name of the test
- `input` : the input of the test

There are 2 optional fields:
- `checks` : indicates which checks needs to be performed for the given test 4 checks are available:
  - `stdout` : check that the stdout of the reference is equal to the binary's stdout
  - `stderr` : check that the stderr of the reference is equal to the binary's stderr
  - `err_msg` : check that the binary wrote an error msg in stderr (doesn't check the content of stderr)
  - `exitcode` : check that the exit code of the reference is equal to the binary's exit code
- `type` : test type is a preset for the `checks` fields (`testtype` over wright `checks`) :
  - `success`: `stdout stderr exitcode`,
  - `failed`: `stdout err_msg exitcode`,
  - `noerrcheck`: `exitcode stdout`
- If none of the fields is indicated in the test, it will perform all the checks by default.

### Test examples
Simple test
```yaml
- name: Echo basic
  input: echo coucou
  type: success
```
Is equivalent to
```yaml
- name: Echo basic
  input: echo coucou
  checks: stdout stderr exitcode
```
Test with fail (we want to check the exitcode the stdout and that the program wrote any kind of error message in stderr)
````yaml
- name: Pipe no end
  input: echo coucou |
  checks: stdout err_msg exitcode
````
Is equivalent to
````yaml
- name: Pipe no end
  input: echo coucou |
  type: failed
````
Multiline input
````yaml
- name: Simple if clausure
  input: |
    if true
    then
      echo coucou
    else
      exit 1; fi
  type: success
````