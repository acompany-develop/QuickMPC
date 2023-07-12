[Japanese version](./README-ja.md)

# How to run the demo
The demo can be performed in the virtual environment which is installed QuickMPC-libClient-py via pip, assuming actual usage. Note that a different environment from the development Python environment is required.

### 1. Start up the container
Refer to the **Debugging Method** in [QuickMPC/scripts/README.md](../../scripts/README.md), and set up all QuickMPC containers in the dev stage.

### 2. Execute the following commands in this directory to establish the environment
```console
$ pip install quickmpc
```

### 3. Navigate and run in the directory
```console
$ cd scripts/
$ python execute_demo.py
$ python join_demo.py
```
