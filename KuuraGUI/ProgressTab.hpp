#pragma once

#include <QWidget>
#include <QDateTime>

#include <filesystem>

namespace Ui
{
	class ProgressTab;
}

class ProgressTab : public QWidget
{
	Q_OBJECT

public:
	explicit ProgressTab(QWidget *parent = nullptr);
	~ProgressTab();

	void addTarget(const std::filesystem::path& path);
	void setPassCount(uint16_t);

	void onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass);
	void onPassStarted(const std::filesystem::path& path, uint16_t pass);
	void onPassProgressed(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten);
	void onPassFinished(const std::filesystem::path& path, uint16_t pass);
	void onOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten);

signals:
	void nextRequested();

private:
	void updateTotalProgress();

	Ui::ProgressTab *ui;
	double _bytesToProcess = 0;
	std::chrono::steady_clock::time_point _overwriteStartTime;
	std::map<uint16_t, uint64_t> _bytesProcessed;
};

