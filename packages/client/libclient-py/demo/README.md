[Japanese version](./README-ja.md)

# How to run the demo
The demo can be performed in the virtual environment which is installed QuickMPC-libClient-py via pip, assuming actual usage. Note that a different environment from the development Python environment is required.

### 1. Start up the container
Refer to the **Debugging Method** in [QuickMPC/scripts/README.md](../../../../scripts/README.md), and set up all QuickMPC containers in the dev stage.

### 2. Execute the following commands in this directory to establish the environment
```console
$ pipenv install --skip-lock
```
If you want to check the execution on a machine other than Python 3.7, run the following command before installing.
```console $ pipenv install --skip-lock
$ pipenv --rm # Remove the current environment.
$ pipenv --python=3.8 # For Python 3.8
$ pipenv --python=3.9 # for Python 3.9
```

### 3. Navigate and run in the directory
Demos for each operation can be run in `unit_demo/` and for the whole demo in `integration_demo/`.
Also, `send_share.py`, `send_asss_share.py` and `demo_sharize.py` require the file path to be specified in the command line argument.
```console
$ cd unit_demo/
$ pipenv run python send_share.py [file_path] # Run the demo of sending share
$ pipenv run python execute_computation.py # Run the demo of executing computation
```
Note that some unit_demos such as ``unit_demo/execute_computation.py`` depend on ID information obtained from send_share, etc., so you need to rewrite the ID accordingly.
