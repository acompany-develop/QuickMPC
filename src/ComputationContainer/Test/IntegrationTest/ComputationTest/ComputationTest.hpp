#pragma once
#include <iostream>
#include <string>
#include <thread>
#include "unistd.h"

#include "ConfigParse/ConfigParse.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Server/ManageToComputationContainer/Server.hpp"
#include "Test/IntegrationTest/ShareCompTest.hpp"  // これを1番最初に実行しないとIncrementIdで落ちる
#include "Test/IntegrationTest/ShareTest.hpp"
#include "Test/IntegrationTest/MathTest.hpp"
#include "Test/IntegrationTest/MatrixTest.hpp"
#include "Test/IntegrationTest/MeshCodeTest.hpp"
#include "Test/IntegrationTest/ModelTest.hpp"
#include "Test/IntegrationTest/OptimizeTest.hpp"
#include "Test/IntegrationTest/GBDTTest.hpp"
#include "Test/IntegrationTest/ReadTripleFromBtsTest.hpp"
#include "Test/IntegrationTest/ValueTableTest.hpp"
#include "Test/IntegrationTest/ComputationToDbGateTest.hpp"
#include "Test/IntegrationTest/AnyToDbGateTest.hpp"

int main(int argc, char **argv);
