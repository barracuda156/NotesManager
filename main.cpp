/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the
** Free Software Foundation, Inc.,
** 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**************************************************************************/

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
#include "version.h"
#include "QtSingleApplication"

/// TODO: может корзину сделать частью группирования заметок?
/// TODO: добавить возможность группировки заметок по блокнотам

// TODO: изменение таблицы, списка и гиперссылок через меню редактора заметки
// TODO: добавить режим чтения (а может и редактирования?)
// заметок: полноэкранный режим (goodword1.04)
// TODO: удаленные заметки не отображаются, не редактируются, но их можно просматривать через мереджер
// TODO: у корзины и блокнота на иконке рисовать кол-во элементов в них
// TODO: напоминания. думаю разумно напоминалки различать через полный путь заметок
// TODO: прикрепленные картинки можно вставлять
// TODO: где то у заметок походу происходит утечка памяти покрайней мере, это фиксируется в диспетчее задач при создании и удалении заметки
// TODO: панель справа и снизу, я на время уберу
// TODO: выделять память только под видимые заметки
// TODO: панель форматирования подчеркивание - не правильно сохраняет
// TODO: вставку картинок из буфера обмена
// TODO: настройка: активность автодополнения
// TODO: настройка: кнопка восстановления значений по умолчанию
// TODO: при сохранении заметки дата изменения меняется
// TODO: ЗАМЕТКА: выбор цвета для: заголовка и тела заметки (взять из панели кнопку)
// TODO: гиперссылки у заметок
// TODO: Настройки: "думаю самое простое убрать кнопку применить)) и сделать автоматом))"
// TODO: настройки: добавить возможность у заметок убирать лишние компоненты: быстрый поиск и панель инструментов.
// TODO: заметка/настройки: возможность сделать прозрачным фон редактора заметок, как в старых.
// TODO: проверить работу дублирования
// TODO: Заметка -> При сохранить как вставлять в качестве имени заголовок заметки
// (учитывать, что в ос не все символы в названиях файлов разрешены)

/// NOTE: git после выпуска стабильных версий, создавать ветку develop для новой версии

QString Note::style = "";

int main( int argc, char *argv[] )
{
    qsrand( QDateTime().toMSecsSinceEpoch() );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( codec ) );

    QtSingleApplication app( argc, argv );
    qApp->setApplicationName( App::name );
    qApp->setApplicationVersion( App::version );
    qApp->setQuitOnLastWindowClosed( false );

    if ( app.isRunning() )
    {
        app.sendMessage( "-show -message" );
        return 0;
    }

    QFile file( ":/Note.qss" );
    if ( file.open( QIODevice::ReadOnly ) )
        Note::style = file.readAll();

    QSettings * settings = createSettings();

    MySplashScreen * splashScreen = new MySplashScreen();
    QPixmap pixmap = QPixmap( ":/splash" );
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
    splashScreen->finish( &manager );

    manager.setSettings( settings );
    manager.setWindowTitle( getWindowTitle() );

    splashScreen->setMessage( QTranslator::tr( "Loading settings" ), font );
    manager.readSettings();

    splashScreen->setMessage( QTranslator::tr( "Now ready" ), font );
    manager.nowReadyPhase();

    qApp->setActiveWindow( &manager );
    splashScreen->deleteLater();

    manager.loadNotes();

    QObject::connect( &app, SIGNAL( messageReceived(QString) ), &manager, SLOT( messageReceived(QString) ) );

    return app.exec();
}
