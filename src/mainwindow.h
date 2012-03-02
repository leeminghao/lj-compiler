/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : mainwindow.h                                                              *
 * Details: Main Window interface for the LJ compiler                                 *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.28                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define PGMSIZE 256

class QAction;
class QActionGroup;
class QLabel;
class QMdiArea;
class QMenu;
class QToolBar;
class Editor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( void );

public slots:
    void newFile( void );
    void openFile( const QString &fileName );

protected:
    void closeEvent( QCloseEvent *event );

private slots:
    void open( void );
    void save( void );
    void saveAs( void );
    void cut( void );
    void copy( void );
    void paste( void );
    void compile( void );
    void about( void );
    void updateActions( void );
    void loadFiles( void );

private:
    void    delay( void );
    void    createActions( void );
    void    createMenus( void );
    void    createToolBars( void );
    void    createStatusBar( void );
    void    addEditor( Editor *editor );
    Editor *activeEditor( void );

    QMdiArea   *m_mdiArea;
    QLabel     *m_readyLabel;
    QWidgetList m_windows;

    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_buildMenu;
    QMenu *m_windowMenu;
    QMenu *m_helpMenu;

    QToolBar *m_fileToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_buildToolBar;

    QActionGroup *m_windowActionGroup;
    QAction      *m_newAction;
    QAction      *m_openAction;
    QAction      *m_saveAction;
    QAction      *m_saveAsAction;
    QAction      *m_exitAction;
    QAction      *m_cutAction;
    QAction      *m_copyAction;
    QAction      *m_pasteAction;
    QAction      *m_compileAction;
    QAction      *m_closeAction;
    QAction      *m_closeAllAction;
    QAction      *m_tileAction;
    QAction      *m_cascadeAction;
    QAction      *m_nextAction;
    QAction      *m_previousAction;
    QAction      *m_separatorAction;
    QAction      *m_aboutAction;
    QAction      *m_aboutQtAction;
};

#endif // MAINWINDOW_H
