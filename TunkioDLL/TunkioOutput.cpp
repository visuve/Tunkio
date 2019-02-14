#include "PCH.hpp"
#include "TunkioOutput.hpp"
#include "TunkioTiming.hpp"

namespace Tunkio::Output
{
    bool Fill(FileStream& file, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, TunkioProgressCallback progress)
    {
        const std::vector<uint8_t> fakeData(1024, 0);
        Timing::Timer timer;

        StreamBuffer* stream = file.rdbuf();

        while (bytesLeft)
        {
            const uint64_t bytesToWrite = bytesLeft < fakeData.size() ? bytesLeft : fakeData.size();
            const char* data = reinterpret_cast<const char*>(&fakeData.front());
            const uint64_t writtenBytes = stream->sputn(data, bytesToWrite);
            bytesLeft -= writtenBytes;
            writtenBytesTotal += writtenBytes;

            if (!writtenBytes)
            {
                return false;
            }

            if (progress && timer.Elapsed<Timing::Seconds>() > Timing::Seconds(1))
            {
                progress(writtenBytesTotal, timer.Elapsed<Timing::Seconds>().count());
            }
        }

        return bytesLeft == 0 && writtenBytesTotal > 0;
    }
}