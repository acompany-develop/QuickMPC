# Test
====
The directory containing the test code for all services and docker-compose.yml to run them.

## How to develop docker-compose.yml
The following stage name is used for Docker's Image.
| stage name | purpose                                                                                                          |
| :--------- | :--------------------------------------------------------------------------------------------------------------- |
| src        | For developing under src/.                                                                                       |
| dev        | For building 3 units at the same time                                                                            |
| small      | For CI's small test                                                                                              |
| medium     | For CI's medium test                                                                                             |
| large      | For CI's large test                                                                                              |
| dep        | For deployment                                                                                                   |
| builder    | Base stage where all source code is built in Image                                                               |
| *-runner   | Suffixes to add when you want to make it clear that this is a lightweight `alpine` image for execution purposes. |

Specify which stage Image to use in docker-compose.yml as follows.
```yaml
...

services:
  <stage_name>_<container_name>:
    ...
    build:
      context: ../
      dockerfile: src/HogeContainer/Dockerfile
      target: src # dev, dep, small, medium, large, etc...
    ...
```


## How to run test suites
Run the following commands in `Test/`
```sh
make test
```
If you want to specify a specific test to run, do the following command. Multiple options can be specified. <br>

### Options <br>
t = hoge	# Run test under hoge<br>
p = huga	# Run huga*test.sh<br>
m = build	# Run only build() of test.sh<br>
m = run		# Run only run() of test.sh<br>

```sh
make test t=./ComputationContainer/ #  Run the test directly under Test/ComputationContainer/
make test p=small # Run `small*test.sh`
make test m=build # Run only the build process of `*test.sh`
make test m=run # Run only the run process of `*test.sh`
make test t=./ComputationContainer/ p=small m=run # Run only the `run` process in small*test.sh under Test/ComputationContainer/
```

## Adding Tests
Create test.sh in the following format.

NOTE: The internal processing of the function is always written in a one-liner to take the AND of exit_status.

(例
```
build() {
    docker-compose -f docker-compose.yml pull dev_bts && \
    docker-compose -f docker-compose.yml build medium_cc1 medium_cc2 medium_cc3
}
```

```sh
# !!!!!!!!!! NOTE: The internal processing of the function is always written in a one-liner to take the AND of exit_status !!!!!!!!!!


# Function to describe the build process
build() {
	# Describe build process here
	docker-compose -f docker-compose.yml build small_cc
}

# Function describing the setup process to be executed before run
# INFO: Initialization is performed to enable the run to be executed idempotency
setup() {
	# Describe run preprocessing here
	docker-compose -f docker-compose.yml down -v
}

# Function to describe the run process
# NOTE: This function is an exception and does not have to be written in a one-liner
run() {
	# Describe the `test` execution process here
	docker-compose -f docker-compose.yml run small_cc /bin/sh -c "cd /QuickMPC && bazel test //Test/UnitTest:all --test_env=IS_TEST=true --test_output=errors"
}

# Function describing the teardown process to be executed after run
# INFO: Initialize without leaving side effects after run
teardown() {
	# Describe post-processing of run here
	docker-compose -f docker-compose.yml down -v
}
```

## How to debug
Run the following commands in `Test/`
```sh
make debug # same as `make debug p=mc`
```
If you want to specify a specific debug.sh to run, do the following command. Multiple options can be specified.<br>

### Options <br>
t = hoge	# Run test under hoge<br>
p = huga	# Run huga*debug.sh<br>
m = build	# Run only build() of debug.sh<br>
m = run		# Run only run() of debug.sh<br>

```sh
make debug t=./ComputationContainer/ # Run the debugging directly under Test/
make debug p=cc # Run `cc*debug.sh`
make debug m=build # Run only the build process of `*debug.sh`
make debug m=run # Run only the run processes of `*debug.sh`
make debug t=./ComputationContainer/ m=run # Run only the `run` process in debug.sh under Test/ComputationContainer/
```

## テストの追加
Create debug.sh in the following format.
```sh
build() {
	# Describe build process here
	docker-compose -f docker-compose.yml build dev_cc1 dev_cc2 dev_cc3
}

setup() {
	# Describe run preprocessing here
	docker-compose -f docker-compose.yml down -v
}

run() {
	# Describe debug execution process here
	docker-compose -f docker-compose.yml up dev_cc1 dev_cc2 dev_cc3
}
```
