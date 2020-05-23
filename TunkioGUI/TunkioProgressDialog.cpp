#include "PCH.hpp"
#include "TunkioProgressDialog.hpp"

namespace Tunkio
{
    ProgressDialog::ProgressDialog(const std::string& cmdline, TunkioHandle* tunkio) :
        m_form(nana::API::make_center(500, 300)),
        m_cmdParamsLabel(m_form, "Parameters given:\n"),
        m_progressLabel(m_form, "Progress:\nNot started."),
        m_progressBar(m_form),
        m_exitButton(m_form),
        m_stopButton(m_form),
        m_startButton(m_form),
        m_place(m_form),
        m_tunkio(tunkio)
    {
        m_form.caption(L"Tunkio GUI");

        if (cmdline.empty())
        {
            m_cmdParamsLabel.caption(m_cmdParamsLabel.caption() + "None.");
        }
        else
        {
            m_cmdParamsLabel.caption(m_cmdParamsLabel.caption() + cmdline);
        }

        m_exitButton.caption(L"Exit");
        m_exitButton.events().click(std::bind(&ProgressDialog::OnExit, this));

        m_stopButton.caption(L"Stop");
        m_stopButton.events().click(std::bind(&ProgressDialog::OnStop, this));

        m_startButton.caption(L"Start");
        m_startButton.events().click(std::bind(&ProgressDialog::OnStart, this));

        m_place.div("vert margin=3% <labels vertical gap=10 arrange=[50,100,25]> <buttons weight=25 gap=10 >");
        m_place.field("labels") << m_cmdParamsLabel << m_progressLabel << m_progressBar;
        m_place.field("buttons") << m_exitButton << m_stopButton << m_startButton;

        m_place.collocate();
    }

    ProgressDialog::~ProgressDialog()
    {
        WaitForFinished();
    }

    void ProgressDialog::Show()
    {
        m_form.show();
    }

    void ProgressDialog::OnStarted(uint64_t bytesLeft)
    {
        m_progressLabel.caption("Progress: Tunkio started! Bytes to write: " + std::to_string(bytesLeft) + '.');
        SetProgressBarAmount(bytesLeft);

        m_totalTimer.Reset();
        m_currentTimer.Reset();
        m_bytesToWrite = bytesLeft;
        m_bytesWrittenLastTime = 0;
    }

    bool ProgressDialog::OnProgress(uint64_t bytesWritten)
    {
        if (!m_keepRunning)
        {
            return false;
        }

        SetProgressBarValue(bytesWritten);

        const auto elapsedSince = m_currentTimer.Elapsed<Time::MilliSeconds>();
        const auto elapsedTotal = m_totalTimer.Elapsed<Time::MilliSeconds>();
        const DataUnit::Byte bytesWrittenSince(bytesWritten - m_bytesWrittenLastTime);
        const DataUnit::Byte bytesWrittenTotal(bytesWritten);

        if (bytesWrittenTotal.Value() && elapsedSince.count())
        {
            const DataUnit::Byte bytesLeft = m_bytesToWrite - bytesWritten;

            std::stringstream ss;

            ss << "Progress:" << std::endl
                << '\t' << DataUnit::HumanReadable(bytesWrittenTotal) << " written." << std::endl
                << "\tSpeed: " << DataUnit::SpeedPerSecond(bytesWrittenSince, elapsedSince) << '.' << std::endl
                << "\tAvg. speed: " << DataUnit::SpeedPerSecond(bytesWrittenTotal, elapsedTotal) << '.' << std::endl
                << "\tTime left: " << Time::HumanReadable(DataUnit::TimeLeft(bytesLeft, bytesWrittenTotal, m_totalTimer)) << '.';

            m_progressLabel.caption(ss.str());
        }

        m_currentTimer.Reset();
        m_bytesWrittenLastTime = bytesWritten;
        return m_keepRunning;
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

        ss << "Finished:" << std::endl << "\tBytes written: " << bytesWritten << std::endl;

        const DataUnit::Byte bytes(bytesWritten);
        const auto elapsed = m_totalTimer.Elapsed<Time::MilliSeconds>();

        if (bytes.Value() && elapsed.count())
        {
            ss << "\tAverage speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
        }

        m_progressLabel.caption(ss.str());
    }


    void ProgressDialog::OnExit()
    {
        WaitForFinished();
        nana::API::exit();
    }

    void ProgressDialog::OnStop()
    {
        m_keepRunning = false;
        m_progressLabel.caption(m_progressLabel.caption() + "\nStopped!");
    }

    void ProgressDialog::OnStart()
    {
        m_future = std::async(std::launch::async, [&]
        {
            return TunkioRun(m_tunkio);
        });
    }

    void ProgressDialog::WaitForFinished()
    {
        m_keepRunning = false;

        if (m_future.valid())
        {
            std::cout << std::boolalpha << m_future.get() << std::endl;
        }
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