/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryosuke Kanata
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "mutl.h"

struct mutl::ArgumentDescription descs[] =
{
    {"o" , 1},
    {"v" , 0},
    {"a" , 2},
    {"be", 0},
    {"bf", 1},
    {NULL, 7}
};

void ShowEnabled(mutl::ArgumentParser& parser, const char* option)
{
    printf("%s: %s\n", option, parser.IsEnabled(option) ? "Enabled" : "Disabled");
}

int main(int argc, const char* argv[])
{
    mutl::ArgumentParser parser;

    LOG("ArgmentCount=%d\n", argc);
    if (false == parser.Initialize(argc, argv, descs) )
    {
        LOG("Faied in initializing ... \n");
        return 0;
    }

    // テストパターン:
    //    -o sine a -p -v b -a c d e -be tan -bf 3 4 5 6 7

    CHECK_VALUE(parser.GetOptionCount(), 5);
    CHECK_VALUE(parser.GetArgumentCount(), 8);

    CHECK_VALUE(parser.IsEnabled("o") ,  true);
    CHECK_VALUE(parser.IsEnabled("p") , false);
    CHECK_VALUE(parser.IsEnabled("v") ,  true);
    CHECK_VALUE(parser.IsEnabled("be"),  true);
    CHECK_VALUE(parser.IsEnabled("bf"),  true);

    CHECK_VALUE(parser.GetOption("o").compare("sine"), 0);
    CHECK_VALUE(parser.GetOption("o", 1).empty(), true);
    CHECK_VALUE(parser.GetOption("o", 2).empty(), true);
    CHECK_VALUE(parser.GetOption("o", 100).empty(), true);

    CHECK_VALUE(parser.GetOption("v").empty(), true);
    CHECK_VALUE(parser.GetOption("v", 1).empty(), true);
    CHECK_VALUE(parser.GetOption("v", 2).empty(), true);
    CHECK_VALUE(parser.GetOption("v", 100).empty(), true);

    CHECK_VALUE(parser.GetOption("a").compare("c"), 0);
    CHECK_VALUE(parser.GetOption("a", 1).compare("d"), 0);
    CHECK_VALUE(parser.GetOption("a", 2).empty(), true);
    CHECK_VALUE(parser.GetOption("a", 100).empty(), true);

    CHECK_VALUE(parser.GetOption("be").empty(), true);
    CHECK_VALUE(parser.GetOption("be", 1).empty(), true);
    CHECK_VALUE(parser.GetOption("be", 2).empty(), true);
    CHECK_VALUE(parser.GetOption("be", 100).empty(), true);

    CHECK_VALUE(parser.GetOption("bf").compare("ab3"), 0);
    CHECK_VALUE(parser.GetOption("bf", 1).empty(), true);
    CHECK_VALUE(parser.GetOption("bf", 2).empty(), true);
    CHECK_VALUE(parser.GetOption("bf", 100).empty(), true);

    CHECK_VALUE(parser.GetArgument(0).compare("a"), 0);
    CHECK_VALUE(parser.GetArgument(1).compare("b"), 0);
    CHECK_VALUE(parser.GetArgument(2).compare("e"), 0);
    CHECK_VALUE(parser.GetArgument(3).compare("tan"), 0);
    CHECK_VALUE(parser.GetArgument(4).compare("4"), 0);
    CHECK_VALUE(parser.GetArgument(5).compare("5"), 0);
    CHECK_VALUE(parser.GetArgument(6).compare("6"), 0);
    CHECK_VALUE(parser.GetArgument(7).compare("7"), 0);
    CHECK_VALUE(parser.GetArgument(8).empty(), true);
    CHECK_VALUE(parser.GetArgument(100).empty(), true);

    return 0;
}
