# QuickMPC
[![Build and Run Tests on CC, MC, and BTS Containers](https://github.com/acompany-develop/QuickMPC/actions/workflows/test-server-all.yml/badge.svg)](https://github.com/acompany-develop/QuickMPC/actions/workflows/test-server-all.yml)
[![Syntax Check and Unit Test on libclient-py](https://github.com/acompany-develop/QuickMPC/actions/workflows/test-client-libclient-py.yml/badge.svg)](https://github.com/acompany-develop/QuickMPC/actions/workflows/test-client-libclient-py.yml)

<img src="https://user-images.githubusercontent.com/1694907/182115030-90fda7cf-068a-48bb-ba50-ee12be6af0b4.png" width=128>

[Japanese version](./README-ja.md)

A practical engine for Secure Multiparty Computation (SMPC) that is part of privacy-enhancing computation (PEC). QuickMPC can perform computations on personal data used in fundamental static calculations while keeping confidentiality.

This will enable businesses and engineers worldwide to use Python client libraries to easily develop SaaS products that utilize SMPC functions without the need for expert SMPC knowledge.

![oss_eg](https://user-images.githubusercontent.com/1694907/182254973-ee3092a6-ee28-49bb-aaf6-637225271a0b.png)

## Dependent Tools/Commands
- `make`
- `docker`
- `docker-compose`
- [`vscode-yaml-sort`](https://marketplace.visualstudio.com/items?itemName=PascalReitermann93.vscode-yaml-sort)

## Getting Started
[The demonstration with libClient-py](packages/client/libclient-py/demo/README.md)

## Development Requirements
### Recommended

- CPU: `4 Core`
- memory: `64 GB`
- storage: `64 GB`

## Run all test suite
1. Change your current directory into `scripts/`
```sh
cd QuickMPC/scripts
```
2. Launch all containers
```sh
make debug t=./manage_container
```
3. Execute libClient Demo
- [libClient-py/demo](packages/client/libclient-py/demo/README.md)

## How to test each container
[Detail](scripts/README.md)
1. Change your current directory into `scripts/`
```sh
cd QuickMPC/scripts
```
2. Run test
```sh
# Testing all containers
make test
# Only the Computation Container test
make test t=./computation_container/
# Only the Manage Container test
make test t=./manage_container/
```

## How to start each container
This section is usefule when you want to stand 2 containers and dare to test manually.
```sh
make debug t=./computation_container # Launch of the dev_cc{1,2,3}
make debug t=./manage_container # Then, all container is launched
```

## How to develop each container
### Computation Container
[Detail](packages/server/computation_container/README.md)
1. Change your current directory
```sh
cd packages/server/computation_container
```
2. Start the container and enter the container
```sh
make upd
make login
cd QuickMPC
```

3. Develop and bulid and test
```sh
# build
bazel build //:all
# test
bazel test ... --test_env=IS_TEST=true --test_output=errors
```

#### Delete temporary files generated by Bazel
The files generated by bazel can be removed by running the following in the `/QuickMPC` directory in the container

```
bazel clean
```

### Manage Container

[Detail](packages/server/manage_container/README.md)
1. Change your current directory
```sh
cd packages/server/manage_container
```
2. Start the container and enter the container
```sh
make upd
make login
```
3. Develop and bulid and test
```sh
# Create a vendor directory directly under QuickMPC and copy all dependent packages
go mod vendor
# build
go build
# test
go test ... -v
```
