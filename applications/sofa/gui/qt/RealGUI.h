/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 INRIA, USTL, UJF, CNRS, MGH                    *
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
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_GUI_VIEWER_REALGUI_H
#define SOFA_GUI_VIEWER_REALGUI_H

#ifdef SOFA_PML
#  include <sofa/filemanager/sofapml/PMLReader.h>
#  include <sofa/filemanager/sofapml/LMLReader.h>
#endif

#include "GUI.h"
#include "SofaGUIQt.h"
#include <sofa/gui/BaseGUI.h>
#include <time.h>
#include "../BaseViewer.h"
#include "viewer/SofaViewer.h"
#include "../ViewerFactory.h"
#include "../BaseGUIUtil.h"
#include "QSofaListView.h"
#include "GraphListenerQListView.h"
#include "FileManagement.h"
#include "AddObject.h"
#include "ModifyObject.h"
#include "DisplayFlagsDataWidget.h"
#include "QMenuFilesRecentlyOpened.h"
#include "SofaPluginManager.h"
#include "SofaMouseManager.h"
#include "SofaVideoRecorderManager.h"
#include "PickHandlerCallBacks.h"
#include <sofa/simulation/common/xml/XML.h>
#include <sofa/helper/system/SetDirectory.h>

#ifdef SOFA_DUMP_VISITOR_INFO
#include "WindowVisitor.h"
#include "GraphVisitor.h"
#endif

#ifdef SOFA_QT4
#include <QApplication>
#include <QDesktopWidget>
#include <Q3ListViewItem>
#include <QStackedWidget>
#include <QSlider>
#include <QTimer>
#include <Q3TextDrag>
#include <Q3PopupMenu>
#include <QLibrary>
#include <QTextBrowser>
#include <QUrl>
#include <QStatusBar>
typedef Q3ListViewItem QListViewItem;
typedef QStackedWidget QWidgetStack;
typedef Q3PopupMenu QPopupMenu;
#else
typedef QTextDrag Q3TextDrag;
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qdragobject.h>
#include <qwidgetstack.h>
#include <qlistview.h>
#include <qslider.h>
#include <qpopupmenu.h>
#include <qlibrary.h>
#include <qtextbrowser.h>
#include <qurl.h>
#include <qstatusbar.h>
#endif

#ifdef SOFA_PML
#include <sofa/simulation/tree/GNode.h>
#endif

namespace sofa
{

namespace gui
{

class CallBackPicker;


namespace qt
{

//enum TYPE{ NORMAL, PML, LML};
enum SCRIPT_TYPE { PHP, PERL };

#ifdef SOFA_PML
using namespace sofa::filemanager::pml;
#endif

class QSofaListView;
#ifndef SOFA_GUI_QT_NO_RECORDER
class QSofaRecorder;
#endif

class QSofaStatWidget;

class SOFA_SOFAGUIQT_API RealGUI : public ::GUI, public sofa::gui::BaseGUIUtil
{
    Q_OBJECT

    /// @name BaseGUI Interface
    /// @{


public:


    static int InitGUI(const char* name, const std::vector<std::string>& options);
    static BaseGUI* CreateGUI(const char* name, const std::vector<std::string>& options, sofa::simulation::Node::SPtr groot = NULL, const char* filename = NULL);

protected:

    static void CreateApplication(int _argc=0, char** _argv=0l);
    static void InitApplication( RealGUI* _gui);

public:

    int mainLoop();

    int closeGUI();

    /// @}


#ifndef SOFA_QT4
    void setWindowFilePath(const QString &filePath) { filePath_=filePath;};
    QString windowFilePath() const { QString filePath = filePath_; return filePath; }
#endif

#ifdef SOFA_GUI_INTERACTION
    QPushButton *interactionButton;
#endif

    QSofaListView* simulationGraph;


    RealGUI( const char* viewername, const std::vector<std::string>& options = std::vector<std::string>() );
    ~RealGUI();

    virtual void createViewers(const char* viewerName);
    virtual void initViewer();

    /// Our viewer is a QObject SofaViewer
    virtual bool isEmbeddedViewer()
    {
        return dynamic_cast<sofa::gui::qt::viewer::SofaViewer*>(mViewer) ? true : false;
    }

