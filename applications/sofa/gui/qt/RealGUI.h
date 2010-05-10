/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     **
 under the terms of the GNU General Public License as published by the Free  *
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
#ifndef SOFA_GUI_VIEWER_REALGUI_H
#define SOFA_GUI_VIEWER_REALGUI_H

#ifdef SOFA_PML
#  include <sofa/filemanager/sofapml/PMLReader.h>
#  include <sofa/filemanager/sofapml/LMLReader.h>
#endif

#include "GUI.h"
#include "SofaGUIQt.h"
#include <sofa/gui/SofaGUI.h>
#include <time.h>

#include <sofa/gui/qt/QSofaListView.h>
#include <sofa/gui/qt/GraphListenerQListView.h>
#include <sofa/gui/qt/FileManagement.h>
#include <sofa/gui/qt/viewer/SofaViewer.h>
#include <sofa/gui/qt/AddObject.h>
#include <sofa/gui/qt/ModifyObject.h>
#include <sofa/gui/qt/DisplayFlagWidget.h>
#include <sofa/gui/qt/SofaPluginManager.h>
#include <sofa/gui/qt/SofaMouseManager.h>
#include <sofa/gui/qt/SofaVideoRecorderManager.h>

#include <sofa/simulation/common/xml/XML.h>
#include <sofa/helper/system/SetDirectory.h>

#ifdef SOFA_DUMP_VISITOR_INFO
#include <sofa/gui/qt/WindowVisitor.h>
#include <sofa/gui/qt/GraphVisitor.h>
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
/*namespace simulation{
  class Node;
}*/

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

class SOFA_SOFAGUIQT_API RealGUI : public ::GUI, public SofaGUI
{
    Q_OBJECT

    /// @name SofaGUI Interface
    /// @{


public:

#ifndef SOFA_QT4
    void setWindowFilePath(const QString &filePath) { filePath_=filePath;};
    QString windowFilePath() const { QString filePath = filePath_; return filePath; }
#endif

    static int InitGUI(const char* name, const std::vector<std::string>& options);
    static SofaGUI* CreateGUI(const char* name, const std::vector<std::string>& options, sofa::simulation::Node* groot = NULL, const char* filename = NULL);

    int mainLoop();

    int closeGUI();

    Node* currentSimulation();

    /// @}

    const char* viewerName;

    sofa::gui::qt::viewer::SofaViewer* viewer;
    QSofaListView* simulationGraph;
#ifndef SOFA_CLASSIC_SCENE_GRAPH
    QSofaListView* visualGraph;
#endif

    RealGUI( const char* viewername, const std::vector<std::string>& options = std::vector<std::string>() );
    ~RealGUI();

    static void setPixmap(std::string pixmap_filename, QPushButton* b);

    virtual void fileOpen(std::string filename, bool temporaryFile=false);
    virtual void fileOpenSimu(std::string filename);
    virtual void setScene(Node* groot, const char* filename=NULL, bool temporaryFile=false);

    //Configuration methods
    virtual void setViewerResolution(int w, int h);
    virtual void setFullScreen();
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
    virtual void fileSaveAs() {fileSaveAs((Node *)NULL);};

    virtual void fileSaveAs(Node* node,const char* filename);

    virtual void fileReload();
    virtual void fileExit();
    virtual void saveXML();
    virtual void viewerOpenGL();
    virtual void viewerQGLViewer();
    virtual void viewerOGRE();

    virtual void editRecordDirectory();
    virtual void editGnuplotDirectory();
    virtual void showPluginManager();
    virtual void showMouseManager();
    virtual void showVideoRecorderManager();


    void dragEnterEvent( QDragEnterEvent* event) {event->accept();}
    void dropEvent(QDropEvent* event);

    void initRecentlyOpened();

public slots:
    void NewRootNode(sofa::simulation::Node* root, const char* path);
    void ActivateNode(sofa::simulation::Node* , bool );
    void Update();
    virtual void fileSaveAs(sofa::simulation::Node *node);
    void LockAnimation(bool);

