#pragma once

#include "../TunkioDLL/TunkioAPI.h"

#include <string>

namespace Tunkio::Memory
{
    struct TunkioOptionsDeleter
    {
        void operator()(TunkioOptions* options)
        {
            if (options)
            {
                if (options->Path.Data)
                {
                    delete[] options->Path.Data;
                }

                delete options;
            }
        }
    };

    struct TunkioDeleter
    {
        void operator()(TunkioHandle* tunkio)
        {
            if (tunkio)
            {
                TunkioFree(tunkio);
            }
        }
    };

    template <typename T>
    const T* CloneString(const std::basic_string<T>& str)
    {
        const size_t bytes = str.length() * sizeof(T) + sizeof(T);
        return reinterpret_cast<const T*>(memcpy(new T[bytes], str.c_str(), bytes));
    }

    using AutoOptionsHandle = std::unique_ptr<TunkioOptions, Tunkio::Memory::TunkioOptionsDeleter>;
    using AutoHandle = std::unique_ptr<TunkioHandle, Tunkio::Memory::TunkioDeleter>;
}