#pragma once

#include "TunkioProcess.hpp"
#include "TunkioPosixAutoHandle.hpp"

namespace Tunkio
{
    class PosixChildProcess : public IProcess<char>
    {
    public:
        PosixChildProcess(const std::filesystem::path& path, const std::string& arguments);
        bool Start() override;
    };
}