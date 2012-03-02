/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : editor.h                                                                  *
 * Details: Editor interface for the LJ compiler                                      *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.28                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "editor.h"
#include <QtGui>

Editor::Editor( QWidget *parent )
    : QTextEdit( parent )
{
    m_action = new QAction( this );
    m_action->setCheckable( true );
    connect( m_action, SIGNAL( triggered() ), this, SLOT( show() ) );
    connect( m_action, SIGNAL( triggered() ), this, SLOT( setFocus() ) );
    m_isUntitled = true;
    connect(
        document(), SIGNAL( contentsChanged() ),
        this,       SLOT( documentWasModified() )
        );
    setWindowIcon( QPixmap( ":/images/document.png" ) );
    setWindowTitle( "[*]" );
    setAttribute( Qt::WA_DeleteOnClose );
}

void
Editor::newFile( void )
{
    static int documentNumber = 1;

    m_curFile = tr( "document%1.lj" ).arg( documentNumber );
    setWindowTitle( m_curFile + "[*]" );
    m_action->setText( m_curFile );
    m_isUntitled = true;
    ++documentNumber;
}

QString
Editor::currentFile( void )
{
    return m_curFile;
}

bool
Editor::save( void )
{
    if ( m_isUntitled )
    {
        return saveAs();
    }
    else
    {
        return saveFile( m_curFile );
    }
}

bool
Editor::saveAs( void )
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr( "Save As" ),
        m_curFile
        );

    if ( fileName.isEmpty() )
    {
        return false;
    }

    return saveFile( fileName );
}

QSize
Editor::sizeHint( void ) const
{
    return QSize( 72 * fontMetrics().width( 'x' ),
                  25 * fontMetrics().lineSpacing()
                  );
}

Editor *
Editor::open( QWidget *parent )
{
    QString fileName =
        QFileDialog::getOpenFileName( parent, tr( "Open" ), "." );

    if ( fileName.isEmpty() )
    {
        return 0;
    }

    return openFile( fileName, parent );
}

Editor *
Editor::openFile( const QString &fileName, QWidget *parent )
{
    Editor *editor = new Editor( parent );

    if ( editor->readFile( fileName ) )
    {
        editor->setCurrentFile( fileName );
        return editor;
    }
    else
    {
        delete editor;
        return 0;
    }
}

void
Editor::closeEvent( QCloseEvent *event )
{
    if ( okToContinue() )
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void
Editor::documentWasModified( void )
{
    setWindowModified( true );
}

bool
Editor::okToContinue( void )
{
    if ( document()->isModified() )
    {
        int r = QMessageBox::warning(
            this,
            tr( "LJ Compiler" ),
            tr( "File %1 has been modified.\n"
                "Do you want to save your changes?"
              ).arg( strippedName( m_curFile ) ),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

        if ( r == QMessageBox::Yes )
        {
            return save();
        }
        else if ( r == QMessageBox::Cancel )
        {
            return false;
        }
    }

    return true;
}

bool
Editor::saveFile( const QString &fileName )
{
    if ( writeFile( fileName ) )
    {
        setCurrentFile( fileName );
        return true;
    }
    else
    {
        return false;
    }
}

void
Editor::setCurrentFile( const QString &fileName )
{
    m_curFile    = fileName;
    m_isUntitled = false;
    m_action->setText( strippedName( m_curFile ) );
    document()->setModified( false );
    setWindowTitle( strippedName( m_curFile ) + "[*]" );
    setWindowModified( false );
}

bool
Editor::readFile( const QString &fileName )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        QMessageBox::warning(
            this,
            tr( "LJ Compiler" ),
            tr( "Cannot read file %1:\n%2." )
            .arg( file.fileName() )
            .arg( file.errorString() )
            );

        return false;
    }

    QTextStream in( &file );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    setPlainText( in.readAll() );
    QApplication::restoreOverrideCursor();

    return true;
}

bool
Editor::writeFile( const QString &fileName )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        QMessageBox::warning(
            this,
            tr( "LJ compiler" ),
            tr( "Connot write file %1:\n%2." )
            .arg( file.fileName() )
            .arg( file.errorString() )
            );

        return false;
    }

    QTextStream out( &file );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    return true;
}

QString
Editor::strippedName( const QString &fullFileName )
{
    return QFileInfo( fullFileName ).fileName();
}
