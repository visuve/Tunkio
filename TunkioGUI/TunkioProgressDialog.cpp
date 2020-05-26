#include "PCH.hpp"
#include "TunkioProgressDialog.hpp"

namespace Tunkio
{
    ProgressDialog::ProgressDialog(const std::string& cmdline, TunkioHandle* tunkio) :
        m_form(nana::API::make_center(500, 550)),
        m_cmdParamsGroup(m_form, "Parameters:"),
        m_cmdParamsTextbox(m_cmdParamsGroup),
        m_progressMsgGroup(m_form, "Progress:"),
        m_progressMsgTextbox(m_progressMsgGroup, "Not started."),
        m_progressBar(m_progressMsgGroup),
        m_errorMsgGroup(m_form, "Errors:"),
        m_errorMsgTextbox(m_errorMsgGroup, "No errors."),
        m_exitButton(m_form, "Exit"),
        m_stopButton(m_form, "Stop"),
        m_startButton(m_form, "Start"),
        m_place(m_form),
        m_tunkio(tunkio)
    {
        m_form.caption(L"Tunkio GUI");
        m_form.icon(nana::paint::image("D:\\DEV\\Tunkio\\out\\x64\\bin\\Tunkio.ico")); // TODO:!

        if (cmdline.empty())
        {
            m_cmdParamsTextbox.caption(m_cmdParamsTextbox.caption() + "None.");
        }
        else
        {
            m_cmdParamsTextbox.caption(m_cmdParamsTextbox.caption() + cmdline);
        }

        m_cmdParamsGroup.div("<cmdparams margin=2%>");
        m_cmdParamsGroup["cmdparams"] << m_cmdParamsTextbox;

        m_progressMsgGroup.div("<vertical progress margin=2% gap=10 arrange=[75%,20%]>");
        m_progressMsgGroup["progress"] << m_progressMsgTextbox;
        m_progressMsgGroup["progress"] << m_progressBar;

        m_errorMsgGroup.div("<errors margin=2%>");
        m_errorMsgGroup["errors"] << m_errorMsgTextbox;

        m_exitButton.events().click(std::bind(&ProgressDialog::OnExit, this));
        m_stopButton.events().click(std::bind(&ProgressDialog::OnStop, this));
        m_stopButton.enabled(false);
        m_startButton.events().click(std::bind(&ProgressDialog::OnStart, this));

        m_place.div("vert margin=1% <groups vertical gap=10 arrange=[20%,50%,20%]> <buttons weight=25 gap=10 >");
        m_place.field("groups") << m_cmdParamsGroup << m_progressMsgGroup << m_errorMsgGroup;
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
        m_progressMsgTextbox.caption("Tunkio started!\nBytes to write: " + DataUnit::HumanReadable(DataUnit::Byte(bytesLeft)) + '.');
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

            m_progressMsgTextbox.caption(DataUnit::HumanReadable(bytesWrittenTotal) + " written.\n");
            m_progressMsgTextbox.append(DataUnit::HumanReadable(bytesLeft) + " left.\n", false);
            m_progressMsgTextbox.append("Speed: " + DataUnit::SpeedPerSecond(bytesWrittenSince, elapsedSince) + ".\n", false);
            m_progressMsgTextbox.append("Avg. speed: " + DataUnit::SpeedPerSecond(bytesWrittenTotal, elapsedTotal) + ".\n", false);
            m_progressMsgTextbox.append("Time left: " + Time::HumanReadable(DataUnit::TimeLeft(bytesLeft, bytesWrittenTotal, m_totalTimer)) + '.', false);
        }

        m_currentTimer.Reset();
        m_bytesWrittenLastTime = bytesWritten;
        return m_keepRunning;
    }

    void ProgressDialog::OnErrors(uint32_t error, uint64_t bytesWritten)
    {
        m_stopButton.enabled(false);
        m_errorMsgTextbox.caption("Error " + std::to_string(error) + " occurred.\n");

        if (bytesWritten)
        {
            m_errorMsgTextbox.append("Bytes written: " + std::to_string(bytesWritten), false);
        }

#ifdef WIN32
        std::array<char, 0x400> buffer;

        DWORD formattedSize = FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer.data(),
            static_cast<DWORD>(buffer.size()),
            nullptr);

        if (formattedSize > 2)
        {
            m_errorMsgTextbox.append("Detailed description: " + std::string(buffer.data(), formattedSize) + '\n', false); // Trim \r\n
        }
#endif
        m_errorMsgTextbox.append("Time: " + Time::Timestamp(), false);
    }

    void ProgressDialog::OnCompleted(uint64_t bytesWritten)
    {
        SetProgressBarValue(bytesWritten);

        m_progressMsgTextbox.append("\nFinished!\nBytes written: " + std::to_string(bytesWritten) + ".\n", false);

        const DataUnit::Byte bytes(bytesWritten);
        const auto elapsed = m_totalTimer.Elapsed<Time::MilliSeconds>();

        if (bytes.Value() && elapsed.count())
        {
            m_progressMsgTextbox.append("Time taken: " + Time::HumanReadable(elapsed) + ".\n", false);
        }

        m_progressMsgTextbox.append("Time finished: " + Time::Timestamp(), false);
    }


    void ProgressDialog::OnExit()
    {
        WaitForFinished();
        nana::API::exit();
    }

    void ProgressDialog::OnStop()
    {
        m_keepRunning = false;
        m_progressMsgTextbox.append("\nStopped!", false);
        m_stopButton.enabled(false);
    }

    void ProgressDialog::OnStart()
    {
        m_startButton.enabled(false);

        m_future = std::async(std::launch::async, [&]
        {
            return TunkioRun(m_tunkio);
        });

        m_stopButton.enabled(true);
        m_progressMsgTextbox.caption("");
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