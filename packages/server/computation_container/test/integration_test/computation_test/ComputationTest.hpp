#pragma once
#include <iostream>
#include <string>
#include <thread>

#include "config_parse/config_parse.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Server/ManageToComputationContainer/Server.hpp"
#include "test/integration_test/MathTest.hpp"
#include "test/integration_test/MatrixTest.hpp"
#include "test/integration_test/MeshCodeTest.hpp"
#include "test/integration_test/ReadTripleFromBtsTest.hpp"
#include "test/integration_test/ShareTest.hpp"
#include "test/integration_test/ValueTableTest.hpp"
#include "unistd.h"

int main(int argc, char **argv);