    /// We are sur we use a QObject SofaViewer and return its QWidget
    QWidget* getViewerWidget()
    {
        return dynamic_cast<sofa::gui::qt::viewer::SofaViewer*>(mViewer)->getQWidget();
    }

    static void setPixmap(std::string pixmap_filename, QPushButton* b);

    virtual void fileOpen(std::string filename, bool temporaryFile=false);
    virtual void fileOpenSimu(std::string filename);
    virtual void setScene(Node::SPtr groot, const char* filename=NULL, bool temporaryFile=false);

    //Configuration methods
    virtual void setViewerResolution(int w, int h);
    virtual void setFullScreen(bool enable = true);
    virtual void setBackgroundColor(const defaulttype::Vector3& c);
    virtual void setBackgroundImage(const std::string& i);
    virtual void setDumpState(bool);
    virtual void setLogTime(bool);
    virtual void setExportState(bool);
#ifdef SOFA_DUMP_VISITOR_INFO
    virtual void setTraceVisitors(bool);
#endif
    virtual void setRecordPath(const std::string & path);
    virtual void setGnuplotPath(const std::string & path);

    virtual void setViewerConfiguration(sofa::component::configurationsetting::ViewerSetting* viewerConf);
    virtual void setMouseButtonConfiguration(sofa::component::configurationsetting::MouseButtonSetting *button);

    virtual void setTitle( std::string windowTitle );

    //public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs() {fileSaveAs((Node *)NULL);}

    virtual void fileSaveAs(Node* node,const char* filename);

    virtual void fileReload();
    virtual void fileExit();
    virtual void saveXML();
    virtual void editRecordDirectory();
    virtual void editGnuplotDirectory();
    virtual void showPluginManager();
    virtual void showMouseManager();
    virtual void showVideoRecorderManager();
    void dragEnterEvent( QDragEnterEvent* event) {event->accept();}
    void dropEvent(QDropEvent* event);


public slots:
    virtual void NewRootNode(sofa::simulation::Node* root, const char* path);
    virtual void ActivateNode(sofa::simulation::Node* , bool );
    virtual void Update();
    virtual void fileSaveAs(sofa::simulation::Node *node);
    virtual void LockAnimation(bool);
    virtual void fileRecentlyOpened(int id);
    virtual void playpauseGUI(bool value);
//#ifdef SOFA_GUI_INTERACTION
    virtual void interactionGUI(bool value);
//#endif
    virtual void step();
    virtual void setDt(double);
    virtual void setDt(const QString&);
    virtual void resetScene();
    virtual void screenshot();
    virtual void showhideElements();
    virtual void updateViewerParameters();
    virtual void updateBackgroundColour();
    virtual void updateBackgroundImage();

// Propagate signal to call viewer method in case of it is not a widget
// Maybe, have to create a BaseGUIViewerMediator class to provide this
    virtual void resetView()            {mViewer->resetView();       }
    virtual void saveView()             {mViewer->saveView();        }
    virtual void setSizeW ( int _valW ) {mViewer->setSizeW(_valW);   }
    virtual void setSizeH ( int _valH ) {mViewer->setSizeH(_valH);   }

#ifdef SOFA_QT4
    virtual void changeHtmlPage( const QUrl&);
#else
    virtual void changeHtmlPage( const QString&);
#endif
    virtual void Clear();
    //Used in Context Menu
    //refresh the visualization window
    virtual void redraw();
    virtual void exportOBJ(sofa::simulation::Node* node, bool exportMTL=true);
    virtual void dumpState(bool);
    virtual void displayComputationTime(bool);
    virtual void setExportGnuplot(bool);
    virtual void setExportVisitor(bool);
    virtual void currentTabChanged(QWidget*);
protected slots:
    /// Allow to dynamicly change viewer. Called when click on another viewer in GUI Qt viewer list.
    /// Note: When the app start, we registred GUI and static create/init it with a guiName;
    /// during the app, if you change viewer you keep the same GUI.
    virtual void changeViewer();
    virtual void updateViewerList();

signals:
    void reload();
    void newScene();
    void newStep();
    void quit();

protected:
    void eventNewStep();
    void eventNewTime();
    void init();
    void keyPressEvent ( QKeyEvent * e );
#ifdef SOFA_GUI_INTERACTION
    void mouseMoveEvent( QMouseEvent * e);
    void wheelEvent( QWheelEvent * event );
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void keyReleaseEvent(QKeyEvent * e);
    bool eventFilter(QObject *obj, QEvent *event);
#endif

