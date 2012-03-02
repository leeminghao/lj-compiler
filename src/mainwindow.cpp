/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : mainwindow.cpp                                                            *
 * Details: Main Window implementation for the LJ compiler                            *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.28                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "editor.h"
#include "mainwindow.h"

#include <QtGui>

/* core include */
#include "globals.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "symtab.h"
#include "analyze.h"
#include "code.h"
#include "cgen.h"

/* allocate global variables */
int   g_lineno  = 0;
FILE *g_source  = NULL;
FILE *g_listing = NULL;
FILE *g_code    = NULL;

/* allocate and set tracing flags */
bool g_echoSource   = true;
bool g_traceScan    = true;
bool g_traceParse   = true;
bool g_traceAnalyze = true;
bool g_traceCode    = true;
bool g_error        = false;

MainWindow::MainWindow( void )
{
    delay();
    m_mdiArea = new QMdiArea;
    setCentralWidget( m_mdiArea );
    connect(
        m_mdiArea, SIGNAL( subWindowActivated( QMdiSubWindow * ) ),
        this,      SLOT( updateActions() )
        );
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    setWindowIcon( QPixmap( ":/images/icon.png" ) );
    setWindowTitle( tr( "LJ Compiler" ) );
    QTimer::singleShot( 0, this, SLOT( loadFiles() ) );
}

void
MainWindow::loadFiles( void )
{
    QStringList args = QApplication::arguments();

    args.removeFirst();
    if ( !args.isEmpty() )
    {
        foreach ( QString arg, args )
        {
            openFile( arg );
        }
        m_mdiArea->cascadeSubWindows();
    }
    else
    {
        newFile();
    }
    m_mdiArea->activateNextSubWindow();
}

void
MainWindow::newFile( void )
{
    Editor *editor = new Editor;

    editor->newFile();
    addEditor( editor );
}

void
MainWindow::openFile( const QString &fileName )
{
    Editor *editor = Editor::openFile( fileName, this );

    if ( editor )
    {
        addEditor( editor );
    }
}

