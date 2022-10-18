#pragma once
#include <iostream>
#include <string>
#include <thread>

#include "ConfigParse/ConfigParse.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Server/ManageToComputationContainer/Server.hpp"
#include "unistd.h"
// clang-format off
#include "Test/IntegrationTest/ShareCompTest.hpp"
// clang-format on
#include "Test/IntegrationTest/GBDTTest.hpp"
#include "Test/IntegrationTest/MathTest.hpp"
#include "Test/IntegrationTest/MatrixTest.hpp"
#include "Test/IntegrationTest/MeshCodeTest.hpp"
#include "Test/IntegrationTest/ModelTest.hpp"
#include "Test/IntegrationTest/OptimizeTest.hpp"
#include "Test/IntegrationTest/ReadTripleFromBtsTest.hpp"
#include "Test/IntegrationTest/ShareTest.hpp"
#include "Test/IntegrationTest/ValueTableTest.hpp"

int main(int argc, char **argv);
