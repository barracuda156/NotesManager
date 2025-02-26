#include "texteditor.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QMenu>
#include <QToolTip>
#include "FormattingToolbar/DialogInsertHyperlink.h"

TextEditor::TextEditor( QWidget * parent ):
    QTextBrowser( parent )
{
    // Возвращаем функциональность текстового редактора и настраиваем работу с гиперссылками
    setUndoRedoEnabled( true );
    setReadOnly( false );

    anchorTooltipTimer.setInterval( 1000 );
    anchorTooltipTimer.setSingleShot( true );
    QObject::connect( &anchorTooltipTimer, SIGNAL(timeout()), SLOT(sl_AnchorTooltipTimer_Timeout()) );
}

QTextFragment TextEditor::findFragmentAtPos(QPoint pos)
{
    QTextCursor cursor = cursorForPosition(pos);
    qDebug() << "Cursor data: start: " << cursor.position() << ", end: " << cursor.selectionEnd() - cursor.position();
    QTextBlock block = cursor.block();

    if (!block.isValid()) {return QTextFragment();}
    QTextBlock::iterator it;
    for(it = block.begin(); !(it.atEnd()); ++it) {
        QTextFragment currentFragment = it.fragment();
        if (!currentFragment.isValid())
            continue;
        if (cursor.position() >= currentFragment.position() &&
                cursor.position() <= currentFragment.position() + currentFragment.length()) {
            return currentFragment;
        }
    }
    return QTextFragment();
}

void TextEditor::openUrl(const QUrl & url)
{
    bool isFile = QFileInfo( url.toString() ).isFile();
    // TODO: добавить возможность открывать файлы и папки (https://github.com/gil9red/ContainerExes/blob/master/USupport.h: runFile()
    // и https://github.com/gil9red/BatchImageConverter/blob/master/UBatchImageConverter/UProgressFileDialog.cpp: openFolder)
    bool successful = QDesktopServices::openUrl( isFile ? QUrl::fromLocalFile( url.toString() ) : url );
    if ( !successful )
        WARNING("Error when to link " + url.toString());
    emit sg_LinkClicked( url );
}

void TextEditor::sl_FollowLinkAction()
{
    QAction * action = qobject_cast < QAction * > ( QObject::sender() );
    if ( !action )
    {
        WARNING("null pointer!");
        return;
    }

    QPoint pos = action->data().toPoint();
    if ( pos.isNull() )
    {
        WARNING("pos is null");
        return;
    }

    openUrl( QUrl( anchorAt(pos) ) );
}
void TextEditor::sl_RemoveLinkAction()
{
    QAction * action = qobject_cast < QAction * > ( QObject::sender() );
    if ( !action )
    {
        WARNING("null pointer!");
        return;
    }

    QPoint pos = action->data().toPoint();
    if ( pos.isNull() )
    {
        WARNING("pos is null");
        return;
    }

    QTextFragment currentFragment = findFragmentAtPos(pos);
    if (!currentFragment.isValid())
    {
        WARNING("Found invalid fragment.");
        return;
    }

    QTextCharFormat format = currentFragment.charFormat();
    if (!format.isAnchor())
    {
        WARNING("Found fragment isn't anchor");
        return;
    }

    QTextCursor cursor(document());
    format.clearProperty(QTextFormat::IsAnchor);
    format.clearProperty(QTextFormat::AnchorHref);
    format.clearProperty(QTextFormat::AnchorName);
    format.clearProperty(QTextFormat::ForegroundBrush);
    format.setFontUnderline(false);
    format.setUnderlineColor(QColor());
    cursor.setPosition(currentFragment.position());
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, currentFragment.length());
    cursor.beginEditBlock();
    cursor.setCharFormat(format);
    cursor.endEditBlock();
    return;

}
void TextEditor::sl_EditLinkAction()
{
    QAction * action = qobject_cast < QAction * > ( QObject::sender() );
    if ( !action )
    {
        WARNING("null pointer!");
        return;
    }
    QPoint pos = action->data().toPoint();
    if ( pos.isNull() )
    {
        WARNING("pos is null");
        return;
    }

    QTextFragment currentFragment = findFragmentAtPos(pos);
    if (!currentFragment.isValid()) {
        WARNING("Found invalid fragment.");
        return;
    }
    QTextCharFormat format = currentFragment.charFormat();
    if (!format.isAnchor()) {
        WARNING("Found fragment isn't anchor");
        return;
    }

    // Хоть этот диалог не предназначен для этого, воспользуемся им
    // TODO: сделать отдельный диалог для редактирования гиперссылок
    DialogInsertHyperlink dialogInsertHyperlink( this );
    dialogInsertHyperlink.setWindowTitle( tr( "Edit hyperlink" ) );
    dialogInsertHyperlink.setHyperlink( format.property( QTextFormat::AnchorHref ).toString() );
    dialogInsertHyperlink.setTextHyperlink( currentFragment.text() );
    if ( dialogInsertHyperlink.exec() == QDialog::Rejected )
        return;

    format.setProperty( QTextFormat::AnchorHref, dialogInsertHyperlink.getHyperlink() );

    QTextCursor cursor(document());
    cursor.setPosition(currentFragment.position());
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, currentFragment.length());
    cursor.beginEditBlock();
    cursor.setCharFormat(format);
    cursor.insertText(dialogInsertHyperlink.getTextHyperlink());
    cursor.endEditBlock();
}

