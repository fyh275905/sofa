/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
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
* with this program. If not, see <http://www.gnu.org/licenses/>.              *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <sofa/gui/qt/config.h>

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QPushButton>


#include <sofa/gui/qt/config.h>
#include <sofa/simulation/fwd.h>
#include <sofa/core/objectmodel/BaseData.h>
#include <sofa/core/objectmodel/BaseObject.h>

#include <map>

namespace sofa::gui::qt
{

class AddObject;
class GraphListenerQListView;
class QDisplayPropertyWidget;

enum ObjectModelType { typeNode, typeObject, typeData };
typedef union ObjectModelPtr
{
    sofa::simulation::Node* Node;
    core::objectmodel::BaseObject* Object;
    core::objectmodel::BaseData* Data;
} ObjectModelPtr;

typedef struct ObjectModel
{
public:
    ObjectModelType type;
    ObjectModelPtr ptr;
    bool isNode()   { return type == typeNode;   }
    bool isObject() { return type == typeObject; }
    bool isData()   { return type == typeData;   }
    bool isBase()   { return isNode() || isObject(); }
    sofa::core::objectmodel::Base* asBase()
    {
        if( isNode() )
            return sofa::core::castToBase(ptr.Node);
        if( isObject() )
            return dynamic_cast<sofa::core::objectmodel::Base*>(ptr.Object);
        return nullptr;
    }
} ObjectModel;

enum SofaListViewAttribute
{
    SIMULATION,
    VISUAL,
    MODELER
};

class SOFA_GUI_QT_API QSofaListView : public QTreeWidget
{
    Q_OBJECT
public:
    class LockContextManager
    {
    public:
        QSofaListView* self{nullptr};
        bool state{true};

        LockContextManager(QSofaListView* view, bool isLocked)
        {
            self = view;
            state = view->isLocked();
            if(isLocked)
                view->lock();
            else
                view->unLock();
        }

        ~LockContextManager()
        {
            if(state)
                self->lock();
            else
                self->unLock();
        }
    };

    QSofaListView(const SofaListViewAttribute& attribute,
            QWidget* parent = nullptr,
            const char* name = nullptr,
            Qt::WindowFlags f = Qt::WindowType::Widget );
    ~QSofaListView() override;

    GraphListenerQListView* getListener() const { return  graphListener_; }

    void setPropertyWidget(QDisplayPropertyWidget* propertyWid) {propertyWidget = propertyWid;}
    void addInPropertyWidget(QTreeWidgetItem *item, bool clear);

    void Clear(sofa::simulation::Node* rootNode);
    void lock();
    void unLock();

    /// Returns true if the view is not syncrhonized anymore with the simulation graph.
    /// To re-syncronize the view you can:
    ///     - call unfreeze() so any future change will be reflected
    ///     - call update(), to update one time the graph.
    bool isDirty();

    /// Returns true if the view updates for any scene graph change is disable.
    bool isLocked();

    /// call this method to indicate that the internal model has changed
    /// and thus the view is now dirty.
    void setViewToDirty();

    /// Updates the view so it is synchronized with the simulation graph.
    /// The view can be visually de-synchronized with the simulation graph. This happens
    /// when the view is "frozen" for performance reason. In that case, use isDirty to
    /// get current view state or the dirtynessChanged() signal.
    /// To resynchronize the view call the update methid.
    void update();
    void setRoot(sofa::simulation::Node*);

    SofaListViewAttribute getAttribute() const { return attribute_; }

    void contextMenuEvent(QContextMenuEvent *event) override;

    void expandPathFrom(const std::vector<std::string>& pathes);
    void getExpandedNodes(std::vector<std::string>&);

    void loadObject ( std::string path, double dx, double dy, double dz,  double rx, double ry, double rz,double scale );

public Q_SLOTS:
    void Export();
    void CloseAllDialogs();
    void UpdateOpenedDialogs();
    void ExpandRootNodeOnly();

Q_SIGNALS:
    void Close();
    void Lock(bool);
    void RequestSaving(sofa::simulation::Node*);
    void RequestExportOBJ(sofa::simulation::Node* node, bool exportMTL);
    void RequestActivation(sofa::simulation::Node*,bool);
    void RequestSleeping(sofa::simulation::Node*, bool);
    void RootNodeChanged(sofa::simulation::Node* newroot, const char* newpath);
    void NodeRemoved();
    void Updated();
    void NodeAdded();
    void focusChanged(sofa::core::objectmodel::BaseObject*);
    void focusChanged(sofa::core::objectmodel::BaseNode*);
    void dataModified( QString );

    /// Connect to this signal to be notified when the dirtyness status of the QSofaListView changed.
    void dirtynessChanged(bool isDirty);

    /// Connect to this signal to be notified when the locking status changed
    void lockingChanged(bool isLocked);

protected Q_SLOTS:
    void SaveNode();
    void exportOBJ();
    void collapseNode();
    void expandNode();
    void modifyUnlock(void* Id);
    void RaiseAddObject();
    void RemoveNode();
    void Modify();
    void openInEditor();
    void openInstanciation();
    void openImplementation();
    void copyFilePathToClipBoard();
    void DeactivateNode();
    void ActivateNode();
    void PutNodeToSleep();
    void WakeUpNode();

    void updateMatchingObjectmodel(QTreeWidgetItem* item, int);
    void updateMatchingObjectmodel(QTreeWidgetItem* item);

    void RunSofaRightClicked(const QPoint& point);
    void RunSofaDoubleClicked( QTreeWidgetItem* item, int index);

    void nodeNameModification( simulation::Node*);
    void focusObject();
    void focusNode();

protected:
    void expandPath(const std::string& path) ;
    void getExpandedNodes(QTreeWidgetItem* item, std::vector<std::string>&) ;
    void collapseNode(QTreeWidgetItem* item);
    void expandNode(QTreeWidgetItem* item);
    void transformObject ( sofa::simulation::Node *node, double dx, double dy, double dz,  double rx, double ry, double rz, double scale );
    bool isNodeErasable( core::objectmodel::BaseNode* node);

    std::list<core::objectmodel::BaseNode*> collectNodesToChange(core::objectmodel::BaseNode* node);
    std::map< void*, QTreeWidgetItem* > map_modifyDialogOpened;
    std::map< void*, QDialog* > map_modifyObjectWindow;
    GraphListenerQListView* graphListener_;
    std::vector< std::string > list_object;
    AddObject* AddObjectDialog_;
    ObjectModel object_;
    SofaListViewAttribute attribute_;
    QDisplayPropertyWidget* propertyWidget;

    /// Indicate that the view is de-synchronized with the real content of the simulation graph.
    /// This can happen if the graph has been freezed (i.e. not graphically updated) for performance
    /// reason while simulating complex scenes.
    bool m_isDirty;
    bool m_isLocked;
};

} //namespace sofa::gui::qt
