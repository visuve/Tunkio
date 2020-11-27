#include "TunkioGUI-PCH.hpp"
#include "MainWindow.hpp"

char qtMesssageTypeToChar(QtMsgType type)
{
	switch (type)
	{
		case QtDebugMsg:
			return 'D';
		case QtInfoMsg:
			return 'I';
		case QtWarningMsg:
			return 'W';
		case QtCriticalMsg:
			return 'C';
		case QtFatalMsg:
			return 'F';
	}

	return '?';
}

std::ostream& qtMessageTypeToStreamType(QtMsgType type)
{
	switch (type)
	{
		case QtDebugMsg:
		case QtInfoMsg:
			return std::cout;
		case QtWarningMsg:
			return std::clog;
		case QtCriticalMsg:
		case QtFatalMsg:
			return std::cerr;
	}

	return std::cerr;
}

void tunkioLogMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	const QString time = QTime::currentTime().toString();

	if (context.function && !message.isEmpty())
	{
		qtMessageTypeToStreamType(type)
			<< time.toStdString() << " ["
			<< qtMesssageTypeToChar(type) << "] "
			<< context.function << ':'
			<< context.line << ": "
			<< message.toStdString() << std::endl;
	}

	if (context.function && message.isEmpty())
	{
		qtMessageTypeToStreamType(type)
			<< time.toStdString() << " ["
			<< qtMesssageTypeToChar(type) << "] "
			<< context.function << ':'
			<< context.line << std::endl;
	}
}

void loadIcon(QApplication& application)
{
	QPixmap pixmap;

	if (!pixmap.load(":/Tunkio.png"))
	{
		qWarning() << "Failed to load application icon!";
		return;
	}

	application.setWindowIcon(pixmap);
}

void resizeToScreen(MainWindow& window)
{
	const QScreen* screen = QGuiApplication::primaryScreen();

	if (!screen)
	{
		qWarning() << "Failed to detect primary screen!";
	}
	else
	{
		const QRect screenGeometry = screen->geometry();
		window.resize(screenGeometry.width() / 2, screenGeometry.height() / 3);
	}
}

int main(int argc, char* argv[])
{
	qInstallMessageHandler(tunkioLogMessageHandler);
	qRegisterMetaType<uint8_t>("uint8_t");
	qRegisterMetaType<uint16_t>("uint16_t");
	qRegisterMetaType<uint32_t>("uint32_t");
	qRegisterMetaType<uint64_t>("uint64_t");
	qRegisterMetaType<TunkioTargetType>("TunkioTargetType");
	qRegisterMetaType<TunkioFillType>("TunkioFillType");
	qRegisterMetaType<TunkioStage>("TunkioStage");

	QApplication application(argc, argv);
	MainWindow window;

	loadIcon(application);
	resizeToScreen(window);

	window.show();

	return application.exec();
}