void TextEditor::sl_AnchorTooltipTimer_Timeout()
{
    const QString & href = anchorAt( mapFromGlobal( QCursor::pos() ) );
    if ( !href.isEmpty() )
        QToolTip::showText( QCursor::pos(), href + "\n" + tr( "Ctrl+Click to go" ), this );
}

void TextEditor::focusInEvent( QFocusEvent * event )
{
    // Если словарь не пуст
    if ( Completer::instance()->count() )
    {
        Completer::instance()->setEditor( this );

        // язык назначается при назначении редактора и меняется при смене раскладки
        QString lang = getTheCurrentLanguageKeyboardLayouts();
        Completer::instance()->setDictionary( lang );
    }

    QTextBrowser::focusInEvent( event );
}
void TextEditor::changeEvent( QEvent * event )
{
    if ( event->type() == QEvent::KeyboardLayoutChange )
    {
        // Если словарь не пуст
        if ( Completer::instance()->count() )
        {
            // язык назначается при назначении редактора и меняется при смене раскладки
            QString lang = getTheCurrentLanguageKeyboardLayouts();
            Completer::instance()->setDictionary( lang );
        }
    }

    QTextBrowser::changeEvent(event);
}
void TextEditor::contextMenuEvent( QContextMenuEvent * event )
{
    QMenu * menu = QTextEdit::createStandardContextMenu();
    if ( !actions().isEmpty() )
    {
        menu->addSeparator();
        menu->addActions( actions() );
    }

//    QTextCursor cursor = cursorForPosition( event->pos() );
//    qDebug();
//    qDebug() << textCursor().currentTable()
//             << textCursor().currentList()
//             << textCursor().currentFrame();

//    qDebug() << cursor.currentTable()
//             << cursor.currentList()
//             << cursor.currentFrame();

//    if ( cursor.currentTable() )
//        menu->addAction( "Table" );

//    if ( cursor.currentList() )
//        menu->addAction( "List" );

//    if ( cursor.currentFrame() )
//    {
//        menu->addAction( "Frame" );
//    }

    const QString & anchor = anchorAt( event->pos() );
    if ( !anchor.isEmpty() )
    {
        menu->addSeparator();
        // TODO: добавить иконки действиям
        QAction * followLinkAction = menu->addAction( tr( "Follow link" ) );
        followLinkAction->setData(event->pos());
        QObject::connect( followLinkAction, SIGNAL(triggered()), SLOT(sl_FollowLinkAction()) );

        QAction * removeLinkAction = menu->addAction( tr( "Remove link" ) );
        removeLinkAction->setData(event->pos());
        QObject::connect( removeLinkAction, SIGNAL(triggered()), SLOT(sl_RemoveLinkAction()) );

        QAction * editLinkAction = menu->addAction( tr( "Edit link" ) );
        editLinkAction->setData(event->pos());
        QObject::connect( editLinkAction, SIGNAL(triggered()), SLOT(sl_EditLinkAction()) );
    }

    menu->exec( event->globalPos() );
    menu->deleteLater();
}

void TextEditor::mouseMoveEvent( QMouseEvent * event )
{
    QTextBrowser::mouseMoveEvent( event );

    const QString & href = anchorAt( event->pos() );
    if ( !href.isEmpty() )
    {
        anchorTooltipTimer.start();

        if ( event->modifiers() == Qt::ControlModifier && viewport()->cursor().shape() != Qt::PointingHandCursor )
            viewport()->setCursor( Qt::PointingHandCursor );
    } else
    {
        if ( viewport()->cursor().shape() != Qt::IBeamCursor )
            viewport()->setCursor( Qt::IBeamCursor );

        if ( anchorTooltipTimer.isActive() )
            anchorTooltipTimer.stop();
        if ( QToolTip::isVisible() )
            QToolTip::hideText();
    }
}
void TextEditor::mousePressEvent( QMouseEvent * event )
{
    QTextBrowser::mousePressEvent( event );

    if ( event->modifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton )
    {
        const QString & href = anchorAt( event->pos() );
        if ( !href.isEmpty() )
            openUrl( QUrl( href ) );
    }
}
