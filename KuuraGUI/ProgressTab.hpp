#pragma once

#include <QWidget>
#include <QDateTime>

#include <filesystem>

class ProgressModel;

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

	void onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass);
	void onTargetStarted(const std::filesystem::path& path, uint64_t bytesToWrite);
	void onPassStarted(const std::filesystem::path& path, uint16_t pass);
	void onPassProgressed(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten);
	void onPassCompleted(const std::filesystem::path& path, uint16_t pass);
	void onTargetCompleted(const std::filesystem::path& path, uint64_t bytesWritten);
	void onOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten);

signals:
	void nextRequested();

private:
	void updateTotalProgress();

	Ui::ProgressTab *ui;
	ProgressModel* _model;

	float _bytesToProcess = 0.0f;
	std::chrono::steady_clock::time_point _overwriteStartTime;
};

