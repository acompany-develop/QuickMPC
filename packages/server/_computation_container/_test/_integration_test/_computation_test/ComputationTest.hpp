#pragma once
#include <iostream>
#include <string>
#include <thread>

#include "ConfigParse/ConfigParse.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Server/ManageToComputationContainer/Server.hpp"
#include "Test/IntegrationTest/MathTest.hpp"
#include "Test/IntegrationTest/MatrixTest.hpp"
#include "Test/IntegrationTest/MeshCodeTest.hpp"
#include "Test/IntegrationTest/ReadTripleFromBtsTest.hpp"
#include "Test/IntegrationTest/ShareTest.hpp"
#include "Test/IntegrationTest/ValueTableTest.hpp"
#include "unistd.h"

int main(int argc, char **argv);
