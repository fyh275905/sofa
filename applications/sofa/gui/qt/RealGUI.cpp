/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/gui/qt/RealGUI.h>
#include <sofa/gui/qt/ImageQt.h>
#ifndef SOFA_GUI_QT_NO_RECORDER
#include "QSofaRecorder.h"
#endif
#include "QSofaStatWidget.h"
#include "GenGraphForm.h"
#include "QSofaListView.h"

#ifdef SOFA_GUI_QTOGREVIEWER
#include <sofa/gui/qt/viewer/qtogre/QtOgreViewer.h>
#endif

#ifdef SOFA_GUI_QTVIEWER
#include <sofa/gui/qt/viewer/qt/QtViewer.h>
#endif

#ifdef SOFA_GUI_QGLVIEWER
#include <sofa/gui/qt/viewer/qgl/QtGLViewer.h>
#endif



#ifdef SOFA_DEV

#include <sofa/simulation/automatescheduler/ThreadSimulation.h>
#include <sofa/simulation/automatescheduler/ExecBus.h>
#include <sofa/simulation/automatescheduler/Node.h>
#include <sofa/simulation/automatescheduler/AutomateUtils.h>

#endif // SOFA_DEV

#ifdef SOFA_HAVE_CHAI3D
#include <sofa/simulation/common/PropagateEventVisitor.h>
#include <sofa/core/objectmodel/GLInitializedEvent.h>
#endif // SOFA_HAVE_CHAI3D


#include <sofa/component/visualmodel/VisualModelImpl.h>
#include <sofa/simulation/common/Visitor.h>
#include <sofa/simulation/common/xml/XML.h>
#include <sofa/simulation/common/InitVisitor.h>
#include <sofa/simulation/common/UpdateContextVisitor.h>

#include <sofa/helper/system/FileRepository.h>

#define MAX_RECENTLY_OPENED 10

#ifdef SOFA_QT4
#include <QWidget>
#include <QDockWidget>
#include <QStackedWidget>
#include <QLayout>
#include <Q3ListViewItem>
#include <Q3ListView>
#include <QStatusBar>
#include <QRadioButton>
#include <QCheckBox>
#include <QSplitter>
#include <Q3TextEdit>
#include <QCursor>
#include <QAction>
#include <QMessageBox>
#include <QTabWidget>
#include <QToolTip>
#include <QButtonGroup>
#include <QRadioButton>
#include <QInputDialog>
#include <Q3DockWindow>
#include <Q3DockArea>
#else
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qstatusbar.h>
#include <qheader.h>
#include <qimage.h>
#include <qsplitter.h>
#include <qtextedit.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qtabwidget.h>
#include <qtooltip.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qinputdialog.h>
#include <qmime.h>
#include <qdockwindow.h>
#include <qdockarea.h>
#endif



