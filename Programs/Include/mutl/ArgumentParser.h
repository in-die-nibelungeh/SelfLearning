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

#include <stdint.h> // uint
#include <stdlib.h> // atoi
#include <string>  // std::string

namespace mutl {

struct ArgumentDescription
{
    const char* option;
    uint count;
};

// Note:
// Used are  ',' and  ':' as a separator in this class, so
// ArgumentParser class will not work as expected when a program's arguments contain each of them.
// Refer to $(ROOT)/Programs/Sources/Libraries/mutl/CommandLine/main.cpp on how to use.
class ArgumentParser
{
public:
    ArgumentParser()
        : m_Options()
        , m_Inputs()
        , m_OptionCount(0)
        , m_InputCount(0)
        , m_IsInitialized(false)
    {}

    ~ArgumentParser()
    {}

    bool Initialize(int argc, const char* argv[], const struct ArgumentDescription descs[], size_t descsCount)
    {
        // 再度コールする場合は Finalize を読んでから
        if ( IsInitialized() )
        {
            return true;
        }
        bool isValid = true;  // desc に従うオプション入力かどうか
        int descIndex = Unselected;
        uint arguments = 0;

        // プログラムパスはスキップする
        for ( int k = 1; k < argc; ++k )
        {
            const char* arg = argv[k];

            // オプションなら
            if ( arg[0] == '-' )
            {
                // オプションの引数の数が満足されているなら、Unselected になる。
                // Unselected でないなら引数の与え方が間違っている。
                if (descIndex != Unselected)
                {
                    isValid = false;
                    break;
                }
                // オプションが指定され、前のオプションの引数が正常なら
                // descIndex は Unselected であることが保証されている。
                bool isFound = false;
                // desc から探す
                for (uint m = 0; m < descsCount; ++m)
                {
                    // 一致するなら
                    if (!strcmp(descs[m].option, arg + 1)) // + 1 は先頭の '-' をスキップ
                    {
                        // 見つかった、と。
                        isFound = true;

                        char count[11];
                        sprintf(count, "%d", descs[m].count);

                        // 追加していく準備
                        m_Options.append( std::string(" ")
                            + std::string(descs[m].option)
                            + std::string(":")
                            + std::string(count)
                            + std::string(":") );

                        // リセット
                        arguments = 0;

                        // 引数を持つならインデックスを設定
                        if ( 0 < descs[m].count )
                        {
                            descIndex = m;
                        }
                        ++m_OptionCount;

                        break;
                    }
                }
                // 害がないので無視する (TBD)
                if (false == isFound)
                {
                    DEBUG_LOG("Unknown option: %s\n", arg);
                }
            }
            else
            {
                // desc が選択されていない場合、プログラムの入力に登録
                if (descIndex == Unselected )
                {
                    ++m_InputCount;
                    m_Inputs.append(std::string(arg) + std::string(","));
                }
                else
                {
                    // desc が選択されている場合、オプションに登録する。
                    // desc の個数だけカウントしたら、desc を未選択状態に戻す。
                    ++arguments;
                    m_Options.append(std::string(arg));
                    if (descs[descIndex].count == arguments)
                    {
                        descIndex = Unselected;
                    }
                    else
                    {
                        m_Options.append( std::string(",") );
                    }
                }
            }
        }

        if (descIndex != Unselected)
        {
            DEBUG_LOG("Still indicating an index (%d) ... (Illegal) \n", descIndex);
            isValid = false;
        }

        // プログラム入力があれば文字列が ',' で終わっているはず。
        // 末尾の ',' を削除する。
        if (m_InputCount > 0)
        {
            if (m_Inputs.at(m_Inputs.length() - 1) == ',')
            {
                m_Inputs.erase( m_Inputs.length() - 1, 1);
            }
        }
        DEBUG_LOG("Inputs (%d): %s\n", m_InputCount, m_Inputs.c_str());
        DEBUG_LOG("Options(%d): %s\n", m_OptionCount, m_Options.c_str());

        m_IsInitialized = isValid;

        return m_IsInitialized;
    }

    void Finalize()
    {
        m_IsInitialized = false;
        m_Options.erase();
        m_Inputs.erase();
        m_OptionCount = 0;
        m_InputCount = 0;

    }

    inline bool IsEnabled(const char* _option) const
    {
        ASSERT( IsInitialized() );

        std::string option = std::string(" ") + std::string(_option) + std::string(":");
        return std::string::npos != m_Options.find( option, 0 );
    }

