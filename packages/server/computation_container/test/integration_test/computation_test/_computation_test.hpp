#pragma once
#include <iostream>
#include <string>
#include <thread>

#include "config_parse/config_parse.hpp"
#include "server/computation_to_computation_container/server.hpp"
#include "server/manage_to_computation_container/server.hpp"
#include "test/integration_test/MathTest.hpp"
#include "test/integration_test/MatrixTest.hpp"
#include "test/integration_test/MeshCodeTest.hpp"
#include "test/integration_test/ReadTripleFromBtsTest.hpp"
#include "test/integration_test/ShareTest.hpp"
#include "test/integration_test/ValueTableTest.hpp"
#include "unistd.h"

int main(int argc, char **argv);
