#pragma once
#include <codecvt>

namespace MIKU
{
    // Character Encoding
   // https://utf8everywhere.org/
    inline std::string UTF16ToUTF8(const wchar_t* s)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(s);
    }

    inline std::wstring UTF8ToUTF16(const char* s)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(s);
    }

    inline std::string UTF16ToUTF8(const std::wstring& s)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(s);
    }

    inline std::wstring UTF8ToUTF16(const std::string& s)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(s);
    }

    template<class T>
    class Singleton 
    {
    private:
        Singleton(const Singleton<T>&) = delete;
        Singleton& operator=(const Singleton<T>&) = delete;
    protected:
        static T* Instance;
        Singleton(void)
        {
            ASSERT(!Instance && "Only one instance can exist for a singlton class");
            Instance = static_cast<T*>(this);
        }
        ~Singleton()
        {
            ASSERT(Instance && "No instance of this singleton has been initialized.");
            Instance = nullptr;
        }
    public:
        static T& Get()
        {
            return *Instance;
        }
        static T* GetPtr()
        {
            return Instance;
        }
    };

}