    void loadSimulation(bool one_step=false);

    virtual int exitApplication(unsigned int _retcode = 0) {return _retcode;}
    /// create a viewer by default, otherwise you have to manage your own viewer
    bool mCreateViewersOpt;

    void startDumpVisitor();
    void stopDumpVisitor();

    bool m_dumpState;
    std::ofstream* m_dumpStateStream;
    std::ostringstream m_dumpVisitorStream;
    bool m_exportGnuplot;
    bool _animationOBJ; int _animationOBJcounter;// save a succession of .obj indexed by _animationOBJcounter
    bool m_displayComputationTime;
    bool m_fullScreen;

    std::map< helper::SofaViewerFactory::Key, QAction* > viewerMap;
    InformationOnPickCallBack informationOnPickCallBack;

    QWidget* currentTab;
#ifndef SOFA_GUI_QT_NO_RECORDER
    QSofaRecorder* recorder;
#else
    QLabel* fpsLabel;
    QLabel* timeLabel;
#endif
    QSofaStatWidget* statWidget;
    QTimer* timerStep;
    WDoubleLineEdit *background[3];
    QLineEdit *backgroundImage;
    /// Stack viewer widget
    QWidgetStack* left_stack;
    SofaPluginManager* pluginManager_dialog;
    QMenuFilesRecentlyOpened recentlyOpenedFilesManager;

    std::string simulation_name;
    std::string gnuplot_directory;
    std::string pathDumpVisitor;

    void sleep(float seconds, float init_time)
    {
        unsigned int t = 0;
        clock_t goal = (clock_t) (seconds + init_time);
        while (goal > clock()/(float)CLOCKS_PER_SEC) t++;
    }

private:

#ifndef SOFA_QT4
    QString filePath_;
#endif
    //currently unused: scale is experimental
    float object_Scale[2];
    bool saveReloadFile;

    int frameCounter;

    void addViewer();
#ifdef SOFA_GUI_INTERACTION
    bool m_interactionActived;
#endif

#ifdef SOFA_PML
    virtual void pmlOpen(const char* filename, bool resetView=true);
    virtual void lmlOpen(const char* filename);
    PMLReader *pmlreader;
    LMLReader *lmlreader;
#endif

    DisplayFlagsDataWidget *displayFlag;

#ifdef SOFA_DUMP_VISITOR_INFO
    WindowVisitor* windowTraceVisitor;
    GraphVisitor* handleTraceVisitor;
#endif
    QDialog* descriptionScene;
    QTextBrowser* htmlPage;
    bool animationState;
};





struct ActivationFunctor
{
    ActivationFunctor(bool act, GraphListenerQListView* l):active(act), listener(l)
    {
        pixmap_filename= std::string("textures/media-record.png");
        if ( sofa::helper::system::DataRepository.findFile ( pixmap_filename ) )
            pixmap_filename = sofa::helper::system::DataRepository.getFile ( pixmap_filename );
    }
    void operator()(core::objectmodel::BaseNode* n)
    {
        if (active)
        {
            //Find the corresponding node in the Qt Graph
            QListViewItem *item=listener->items[n];
            //Remove the text
            QString desact_text = item->text(0);
            desact_text.remove(QString("Deactivated "), true);
            item->setText(0,desact_text);
            //Remove the icon
            QPixmap *p = getPixmap(n);
            item->setPixmap(0,*p);
            item->setOpen(true);
        }
        else
        {
            //Find the corresponding node in the Qt Graph
            QListViewItem *item=listener->items[n];
            //Remove the text
            item->setText(0, QString("Deactivated ") + item->text(0));
#ifdef SOFA_QT4
            item->setPixmap(0,QPixmap::fromImage(QImage(pixmap_filename.c_str())));
#else
            item->setPixmap(0,QPixmap(QImage(pixmap_filename.c_str())));
#endif
            item->setOpen(false);
        }
    }
protected:
    std::string pixmap_filename;
    bool active;
    GraphListenerQListView* listener;

};

} // namespace qt

} // namespace gui

} // namespace sofa

#endif // SOFA_GUI_VIEWER_REALGUI_H
