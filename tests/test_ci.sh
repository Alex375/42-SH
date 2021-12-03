#!/bin/sh

cd tests/integration_test/

python3 -m venv env

source env/bin/activate

pip install -r requirements.txt

python testsuite.py --builddir ../../builddir --binary 42sh --no_compile

deactivate