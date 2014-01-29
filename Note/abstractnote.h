/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com
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


#ifndef ABSTRACTNOTE_H
#define ABSTRACTNOTE_H

#include <QWidget>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStatusBar>
#include <QLabel>
#include <QToolButton>
#include <QToolBar>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QSettings>
#include <QPaintEvent>
#include <QPainter>

class d_AbstractNote;
class d_AbstractBody;

#include "abstractnotehead.h"
#include "abstractnotebody.h"
#include "utils/propertyattachable.h"

//! Пространство содержит перечисления событий, отправляемые заметкой.
namespace EventsNote
{
    //! События, отправляемые заметкой.
    enum EventNote
    {
        Remove,           /**< Удаление */
        SaveEnded,        /**< Завершено сохранение настроек */
        LoadEnded,        /**< Завершена загрузка настроек */
        Close,            /**< Закрытие */
        ChangeTitle,      /**< Изменен заголовок (название) заметки */
        ChangeFontTitle,  /**< Изменен шрифт заголовка заметки */
        ChangeColorTitle, /**< Изменен цвет заголовка (головы) заметки */
        ChangeColorNote,  /**< Изменен цвет тела заметки */
        ChangeTop,        /**< Изменение состояния "поверх всех окон" на "позади всех окон", или наоборот */
        IShow,            /**< Заметка видима */
        IHidden,          /**< Заметка спрятана */
        ChangeVisibility, /**< Изменение видимости заметки */
        ChangeText,       /**< Изменение контекста заметки */
//        ChangeReadOnly,   /**< Изменение состояния "только чтение" */
        ChangeAttach,     /**< Произошло прикрепление файла или удаление прикрепленного файла */
        Duplicate         /**< Произошло дуплирование (создание копии) заметки */
    };
}

//! Класс реализует абстрактную заметку.
/*! Абстрактная заметка - окно с возможностью добавления на него любого функционала. */
class AbstractNote: public QMainWindow
{
    Q_OBJECT

public:
    //! Создание и инициализация заметки.
    explicit AbstractNote( QWidget * parent = 0 );
    ~AbstractNote();

    //! Указание центрального виджета.
    /*! Центральный виджет будет размещен на теле заметки.
     *  \param w указатель на центральный виджет
     *  \sa widget(), AbstractNoteHead::setWidget()
     */
    void setWidget( QWidget * w );

    //! Функция возвращает указатель на центральный виджет.
    /*! \sa setWidget(), AbstractNoteHead::widget() */
    QWidget * widget();

    //! Добавление виджета на панель инструментов заметки.
    /*! \param w указатель на добавляемый виджет
     *  \sa addSeparatorToToolBar(), AbstractNoteHead::addWidgetToToolBar()
     */
    void addWidgetToToolBar( QWidget * w );

    //! Добавление разделителя на панель инструментов заметки.
    /*! \sa addWidgetToToolBar(), AbstractNoteHead::addSeparatorToToolBar() */
    void addSeparatorToToolBar();

    //! Добавление действия в заметку.
    /*! \param action указатель на действие
     *  \sa addActions(), insertAction(), insertActions(), removeAction()
     */
    void addAction( QAction * action );

    //! Добавление списка действий в заметку.
    /*! \param actions ссылка на список действий
     *  \sa addAction(), insertAction(), insertActions(), removeAction()
     */
    void addActions( const QList < QAction * > & actions );

    //! Вставка действия в заметку.
    /*! Вставка действия перед before.
     *  \param before указатель на действие
     *  \param action ссылка на вставляемое действие
     *  \sa addAction(), addActions(), insertActions(), removeAction()
     */
    void insertAction( QAction * before, QAction * action );

    //! Вставка списка действий в заметку.
    /*! Вставка списка перед before.
     *  \param before указатель на действие
     *  \param actions ссылка на список действий
     *  \sa addAction(), addActions(), insertAction(), removeAction()
     */
    void insertActions( QAction * before, QList < QAction * > & actions );

    //! Удаление действия из заметки.
    /*! \sa addAction(), addActions(), insertAction(), insertActions() */
    void removeAction( QAction * action );

    //! Возврат меню заметки.
    /*! Меню содержит все добавленные действия и меню заметки.
     *  \return указатель на меню
     */
    QMenu * menu();

    //! Добавление разделителя.
    void addSeparator();

    //! Добавление меню в заметку.
    /*! \param menu ссылка на список действий */
    void addMenu( QMenu * menu );

protected:
    d_AbstractNote * d; //!< Дата абстрактной заметки
    AbstractNoteHead * head; //!< Голова (шапка) заметки
    AbstractNoteBody * body; //!< Тело заметки
    QMenu contextMenu; //!< Меню заметки
    PropertyAttachable * propertyAttachable; //!< Класс, который дает возможность прикрепляться к краю экрана
    QVariantMap mapSettings;

public slots:    
    //! Функция устанавливает минимальную дистанцию прикрепления.
    /*! \param d расстояние в пикселях
     *  \sa PropertyAttachable::setDistance(), distanceAttachable()
     */
    void setDistanceAttachable( int d );

