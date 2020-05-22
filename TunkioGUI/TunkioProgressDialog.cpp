#include "PCH.hpp"
#include "TunkioProgressDialog.hpp"

namespace Tunkio
{
    ProgressDialog::ProgressDialog(const std::string& cmdline, TunkioHandle* tunkio) :
        m_form(nana::API::make_center(500, 300)),
        m_cmdParamsLabel(m_form, "Parameters given: " + cmdline.empty() ? "none." : cmdline),
        m_progressLabel(m_form, "Progress: not started."),
        m_progressBar(m_form),
        m_exitButton(m_form),
        m_abortButton(m_form),
        m_runButton(m_form),
        m_place(m_form),
        m_tunkio(tunkio)
    {
        m_form.caption(L"Tunkio GUI");

        m_progressBar.amount(100);
        m_progressBar.value(50);

        m_exitButton.caption(L"Exit");
        m_exitButton.events().click([]
        {
            nana::API::exit();
        });

        m_abortButton.caption(L"Abort");
        m_abortButton.events().click([=]
        {
            // TODO
            std::cout << "ABORT!" << std::endl;
        });

        m_runButton.caption(L"Run");
        m_runButton.events().click([=]()
        {
            // TODO check result
            TunkioRun(m_tunkio);
        });

        m_place.div("vert margin=3% <labels vertical gap=10 arrange=[75,75,25]> <buttons weight=25 gap=10 >");
        m_place.field("labels") << m_cmdParamsLabel << m_progressLabel << m_progressBar;
        m_place.field("buttons") << m_exitButton << m_abortButton << m_runButton;

        m_place.collocate();
    }

    void ProgressDialog::Show()
    {
        m_form.show();
    }

    void ProgressDialog::OnStarted(uint64_t bytesLeft)
    {
        m_progressLabel.caption("Progress: Tunkio started! Bytes to write: " + std::to_string(bytesLeft) + '.');
        SetProgressBarAmount(bytesLeft);
    }

    void ProgressDialog::OnProgress(uint64_t bytesWritten)
    {
        if (!bytesWritten)
        {
            return;
        }

        SetProgressBarValue(bytesWritten);

        const auto elapsedSince = m_currentTimer.Elapsed<Time::MilliSeconds>();
        const auto elapsedTotal = m_totalTimer.Elapsed<Time::MilliSeconds>();
        const DataUnit::Byte bytesWrittenSince(bytesWritten - m_bytesWrittenLastTime);
        const DataUnit::Byte bytesWrittenTotal(bytesWritten);

        if (bytesWrittenTotal.Value() && elapsedSince.count())
        {
            std::stringstream ss;

            const DataUnit::Byte bytesLeft = m_bytesToWrite - bytesWritten;
            ss << DataUnit::HumanReadable(bytesWrittenTotal) << " written."
                << " Speed: " << DataUnit::SpeedPerSecond(bytesWrittenSince, elapsedSince)
                << ". Avg. speed: " << DataUnit::SpeedPerSecond(bytesWrittenTotal, elapsedTotal)
                << ". Time left: " << Time::HumanReadable(DataUnit::TimeLeft(bytesLeft, bytesWrittenTotal, m_totalTimer))
                << std::endl;
        }

        m_currentTimer.Reset();
        m_bytesWrittenLastTime = bytesWritten;
    }

    void ProgressDialog::OnErrors(uint32_t error, uint64_t bytesWritten)
    {
        std::stringstream ss;

        if (bytesWritten)
        {
            ss << "Error " << error << " occurred. Bytes written: " << bytesWritten << std::endl;
        }
        else
        {
            ss << "Error " << error << " occurred." << std::endl;
        }

#ifdef WIN32
        std::string buffer(0x400, 0);
        if (FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer.data(),
            static_cast<DWORD>(buffer.size()),
            nullptr))
        {
            ss << "Detailed description: " << buffer << std::endl;
        }
#endif

        m_progressLabel.caption(ss.str());
    }

    void ProgressDialog::OnCompleted(uint64_t bytesWritten)
    {
        SetProgressBarValue(bytesWritten);

        std::stringstream ss;

        ss << "Finished. Bytes written: " << bytesWritten << std::endl;

        const DataUnit::Byte bytes(bytesWritten);
        const auto elapsed = m_totalTimer.Elapsed<Time::MilliSeconds>();

        if (bytes.Value() && elapsed.count())
        {
            ss << "Average speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
        }

        m_progressLabel.caption(ss.str());
    }

    void ProgressDialog::SetProgressBarAmount(uint64_t bytes)
    {
        m_progressBar.amount(static_cast<uint32_t>(DataUnit::Mebibyte(DataUnit::Byte(bytes)).Value()));
    }

    void ProgressDialog::SetProgressBarValue(uint64_t bytes)
    {
        m_progressBar.value(static_cast<uint32_t>(DataUnit::Mebibyte(DataUnit::Byte(bytes)).Value()));
    }
}