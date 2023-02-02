/**
 * @file TestMain.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the entry point for the GTest suite,
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors.
 */

#include <gtest/gtest.h>

int main(int32_t argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}