#include "appversion.hpp"
#include "util/icon.hpp"
#include "lib/qtpaths.hpp"

#include <QApplication>
#include <QCoreApplication>

#include "mainwindow.hpp"
#include "dialog/setupdialog.hpp"

int main(int argc, char *argv[])
{
	// Set name for settings etc.
	QCoreApplication::setOrganizationName("kraxarn");
	QCoreApplication::setApplicationName("spotify-qt");
	QCoreApplication::setApplicationVersion(APP_VERSION);

	// High-DPI support
	// These flags are deprecated and always enabled in Qt 6 and newer
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

	// Create Qt application
	QApplication app(argc, argv);

	// Settings
	QtPaths paths(nullptr);
	lib::settings settings(paths);

	// Check fallback icons
	Icon::useFallbackIcons = settings.general.fallback_icons;

	// Show version if requested
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.addHelpOption();
	parser.addOptions({
		{"dev", "Enable developer mode for troubleshooting issues."},
		{"reset-credentials", "Allows providing new Spotify credentials."}
	});
	parser.process(app);

	if (parser.isSet("dev"))
	{
		lib::developer_mode::enabled = true;
	}

	// First setup window
	if (settings.account.refresh_token.empty()
		|| parser.isSet("reset-credentials"))
	{
		SetupDialog dialog(settings);
		if (dialog.exec() == QDialog::Rejected)
		{
			return 0;
		}
	}

	// Create main window
	MainWindow w(settings, paths);

	// Show window and run application
	if (!w.isValid())
	{
		return 1;
	}

	w.show();

	// Run application
	return QApplication::exec();
}
