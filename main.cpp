#include <QTextCodec>
#include <QThreadPool>
#include <QSettings>
#include <QStringList>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QFontDatabase>
#include <QPluginLoader>
#include <QSplashScreen>
#include <QTranslator>
#include <QDebug>
#include <QDateTime>

static const char codec[] = "utf8";

#include "Manager/Manager.h"
#include "utils/func.h"
#include "utils/mysplashscreen.h"
#include "appinfo.h"
#include "QtSingleApplication"

#include "NoteShared.h"

#include "ScriptModule/scriptengine.h"
//! Дает возможность получить из сценария доступ к любому виджету нашего приложения по имени.
QScriptValue getWidgetByName( QScriptContext * content, QScriptEngine * engine )
{
    const QString & name = content->argument(0).toString();
    QObject * object = 0;
    foreach( QWidget * widget, qApp->topLevelWidgets() )
    {
        if ( name == widget->objectName() )
        {
            object = widget;
            break;

        } else if ( object == widget->findChild < QObject * > ( name ) )
            break;
    }

    return engine->newQObject( object );
}
QScriptValue getAllNotes( QScriptContext *, QScriptEngine * engine )
{
    return qScriptValueFromSequence( engine, QStringList() << "1" << "2" << "666" );
}
static QScriptValue importExtension( QScriptContext *context, QScriptEngine * engine )
{
    return engine->importExtension( context->argument(0).toString() );
}

// TODO: изменение таблицы, списка и гиперссылок через меню редактора заметки
// TODO: добавить режим чтения (а может и редактирования?) заметок: полноэкранный режим (goodword1.04)
// TODO: добавить напоминания
// TODO: прикрепленные картинки можно вставлять
// TODO: панель форматирования подчеркивание - не правильно сохраняет
// TODO: вставку картинок из буфера обмена
// TODO: настройка: активность автодополнения и сохранение значения автодополнения
// TODO: настройка: кнопка восстановления значений по умолчанию
// TODO: при сохранении заметки дата изменения меняется
// TODO: У заметок, выбор цвета для: заголовка и тела заметки (взять из панели кнопку)
// TODO: гиперссылки у заметок
// TODO: Вставка текстовых смайлов в редактор заметок
//
/// TODO: возможность добавления тэгов
/// TODO: возможность загружать скрипты от файлов и локальных баз данных
/// TODO: возможность сохранять скрипты как файлы и локальные базы данных
/// TODO: добавить в список менеджера контекстное меню.
/// TODO: поразбираться с QScintilla. Возможно нужно будет перенастроить стиль редактора.
/// TODO: Добавить "Выделить текущую вкладку на дереве"
/// TODO: при создании заметки от буфера обмена, можно заголовок ее брать от части текста (например, первые 10 символов)
/// TODO: добавить возможность выбора иконок для иерархического дерева
/// TODO: расширить функционал панели форматирования на странице заметок
/// TODO: при добавлении заметки от буфера обмена, пропадают символы перехода на следующую строку.
// TODO: добавить в tooltip трея инфу об количестве заметок, сколько видимо/скрыто
/// TODO: диалог подтвержения удаления/перемещения - выводить список итемов в qlistwidget
/// TODO: допилить словарь автодополнений

QString nm_Note::style = "";

int main( int argc, char *argv[] )
{
    qsrand( QTime( 0, 0, 0 ).secsTo( QTime::currentTime() ) );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( codec ) );    

    QtSingleApplication app( argc, argv );
    app.addLibraryPath( "plugins" );
    app.setApplicationName( App::name );
    app.setApplicationVersion( App::version );
    app.setQuitOnLastWindowClosed( false ); // Приложение не завершится, даже если все окна закрыты/скрыты    

    // Если копия приложения уже запущена, тогда отсылаем сообщение той копии и заканчиваем процесс
    if ( app.isRunning() )
    {
        app.sendMessage( "-show -message" );
        return 0;
    }    

    QFile file( ":/Note/Note.qss" );
    if ( file.open( QIODevice::ReadOnly ) )
        nm_Note::style = file.readAll();

    QSettings * settings = createSettings();

    MySplashScreen * splashScreen = new MySplashScreen();
    QPixmap pixmap = QPixmap( ":/App/splash" );
    splashScreen->setPixmap( pixmap );
    splashScreen->show();

    initThreadCount();
    loadTranslations( settings );

    QFont font( "Times New Roman", 9, QFont::Bold );
    splashScreen->setMessage( QTranslator::tr( "Loading fonts" ), font );
    loadFonts();

    splashScreen->setMessage( QTranslator::tr( "Loading dictionaries" ), font );
    loadDictionaries();

    splashScreen->setMessage( QTranslator::tr( "Creation" ), font );

    Manager manager;
    QObject::connect( &app, SIGNAL( messageReceived(QString) ), &manager, SLOT( messageReceived(QString) ) );

    splashScreen->finish( &manager );

    manager.setSettings( settings );
    manager.setWindowTitle( getWindowTitle() );

    splashScreen->setMessage( QTranslator::tr( "Loading settings" ), font );
    manager.readSettings();

    splashScreen->setMessage( QTranslator::tr( "Loading scripts" ), font );
    manager.loadScripts();

    splashScreen->setMessage( QTranslator::tr( "Now ready" ), font );
    manager.nowReadyPhase();

    app.setActiveWindow( &manager );
    splashScreen->deleteLater();

    manager.loadNotes();


    // Расширения скриптового движка
    {
        Script::ScriptEngine * engine = Script::ScriptEngine::instance();
        QScriptValue globalObject = engine->globalObject();

//        globalObject.setProperty( "qApp", engine->newQObject( app ) );
//        {
//            QScriptValue qscript = engine->newObject();
//            qscript.setProperty( "importExtension", engine->newFunction( importExtension ) );
//            globalObject.property( "qt" ).setProperty( "script", qscript );
//        }

        globalObject.setProperty( "importExtension", engine->newFunction( importExtension ) );

        globalObject.setProperty( "getWidgetByName", engine->newFunction( getWidgetByName ) );
        globalObject.setProperty( "getAllNotes", engine->newFunction( getAllNotes ) );

        QScriptValue scriptManager = engine->newQObject( &manager );
        scriptManager.setProperty( "PageNotes", engine->newQObject( manager.pageNotes ) );
        scriptManager.setProperty( "PageSettings", engine->newQObject( manager.pageSettings ) );
        scriptManager.setProperty( "PageAbout", engine->newQObject( manager.pageAbout ) );

        globalObject.setProperty( "Manager", scriptManager );


        // Загрузка плагинов
        {            
            foreach ( const QFileInfo & fileInfo, QDir( "plugins/script" ).entryInfoList() )
            {
                QString fileName = fileInfo.baseName();
                if ( fileName.isEmpty() || fileName.isNull() )
                    continue;

                QString name = fileName.replace( "qtscript_", "qt.", Qt::CaseInsensitive );
                QScriptValue extension_Qt = engine->importExtension( name );
                if ( extension_Qt.isError() )
                    WARNING( qPrintable( extension_Qt.toString() ) );
            }
        }
    }

    int code = app.exec();
    printf( "Exit code: %i\n", code );
    return code;
}
