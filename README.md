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
- `--binary` (optional): binary file to test (default 42SH)
- `–builddir`(optional) : build directory of the project (default `../../cmake-build-debug`)
- `--reference` (optional) : reference program (`dash` by default)
- `--category` (optional) : categories of test to be run categories are files stored in`yaml_tests` (by default all categories are run)
- `--no_compile` (optional): if present the test suite will no try to compile the project

If the `--no_compile` isn’t present the test suite will compile the project with the following instructions:

```shell
$ cd <builddir>
$ make <binary>
```

Then it will try to copy `<builddir>/<binary>` in the test directory (no errors is raised if it failed to copy)(error is raised if the binary isn’t present in the test directory after all the compiling and copying procedures)




Exemple :

```shell
$ python testsuite.py --binary
$
$ python testsuite.py --binary 42SH --category "echo sample"
$
$ python testsuite.py --baniry 42sh --builddir builddir --no_compile
```

### Add your test
If you want to add a category create a `yaml` file in `yaml_tests`.

Open the desired yaml file to add your test.

Tests are composed of 1 required fields:
- `name` : the name of the test

There are 4 optional fields:

-   `input` : stdin of the test (default empty)
-   `arguments`: List of arguments passe to the program (syntax see last test examples)(default empty)

- `checks` : indicates which checks needs to be performed for the given test 4 checks are available:
  
  - `stdout` : check that the stdout of the reference is equal to the binary's stdout
  - `stderr` : check that the stderr of the reference is equal to the binary's stderr
  - `err_msg` : check that the binary wrote an error msg in stderr (doesn't check the content of stderr)
  - `exitcode` : check that the exit code of the reference is equal to the binary's exit code
  
- `type` : test type is a preset for the `checks` fields (`testtype` over wright `checks`) :
  - `success`: `stdout stderr exitcode`,
  - `failed`: `stdout err_msg exitcode`,
  - `noerrcheck`: `exitcode stdout`
  
  __If `type` and `ceck` aren’t indicates in the test, it will perform all the checks by default.__

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

Test with arguments

```yaml
- name: Test script
  arguments: 
  	- ../../debug.sh 
  	- arg1 
  	- space    arg
  type: success
```

