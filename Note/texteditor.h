#ifndef TextEditor_H
#define TextEditor_H

#include <QTextEdit>
#include <QKeyEvent>
#include <QTextBrowser>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>

#include "completer.h"
#include "utils/func.h"
#include <QTimer>

//! Редактор заметки
class TextEditor: public QTextBrowser
{
    Q_OBJECT

public:
    TextEditor( QWidget * parent = 0 );

private:
    QTimer anchorTooltipTimer;

private slots:
    void sl_AnchorTooltipTimer_Timeout();

signals:
    void sg_LinkClicked( QUrl );

protected:
    void focusInEvent( QFocusEvent * event );
    void changeEvent( QEvent * event );
    void contextMenuEvent( QContextMenuEvent * event );
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
};

#endif
