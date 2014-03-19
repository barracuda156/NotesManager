#include "mysplashscreen.h"
#include <QApplication>

MySplashScreen::MySplashScreen( const QPixmap & pixmap, Qt::WindowFlags f )
    : QSplashScreen( pixmap, f )
{
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
    setCursor( Qt::ArrowCursor );

    labelMessage = new QLabel();

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addSpacing( 60 );
    layout->addWidget( labelMessage, 0, Qt::AlignCenter );
    setLayout(layout);
}

void MySplashScreen::setMessage( const QString & message, const QFont & font, const QColor & color )
{
    qApp->processEvents();

    labelMessage->setText( QString( "<font color=\"%2\">%1</font>" ).arg( message ).arg( color.name() ) );
    labelMessage->setFont( font );
}

void MySplashScreen::mousePressEvent( QMouseEvent * event )
{
    if ( event->button() == Qt::LeftButton )
    {
        oldPosition = event->pos();
        buttonMousePress = true;
    }
}
void MySplashScreen::mouseReleaseEvent( QMouseEvent * event )
{
    buttonMousePress = false;
}
void MySplashScreen::mouseMoveEvent( QMouseEvent * event )
{
    if ( buttonMousePress )
    {
        QPoint delta = event->pos() - oldPosition;
        move( pos() + delta );
    }
}