void
MainWindow::closeEvent( QCloseEvent *event )
{
    m_mdiArea->closeAllSubWindows();
    if ( !m_mdiArea->subWindowList().isEmpty() )
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void
MainWindow::open( void )
{
    Editor *editor = Editor::open( this );

    if ( editor )
    {
        addEditor( editor );
    }
}

void
MainWindow::save( void )
{
    if ( activeEditor() )
    {
        activeEditor()->save();
    }
}

void
MainWindow::saveAs( void )
{
    if ( activeEditor() )
    {
        activeEditor()->saveAs();
    }
}

void
MainWindow::cut( void )
{
    if ( activeEditor() )
    {
        activeEditor()->cut();
    }
}

void
MainWindow::copy( void )
{
    if ( activeEditor() )
    {
        activeEditor()->copy();
    }
}

void
MainWindow::paste( void )
{
    if ( activeEditor() )
    {
        activeEditor()->paste();
    }
}

void
MainWindow::compile( void )
{
    QString   fileName     = activeEditor()->currentFile();
    char      pgm[PGMSIZE] = { '\0' };
    char     *codefile;
    char     *listFile;
    int       fnlen;
    TreeNode *syntaxTree;

    strncpy( pgm, fileName.toStdString().c_str(), PGMSIZE - 1 );
    fnlen = strcspn( pgm, "." );

    listFile = new char[fnlen + 6];
    strncpy( listFile, pgm, strlen( pgm ) - 3 );
    strcat( listFile, ".list" );

    codefile = new char[fnlen + 4];
    strncpy( codefile, pgm, strlen( pgm ) - 3 );
    strcat( codefile, ".lm" );

    if ( strchr( pgm, '.' ) == NULL )
    {
        strcat( pgm, ".lj" );
    }
    g_source = fopen( pgm, "r" );
    if ( g_source == NULL )
    {
        fprintf( stderr, "File %s not found\n", pgm );
        exit( 1 );
    }
    g_listing = fopen( listFile, "wa" );
    if ( g_listing == NULL )
    {
        fprintf( stderr, "File %s not found\n", listFile );
        exit( 1 );
    }
    fprintf( g_listing, "\nLJ COMPILATION: %s\n", pgm );
    syntaxTree = Parser::parse();
    if ( g_traceParse )
    {
        fprintf( g_listing, "\nSyntax tree:\n" );
        Util::printTree( syntaxTree );
    }
    if ( !g_error )
    {
        fprintf( g_listing, "\nBuilding Symtbol Table ...\n" );
        Analyze::buildSymtab( syntaxTree );
        fprintf( g_listing, "\nChecking Types..." );
        Analyze::typeCheck( syntaxTree );
        fprintf( g_listing, "\nType Checking Finished\n" );
    }
    if ( !g_error )
    {
        g_code = fopen( codefile, "w" );
        if ( g_code == NULL )
        {
            printf( "Unable to open %s\n", codefile );
            exit( 1 );
        }
        CodeGenerator::codeGen( syntaxTree, codefile );
        fclose( g_code );
    }
    fclose( g_listing );
    fclose( g_source );
    m_compileAction->setEnabled( false );
}

void
MainWindow::about( void )
{
    QMessageBox::about(
        this,
        tr( "About LJ Compiler" ),
        tr( "<h2>LJ Compiler 0.0</h2>"
            "<p>Copyright &copy; 2011 Software Inc."
            "<p>LJ Compiler is a small compiler for my girl friend guolinjing." )
        );
}

void
MainWindow::updateActions( void )
{
    bool hasEditor    = (activeEditor() != 0);
    bool hasSelection = activeEditor()
        && activeEditor()->textCursor().hasSelection();

    m_saveAction->setEnabled( hasEditor );
    m_saveAsAction->setEnabled( hasEditor );
    m_cutAction->setEnabled( hasSelection );
    m_copyAction->setEnabled( hasSelection );
    m_pasteAction->setEnabled( hasEditor );
    m_compileAction->setEnabled( hasEditor );
    m_closeAction->setEnabled( hasEditor );
    m_closeAllAction->setEnabled( hasEditor );
    m_tileAction->setEnabled( hasEditor );
    m_cascadeAction->setEnabled( hasEditor );
    m_nextAction->setEnabled( hasEditor );
    m_previousAction->setEnabled( hasEditor );
    m_separatorAction->setVisible( hasEditor );

    if ( activeEditor() )
    {
        activeEditor()->windowMenuAction()->setChecked( true );
    }
}

void
MainWindow::createActions( void )
{
    m_newAction = new QAction(tr("&New"), this);
    m_newAction->setIcon(QIcon(":/images/new.png"));
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip(tr("Create a new file"));
    connect( m_newAction, SIGNAL( triggered() ), this, SLOT( newFile() ) );

    m_openAction = new QAction(tr("&Open..."), this);
    m_openAction->setIcon(QIcon(":/images/open.png"));
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip(tr("Open an existing file"));
    connect(m_openAction, SIGNAL(triggered()), this, SLOT(open()));

    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setIcon(QIcon(":/images/save.png"));
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip(tr("Save the file to disk"));
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAction = new QAction(tr("Save &As..."), this);
    m_saveAsAction->setStatusTip(tr("Save the file under a new name"));
    connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(tr("Ctrl+Q"));
    m_exitAction->setStatusTip(tr("Exit the application"));
    connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));

    m_cutAction = new QAction(tr("Cu&t"), this);
    m_cutAction->setIcon(QIcon(":/images/cut.png"));
    m_cutAction->setShortcut(QKeySequence::Cut);
    m_cutAction->setStatusTip(tr("Cut the current selection to the "
                               "clipboard"));
    connect(m_cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    m_copyAction = new QAction(tr("&Copy"), this);
    m_copyAction->setIcon(QIcon(":/images/copy.png"));
    m_copyAction->setShortcut(QKeySequence::Copy);
    m_copyAction->setStatusTip(tr("Copy the current selection to the "
                                "clipboard"));
    connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    m_pasteAction = new QAction(tr("&Paste"), this);
    m_pasteAction->setIcon(QIcon(":/images/paste.png"));
    m_pasteAction->setShortcut(QKeySequence::Paste);
    m_pasteAction->setStatusTip(tr("Paste the clipboard's contents at "
                                 "the cursor position"));
    connect(m_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    m_compileAction = new QAction( tr( "Co&mpile" ), this );
    m_compileAction->setIcon( QIcon( ":/images/compiler.png" ) );
    m_compileAction->setShortcut( tr( "Ctrl+R" ) );
    m_compileAction->setStatusTip( tr( "Compile the source code use LJ compiler" ) );
    connect( m_compileAction, SIGNAL( triggered() ), this, SLOT( compile() ) );

    m_closeAction = new QAction(tr("Cl&ose"), this);
    m_closeAction->setShortcut(QKeySequence::Close);
    m_closeAction->setStatusTip(tr("Close the active window"));
    connect(m_closeAction, SIGNAL(triggered()),
            m_mdiArea, SLOT(closeActiveSubWindow()));

    m_closeAllAction = new QAction(tr("Close &All"), this);
    m_closeAllAction->setStatusTip(tr("Close all the windows"));
    connect(m_closeAllAction, SIGNAL(triggered()), this, SLOT(close()));

    m_tileAction = new QAction(tr("&Tile"), this);
    m_tileAction->setStatusTip(tr("Tile the windows"));
    connect(m_tileAction, SIGNAL(triggered()),
            m_mdiArea, SLOT(tileSubWindows()));

    m_cascadeAction = new QAction(tr("&Cascade"), this);
    m_cascadeAction->setStatusTip(tr("Cascade the windows"));
    connect(m_cascadeAction, SIGNAL(triggered()),
            m_mdiArea, SLOT(cascadeSubWindows()));

    m_nextAction = new QAction(tr("Ne&xt"), this);
    m_nextAction->setShortcut(QKeySequence::NextChild);
    m_nextAction->setStatusTip(tr("Move the focus to the next window"));
    connect(m_nextAction, SIGNAL(triggered()),
            m_mdiArea, SLOT(activateNextSubWindow()));

    m_previousAction = new QAction(tr("Pre&vious"), this);
    m_previousAction->setShortcut(QKeySequence::PreviousChild);
    m_previousAction->setStatusTip(tr("Move the focus to the previous "
                                    "window"));
    connect(m_previousAction, SIGNAL(triggered()),
            m_mdiArea, SLOT(activatePreviousSubWindow()));

    m_separatorAction = new QAction(this);
    m_separatorAction->setSeparator(true);

    m_aboutAction = new QAction(tr("&About"), this);
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    m_aboutQtAction = new QAction(tr("About &Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    m_windowActionGroup = new QActionGroup(this);
}

void
MainWindow::createMenus( void )
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_editMenu->addAction(m_cutAction);
    m_editMenu->addAction(m_copyAction);
    m_editMenu->addAction(m_pasteAction);

    m_buildMenu = menuBar()->addMenu( tr( "&Build" ) );
    m_buildMenu->addAction( m_compileAction );

    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    m_windowMenu->addAction(m_closeAction);
    m_windowMenu->addAction(m_closeAllAction);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_tileAction);
    m_windowMenu->addAction(m_cascadeAction);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_nextAction);
    m_windowMenu->addAction(m_previousAction);
    m_windowMenu->addAction(m_separatorAction);

    menuBar()->addSeparator();

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAction);
    m_helpMenu->addAction(m_aboutQtAction);
}

