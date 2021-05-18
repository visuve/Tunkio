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

	void addTarget(const QString& path);
	void addTarget(const QFileInfo& path);
	void addPass(const QVariantList& pass);

	void onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass);
	void onTargetStarted(const std::filesystem::path& path, uint64_t bytesToWrite);
	void onPassStarted(const std::filesystem::path& path, uint16_t pass);
	void onPassProgressed(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten);
	void onPassCompleted(const std::filesystem::path& path, uint16_t pass);
	void onTargetCompleted(const std::filesystem::path& path, uint64_t bytesWritten);
	void onOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten);

	void onError();

signals:
	void nextRequested();

private:
	class Progress
	{
	public:
		inline float bytesWritten() const
		{
			return _bytesWritten;
		}

		inline float bytesLeft() const
		{
			return _bytesLeft;
		}

		inline float milliSecondsTaken() const
		{
			return _elapsed.elapsed();
		}

		inline float bytesPerMillisecond() const
		{
			const float time = milliSecondsTaken();

			if (time >= 0.00f && _bytesWritten >= 0.00f)
			{
				return _bytesWritten / time;
			}

			return 0.00f;
		}

		inline float millisecondsLeft() const
		{
			const float speed = bytesPerMillisecond();

			if (speed >= 0.00f && _bytesLeft >= 0.00f)
			{
				return _bytesLeft / speed;
			}

			return 0.00f;
		}

		inline float percentageDone() const
		{
			if (_bytesBeginning >= 0.00f && _bytesWritten >= 0.00f)
			{
				const float percentage = (_bytesWritten / _bytesBeginning) * 100.0f;
				Q_ASSERT(percentage <= 100.00f);
				return std::clamp(percentage, 0.00f, 100.00f);
			}

			return 0.00f;
		}

		inline void reset(float bytesLeft)
		{
			_bytesBeginning = bytesLeft;
			_bytesWritten = 0.00f;
			_bytesLeft = bytesLeft;
			_elapsed.restart();
		}

		inline void update(float bytesWritten)
		{
			Q_ASSERT(bytesWritten >= 0.00f);
			Q_ASSERT(bytesWritten <= _bytesBeginning);

			_bytesWritten = std::clamp(bytesWritten, 0.00f, _bytesBeginning);
			_bytesLeft = std::clamp(_bytesBeginning - _bytesWritten, 0.00f, _bytesBeginning);

			Q_ASSERT(_bytesLeft >= 0.00f);
			Q_ASSERT(_bytesWritten >= 0.00f);
			Q_ASSERT(_bytesWritten <= _bytesBeginning);
		}

		inline void complete()
		{
			_bytesWritten = _bytesBeginning;
			_bytesLeft = 0.00f;
		}

	private:
		float _bytesBeginning = 0.00f;
		float _bytesWritten = 0.00f;
		float _bytesLeft = 0.00f;
		QElapsedTimer _elapsed;
	};

	void updatePassProgress();
	void updateTotalProgress();

	Ui::ProgressTab *ui;
	ProgressModel* _model;

	Progress _totalProgress;
	Progress _currentProgress;

	QTimer _totalProgressUpdater;
};

