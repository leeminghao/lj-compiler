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

#ifndef EDITOR_H
#define EDITOR_H

#include <QTextEdit>

class Editor : public QTextEdit
{
    Q_OBJECT

public:
    Editor( QWidget *parent = 0 );

    void     newFile( void );
    QString  currentFile( void );
    bool     save( void );
    bool     saveAs( void );
    QSize    sizeHint( void ) const;
    QAction *windowMenuAction() const { return m_action; }

    static Editor *open( QWidget *parent = 0 );
    static Editor *openFile( const QString &fileName, QWidget *parent = 0 );

protected:
    void closeEvent( QCloseEvent *event );

private slots:
    void documentWasModified( void );

private:
    bool okToContinue( void );
    bool saveFile( const QString &fileName );
    void setCurrentFile( const QString &fileName );
    bool readFile( const QString &fileName );
    bool writeFile( const QString &fileName );
    QString strippedName( const QString &fullFileName );

    QString  m_curFile;
    bool     m_isUntitled;
    QAction *m_action;
};

#endif // EDITOR_H
