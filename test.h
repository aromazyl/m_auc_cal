/*
 * test.h
 * Copyright (C) 2016 zhangyule <zhangyule@cmcm.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TEST_H
#define TEST_H

#define public public
#define protected public
#define private public

#include <gtest/gtest.h>
#define TEST_MAIN \
  int main(int argc, char* argv[]) { \
    testing::InitGoogleTest(&argc, argv); \
    return RUN_ALL_TESTS(); \
  }

#endif /* !TEST_H */
