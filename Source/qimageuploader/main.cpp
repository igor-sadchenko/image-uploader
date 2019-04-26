#include "Gui/mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QTemporaryDir>
#include <QDebug>
//#include <3rdparty/qtdotnetstyle.h>
#include "Core/Logging.h"
#include <boost/filesystem/path.hpp>
#include <boost/locale.hpp>
#include "Core/CommonDefs.h"
#include "Core/ServiceLocator.h"
#include "Core/AppParams.h"

#include "QtUploadErrorHandler.h"
#include "QtDefaultLogger.h"
#include "QtScriptDialogProvider.h"
#include "Gui/LogWindow.h"

#include <Core/Settings/QtGuiSettings.h>
#include <QMessageBox>
#include "Core/i18n/Translator.h"

#ifdef _WIN32
CAppModule _Module;
QString dataFolder = "Data/";
#else
QString dataFolder = "/usr/share/imageuploader/";
#endif
QtGuiSettings Settings;

class Translator : public ITranslator {
public:
	virtual std::string getCurrentLanguage() override {
		return "English";
	}
	virtual std::string getCurrentLocale() override {
		return "en_US";
	}
	virtual std::string translate(const char* str) override {
		return str;
	}
#ifdef _WIN32
	virtual const wchar_t* translateW(const wchar_t* str) override {
		return str;
	}
#endif
};
Translator translator; // dummy translator

int main(int argc, char *argv[])
{
    ServiceLocator::instance()->setSettings(&Settings);
#if defined(_WIN32) && !defined(NDEBUG)
    // These global strings in GLOG are initially reserved with a small
    // amount of storage space (16 bytes). Resizing the string larger than its
    // initial size, after the _CrtMemCheckpoint call, can be reported as
    // a memory leak.
    // So for 'debug builds', where memory leak checking is performed,
    // reserve a large enough space so the string will not be resized later.
    // For these variables, _MAX_PATH should be fine.
    FLAGS_log_dir.reserve(_MAX_PATH);  // comment out this line to trigger false memory leak
    FLAGS_log_link.reserve(_MAX_PATH);
    FLAGS_logtostderr = false;
    FLAGS_alsologtostderr = true;
    // Enable memory dump from within VS.
#else
        FLAGS_logtostderr = true;
#endif

    google::InitGoogleLogging(argv[0]);

	QApplication a(argc, argv);
	LogWindow* logWindow = new LogWindow();
  
	logWindow->show();
	QtDefaultLogger logger(logWindow);
	QtUploadErrorHandler errorHandler(&logger);
	QtScriptDialogProvider dlgProvider;
	ServiceLocator::instance()->setTranslator(&translator);
	ServiceLocator::instance()->setUploadErrorHandler(&errorHandler);
	ServiceLocator::instance()->setLogger(&logger);
	ServiceLocator::instance()->setDialogProvider(&dlgProvider);
    QString appDirectory = QCoreApplication::applicationDirPath();
    QString settingsFolder;
    setlocale(LC_ALL, "");

    if(QFileInfo::exists(appDirectory + "/Data/servers.xml")){
        dataFolder = appDirectory+"/Data/";
        settingsFolder = dataFolder;
    }
#ifndef _WIN32
   else {
dataFolder = "/usr/share/qimageuploader/";
   }

#ifndef __APPLE__
settingsFolder = getenv("HOME")+QString("/.config/qimageuploader/");
QDir settingsDir = QDir::root();
settingsDir.mkpath(settingsFolder);
#endif

#endif
    qDebug() << "Data directory:" << dataFolder;
    qDebug() << "Settings directory:" << settingsFolder;
    AppParams* params = AppParams::instance();
    params->setDataDirectory(Q2U(dataFolder));
    params->setSettingsDirectory(Q2U(settingsFolder));

    QTemporaryDir dir;
    if (dir.isValid()) {
        params->setTempDirectory(Q2U(dir.path()));
    } else {
        LOG(ERROR) << "Unable to create temp directory!";
    }

	Settings.LoadSettings(AppParams::instance()->settingsDirectory());
	CUploadEngineList engineList;
	if (!engineList.loadFromFile(AppParams::instance()->dataDirectory() + "servers.xml", Settings.ServersSettings)) {
		QMessageBox::warning(nullptr, "Failure", "Unable to load servers.xml");
	}
    //google::AddLogSink(&logSink);
    //serviceLocator->setUploadErrorHandler(&uploadErrorHandler);
    //serviceLocator->setLogger(&defaultLogger);

	Settings.setEngineList(&engineList);
	
    //QApplication::setStyle(new QtDotNetStyle);
    MainWindow w(&engineList, logWindow);
    w.show();
    
    int res =  a.exec();

    //google::RemoveLogSink(&logSink);
	Settings.SaveSettings();
    google::ShutdownGoogleLogging();
    return res;
}
