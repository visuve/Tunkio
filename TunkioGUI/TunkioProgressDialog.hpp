#pragma once

namespace Tunkio
{
	class ProgressDialog
	{
	public:
		ProgressDialog(const std::string& cmdline, TunkioHandle* tunkio);
		~ProgressDialog();
		void Show();
		void OnStarted(uint64_t bytesLeft);
		bool OnProgress(uint64_t bytesWritten);
		void OnErrors(uint32_t error, uint64_t bytesWritten);
		void OnCompleted(uint64_t bytesWritten);

	private:
		void OnExit();
		void OnStop();
		void OnStart();
		void WaitForFinished();

		void SetProgressBarAmount(uint64_t bytes);
		void SetProgressBarValue(uint64_t bytes);

		nana::form m_form;
		nana::group m_cmdParamsGroup;
		nana::textbox m_cmdParamsTextbox;
		nana::group m_progressMsgGroup;
		nana::textbox m_progressMsgTextbox;
		nana::progress m_progressBar;
		nana::group m_errorMsgGroup;
		nana::textbox m_errorMsgTextbox;
		nana::button m_exitButton;
		nana::button m_stopButton;
		nana::button m_startButton;
		nana::place m_place;
		Time::Timer m_totalTimer;
		Time::Timer m_currentTimer;
		uint64_t m_bytesToWrite = 0;
		uint64_t m_bytesWrittenLastTime = 0;
		TunkioHandle* m_tunkio;
		std::future<bool> m_future;
		std::atomic<bool> m_keepRunning = true;
	};
}