namespace sofa
{

namespace gui
{

namespace qt
{

SOFA_LINK_CLASS(ImageQt);

#ifdef SOFA_QT4
typedef Q3ListView QListView;
typedef Q3DockWindow QDockWindow;
typedef QStackedWidget QWidgetStack;
typedef Q3TextEdit QTextEdit;
#endif


using sofa::core::objectmodel::BaseObject;
using namespace sofa::helper::system::thread;
using namespace sofa::simulation;
//       using namespace sofa::simulation::tree;

#ifdef SOFA_DEV
using namespace sofa::simulation::automatescheduler;
typedef sofa::simulation::automatescheduler::Node AutomateNode;
#endif // SOFA_DEV


///////////////////////////////////////////////////////////
//////////////////// SofaGUI Interface ////////////////////
///////////////////////////////////////////////////////////

#ifdef SOFA_GUI_QGLVIEWER
SOFA_DECL_CLASS ( QGLViewerGUI )
#endif
#ifdef SOFA_GUI_QTVIEWER
SOFA_DECL_CLASS ( QTGUI )
#endif
#ifdef SOFA_GUI_QTOGREVIEWER
SOFA_DECL_CLASS ( OgreGUI )
#endif

int RealGUI::InitGUI ( const char* name, const std::vector<std::string>& /* options */ )
{
    ImageQt::Init();
#ifdef SOFA_GUI_QGLVIEWER
    if ( !name[0] || !strcmp ( name,"qglviewer" ) )
    {
        return sofa::gui::qt::viewer::qgl::QtGLViewer::EnableViewer();
    }
    else
#endif
#ifdef SOFA_GUI_QTVIEWER
        if ( !name[0] || !strcmp ( name,"qt" ) )
        {
            return sofa::gui::qt::viewer::qt::QtViewer::EnableViewer();
        }
        else
#endif
#ifdef SOFA_GUI_QTOGREVIEWER
            if ( !name[0] || !strcmp ( name,"ogre" ) )
            {
                return sofa::gui::qt::viewer::qtogre::QtOgreViewer::EnableViewer();
            }
            else
#endif
            {
                std::cerr << "ERROR(QtGUI): unknown or disabled gui name "<<name<<std::endl;
                return 1;
            }
}

extern QApplication* application; // = NULL;
extern RealGUI* gui;

#ifdef SOFA_QT4
/// Custom QApplication class handling FileOpen events for MacOS
class QSOFAApplication : public QApplication
{
public:
    QSOFAApplication(int &argc, char ** argv)
        : QApplication(argc,argv)
    {
    }
protected:
    bool event(QEvent *event)
    {
        switch (event->type())
        {
        case QEvent::FileOpen:
            static_cast<RealGUI*>(mainWidget())->fileOpen(static_cast<QFileOpenEvent *>(event)->file().ascii());

            return true;
        default:
            return QApplication::event(event);
        }
    }
};
#else
typedef QApplication QSOFAApplication;
#endif

SofaGUI* RealGUI::CreateGUI ( const char* name, const std::vector<std::string>& options, sofa::simulation::Node* root, const char* filename )
{
    {
        int  *argc = new int;
        char **argv=new char*[2];
        *argc = 1;
        argv[0] = strdup ( SofaGUI::GetProgramName() );
        argv[1]=NULL;
        application = new QSOFAApplication ( *argc,argv );

    }
    // create interface
    gui = new RealGUI ( name, options );
    if ( root )
    {
        gui->setScene ( root, filename );
        gui->setWindowFilePath(QString(filename));
    }

    //gui->viewer->resetView();

    application->setMainWidget ( gui );

    QString pathIcon=(sofa::helper::system::DataRepository.getFirstPath() + std::string( "/icons/SOFA.png" )).c_str();
#ifdef SOFA_QT4
    application->setWindowIcon(QIcon(pathIcon));
#else
    gui->setIcon(QPixmap(pathIcon));
#endif

#ifdef SOFA_DEV

    // Threads Management
    if ( sofa::simulation::automatescheduler::ThreadSimulation::initialized() )
    {
        sofa::simulation::automatescheduler::ThreadSimulation::getInstance()->computeVModelsList ( root );
        root->setMultiThreadSimulation ( true );
        sofa::simulation::automatescheduler::groot = root;

        sofa::simulation::automatescheduler::Automate::setDrawCB ( gui->viewer );

        gui->viewer->getQWidget()->update();
        sofa::simulation::automatescheduler::ThreadSimulation::getInstance()->start();
    }

#endif // SOFA_DEV

    // show the gui
    gui->show();


#ifdef SOFA_HAVE_CHAI3D
    // Tell nodes that openGl is initialized
    // especialy the GL_MODELVIEW_MATRIX
    sofa::core::objectmodel::GLInitializedEvent ev;
    sofa::simulation::PropagateEventVisitor act(&ev);
    root->execute(act);
#endif // SOFA_HAVE_CHAI3D

    return gui;
}

int RealGUI::mainLoop()
{
    std::string filename=windowFilePath().ascii();
    if (filename.size() > 5 && filename.substr(filename.size()-5) == ".simu")
    {
        fileOpenSimu(filename);
    }
    return application->exec();
}

void RealGUI::redraw()
{
    emit newStep();
}

int RealGUI::closeGUI()
{
    delete this;
    return 0;
}

Node* RealGUI::currentSimulation()
{
    return viewer->getScene();
}

RealGUI::RealGUI ( const char* viewername, const std::vector<std::string>& /*options*/ )
    : viewerName ( viewername ),
      viewer ( NULL ),
      simulationGraph(NULL),
#ifndef SOFA_CLASSIC_SCENE_GRAPH
      visualGraph(NULL),
#endif
      currentTab ( NULL ),
      tabInstrument (NULL),
#ifndef SOFA_GUI_QT_NO_RECORDER
      recorder(NULL),
#else
      fpsLabel(NULL),
      timeLabel(NULL),
#endif
      statWidget(NULL),
      timerStep(NULL),
      backgroundImage(NULL),
      left_stack(NULL),
      saveReloadFile(false)
{

    connect(this, SIGNAL(quit()), this, SLOT(fileExit()));

#ifdef SOFA_QT4
    fileMenu->removeAction(Action);
#endif

    displayFlag = new DisplayFlagWidget(tabView);
    connect( displayFlag, SIGNAL( change(int,bool)), this, SLOT(showhideElements(int,bool) ));
    gridLayout1->addWidget(displayFlag,0,0);


    left_stack = new QWidgetStack ( splitter2 );
    connect ( startButton, SIGNAL ( toggled ( bool ) ), this , SLOT ( playpauseGUI ( bool ) ) );
    connect ( ResetSceneButton, SIGNAL ( clicked() ), this, SLOT ( resetScene() ) );
    connect ( dtEdit, SIGNAL ( textChanged ( const QString& ) ), this, SLOT ( setDt ( const QString& ) ) );
    connect ( stepButton, SIGNAL ( clicked() ), this, SLOT ( step() ) );
    connect ( dumpStateCheckBox, SIGNAL ( toggled ( bool ) ), this, SLOT ( dumpState ( bool ) ) );
    connect ( displayComputationTimeCheckBox, SIGNAL ( toggled ( bool ) ), this, SLOT ( displayComputationTime ( bool ) ) );
    connect ( exportGnuplotFilesCheckbox, SIGNAL ( toggled ( bool ) ), this, SLOT ( setExportGnuplot ( bool ) ) );
#ifdef SOFA_DUMP_VISITOR_INFO
    connect ( exportVisitorCheckbox, SIGNAL ( toggled ( bool ) ), this, SLOT ( setExportVisitor ( bool ) ) );
#endif
    connect( recentlyOpened, SIGNAL(activated(int)), this, SLOT(fileRecentlyOpened(int)));
    //Recently Opened Files
    std::string scenes ( "config/Sofa.ini" );
    if ( !sofa::helper::system::DataRepository.findFile ( scenes ) )
    {
        std::string fileToBeCreated = sofa::helper::system::DataRepository.getFirstPath() + "/" + scenes;

        std::ofstream ofile(fileToBeCreated.c_str());
        ofile << "";
        ofile.close();
    }
    pathDumpVisitor = sofa::helper::system::SetDirectory::GetParentDir(sofa::helper::system::DataRepository.getFirstPath().c_str()) + std::string( "/dumpVisitor.xml" );
    scenes = sofa::helper::system::DataRepository.getFile ( scenes );

    initRecentlyOpened();
    connect ( tabs, SIGNAL ( currentChanged ( QWidget* ) ), this, SLOT ( currentTabChanged ( QWidget* ) ) );

    //Create a Dock Window to receive the Sofa Recorder
#ifndef SOFA_GUI_QT_NO_RECORDER
    QDockWindow *dockRecorder=new QDockWindow(this);
    dockRecorder->setResizeEnabled(true);
    this->moveDockWindow( dockRecorder, Qt::DockBottom);
    this->leftDock() ->setAcceptDockWindow(dockRecorder,false);
    this->rightDock()->setAcceptDockWindow(dockRecorder,false);

    recorder = new QSofaRecorder(dockRecorder);
    dockRecorder->setWidget(recorder);

    connect(startButton, SIGNAL(  toggled ( bool ) ), recorder, SLOT( TimerStart(bool) ) );
#else
    //Status Bar Configuration
    fpsLabel = new QLabel ( "9999.9 FPS", statusBar() );
    fpsLabel->setMinimumSize ( fpsLabel->sizeHint() );
    fpsLabel->clear();

    timeLabel = new QLabel ( "Time: 999.9999 s", statusBar() );
    timeLabel->setMinimumSize ( timeLabel->sizeHint() );
    timeLabel->clear();
    statusBar()->addWidget ( fpsLabel );
    statusBar()->addWidget ( timeLabel );
#endif

    statWidget = new QSofaStatWidget(TabStats);
    TabStats->layout()->add(statWidget);


    addViewer();
    currentTabChanged ( tabs->currentPage() );

    //ADD GUI for Background
    //------------------------------------------------------------------------
    //Informations
    Q3GroupBox *groupInfo = new Q3GroupBox(QString("Background"), TabPage);
    groupInfo->setColumns(4);
    QWidget     *global    = new QWidget(groupInfo);
    QGridLayout *globalLayout = new QGridLayout(global);


    globalLayout->addWidget(new QLabel(QString("Colour "),global),1,0);
    for (unsigned int i=0; i<3; ++i)
    {
        std::ostringstream s;
        s<<"background" <<i;
        background[i] = new WFloatLineEdit(global,s.str().c_str());
        background[i]->setMinFloatValue( 0.0f);
        background[i]->setMaxFloatValue( 1.0f);
        background[i]->setFloatValue( 1.0f);
        globalLayout->addWidget(background[i],1,i+1);
        connect( background[i], SIGNAL( returnPressed() ), this, SLOT( updateBackgroundColour() ) );
    }

    QWidget     *global2    = new QWidget(groupInfo);
    groupInfo->setColumns(1);
    QGridLayout *globalLayout2 = new QGridLayout(global2);
    globalLayout2->addWidget(new QLabel(QString("Image "),global2),2,0);
    backgroundImage = new QLineEdit(global2,"backgroundImage");
    backgroundImage->setMinimumWidth( 200 );
    backgroundImage->setText( QString(viewer->getBackgroundImage().c_str()) );
    globalLayout2->addWidget(backgroundImage,2,1);
    connect( backgroundImage, SIGNAL( returnPressed() ), this, SLOT( updateBackgroundImage() ) );

    ((QVBoxLayout*)(TabPage->layout()))->insertWidget(1,groupInfo);

    //---------------------------------------------------------------------------------------------------
#ifdef SOFA_PML
    pmlreader = NULL;
    lmlreader = NULL;
#endif
    simulationGraph = new QSofaListView(SIMULATION,TabGraph,"SimuGraph");
    ((QVBoxLayout*)TabGraph->layout())->addWidget(simulationGraph);
    connect ( ExportGraphButton, SIGNAL ( clicked() ), simulationGraph, SLOT ( Export() ) );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
    visualGraph = new QSofaListView(VISUAL,TabVisualGraph,"VisualGraph");
    ((QVBoxLayout*)TabVisualGraph->layout())->addWidget(visualGraph);
    connect ( ExportVisualGraphButton, SIGNAL ( clicked() ), visualGraph, SLOT ( Export() ) );
#else
#ifdef SOFA_QT4
    tabs->removeTab(tabs->indexOf(TabVisualGraph));
#endif
#endif

    //Center the application
    const QRect screen = QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen());
    this->move(  ( screen.width()- this->width()  ) / 2,  ( screen.height() - this->height()) / 2  );

    connect(simulationGraph, SIGNAL( RootNodeChanged(sofa::simulation::Node*, const char*) ), this, SLOT ( NewRootNode(sofa::simulation::Node* , const char*) ) );
    connect(simulationGraph, SIGNAL( NodeRemoved() ), this, SLOT( Update() ) );
    connect(simulationGraph, SIGNAL( Lock(bool) ), this, SLOT( LockAnimation(bool) ) );
    connect(simulationGraph, SIGNAL( RequestSaving(sofa::simulation::Node*) ), this, SLOT( fileSaveAs(sofa::simulation::Node*) ) );
    connect(simulationGraph, SIGNAL( currentActivated(bool) ), viewer->getQWidget(), SLOT( resetView() ) );
    connect(simulationGraph, SIGNAL( currentActivated(bool) ), this, SLOT( Update() ) );
    connect(simulationGraph, SIGNAL( Updated() ), this, SLOT( redraw() ) );
    connect(simulationGraph, SIGNAL( NodeAdded() ), this, SLOT( Update() ) );
    connect(this, SIGNAL( newScene() ), simulationGraph, SLOT( CloseAllDialogs() ) );
    connect(this, SIGNAL( newStep() ), simulationGraph, SLOT( UpdateOpenedDialogs() ) );
#ifndef SOFA_GUI_QT_NO_RECORDER
    if (recorder)
        connect( recorder, SIGNAL( RecordSimulation(bool) ), startButton, SLOT( setOn(bool) ) );
    if (recorder)
        connect( recorder, SIGNAL( NewTime() ), viewer->getQWidget(), SLOT( update() ) );
#endif
    timerStep = new QTimer(this);
    connect ( timerStep, SIGNAL ( timeout() ), this, SLOT ( step() ) );

