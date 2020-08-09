#include "../PCH.hpp"
#include "TunkioPosixChildProcess.hpp"

namespace Tunkio
{  
    PosixChildProcess::PosixChildProcess(const std::filesystem::path& executable, const std::string& arguments) :
        IProcess(executable, arguments)
    {
    }

    bool PosixChildProcess::Start()
    {
        std::array<char, 0xFF> buffer;

        FILE* pipe = popen("/bin/df -h", "r");

        if (!pipe)
        {
            m_errorCode = errno;
            return false;
        }

        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
        {
            m_stdout += buffer.data();
        }

        m_exitCode = WEXITSTATUS(pclose(pipe));
        return true;
    }
}