void
MainWindow::createToolBars()
{
    m_fileToolBar = addToolBar(tr("File"));
    m_fileToolBar->addAction(m_newAction);
    m_fileToolBar->addAction(m_openAction);
    m_fileToolBar->addAction(m_saveAction);

    m_editToolBar = addToolBar(tr("Edit"));
    m_editToolBar->addAction(m_cutAction);
    m_editToolBar->addAction(m_copyAction);
    m_editToolBar->addAction(m_pasteAction);

    m_buildToolBar = addToolBar( tr( "Build" ) );
    m_buildToolBar->addAction( m_compileAction );
}

void
MainWindow::createStatusBar( void )
{
    m_readyLabel = new QLabel( tr(" Welcome to use LJ compiler. Author:lmh" ) );
    statusBar()->addWidget(m_readyLabel, 1);
}

void
MainWindow::addEditor( Editor *editor )
{
    connect(
        editor, SIGNAL( copyAvailable( bool ) ),
        m_cutAction, SLOT( setEnabled( bool ) )
        );
    connect(
        editor, SIGNAL( copyAvailable( bool ) ),
        m_copyAction, SLOT( setEnabled( bool ) )
        );

    QMdiSubWindow *subWindow = m_mdiArea->addSubWindow( editor );

    m_windowMenu->addAction( editor->windowMenuAction() );
    m_windowActionGroup->addAction( editor->windowMenuAction() );
    subWindow->show();
}

Editor *
MainWindow::activeEditor( void )
{
    QMdiSubWindow *subWindow = m_mdiArea->activeSubWindow();

    if ( subWindow )
    {
        return qobject_cast<Editor *>( subWindow->widget() );
    }

    return 0;
}

void
MainWindow::delay( void )
{
    for ( int i = 0; i < 1000000; i++ )
    {
        for ( int j = 0; j < 1000; j++ )
            ;
    }
}