    std::string GetOption(const char* _option, uint index = 0) const
    {
        ASSERT( IsInitialized() );

        std::string argument("");
        std::string option = std::string(" ") + std::string(_option) + std::string(":");
        size_t s = m_Options.find( option, 0 );
        // オプションの文字列を検索する (ここまでは IsEnabled と同じ挙動)
        if (std::string::npos != s)
        {
            // 見つかれば、オプション文字列を切り出して Option に渡す。
            // " " が見つからない場合は (おそらく) 最後のオプションなので、
            // e に終端を設定しておく。
            size_t e = m_Options.find_first_of(" ", s + 1);
            if (e == std::string::npos)
            {
                e = m_Options.length();
            }
            Option object( m_Options.substr(s, e - s) );
            argument = object[index];
        }
        return argument;
    }

    // 処理は Option::operator[] からの抜粋なので、そっちを参照。
    // そして、バグがあったらお互いに修正すること。
    std::string GetArgument(uint index = 0) const
    {
        ASSERT( IsInitialized() );

        std::string argument("");
        // 最初の引数は先頭から最初の ',' までを、
        // 最初と最後以外の引数なら ',' の間を、
        // 最後の引数は最後の ',' から終端までを、
        // (引数が一つなら最初から最後までを)
        // それぞれ切り出して返却する。
        if ( index < m_InputCount )
        {
            size_t s = 0;
            size_t e = m_Inputs.find_first_of(",");
            for (uint k = 0; k < index; ++k)
            {
                s = e + 1;
                e = m_Inputs.find_first_of(",", s);
            }
            if (e == std::string::npos)
            {
                e = m_Inputs.length();
            }
            argument = m_Inputs.substr(s, e - s);
        }
        return argument;
    }

    inline uint GetArgumentCount() const
    {
        ASSERT( IsInitialized() );

        return m_InputCount;
    }

    inline uint GetOptionCount() const
    {
        ASSERT( IsInitialized() );

        return m_OptionCount;
    }

    inline bool IsInitialized() const
    {
        return m_IsInitialized;
    }

private:

    class Option
    {
    public:
        explicit Option(const std::string& arguments);
        ~Option() {}

        std::string operator[](uint index) const;

        inline bool IsInitialized() const
        {
            return m_IsInitialized;
        }
    private:
        std::string m_Option;
        std::string m_Arguments;
        uint m_ArgumentCount;
        bool m_IsInitialized;
    };

    std::string m_Options;
    std::string m_Inputs;
    int m_OptionCount;
    uint m_InputCount;
    bool m_IsInitialized;
    static const int Unselected = -1;
};

ArgumentParser::Option::Option(const std::string& arguments)
    : m_Option()
    , m_Arguments()
    , m_ArgumentCount(0)
    , m_IsInitialized(false)
{
    // 最初の ':' までがオプション文字列
    size_t e = arguments.find_first_of(":");
    // ASSERT で良いかな。。。
    if (e == std::string::npos)
    {
        return ;
    }
    // 格納しておくが、使途不明
    m_Option = arguments.substr(0, e);

    // 次の ':' までが引数の個数
    size_t s = e + 1;
    e = arguments.find_first_of(":", s);
    // ASSERT で良いやろか。。。
    if (e == std::string::npos)
    {
        return ;
    }
    m_ArgumentCount = atoi( arguments.substr(s, e - s).c_str() );

    // 引数があればまとめて保持しておく。
    if (0 < m_ArgumentCount)
    {
        m_Arguments = arguments.substr(e + 1);
    }
    m_IsInitialized = true;
}

std::string ArgumentParser::Option::operator[](uint index) const
{
    ASSERT( IsInitialized() );

    std::string argument("");
    // 最初の引数は先頭から最初の ',' までを、
    // 最初と最後以外の引数なら ',' の間を、
    // 最後の引数は最後の ',' から終端までを、
    // (引数が一つなら最初から最後までを)
    // それぞれ切り出して返却する。
    if ( index < m_ArgumentCount )
    {
        size_t s = 0;
        size_t e = m_Arguments.find_first_of(",");
        for (uint k = 0; k < index; ++k)
        {
            s = e + 1;
            e = m_Arguments.find_first_of(",", s);
        }
        if (e == std::string::npos)
        {
            e = m_Arguments.length();
        }
        argument = m_Arguments.substr(s, e - s);
    }
    return argument;
}


} // namespace mutl {