    //! Функция возвращает минимальную дистанцию прикрепления.
    /*! \return расстояние в пикселях
     *  \sa PropertyAttachable::distance(), setDistanceAttachable()
     */
    int distanceAttachable();

    //! Изменение возможности прикрепления.
    /*! \param act если равно true - можно прикрепляться, иначе нельзя
     *  \sa PropertyAttachable::setActivate(), isActivate()
     */
    void setActivateAttachable( bool act );

    //! Возвращает состояние возможности прикрепления.
    /*! \return true - если прикрепление включено, иначе - false
     *  \sa PropertyAttachable::isActivate(), setActivate()
     */
    bool isActivateAttachable();

    //! Изменение видимости рамки заметки.
    /*! \sa visibleFrame() */
    void setVisibleFrame( bool visible );

    //! Возвращение видимости рамки заметки.
    /*! \sa setVisibleFrame() */
    bool visibleFrame();

    //! Изменение толщины рамки заметки.
    /*! \sa widthPenSides() */
    void setWidthPenSides( float width );

    //! Возвращение толщины рамки заметки.
    /*! \sa setWidthPenSides() */
    float widthPenSides();

    //! Изменение цвета рамки заметки.
    /*! \sa colorSides() */
    void setColorSides( const QColor & color );

    //! Возвращение цвета рамки заметки.
    /*! \sa setColorSides() */
    QColor colorSides();

    //! Изменение видимости панели инструментов.
    /*! \sa isVisibleToolBar(), AbstractNoteBody::setVisibleToolBar() */
    void setVisibleToolBar( bool visible );

    //! Возвращение видимости панели инструментов.
    /*! \retval true - панель видима
     *  \retval false - панель невидима
     *  \sa setVisibleToolBar(), AbstractNoteBody::isVisibleToolBar()
     */
    bool isVisibleToolBar();

    //! Установка заголовка (названия) заметки.
    /*! \param str текст
     *  \sa title(), AbstractNoteHead::setTitle()
     */
    void setTitle( const QString & str );

    //! Возвращение заголовка (названия) заметки.
    /*! \sa setTitle(), AbstractNoteHead::title() */
    QString title();

    //! Установка шрифта заголовка заметки.
    /*! \param f класс шрифта
     *  \sa titleFont(), AbstractNoteHead::setFont()
     */
    void setTitleFont( const QFont & f );

    //! Возвращение шрифта заголовка заметки.
    /*! \sa setTitleFont(), AbstractNoteHead::font() */
    QFont titleFont();

    //! Устанавливает заметку поверх всех окон.
    /*! \sa isTop() */
    void setTop( bool b );

    //! Возвращает состояние "поверх всех окон".
    /*! \return true, если состояние заметки "поверх всех окон", иначе false
     *  \sa setTop()
     */
    bool isTop();

    //! Устанавливает цвет тела заметки.
    void setBodyColor( const QColor & c );

    //! Возвращение цвета тела заметки.
    QColor bodyColor();

    //! Устанавливает цвет головы заметки.
    /*! \sa titleColor(), AbstractNoteHead::setColor() */
    void setTitleColor( const QColor & color );

    //! Возвращение цвета головы заметки.
    /*! \sa setTitleColor(), AbstractNoteHead::color() */
    QColor titleColor();

    //! Устанавливает прозрачность заметки.
    /*! Прозрачность задается в диапазоне от 0.2 до 1.0 (полная видимость).
     *  \param o значение прозрачности
     *  \sa opacity()
     */
    void setOpacity( qreal o );

    //! Возвращение прозрачность заметки.
    /*! \return значение в диапазоне от 0.2 до 1.0
     *  \sa setOpacity()
     */
    qreal opacity();

signals:
    //! Сигнал отсылается при изменении состояния заметки или при происшествии определенного события.
    /*! \sa EventNote() */
    void changed( int );

    //! Сигнал отсылается при изменении видимости панели инструментов.
    /*! \sa AbstractNoteBody::changeVisibleToolbar(bool) */
    void changeVisibleToolbar( bool );

protected:
    void closeEvent( QCloseEvent * );
    void contextMenuEvent( QContextMenuEvent * event );
    void showEvent(QShowEvent * event);
    void hideEvent( QHideEvent * event );
    void paintEvent( QPaintEvent * );
};

//! Класс данных абстрактной заметки.
/*! \sa AbstractNote */
class d_AbstractNote
{
public:
    d_AbstractHead * d_head;    //!< Дата "головы" заметки
    d_AbstractBody * d_body;    //!< Дата "тела" заметки

    bool sides;                 //!< Видимость рамки
    Shared::Sides penSides;     //!< Указание какие стороны рамки показывать
    QColor colorSides;          //!< Цвет рамки
    qreal widthPenSides;        //!< Ширина рамки
};

#endif // ABSTRACTNOTE_H
