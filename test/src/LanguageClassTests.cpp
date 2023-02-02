/**
 * @file LanguageClassTests.cpp
 * @author Simon Cahill (contact@simonc.eu)
 * @brief Contains the implementation of unit tests for testing the language class.
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023 Simon Cahill and Contributors.
 */

#include "borr/language.hpp"

#include <gtest/gtest.h>

class LanguageClassTests: public testing::Test, borr::language {
    protected:
        void SetUp() {
            m_language = borr::language();
        }

        borr::language m_language;
};