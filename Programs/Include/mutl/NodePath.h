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

#pragma once

#include <string>  // std::string

namespace mutl {

class NodePath
{
public:
    NodePath()
        : m_Basename()
        , m_Dirname(".")
        , m_Extension()
        , m_Fullpath()
        , m_IsInitialized(false)
    {}

    NodePath(const std::string& path)
        : m_Basename()
        , m_Dirname(".")
        , m_Extension()
        , m_Fullpath()
        , m_IsInitialized(false)
    {
        Initialize(path);
    }

    NodePath(const char* path)
        : m_Basename()
        , m_Dirname(".")
        , m_Extension()
        , m_Fullpath()
        , m_IsInitialized(false)
    {
        Initialize(std::string(path));
    }

    ~NodePath()
    {}

    void Initialize(const std::string& path)
    {
        m_Fullpath = path;

        // Repalce '\' to '/' so that easy to process.
        for (size_t pos = m_Fullpath.find_first_of("\\");
             pos != std::string::npos;
             pos = m_Fullpath.find_first_of("\\") )
        {
            m_Fullpath.replace(pos, 1, "/");
        }
        // Remove '/' at the end of the string.
        if ( m_Fullpath.length() > 0
            && m_Fullpath.at(m_Fullpath.length() - 1) == '/' )
        {
            m_Fullpath.erase(m_Fullpath.length() - 1, 1);
        }

        // Find the last '/' to get the parent directory path.
        const size_t lastSlash = m_Fullpath.find_last_of("/");
        if (lastSlash != std::string::npos)
        {
            m_Dirname = m_Fullpath.substr(0, lastSlash);
        }

        // Find the last '.' to get the file's extension, which
        // should appear before the last slash.
        const size_t lastComma = m_Fullpath.find_last_of(".");
        if (lastComma != std::string::npos
            && lastSlash != std::string::npos
            && lastComma > lastSlash)
        {
            m_Extension = m_Fullpath.substr(lastComma + 1);
        }

        // Finally get the basename, which is name of a directory or file.
        if (lastComma == std::string::npos
           && lastSlash == std::string::npos)
        {
            m_Basename = m_Fullpath;
        }
        else if (lastComma == std::string::npos)
        {
            m_Basename = m_Fullpath.substr(lastSlash + 1);
        }
        else if (lastSlash == std::string::npos)
        {
            m_Basename = m_Fullpath.substr(0, lastComma - 1);
        }
        else
        {
            m_Basename = m_Fullpath.substr(lastSlash + 1, lastComma - lastSlash - 1);
        }
        m_IsInitialized = true;
    }

    inline std::string GetFilename() const
    {
        ASSERT( IsInitialized() );
        std::string filename(m_Basename);
        if ( ! m_Extension.empty() )
        {
            filename += std::string(".") + m_Extension;
        }
        return filename;
    }
    inline std::string GetBasename() const
    {
        ASSERT( IsInitialized() );
        return m_Basename;
    }
    inline std::string GetDirname() const
    {
        ASSERT( IsInitialized() );
        return m_Dirname;
    }
    inline std::string GetFullpath() const
    {
        ASSERT( IsInitialized() );
        return m_Fullpath;
    }
    inline std::string GetExtension() const
    {
        ASSERT( IsInitialized() );
        return m_Extension;
    }
    inline bool IsInitialized() const
    {
        return m_IsInitialized;
    }
private:

    std::string m_Basename;
    std::string m_Dirname;
    std::string m_Extension;
    std::string m_Fullpath;
    bool m_IsInitialized;

};

} // namespace mutl {

