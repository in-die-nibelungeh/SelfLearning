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
    {"o" , 1}
};

void ShowEnabled(mutl::ArgumentParser& parser, const char* option)
{
    printf("%s: %s\n", option, parser.IsEnabled(option) ? "Enabled" : "Disabled");
}

int main(int argc, const char* argv[])
{
    mutl::ArgumentParser parser;

    // Display messages real-time
    setvbuf(stdout, NULL, _IONBF, 0);

    LOG("ArgmentCount=%d\n", argc);
    if ( false == parser.Initialize(argc, argv, descs, sizeof(descs)/sizeof(struct mutl::ArgumentDescription)) )
    {
        LOG("Faied in initializing ... \n");
        return 0;
    }

    {
        const std::string path("/c/drive/usb/main.cpp");
        const mutl::NodePath filer(path);

        CHECK_VALUE(filer.GetFullpath().compare(path), 0);
        CHECK_VALUE(filer.GetDirname().compare("/c/drive/usb"), 0);
        CHECK_VALUE(filer.GetBasename().compare("main"), 0);
        CHECK_VALUE(filer.GetFilename().compare("main.cpp"), 0);
        CHECK_VALUE(filer.GetExtension().compare("cpp"), 0);
    }

    {
        const std::string path("../../ding.dong.wav");
        const mutl::NodePath filer(path);

        CHECK_VALUE(filer.GetFullpath().compare(path), 0);
        CHECK_VALUE(filer.GetDirname().compare("../.."), 0);
        CHECK_VALUE(filer.GetBasename().compare("ding.dong"), 0);
        CHECK_VALUE(filer.GetFilename().compare("ding.dong.wav"), 0);
        CHECK_VALUE(filer.GetExtension().compare("wav"), 0);
    }

    {
        const std::string path("/c/../../Makefile");
        const mutl::NodePath filer(path);

        CHECK_VALUE(filer.GetFullpath().compare(path), 0);
        CHECK_VALUE(filer.GetDirname().compare("/c/../.."), 0);
        CHECK_VALUE(filer.GetBasename().compare("Makefile"), 0);
        CHECK_VALUE(filer.GetFilename().compare("Makefile"), 0);
        CHECK_VALUE(filer.GetExtension().empty(), true);
    }

    {
        const std::string path("Makefile");
        const mutl::NodePath filer(path);

        CHECK_VALUE(filer.GetFullpath().compare(path), 0);
        CHECK_VALUE(filer.GetDirname().compare("."), 0);
        CHECK_VALUE(filer.GetBasename().compare("Makefile"), 0);
        CHECK_VALUE(filer.GetFilename().compare("Makefile"), 0);
        CHECK_VALUE(filer.GetExtension().empty(), true);
    }

    {
        const std::string path("../dot.files/Makefile.");
        const mutl::NodePath filer(path);

        CHECK_VALUE(filer.GetFullpath().compare(path), 0);
        CHECK_VALUE(filer.GetDirname().compare("../dot.files"), 0);
        CHECK_VALUE(filer.GetBasename().compare("Makefile"), 0);
        CHECK_VALUE(filer.GetFilename().compare("Makefile"), 0);
        CHECK_VALUE(filer.GetExtension().empty(), true);
    }

    {
        const std::string path("~/.bashrc");
        const mutl::NodePath filer(path);

        CHECK_VALUE(filer.GetFullpath().compare(path), 0);
        CHECK_VALUE(filer.GetDirname().compare("~"), 0);
        CHECK_VALUE(filer.GetBasename().empty(), true);
        CHECK_VALUE(filer.GetFilename().compare(".bashrc"), 0);
        CHECK_VALUE(filer.GetExtension().compare("bashrc"), 0);
    }

    if (!parser.IsEnabled("o"))
    {
        return 0;
    }

    std::string path = parser.GetOption("o");

    mutl::NodePath filer;

    filer.Initialize(path);

    DEBUG_LOG("full: %s\n", filer.GetFullpath().c_str());
    DEBUG_LOG("base: %s\n", filer.GetBasename().c_str());
    DEBUG_LOG("dir : %s\n", filer.GetDirname().c_str());
    DEBUG_LOG("ext : %s\n", filer.GetExtension().c_str());

    return 0;
}
