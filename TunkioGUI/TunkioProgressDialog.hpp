#pragma once

namespace Tunkio
{
    class ProgressDialog
    {
    public:
        ProgressDialog(const std::string& cmdline, TunkioHandle* tunkio);
        void Show();
        void OnStarted(uint64_t bytesLeft);
        void OnProgress(uint64_t bytesWritten);
        void OnErrors(uint32_t error, uint64_t bytesWritten);
        void OnCompleted(uint64_t bytesWritten);

    private:
        void SetProgressBarAmount(uint64_t bytes);
        void SetProgressBarValue(uint64_t bytes);

        nana::form m_form;
        nana::label m_cmdParamsLabel;
        nana::label m_progressLabel;
        nana::progress m_progressBar;
        nana::button m_exitButton;
        nana::button m_abortButton;
        nana::button m_runButton;
        nana::place m_place;
        Time::Timer m_totalTimer;
        Time::Timer m_currentTimer;
        uint64_t m_bytesToWrite;
        uint64_t m_bytesWrittenLastTime;
        TunkioHandle* m_tunkio;
    };
}