    void fileRecentlyOpened(int id);
    void updateRecentlyOpened(std::string fileLoaded);
    void playpauseGUI(bool value);
    void step();
    void setDt(double);
    void setDt(const QString&);
    void resetScene();
    void screenshot();
    void showhideElements(int FILTER, bool value);
    void updateViewerParameters();
    void updateBackgroundColour();
    void updateBackgroundImage();

#ifdef SOFA_QT4
    void changeHtmlPage( const QUrl&);
#else
    void changeHtmlPage( const QString&);
#endif
    void changeInstrument(int);
    void Clear();
    //Used in Context Menu
    //refresh the visualization window
    void redraw();
    void exportOBJ(sofa::simulation::Node* node, bool exportMTL=true);
    void dumpState(bool);
    void displayComputationTime(bool);
    void setExportGnuplot(bool);
    void setExportVisitor(bool);
    void currentTabChanged(QWidget*);

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

    void loadSimulation(bool one_step=false);

    void startDumpVisitor();
    void stopDumpVisitor();

    bool m_dumpState;
    std::ofstream* m_dumpStateStream;
    std::ostringstream m_dumpVisitorStream;
    bool m_exportGnuplot;
    bool _animationOBJ; int _animationOBJcounter;// save a succession of .obj indexed by _animationOBJcounter
    bool m_displayComputationTime;


    QWidget* currentTab;
    QWidget *tabInstrument;
#ifndef SOFA_GUI_QT_NO_RECORDER
    QSofaRecorder* recorder;
#else
    QLabel* fpsLabel;
    QLabel* timeLabel;
#endif
    QSofaStatWidget* statWidget;
    QTimer* timerStep;
    WFloatLineEdit *background[3];
    QLineEdit *backgroundImage;
    QWidgetStack* left_stack;

    std::string simulation_name;
    std::string gnuplot_directory;
    std::string pathDumpVisitor;

    sofa::simulation::Node* getScene() { if (viewer) return viewer->getScene(); else return NULL; }

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

    bool setViewer(const char* name);
    void addViewer();
    void setGUI(void);

#ifdef SOFA_PML
    virtual void pmlOpen(const char* filename, bool resetView=true);
    virtual void lmlOpen(const char* filename);
    PMLReader *pmlreader;
    LMLReader *lmlreader;
#endif

    DisplayFlagWidget *displayFlag;

#ifdef SOFA_DUMP_VISITOR_INFO
    WindowVisitor* windowTraceVisitor;
    GraphVisitor* handleTraceVisitor;
#endif
    QDialog* descriptionScene;
    QTextBrowser* htmlPage;
    bool animationState;
};


class InformationOnPickCallBack: public CallBackPicker
{
public:
    InformationOnPickCallBack(RealGUI *g):gui(g) {};

    virtual void execute(const sofa::component::collision::BodyPicked &body)
    {
        core::objectmodel::BaseObject *objectPicked=NULL;
        if (body.body)
        {
            Q3ListViewItem* item=gui->simulationGraph->getListener()->items[body.body];
            gui->simulationGraph->ensureItemVisible(item);
            gui->simulationGraph->clearSelection();
            gui->simulationGraph->setSelected(item,true);
            objectPicked=body.body;
        }
        else if (body.mstate)
        {
            Q3ListViewItem* item=gui->simulationGraph->getListener()->items[body.mstate];
            gui->simulationGraph->ensureItemVisible(item);
            gui->simulationGraph->clearSelection();
            gui->simulationGraph->setSelected(item,true);
            objectPicked=body.mstate;
        }
        else
            gui->simulationGraph->clearSelection();

        if (objectPicked)
        {
            QString messagePicking;
            simulation::Node *n=static_cast<simulation::Node*>(objectPicked->getContext());
            messagePicking=QString("Index ") + QString::number(body.indexCollisionElement)
                    + QString(" of  ")
                    + QString(n->getPathName().c_str())
                    + QString("/") + QString(objectPicked->getName().c_str())
                    + QString(" : ") + QString(objectPicked->getClassName().c_str());
            if (!objectPicked->getTemplateName().empty())
                messagePicking += QString("<") + QString(objectPicked->getTemplateName().c_str()) + QString(">");
            gui->statusBar()->message(messagePicking,3000); //display message during 3 seconds
        }
    }
protected:
    RealGUI *gui;
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