    animationState = false;
}

void RealGUI::fileRecentlyOpened(int id)
{
    fileOpen(recentlyOpened->text(id).ascii());
}

void RealGUI::initRecentlyOpened()
{
    recentlyOpened->clear();
    std::vector< std::string > list_files;


    std::string scenes ( "config/Sofa.ini" );
    scenes = sofa::helper::system::DataRepository.getFile ( scenes );


    std::ifstream end(scenes.c_str());
    std::string s;
    while( std::getline(end,s) )
    {
        list_files.push_back(sofa::helper::system::DataRepository.getFile(s));
        recentlyOpened->insertItem(QString(list_files.back().c_str()));
    }
    end.close();

}

void RealGUI::updateRecentlyOpened(std::string fileLoaded)
{
#ifdef WIN32
    for (unsigned int i=0; i<fileLoaded.size(); ++i)
    {
        if (fileLoaded[i] == '\\') fileLoaded[i] = '/';
    }
#endif
    std::string scenes ( "config/Sofa.ini" );

    scenes = sofa::helper::system::DataRepository.getFile ( scenes );

    std::ofstream out;
    out.open(scenes.c_str(),std::ios::out);
    if (sofa::helper::system::DataRepository.findFile(fileLoaded))
    {
        fileLoaded = sofa::helper::system::DataRepository.getFile(fileLoaded);
        out << fileLoaded << "\n";
        recentlyOpened->insertItem(QString(fileLoaded.c_str()),-1,0);
    }

    for (unsigned int i=1; i<recentlyOpened->count() && i< MAX_RECENTLY_OPENED; ++i)
    {
        std::string entry = (recentlyOpened->text(recentlyOpened->idAt(i))).ascii();
        if (fileLoaded == entry)
        {
            recentlyOpened->removeItemAt(i);
            --i;
        }
        else
        {
            out << entry << "\n";
        }
    }

    while (recentlyOpened->count() > MAX_RECENTLY_OPENED) recentlyOpened->removeItemAt(MAX_RECENTLY_OPENED);

    out.close();
}

void RealGUI::setPixmap(std::string pixmap_filename, QPushButton* b)
{
    if ( sofa::helper::system::DataRepository.findFile ( pixmap_filename ) )
        pixmap_filename = sofa::helper::system::DataRepository.getFile ( pixmap_filename );

#ifdef SOFA_QT4
    b->setPixmap(QPixmap(QPixmap::fromImage(QImage(pixmap_filename.c_str()))));
#else
    b->setPixmap(QPixmap(QImage(pixmap_filename.c_str())));
#endif
}

RealGUI::~RealGUI()
{
#ifdef SOFA_PML
    if ( pmlreader )
    {
        delete pmlreader;
        pmlreader = NULL;
    }
    if ( lmlreader )
    {
        delete lmlreader;
        lmlreader = NULL;
    }
#endif
    delete displayFlag;

#ifdef SOFA_DUMP_VISITOR_INFO
    delete windowTraceVisitor;
    delete handleTraceVisitor;
#endif
    delete viewer;
}

void RealGUI::init()
{

    frameCounter = 0;
    _animationOBJ = false;
    _animationOBJcounter = 0;
    m_dumpState = false;
    m_dumpStateStream = 0;
    m_displayComputationTime = false;
    m_exportGnuplot = false;
    gnuplot_directory = "";





#ifndef SOFA_DUMP_VISITOR_INFO
    //Remove option to see visitor trace
    this->exportVisitorCheckbox->hide();
#else
    //Main window containing a QListView only
    windowTraceVisitor = new WindowVisitor;
    windowTraceVisitor->graphView->setSorting(-1);
    windowTraceVisitor->hide();
    connect(windowTraceVisitor, SIGNAL(WindowVisitorClosed(bool)), this->exportVisitorCheckbox, SLOT(setChecked(bool)));
    handleTraceVisitor = new GraphVisitor(windowTraceVisitor);
#endif
    //--------
    descriptionScene = new QDialog(this);
    descriptionScene->resize(400,400);
    QVBoxLayout *descriptionLayout = new QVBoxLayout(descriptionScene);
    htmlPage = new QTextBrowser(descriptionScene);
    descriptionLayout->addWidget(htmlPage);
#ifdef SOFA_QT4
    connect(htmlPage, SIGNAL(sourceChanged(const QUrl&)), this, SLOT(changeHtmlPage(const QUrl&)));
#else
    // QMimeSourceFactory::defaultFactory()->setExtensionType("html", "text/utf8");
    htmlPage->mimeSourceFactory()->setExtensionType("html", "text/utf8");;
    connect(htmlPage, SIGNAL(sourceChanged(const QString&)), this, SLOT(changeHtmlPage(const QString&)));
#endif
    //--------
    SofaPluginManager::getInstance()->hide();
    SofaMouseManager::getInstance()->hide();

}

void RealGUI::addViewer()
{
    init();
    const char* name = viewerName;

    // set menu state

#ifdef SOFA_GUI_QTVIEWER
    viewerOpenGLAction->setEnabled ( true );
#else
    viewerOpenGLAction->setEnabled ( false );
    viewerOpenGLAction->setToolTip ( "enable SOFA_GUI_QTVIEWER in sofa-local.cfg to activate" );
#endif
#ifdef SOFA_GUI_QGLVIEWER
    viewerQGLViewerAction->setEnabled ( true );
#else
    viewerQGLViewerAction->setEnabled ( false );
    viewerQGLViewerAction->setToolTip ( "enable SOFA_GUI_QGLVIEWER in sofa-local.cfg to activate" );
#endif
#ifdef SOFA_GUI_QTOGREVIEWER
    viewerOGREAction->setEnabled ( true );
#else
    viewerOGREAction->setEnabled ( false );
    viewerOGREAction->setToolTip ( "enable SOFA_GUI_QTOGREVIEWER in sofa-local.cfg to activate" );
#endif

#ifdef SOFA_GUI_QGLVIEWER
    if ( !name[0] || !strcmp ( name,"qglviewer" ) )
    {
        viewer = new sofa::gui::qt::viewer::qgl::QtGLViewer ( left_stack, "viewer" );
        viewerQGLViewerAction->setOn ( true );
    }
    else
#endif
#ifdef SOFA_GUI_QTVIEWER
        if ( !name[0] || !strcmp ( name,"qt" ) )
        {
            viewer = new sofa::gui::qt::viewer::qt::QtViewer ( left_stack, "viewer" );
            viewerOpenGLAction->setOn ( true );
        }
        else
#endif
#ifdef SOFA_GUI_QTOGREVIEWER
            if ( !name[0] || !strcmp ( name,"ogre" ) )
            {
                viewer = new sofa::gui::qt::viewer::qtogre::QtOgreViewer ( left_stack , "viewer" );
                viewerOGREAction->setOn ( true );
            }
            else
#endif
            {
                std::cerr << "ERROR(QtGUI): unknown or disabled viewer name "<<name<<std::endl;
                application->exit();
            }

    //Specify the name of the viewer here
    this->GetGUIName()=name;

#ifdef SOFA_QT4
    left_stack->addWidget ( viewer->getQWidget() );
    left_stack->setCurrentWidget ( viewer->getQWidget() );
    viewer->getQWidget()->setFocusPolicy ( Qt::StrongFocus );
#else
    int id_viewer = left_stack->addWidget ( viewer->getQWidget() );
    left_stack->raiseWidget ( id_viewer );
    viewer->getQWidget()->setFocusPolicy ( QWidget::StrongFocus );
    viewer->getQWidget()->setCursor ( QCursor ( 2 ) );
#endif

    viewer->getQWidget()->setSizePolicy ( QSizePolicy ( ( QSizePolicy::SizeType ) 7, ( QSizePolicy::SizeType ) 7, 100, 1, viewer->getQWidget()->sizePolicy().hasHeightForWidth() ) );
    viewer->getQWidget()->setMinimumSize ( QSize ( 0, 0 ) );
    viewer->getQWidget()->setMouseTracking ( TRUE );

//            viewer->setup();
    viewer->configureViewerTab(tabs);


    connect ( ResetViewButton, SIGNAL ( clicked() ), viewer->getQWidget(), SLOT ( resetView() ) );
    connect ( SaveViewButton, SIGNAL ( clicked() ), viewer->getQWidget(), SLOT ( saveView() ) );
    connect ( screenshotButton, SIGNAL ( clicked() ), this, SLOT ( screenshot() ) );
    connect ( sizeW, SIGNAL ( valueChanged ( int ) ), viewer->getQWidget(), SLOT ( setSizeW ( int ) ) );
    connect ( sizeH, SIGNAL ( valueChanged ( int ) ), viewer->getQWidget(), SLOT ( setSizeH ( int ) ) );
    connect ( viewer->getQWidget(), SIGNAL ( resizeW ( int ) ), sizeW, SLOT ( setValue ( int ) ) );
    connect ( viewer->getQWidget(), SIGNAL ( resizeH ( int ) ), sizeH, SLOT ( setValue ( int ) ) );
    connect ( viewer->getQWidget(), SIGNAL ( quit (  ) ), this, SLOT ( fileExit (  ) ) );

    QSplitter *splitter_ptr = dynamic_cast<QSplitter *> ( splitter2 );
    splitter_ptr->moveToLast ( left_stack );
    splitter_ptr->setOpaqueResize ( false );
#ifdef SOFA_QT4
    splitter_ptr->setStretchFactor( 0, 0);
    splitter_ptr->setStretchFactor( 1, 10);
    QList<int> list;
#else
    QValueList<int> list;
#endif
    list.push_back ( 216 );
    list.push_back ( 640 );
    splitter_ptr->setSizes ( list );

    viewer->getQWidget()->setFocus();
    viewer->getQWidget()->show();
    viewer->getQWidget()->update();
    setGUI();

    SofaMouseManager::getInstance()->setPickHandler(viewer->getPickHandler());
    viewer->getPickHandler()->addCallBack(new InformationOnPickCallBack(this));
}

void RealGUI::viewerOpenGL()
{
    setViewer ( "qt" );
    viewerOpenGLAction->setOn(true);
    viewerQGLViewerAction->setOn(false);
    viewerOGREAction->setOn(false);
}

void RealGUI::viewerQGLViewer()
{
    setViewer ( "qglviewer" );
    viewerOpenGLAction->setOn(false);
    viewerQGLViewerAction->setOn(true);
    viewerOGREAction->setOn(false);
}

void RealGUI::viewerOGRE()
{
    setViewer ( "ogre" );
    viewerOpenGLAction->setOn(false);
    viewerQGLViewerAction->setOn(false);
    viewerOGREAction->setOn(true);

}

bool RealGUI::setViewer ( const char* name )
{

    if ( !strcmp ( name,viewerName ) )
        return true; // nothing to do
    if ( !strcmp ( name,"qt" ) )
    {
#ifndef SOFA_GUI_QTVIEWER
        std::cerr << "OpenGL viewer not activated. Enable SOFA_GUI_QGLVIEWER in sofa-local.cfg to activate.\n";
        return false;
#endif
    }
    else if ( !strcmp ( name,"qglviewer" ) )
    {
#ifndef SOFA_GUI_QGLVIEWER
        std::cerr << "QGLViewer viewer not activated. Enable SOFA_GUI_QGLVIEWER in sofa-local.cfg to activate.\n";
        return false;
#endif
    }
    else if ( !strcmp ( name,"ogre" ) )
    {
#ifndef SOFA_GUI_QTOGREVIEWER
        std::cerr << "OGRE viewer not activated. Enable SOFA_GUI_QTOGREVIEWER in sofa-local.cfg to activate.\n";
        return false;
#endif
    }
    else
    {
        std::cerr << "Unknown viewer.\n";
        return false;
    }
    if ( QMessageBox::warning ( this, "Changing Viewer", "Changing viewer requires to reload the current scene.\nAre you sure you want to do that ?", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No ) != QMessageBox::Yes )
        return false;


    std::string filename(this->windowFilePath().ascii());

    if ( viewer->getScene() !=NULL )
    {
        simulation::getSimulation()->unload ( viewer->getScene() ); delete viewer->getScene() ;
#ifndef SOFA_CLASSIC_SCENE_GRAPH
        if(visualGraph->getListener() != NULL )
            simulation::getSimulation()->getVisualRoot()->removeListener(visualGraph->getListener());
#endif

        //  simulationGraph->Clear(dynamic_cast<Node*>( simulation::getSimulation()->getContext()) );
        //  visualGraph->Clear(dynamic_cast<Node*>( simulation::getSimulation()->getVisualRoot()) );
    }

    // 	fileOpen(filename);
    // 	GNode* groot = new GNode; // empty scene to do the transition
    // 	setScene ( groot,filename.c_str() ); // keep the current display flags

    viewer->removeViewerTab(tabs);

    left_stack->removeWidget ( viewer->getQWidget() );
    delete viewer;
    viewer = NULL;
    // Disable Viewer-specific classes
#ifdef SOFA_GUI_QTVIEWER
    if ( !strcmp ( viewerName,"qt" ) )
    {
        sofa::gui::qt::viewer::qt::QtViewer::DisableViewer();
    }
    else
#endif
#ifdef SOFA_GUI_QGLVIEWER
        if ( !strcmp ( viewerName,"qglviewer" ) )
        {
            sofa::gui::qt::viewer::qgl::QtGLViewer::DisableViewer();
        }
        else
#endif
#ifdef SOFA_GUI_QTOGREVIEWER
            if ( !strcmp ( viewerName,"ogre" ) )
            {
                sofa::gui::qt::viewer::qtogre::QtOgreViewer::DisableViewer();
            }
            else
#endif
            {}
    // Enable Viewer-specific classes
#ifdef SOFA_GUI_QTVIEWER
    if ( !strcmp ( name,"qt" ) )
    {
        sofa::gui::qt::viewer::qt::QtViewer::EnableViewer();
    }
    else
#endif
#ifdef SOFA_GUI_QGLVIEWER
        if ( !strcmp ( name,"qglviewer" ) )
        {
            sofa::gui::qt::viewer::qgl::QtGLViewer::EnableViewer();
        }
        else
#endif
#ifdef SOFA_GUI_QTOGREVIEWER
            if ( !strcmp ( name,"ogre" ) )
            {
                sofa::gui::qt::viewer::qtogre::QtOgreViewer::EnableViewer();
            }
#endif
    {}
    viewerName = name;
    addViewer();

    viewer->configureViewerTab(tabs);
    viewer->getPickHandler()->reset();

    if (filename.rfind(".simu") != std::string::npos)
        fileOpenSimu(filename.c_str() );

    fileOpen ( filename.c_str() ); // keep the current display flags
    return true;
}

void RealGUI::fileOpen ( std::string filename, bool temporaryFile )
{
    if (filename.size() > 5 && filename.substr(filename.size()-5) == ".simu")
    {
        return fileOpenSimu(filename);
    }

    startButton->setOn(false);
    descriptionScene->hide();
    htmlPage->clear();

    if ( sofa::helper::system::DataRepository.findFile (filename) )
        filename = sofa::helper::system::DataRepository.getFile ( filename );
    else
        return;
    startDumpVisitor();

    frameCounter = 0;
    sofa::simulation::xml::numDefault = 0;

    update();
    //Hide all the dialogs to modify the graph
    emit ( newScene() );

    if ( viewer->getScene() !=NULL )
    {
        viewer->getPickHandler()->reset();//activateRay(false);

        simulation::getSimulation()->unload ( viewer->getScene() ); delete viewer->getScene() ;
#ifndef SOFA_CLASSIC_SCENE_GRAPH
        if(visualGraph->getListener() != NULL )
            simulation::getSimulation()->getVisualRoot()->removeListener(visualGraph->getListener());
#endif

    }
    //Clear the list of modified dialog opened

    simulation::Node* root = simulation::getSimulation()->load ( filename.c_str() );
    simulation::getSimulation()->init ( root );
    if ( root == NULL )
    {
        qFatal ( "Failed to load %s",filename.c_str() );
        stopDumpVisitor();
        return;
    }
    this->setWindowFilePath(filename.c_str());
    setScene ( root, filename.c_str(), temporaryFile );
    //need to create again the output streams !!
    simulation::getSimulation()->gnuplotDirectory.setValue(gnuplot_directory);
    setExportGnuplot(exportGnuplotFilesCheckbox->isChecked());
    displayComputationTime(m_displayComputationTime);
    stopDumpVisitor();
}

#ifdef SOFA_PML
void RealGUI::pmlOpen ( const char* filename, bool /*resetView*/ )
{
    std::string scene = "PML/default.scn";
    if ( !sofa::helper::system::DataRepository.findFile ( scene ) )
    {
        std::cerr << "File " << scene << " not found " << std::endl;
        return;
    }
    if ( viewer->getScene() !=NULL )
    {
        simulation::getSimulation()->unload ( viewer->getScene() ); delete viewer->getScene() ;
#ifndef SOFA_CLASSIC_SCENE_GRAPH
        if(visualGraph->getListener() != NULL )
            simulation::getSimulation()->getVisualRoot()->removeListener(visualGraph->getListener());
#endif

    }
    GNode *simuNode = dynamic_cast< GNode *> (simulation::getSimulation()->load ( scene.c_str() ));
    getSimulation()->init(simuNode);
    if ( simuNode )
    {
        if ( !pmlreader ) pmlreader = new PMLReader;
        pmlreader->BuildStructure ( filename, simuNode );
        setScene ( simuNode, filename );
        this->setWindowFilePath(filename.c_str());
    }
}

void RealGUI::lmlOpen ( const char* filename )
{
    if ( pmlreader )
    {
        Node* root;
        if ( lmlreader != NULL ) delete lmlreader;
        lmlreader = new LMLReader; std::cout <<"New lml reader\n";
        lmlreader->BuildStructure ( filename, pmlreader );

        root = viewer->getScene();
        simulation::getSimulation()->init ( root );

    }
    else
        std::cerr<<"You must load the pml file before the lml file"<<endl;
}
#endif



void RealGUI::setScene ( Node* root, const char* filename, bool temporaryFile )
{
    if (filename)
    {
        if (!temporaryFile) updateRecentlyOpened(filename);
        setTitle ( filename );
        saveReloadFile=temporaryFile;
        std::string extension=sofa::helper::system::SetDirectory::GetExtension(filename);
        std::string htmlFile=filename; htmlFile.resize(htmlFile.size()-extension.size()-1);
        htmlFile+=".html";
        if (sofa::helper::system::DataRepository.findFile (htmlFile,"",NULL))
        {
#ifdef WIN32
            htmlFile = "file:///"+htmlFile;
#endif
            descriptionScene->show();
#ifdef SOFA_QT4
            htmlPage->setSource(QUrl(QString(htmlFile.c_str())));
#else
            htmlPage->mimeSourceFactory()->setFilePath(QString(htmlFile.c_str()));
            htmlPage->setSource(QString(htmlFile.c_str()));
#endif

        }

    }

    if (tabInstrument!= NULL)
    {
        tabs->removePage(tabInstrument);
        delete tabInstrument;
        tabInstrument = NULL;
    }
    viewer->setScene ( root, filename );
    this->setWindowFilePath(filename);
    viewer->resetView();
    eventNewTime();

    if (root)
    {
        // set state of display flags
        displayFlag->setFlag(Node::VISUALMODELS,root->getContext()->getShowVisualModels());
        displayFlag->setFlag(Node::BEHAVIORMODELS,root->getContext()->getShowBehaviorModels());
        displayFlag->setFlag(Node::COLLISIONMODELS,root->getContext()->getShowCollisionModels());
        displayFlag->setFlag(Node::BOUNDINGCOLLISIONMODELS,root->getContext()->getShowBoundingCollisionModels());
        displayFlag->setFlag(Node::MAPPINGS,root->getContext()->getShowMappings());
        displayFlag->setFlag(Node::MECHANICALMAPPINGS,root->getContext()->getShowMechanicalMappings());
        displayFlag->setFlag(Node::FORCEFIELDS,root->getContext()->getShowForceFields());
        displayFlag->setFlag(Node::INTERACTIONFORCEFIELDS,root->getContext()->getShowInteractionForceFields());
        displayFlag->setFlag(Node::WIREFRAME,root->getContext()->getShowWireFrame());
        displayFlag->setFlag(Node::NORMALS,root->getContext()->getShowNormals());

        //simulation::getSimulation()->updateVisualContext ( root );
        startButton->setOn ( root->getContext()->getAnimate() );
        dtEdit->setText ( QString::number ( root->getDt() ) );

        simulationGraph->Clear(root);
#ifndef SOFA_CLASSIC_SCENE_GRAPH
        visualGraph->Clear(dynamic_cast<Node*>(simulation::getSimulation()->getVisualRoot()) );
#endif
        statWidget->CreateStats(dynamic_cast<Node*>(simulation::getSimulation()->getContext()) );

#ifndef SOFA_GUI_QT_NO_RECORDER
        if (recorder)
            recorder->Clear();
#endif
    }

#ifdef SOFA_HAVE_CHAI3D
    // Tell nodes that openGl is initialized
    // especialy the GL_MODELVIEW_MATRIX
    sofa::core::objectmodel::GLInitializedEvent ev;
    sofa::simulation::PropagateEventVisitor act(&ev);
    root->execute(act);
#endif // SOFA_HAVE_CHAI3D

#ifdef SOFA_GUI_QTOGREVIEWER
    if (std::string(sofa::gui::SofaGUI::GetGUIName()) == "ogre")
        resetScene();
#endif

}

void RealGUI::Clear()
{
    simulation::Simulation *s = simulation::getSimulation();

    //In case instruments are present in the scene, we create a new tab, and display the listr
    if (s->instruments.size() != 0)
    {
        tabInstrument = new QWidget();
        tabs->addTab(tabInstrument, QString("Instrument"));

        QVBoxLayout *layout = new QVBoxLayout( tabInstrument, 0, 1, "tabInstrument");

        QButtonGroup *list_instrument = new QButtonGroup(tabInstrument);
        list_instrument->setExclusive(true);

#ifdef SOFA_QT4
        connect ( list_instrument, SIGNAL ( buttonClicked(int) ), this, SLOT ( changeInstrument(int) ) );
#else
        connect ( list_instrument, SIGNAL ( clicked(int) ), this, SLOT ( changeInstrument(int) ) );
#endif

        QRadioButton *button = new QRadioButton(tabInstrument); button->setText("None");
#ifdef SOFA_QT4
        list_instrument->addButton(button, 0);
#else
        list_instrument->insert(button);
#endif
        layout->addWidget(button);

        for (unsigned int i=0; i<s->instruments.size(); i++)
        {
            QRadioButton *button = new QRadioButton(tabInstrument);  button->setText(QString( s->instruments[i]->getName().c_str() ) );
#ifdef SOFA_QT4
            list_instrument->addButton(button, i+1);
#else
            list_instrument->insert(button);
#endif
            layout->addWidget(button);
            if (i==0)
            {
                button->setChecked(true); changeInstrument(1);
            }
            else
                s->instruments[i]->setActive(false);

        }
#ifdef SOFA_QT4
        layout->addStretch(1);
#endif
#ifndef SOFA_QT4
        layout->addWidget(list_instrument);
#endif
    }

#ifndef SOFA_GUI_QT_NO_RECORDER
    if (recorder)
        recorder->Clear();
#endif

    simulationGraph->Clear(dynamic_cast<Node*>(simulation::getSimulation()->getContext()));
#ifndef SOFA_CLASSIC_SCENE_GRAPH
    visualGraph->Clear(dynamic_cast<Node*>(simulation::getSimulation()->getVisualRoot()));
#endif
    statWidget->CreateStats(dynamic_cast<Node*>(simulation::getSimulation()->getContext()));


}

void RealGUI::setDimension ( int w, int h )
{
    QSize winSize = size();
    QSize viewSize = viewer->getQWidget()->size();
    //viewer->getQWidget()->setMinimumSize ( QSize ( w, h ) );
    //viewer->getQWidget()->setMaximumSize ( QSize ( w, h ) );
    //viewer->getQWidget()->resize(w,h);
#ifdef SOFA_QT4
    QList<int> list;
#else
    QValueList<int> list;
#endif
    list.push_back ( 216 );
    list.push_back ( w );
    QSplitter *splitter_ptr = dynamic_cast<QSplitter *> ( splitter2 );
    splitter_ptr->setSizes ( list );
#ifdef SOFA_QT4
    layout()->update();
#endif
    resize(winSize.width() - viewSize.width() + w, winSize.height() - viewSize.height() + h);
    //std::cout << "Setting windows dimension to " << size().width() << " x " << size().height() << std::endl;
}
void RealGUI::setFullScreen ()
{
    showFullScreen();
}
void RealGUI::changeInstrument(int id)
{
    std::cout << "Activation instrument "<<id<<std::endl;
    simulation::Simulation *s = simulation::getSimulation();
    if (s->instrumentInUse.getValue() >= 0 && s->instrumentInUse.getValue() < (int)s->instruments.size())
        s->instruments[s->instrumentInUse.getValue()]->setActive(false);

    simulation::getSimulation()->instrumentInUse.setValue(id-1);
    if (s->instrumentInUse.getValue() >= 0 && s->instrumentInUse.getValue() < (int)s->instruments.size())
        s->instruments[s->instrumentInUse.getValue()]->setActive(true);
    viewer->getQWidget()->update();
}


void RealGUI::screenshot()
{

    QString filename;

#ifdef SOFA_HAVE_PNG
    const char* imageString = "Images (*.png)";
#else
    const char* imageString = "Images (*.bmp)";
#endif

    filename = getSaveFileName ( this,
            viewer->screenshotName().c_str(),
            imageString,
            "save file dialog"
            "Choose a filename to save under" );
    viewer->getQWidget()->repaint();
    if ( filename != "" )
    {
        std::ostringstream ofilename;
        const char* begin = filename;
        const char* end = strrchr ( begin,'_' );
        if ( !end ) end = begin + filename.length();
        ofilename << std::string ( begin, end );
        ofilename << "_";
        viewer->setPrefix ( ofilename.str() );
#ifdef SOFA_QT4
        viewer->screenshot ( filename.toStdString() );
#else
        viewer->screenshot ( filename );
#endif

    }
}

void RealGUI::fileOpenSimu ( std::string s )
{
    std::ifstream in(s.c_str());

    if (!in.fail())
    {
        std::string filename;
        std::string initT, endT, dT, writeName;
        in
                >> filename
                        >> initT >> initT
                        >> endT  >> endT >> endT
                        >> dT >> dT
                        >> writeName >> writeName;
        in.close();



        if ( sofa::helper::system::DataRepository.findFile (filename) )
        {
            filename = sofa::helper::system::DataRepository.getFile ( filename );
            simulation_name = s;
            std::string::size_type pointSimu = simulation_name.rfind(".simu");
            simulation_name.resize(pointSimu);
            fileOpen(filename.c_str());
            this->setWindowFilePath(QString(filename.c_str()));
            dtEdit->setText(QString(dT.c_str()));
#ifndef SOFA_GUI_QT_NO_RECORDER
            if (recorder)
                recorder->SetSimulation(initT,endT,writeName);
#endif

        }
    }
}
void RealGUI::fileNew()
{
    std::string newScene("config/newScene.scn");
    if (sofa::helper::system::DataRepository.findFile (newScene))
        fileOpen(sofa::helper::system::DataRepository.getFile ( newScene ).c_str());
}
void RealGUI::fileOpen()
{
    std::string filename(this->windowFilePath().ascii());

    QString s = getOpenFileName ( this, filename.empty() ?NULL:filename.c_str(),
#ifdef SOFA_PML
            "Scenes (*.scn *.xml);;Simulation (*.simu);;Php Scenes (*.pscn);;Pml Lml (*.pml *.lml);;All (*)",
#else
            "Scenes (*.scn *.xml);;Simulation (*.simu);;Php Scenes (*.pscn);;All (*)",
#endif
            "open file dialog",  "Choose a file to open" );

    if ( s.length() >0 )
    {
#ifdef SOFA_PML
        if ( s.endsWith ( ".pml" ) )
            pmlOpen ( s );
        else if ( s.endsWith ( ".lml" ) )
            lmlOpen ( s );
        else
#endif
            if (s.endsWith( ".simu") )
                fileOpenSimu(s.ascii());
            else
            {
                fileOpen (s.ascii());
            }
    }
}

void RealGUI::fileReload()
{

    std::string filename(this->windowFilePath().ascii());
    QString s = filename.c_str();

    if ( filename.empty() ) { std::cerr << "Reload failed: no file loaded.\n"; return;}

#ifdef SOFA_PML
    if ( s.length() >0 )
    {
        if ( s.endsWith ( ".pml" ) )
            pmlOpen ( s );
        else if ( s.endsWith ( ".lml" ) )
            lmlOpen ( s );
        else if (s.endsWith( ".simu") )
            fileOpenSimu(filename);
        else
            fileOpen ( filename, saveReloadFile);
    }
#else
    if (s.endsWith( ".simu") )
        fileOpenSimu(s.ascii());
    else
        fileOpen ( s.ascii(),saveReloadFile );
#endif

}

void RealGUI::fileSave()
{
    std::string filename(this->windowFilePath().ascii());
    std::string message="You are about to overwrite your current scene: "  + filename + "\nAre you sure you want to do that ?";

    if ( QMessageBox::warning ( this, "Saving the Scene",message.c_str(), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No ) != QMessageBox::Yes )
        return;

    Node *node = viewer->getScene();
    fileSaveAs ( node,filename.c_str() );
}


void RealGUI::fileSaveAs(Node *node)
{
    if (node == NULL) node = viewer->getScene();
    QString s;
    std::string filename(this->windowFilePath().ascii());
#ifdef SOFA_PML
    s = getSaveFileName ( this, filename.empty() ?NULL:filename.c_str(), "Scenes (*.scn *.xml *.pml)", "save file dialog",  "Choose where the scene will be saved" );
    if ( s.length() >0 )
    {
        if ( pmlreader && s.endsWith ( ".pml" ) )
            pmlreader->saveAsPML ( s );
        else
            fileSaveAs ( node,s );
    }
#else
    s = getSaveFileName ( this, filename.empty() ?NULL:filename.c_str(), "Scenes (*.scn *.xml)", "save file dialog", "Choose where the scene will be saved" );
    if ( s.length() >0 )
        fileSaveAs ( node,s );
#endif

}

void RealGUI::fileSaveAs ( Node *node, const char* filename )
{
    simulation::getSimulation()->exportXML ( node, filename );
}

void RealGUI::fileExit()
{
    //Hide all opened ModifyObject windows
    emit ( newScene() );
    startButton->setOn ( false);
    this->close();
}

void RealGUI::saveXML()
{
    simulation::getSimulation()->exportXML ( viewer->getScene(), "scene.scn" );
}

void RealGUI::editRecordDirectory()
{
    std::string filename(this->windowFilePath().ascii());
    std::string record_directory;
    QString s = getExistingDirectory ( this, filename.empty() ?NULL:filename.c_str(), "open directory dialog",  "Choose a directory" );
    if (s.length() > 0)
    {
        record_directory = s.ascii();
        if (record_directory.at(record_directory.size()-1) != '/') record_directory+="/";
#ifndef SOFA_GUI_QT_NO_RECORDER
        if (recorder)
            recorder->SetRecordDirectory(record_directory);
#endif
    }

}

void RealGUI::showPluginManager()
{
    SofaPluginManager::getInstance()->show();
}

void RealGUI::showMouseManager()
{
    SofaMouseManager::getInstance()->show();
}

void RealGUI::editGnuplotDirectory()
{
    std::string filename(this->windowFilePath().ascii());
    QString s = getExistingDirectory ( this, filename.empty() ?NULL:filename.c_str(), "open directory dialog",  "Choose a directory" );
    if (s.length() > 0)
    {
        gnuplot_directory = s.ascii();
        if (gnuplot_directory.at(gnuplot_directory.size()-1) != '/') gnuplot_directory+="/";

        simulation::getSimulation()->gnuplotDirectory.setValue(gnuplot_directory);
        setExportGnuplot(exportGnuplotFilesCheckbox->isChecked());
    }
}


void RealGUI::setTitle ( std::string windowTitle )
{
    std::string str = "Sofa";
    if ( !windowTitle.empty() )
    {
        str += " - ";
        str += windowTitle;
    }
#ifdef WIN32
    setWindowTitle ( str.c_str() );
#else
    setCaption ( str.c_str() );
#endif
}


void RealGUI::playpauseGUI ( bool value )
{
    startButton->setOn ( value );
    if ( getScene() )  getScene()->getContext()->setAnimate ( value );
    if(value)
    {
        timerStep->start(0);
    }
    else
    {
        timerStep->stop();
    }
}


void RealGUI::setGUI ( void )
{
    textEdit1->setText ( viewer->helpString() );
    connect ( this, SIGNAL( newStep()), viewer->getQWidget(), SLOT( update()));
}
//###################################################################################################################

void RealGUI::startDumpVisitor()
{
#ifdef SOFA_DUMP_VISITOR_INFO
    Node* root = viewer->getScene();
    if (root && this->exportVisitorCheckbox->isOn())
    {
        m_dumpVisitorStream.str("");
        Visitor::startDumpVisitor(&m_dumpVisitorStream, root->getTime());
    }
#endif
}
void RealGUI::stopDumpVisitor()
{
#ifdef SOFA_DUMP_VISITOR_INFO
    if (this->exportVisitorCheckbox->isOn())
    {
        Visitor::stopDumpVisitor();
        m_dumpVisitorStream.flush();
        //Creation of the graph
        std::string xmlDoc=m_dumpVisitorStream.str();
        handleTraceVisitor->load(xmlDoc);
        m_dumpVisitorStream.str("");
    }
#endif
}
//*****************************************************************************************
//called at each step of the rendering

void RealGUI::step()
{
    Node* root = viewer->getScene();
    if ( root == NULL ) return;

    startDumpVisitor();

#ifdef SOFA_DEV

    if ( root->getContext()->getMultiThreadSimulation() )
    {
        static AutomateNode* n = NULL;

        if ( ExecBus::getInstance() != NULL )
        {
            n = ExecBus::getInstance()->getNext ( "displayThread", n );

            if ( n )
            {
                n->execute ( "displayThread" );
            }
        }
    }
    else

#endif // SOFA_DEV

    {
        if ( viewer->ready() ) return;



        //root->setLogTime(true);
        //T=T+DT
        SReal dt=root->getDt();
        simulation::getSimulation()->animate ( root, dt );
#ifdef SOFA_CLASSIC_SCENE_GRAPH
        simulation::getSimulation()->updateVisual( root , dt );
#else
        simulation::getSimulation()->updateVisual( simulation::getSimulation()->getVisualRoot() , dt );
#endif


        if ( m_dumpState )
            simulation::getSimulation()->dumpState ( root, *m_dumpStateStream );
        if ( m_exportGnuplot )
            simulation::getSimulation()->exportGnuplot ( root, root->getTime() );

        viewer->wait();

        eventNewStep();
        eventNewTime();
    }


    if ( _animationOBJ )
    {
#ifdef CAPTURE_PERIOD
        static int counter = 0;
        if ( ( counter++ % CAPTURE_PERIOD ) ==0 )
#endif
        {
            exportOBJ ( false );
            ++_animationOBJcounter;
        }
    }

    stopDumpVisitor();
    emit newStep();
}

//*****************************************************************************************
// Update sofa Simulation with the time step

void RealGUI::eventNewStep()
{
    static ctime_t beginTime[10];
    static const ctime_t timeTicks = CTime::getRefTicksPerSec();
    Node* root = getScene();
    if ( frameCounter==0 )
    {
        ctime_t t = CTime::getRefTime();
        for ( int i=0; i<10; i++ )
            beginTime[i] = t;
    }
    ++frameCounter;
    if ( ( frameCounter%10 ) == 0 )
    {
        ctime_t curtime = CTime::getRefTime();
        int i = ( ( frameCounter/10 ) %10 );
        double fps = ( ( double ) timeTicks / ( curtime - beginTime[i] ) ) * ( frameCounter<100?frameCounter:100 );

#ifndef SOFA_GUI_QT_NO_RECORDER
        if (recorder)
            recorder->setFPS(fps);
#else
        if (fpsLabel)
        {
            char buf[100];
            sprintf ( buf, "%.1f FPS", fps );
            fpsLabel->setText ( buf );
        }
#endif
        beginTime[i] = curtime;
    }

    if ( m_displayComputationTime && ( frameCounter%100 ) == 0 && root!=NULL )
    {

        std::cout << "========== ITERATION " << frameCounter << " ==========\n";
        const sofa::simulation::Node::NodeTimer& total = root->getTotalTime();
        const std::map<std::string, sofa::simulation::Node::NodeTimer>& times = root->getVisitorTime();
        const std::map<std::string, std::map<sofa::core::objectmodel::BaseObject*, sofa::simulation::Node::ObjectTimer> >& objtimes = root->getObjectTime();
        const double fact = 1000000.0 / ( 100*root->getTimeFreq() );
        for ( std::map<std::string, sofa::simulation::Node::NodeTimer>::const_iterator it = times.begin(); it != times.end(); ++it )
        {
            std::cout << "TIME "<<it->first<<": " << ( ( int ) ( fact*it->second.tTree+0.5 ) ) *0.001 << " ms (" << ( 1000*it->second.tTree/total.tTree ) *0.1 << " %).\n";
            std::map<std::string, std::map<sofa::core::objectmodel::BaseObject*, sofa::simulation::Node::ObjectTimer> >::const_iterator it1 = objtimes.find ( it->first );
            if ( it1 != objtimes.end() )
            {
                for ( std::map<sofa::core::objectmodel::BaseObject*, sofa::simulation::Node::ObjectTimer>::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2 )
                {
                    std::cout << "  "<< sofa::helper::gettypename ( typeid ( * ( it2->first ) ) ) <<" "<< it2->first->getName() <<": "
                            << ( ( int ) ( fact*it2->second.tObject+0.5 ) ) *0.001 << " ms (" << ( 1000*it2->second.tObject/it->second.tTree ) *0.1 << " %).\n";
                }
            }
        }
        for ( std::map<std::string, std::map<sofa::core::objectmodel::BaseObject*, sofa::simulation::Node::ObjectTimer> >::const_iterator it = objtimes.begin(); it != objtimes.end(); ++it )
        {
            if ( times.count ( it->first ) >0 ) continue;
            ctime_t ttotal = 0;
            for ( std::map<sofa::core::objectmodel::BaseObject*, sofa::simulation::Node::ObjectTimer>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
                ttotal += it2->second.tObject;
            std::cout << "TIME "<<it->first<<": " << ( ( int ) ( fact*ttotal+0.5 ) ) *0.001 << " ms (" << ( 1000*ttotal/total.tTree ) *0.1 << " %).\n";
            if ( ttotal > 0 )
                for ( std::map<sofa::core::objectmodel::BaseObject*, sofa::simulation::Node::ObjectTimer>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
                {
                    std::cout << "  "<< sofa::helper::gettypename ( typeid ( * ( it2->first ) ) ) <<" "<< it2->first->getName() <<": "
                            << ( ( int ) ( fact*it2->second.tObject+0.5 ) ) *0.001 << " ms (" << ( 1000*it2->second.tObject/ttotal ) *0.1 << " %).\n";
                }
        }
        std::cout << "TOTAL TIME: " << ( ( int ) ( fact*total.tTree+0.5 ) ) *0.001 << " ms (" << ( ( int ) ( 100/ ( fact*total.tTree*0.000001 ) +0.5 ) ) *0.01 << " FPS).\n";
        root->resetTime();

    }
}

void RealGUI::currentTabChanged ( QWidget* widget )
{
    if ( widget == currentTab ) return;
    if ( currentTab == NULL )
    {
        currentTab = widget;
    }
    if ( widget == TabGraph )
    {
        simulationGraph->Unfreeze( );
    }
    else if ( currentTab == TabGraph )
    {
        simulationGraph->Freeze();
    }
#ifndef SOFA_CLASSIC_SCENE_GRAPH
    else if ( widget == TabVisualGraph )
    {
        visualGraph->Unfreeze( );
    }
    else if ( currentTab == TabVisualGraph )
    {
        visualGraph->Freeze( );

    }
#endif
    else if (widget == TabStats)
        statWidget->CreateStats(dynamic_cast<Node*>(simulation::getSimulation()->getContext()));

    currentTab = widget;
}



void RealGUI::eventNewTime()
{
#ifndef SOFA_GUI_QT_NO_RECORDER
    if (recorder)
        recorder->UpdateTime();
#else
    Node* root = getScene();
    if (root && timeLabel)
    {
        double time = root->getTime();
        char buf[100];
        sprintf ( buf, "Time: %.3g s", time );
        timeLabel->setText ( buf );
    }
#endif
}




//*****************************************************************************************
// Set the time between each iteration of the Sofa Simulation

void RealGUI::setDt ( double value )
{
    Node* root = getScene();
    if ( value > 0.0 )
    {

        if ( root )
            root->getContext()->setDt ( value );
    }
}

void RealGUI::setDt ( const QString& value )
{
    setDt ( value.toDouble() );
}


//*****************************************************************************************
// Reset the simulation to t=0
void RealGUI::resetScene()
{
    Node* root = getScene();
    startDumpVisitor();
    emit ( newScene() );
    //Reset the scene
    if ( root )
    {
        root->setTime(0.);
        eventNewTime();
        simulation::getSimulation()->reset ( root );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
        simulation::getSimulation()->reset ( simulation::getSimulation()->getVisualRoot() );
#endif
        UpdateContextVisitor().execute(root);
#ifndef SOFA_CLASSIC_SCENE_GRAPH
        UpdateContextVisitor().execute(simulation::getSimulation()->getVisualRoot());
#endif
        emit newStep();
    }

    viewer->getPickHandler()->reset();
    stopDumpVisitor();

}

//*****************************************************************************************
//
void RealGUI::displayComputationTime ( bool value )
{
    Node* root = getScene();
    m_displayComputationTime = value;
    if ( root )
    {
        root->setLogTime ( m_displayComputationTime );
    }
}



//*****************************************************************************************
//
void RealGUI::setExportGnuplot ( bool exp )
{
    Node* root = getScene();
    m_exportGnuplot = exp;
    if ( m_exportGnuplot && root )
    {
        simulation::getSimulation()->initGnuplot ( root );
        simulation::getSimulation()->exportGnuplot ( root, root->getTime() );
    }
}

//*****************************************************************************************
//
#ifdef SOFA_DUMP_VISITOR_INFO
void RealGUI::setExportVisitor ( bool exp )
#else
void RealGUI::setExportVisitor ( bool /*exp*/ )
#endif
{
#ifdef SOFA_DUMP_VISITOR_INFO
    if (exp)
    {
        windowTraceVisitor->show();
        handleTraceVisitor->clear();
    }
    else
    {
        windowTraceVisitor->hide();
    }
#endif
}

//*****************************************************************************************
//
void RealGUI::dumpState ( bool value )
{
    m_dumpState = value;
    if ( m_dumpState )
    {
        m_dumpStateStream = new std::ofstream ( "dumpState.data" );
    }
    else if ( m_dumpStateStream!=NULL )
    {
        delete m_dumpStateStream;
        m_dumpStateStream = 0;
    }
}



//*****************************************************************************************
//
void RealGUI::exportOBJ ( bool exportMTL )
{
#ifdef SOFA_CLASSIC_SCENE_GRAPH
    Node* root = getScene();
#else
    Node* root = simulation::getSimulation()->getVisualRoot();
#endif
    if ( !root ) return;
    std::string sceneFileName(this->windowFilePath ().ascii());
    std::ostringstream ofilename;
    if ( !sceneFileName.empty() )
    {
        const char* begin = sceneFileName.c_str();
        const char* end = strrchr ( begin,'.' );
        if ( !end ) end = begin + sceneFileName.length();
        ofilename << std::string ( begin, end );
    }
    else
        ofilename << "scene";

    std::stringstream oss;
    oss.width ( 5 );
    oss.fill ( '0' );
    oss << _animationOBJcounter;

    ofilename << '_' << ( oss.str().c_str() );
    ofilename << ".obj";
    std::string filename = ofilename.str();
    std::cout << "Exporting OBJ Scene "<<filename<<std::endl;
    simulation::getSimulation()->exportOBJ ( root, filename.c_str(),exportMTL );
}


//*****************************************************************************************


void RealGUI::keyPressEvent ( QKeyEvent * e )
{
    // ignore if there are modifiers (i.e. CTRL of SHIFT)
#ifdef SOFA_QT4
    if (e->modifiers()) return;
#else
    if (e->state() & (Qt::KeyButtonMask)) return;
#endif
    switch ( e->key() )
    {

    case Qt::Key_O:
        // --- export to OBJ
    {
        exportOBJ();
        break;
    }
    case Qt::Key_P:
        // --- export to a succession of OBJ to make a video
    {
        _animationOBJ = !_animationOBJ;
        _animationOBJcounter = 0;
        break;
    }
    case Qt::Key_Escape:
    {
        emit(quit());
        break;
    }
    default:
    {
        e->ignore();
        break;
    }
    }
}


void RealGUI::dropEvent(QDropEvent* event)
{
    QString text;
    Q3TextDrag::decode(event, text);
    std::string filename(text.ascii());
#ifdef WIN32
    filename = filename.substr(8); //removing file:///
#else
    filename = filename.substr(7); //removing file://
#endif

    if (filename[filename.size()-1] == '\n')
    {
        filename.resize(filename.size()-1);
        filename[filename.size()-1]='\0';
    }

    if (filename.rfind(".simu") != std::string::npos) fileOpenSimu(filename);
    else  	                                    fileOpen(filename);
}

#ifdef SOFA_QT4
void RealGUI::changeHtmlPage( const QUrl& u)
{
    std::string path=u.path().ascii();
#ifdef WIN32
    path = path.substr(1);
#endif
#else
void RealGUI::changeHtmlPage( const QString& u)
{
    std::string path=u.ascii();
#endif
    path  = sofa::helper::system::DataRepository.getFile(path);
    std::string extension=sofa::helper::system::SetDirectory::GetExtension(path.c_str());
    if (extension == "xml" || extension == "scn") fileOpen(path);
}

void RealGUI::updateViewerParameters()
{
    gui->viewer->getQWidget()->update();
}

void RealGUI::updateBackgroundColour()
{
    viewer->setBackgroundColour(atof(background[0]->text().ascii()),atof(background[1]->text().ascii()),atof(background[2]->text().ascii()));
    updateViewerParameters();
}

void RealGUI::updateBackgroundImage()
{
    viewer->setBackgroundImage( backgroundImage->text().ascii() );
    updateViewerParameters();
}

void RealGUI::showhideElements(int FILTER, bool value)
{
    Node* root = getScene();
    if ( root )
    {
        switch(FILTER)
        {
        case Node::ALLFLAGS:
            root->getContext()->setShowVisualModels ( value );
            root->getContext()->setShowBehaviorModels ( value );
            root->getContext()->setShowCollisionModels ( value );
            root->getContext()->setShowBoundingCollisionModels ( value );
            root->getContext()->setShowMappings ( value );
            root->getContext()->setShowMechanicalMappings ( value );
            root->getContext()->setShowForceFields ( value );
            root->getContext()->setShowInteractionForceFields ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowVisualModels ( value );
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowBehaviorModels ( value );
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowCollisionModels ( value );
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowBoundingCollisionModels ( value );
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowMappings ( value );
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowMechanicalMappings ( value );
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowForceFields ( value );
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowInteractionForceFields ( value );
#endif
            break;
        case  Node::VISUALMODELS:
        {
            root->getContext()->setShowVisualModels ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->setShowVisualModels( value);
#endif
            break;
        }
        case  Node::BEHAVIORMODELS:
        {
            root->getContext()->setShowBehaviorModels ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowBehaviorModels ( value );
#endif
            break;
        }
        case  Node::COLLISIONMODELS:
        {
            root->getContext()->setShowCollisionModels ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowCollisionModels ( value );
#endif
            break;
        }
        case  Node::BOUNDINGCOLLISIONMODELS:
        {
            root->getContext()->setShowBoundingCollisionModels ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowBoundingCollisionModels ( value );
#endif
            break;
        }
        case  Node::MAPPINGS:
        {
            root->getContext()->setShowMappings ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowMappings ( value );
#endif
            break;
        }
        case  Node::MECHANICALMAPPINGS:
        {
            root->getContext()->setShowMechanicalMappings ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowMechanicalMappings ( value );
#endif
            break;
        }
        case  Node::FORCEFIELDS:
        {
            root->getContext()->setShowForceFields ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowForceFields ( value );
#endif
            break;
        }
        case  Node::INTERACTIONFORCEFIELDS:
        {
            root->getContext()->setShowInteractionForceFields ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowWireFrame ( value );
#endif
            break;
        }
        case  Node::WIREFRAME:
        {
            root->getContext()->setShowWireFrame ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowWireFrame ( value );
#endif
            break;
        }
        case  Node::NORMALS:
        {
            root->getContext()->setShowNormals ( value );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
            sofa::simulation::getSimulation()->getVisualRoot()->getContext()->setShowNormals ( value );
#endif
            break;
        }
        }
        sofa::simulation::getSimulation()->updateVisualContext ( root, (simulation::Node::VISUAL_FLAG) FILTER );
#ifndef SOFA_CLASSIC_SCENE_GRAPH
        sofa::simulation::getSimulation()->updateVisualContext ( sofa::simulation::getSimulation()->getVisualRoot(), (simulation::Node::VISUAL_FLAG) FILTER );
#endif
    }
    viewer->getQWidget()->update();
}

void RealGUI::Update()
{
    viewer->getQWidget()->update();
    statWidget->CreateStats(dynamic_cast<Node*>(simulation::getSimulation()->getContext()));
}



void RealGUI::NewRootNode(sofa::simulation::Node* root, const char* path)
{
    if(path != NULL)
    {
        viewer->setScene ( root, viewer->getSceneFileName().c_str() );
    }
    else
    {
        viewer->setScene(root , path);
    }
    viewer->resetView();
    viewer->getQWidget()->update();
    statWidget->CreateStats(root);
}

void RealGUI::LockAnimation(bool value)
{
    if(value)
    {
        animationState = startButton->isOn();
        playpauseGUI(false);
    }
    else
    {
        playpauseGUI(animationState);
    }
}
} // namespace qt

} // namespace gui

} // namespace sofa
