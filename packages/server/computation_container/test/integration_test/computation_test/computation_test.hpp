#pragma once
#include <iostream>
#include <string>
#include <thread>

#include "config_parse/config_parse.hpp"
#include "server/computation_to_computation_container/server.hpp"
#include "server/manage_to_computation_container/server.hpp"
#include "test/integration_test/join_table_test.hpp"
#include "test/integration_test/math_test.hpp"
#include "test/integration_test/matrix_test.hpp"
#include "test/integration_test/mesh_code_test.hpp"
#include "test/integration_test/read_triple_from_bts_test.hpp"
#include "test/integration_test/share_test.hpp"
#include "unistd.h"

int main(int argc, char **argv);
