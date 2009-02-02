/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#include "GraphModeler.h"
#include "AddPreset.h"

#include <sofa/simulation/common/Simulation.h>
#include <sofa/gui/qt/FileManagement.h> //static functions to manage opening/ saving of files
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/SetDirectory.h>

#include <sofa/simulation/tree/xml/ObjectElement.h>
#include <sofa/simulation/tree/xml/AttributeElement.h>
#include <sofa/simulation/tree/xml/DataElement.h>
#include <sofa/simulation/tree/xml/XML.h>
#include <sofa/simulation/common/XMLPrintVisitor.h>

#ifdef SOFA_QT4
#include <Q3Header>
#include <Q3PopupMenu>
#include <QMessageBox>
#else
#include <qheader.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#endif

namespace sofa
{

namespace gui
{

namespace qt
{


#ifndef SOFA_QT4
typedef QPopupMenu Q3PopupMenu;
#endif

GNode *GraphModeler::addGNode(GNode *parent, GNode *child, bool saveHistory)
{
    GNode *lastRoot = getRoot();
    if (!child)
    {
        child = new GNode();
        if (!parent)
            child->setName("Root");
    }



    if (parent != NULL)
    {
        parent->addChild((Node*)child);

        if (saveHistory)
        {
            Operation adding(child, Operation::ADD_GNODE);
            adding.info=std::string("Adding GNODE ") + child->getClassName();
            storeHistory(adding);
        }
    }
    else
    {
        graphListener->addChild(NULL, child);
        //Set up the root
        firstChild()->setExpandable(true);
        firstChild()->setOpen(true);

        if (saveHistory) clearHistory();
    }

    if (!parent && childCount()>1)
    {
        deleteComponent(graphListener->items[lastRoot], saveHistory);
    }
    return child;
}

BaseObject *GraphModeler::addComponent(GNode *parent, ClassInfo* entry, std::string templateName, bool saveHistory, bool displayWarning)
{
    BaseObject *object=NULL;;
    if (!parent || !entry) return object;



    xml::ObjectElement description("Default", entry->className.c_str() );

    if (!templateName.empty()) description.setAttribute("template", templateName.c_str());


    ClassCreator* c;

    if (entry->creatorMap.size() <= 1)
        c=entry->creatorMap.begin()->second;
    else
    {
        if (templateName.empty())
        {
            c=entry->creatorMap.find(entry->defaultTemplate)->second; templateName=entry->defaultTemplate;
        }
        else
            c=entry->creatorMap.find(templateName)->second;
    }

    if (c->canCreate(parent->getContext(), &description))
    {
        object = c->createInstance(parent->getContext(), NULL);
        //parent->addObject(object);
        if (saveHistory)
        {
            Operation adding(object, Operation::ADD_OBJECT);

            adding.info=std::string("Adding Object ") + object->getClassName();
            storeHistory(adding);
        }
    }
    else
    {
        BaseObject* reference = parent->getContext()->getMechanicalState();

        if (displayWarning)
        {
            if (entry->className.find("Mapping") == std::string::npos)
            {
                if (!reference)
                {
                    //we accept the mappings as no initialization of the object has been done
                    const QString caption("Creation Impossible");
                    const QString warning=QString("No MechanicalState found in your Node ") + QString(parent->getName().c_str());
                    if ( QMessageBox::warning ( this, caption,warning, QMessageBox::Cancel | QMessageBox::Default | QMessageBox::Escape, QMessageBox::Ignore ) == QMessageBox::Cancel )
                        return object;
                }
                else
                {
                    const QString caption("Creation Impossible");
                    const QString warning=
                        QString("Your component won't be created: \n \t * <")
                        + QString(reference->getTemplateName().c_str()) + QString("> DOFs are used in the Node ") + QString(parent->getName().c_str()) + QString("\n\t * <")
                        + QString(templateName.c_str()) + QString("> is the type of your ") + QString(entry->className.c_str());
                    if ( QMessageBox::warning ( this, caption,warning, QMessageBox::Cancel | QMessageBox::Default | QMessageBox::Escape, QMessageBox::Ignore ) == QMessageBox::Cancel )
                        return object;
                }
            }
        }
        object = c->createInstance(parent->getContext(), NULL);
        // 	    parent->addObject(object);
    }
    return object;
}




void GraphModeler::dropEvent(QDropEvent* event)
{
    QPushButton *push = (QPushButton *)event->source();
    if (push)  push->setDown(false);

    QString text;
    Q3TextDrag::decode(event, text);
    if (!text.isEmpty())
    {
        std::string filename(text.ascii());
        std::string test = filename; test.resize(4);

        if (test == "file")
        {
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

            QString f(filename.c_str());
            emit(fileOpen(f));
            return;
        }
    }
    if (library.find(event->source()) != library.end())
    {
        std::string templateName =  text.ascii();
        BaseObject *newComponent = addComponent(getGNode(event->pos()), library.find(event->source())->second.first, templateName );
        if (newComponent)
        {
            Q3ListViewItem *after = graphListener->items[newComponent];
            Q3ListViewItem *item = itemAt(event->pos());
            if (getObject(item)) initItem(after, item);
        }
    }
    else
    {
        if (text == QString("GNode"))
        {
            GNode* node=getGNode(event->pos());

            if (node)
            {
                GNode *newNode=addGNode(node);
                if (newNode)
                {
                    Q3ListViewItem *after = graphListener->items[newNode];
                    Q3ListViewItem *item = itemAt(event->pos());
                    if (getObject(item)) initItem(after,item);
                }
            }
        }
    }
}




BaseObject *GraphModeler::getObject(Q3ListViewItem *item)
{
    std::map<core::objectmodel::Base*, Q3ListViewItem* >::iterator it;
    for (it = graphListener->items.begin(); it != graphListener->items.end(); it++)
    {
        if (it->second == item)
        {
            return dynamic_cast< BaseObject *>(it->first);
        }
    }
    return NULL;
}


GNode *GraphModeler::getGNode(const QPoint &pos)
{
    Q3ListViewItem *item = itemAt(pos);
    if (!item) return NULL;
    return getGNode(item);
}



GNode *GraphModeler::getGNode(Q3ListViewItem *item)
{
    if (!item) return NULL;
    sofa::core::objectmodel::Base *object;
    std::map<core::objectmodel::Base*, Q3ListViewItem* >::iterator it;
    for (it = graphListener->items.begin(); it != graphListener->items.end(); it++)
    {
        if (it->second == item)
        {
            object = it->first;
            break;
        }
    }
    if (it == graphListener->items.end()) return NULL;

    if (dynamic_cast<GNode*>(it->first)) return dynamic_cast<GNode*>(it->first);
    else
    {
        item = item->parent();
        for (it = graphListener->items.begin(); it != graphListener->items.end(); it++)
        {
            if (it->second == item)
            {
                object = it->first;
                break;
            }
        }
        if (it == graphListener->items.end()) return NULL;
        if (dynamic_cast<GNode*>(it->first)) return dynamic_cast<GNode*>(it->first);
        else return NULL;
    }
}


void GraphModeler::openModifyObject()
{
    Q3ListViewItem *item = currentItem();
    openModifyObject(item);
}

void GraphModeler::openModifyObject(Q3ListViewItem *item)
{
    if (!item) return;

    std::map<core::objectmodel::Base*, Q3ListViewItem* >::iterator it;
    for (it = graphListener->items.begin(); it != graphListener->items.end(); it++)
    {
        if (it->second == item)
        {
            break;
        }
    }
    if (it == graphListener->items.end()) return;


    //Unicity and identification of the windows
    current_Id_modifyDialog = it->first;
    std::map< void*, QDialog* >::iterator testWindow =  map_modifyObjectWindow.find( current_Id_modifyDialog);
    if ( testWindow != map_modifyObjectWindow.end())
    {
        //Object already being modified: no need to open a new window
        (*testWindow).second->raise();
        return;
    }

    ModifyObjectModeler *dialogModify = new ModifyObjectModeler ( current_Id_modifyDialog, it->first, item,this,item->text(0));
    map_modifyObjectWindow.insert( std::make_pair(current_Id_modifyDialog, dialogModify));
    //If the item clicked is a node, we add it to the list of the element modified

    map_modifyDialogOpened.insert ( std::make_pair ( current_Id_modifyDialog, it->first ) );

    dialogModify->show();
    dialogModify->raise();

}

void GraphModeler::doubleClick(Q3ListViewItem *item)
{
    if (!item) return;
    item->setOpen ( !item->isOpen() );
    openModifyObject(item);

}
void GraphModeler::rightClick(Q3ListViewItem *item, const QPoint &point, int index)
{
    if (!item) return;
    bool isNode=getObject(item)==NULL;

    Q3PopupMenu *contextMenu = new Q3PopupMenu ( this, "ContextMenu" );
    if (isNode)
    {
        contextMenu->insertItem("Collapse", this, SLOT( collapseNode()));
        contextMenu->insertItem("Expand"  , this, SLOT( expandNode()));
        contextMenu->insertSeparator ();
        contextMenu->insertItem("Load"  , this, SLOT( loadNode()));
        contextMenu->insertItem(QIconSet(), tr( "Preset"), preset);
        contextMenu->insertItem("Save"  , this, SLOT( saveNode()));
    }
    int index_menu = contextMenu->insertItem("Delete"  , this, SLOT( deleteComponent()));

    if (isNode)
    {
        if ( !isNodeErasable ( getGNode(item) ) )
            contextMenu->setItemEnabled ( index_menu,false );
    }
    else
    {
        if ( !isObjectErasable ( getObject(item) ))
            contextMenu->setItemEnabled ( index_menu,false );
    }

    contextMenu->insertItem("Modify"  , this, SLOT( openModifyObject()));
    contextMenu->popup ( point, index );

}


void GraphModeler::collapseNode()
{
    collapseNode(currentItem());
}

void GraphModeler::collapseNode(Q3ListViewItem* item)
{
    if (!item) return;

    Q3ListViewItem* child;
    child = item->firstChild();
    while ( child != NULL )
    {
        child->setOpen ( false );
        child = child->nextSibling();
    }
    item->setOpen ( true );
}

void GraphModeler::expandNode()
{
    expandNode(currentItem());
}

void GraphModeler::expandNode(Q3ListViewItem* item)
{
    if (!item) return;

    item->setOpen ( true );
    if ( item != NULL )
    {
        Q3ListViewItem* child;
        child = item->firstChild();
        while ( child != NULL )
        {
            item = child;
            child->setOpen ( true );
            expandNode(item);
            child = child->nextSibling();
        }
    }
}

GNode *GraphModeler::loadNode()
{
    return loadNode(currentItem());
}

GNode *GraphModeler::loadNode(Q3ListViewItem* item, std::string filename)
{
    if (!item) return NULL;
    GNode *node = getGNode(item);
    if (filename.empty())
    {
        QString s = getOpenFileName ( this, NULL,"Scenes (*.scn *.xml *.simu *.pscn)", "open file dialog",  "Choose a file to open" );
        if (s.length() >0)
        {
            filename = s.ascii();
        }
        else return NULL;
    }
    return loadNode(node,filename);
}



GNode *GraphModeler::buildNodeFromBaseElement(GNode *node,xml::BaseElement *elem, bool saveHistory)
{
    const bool displayWarning=true;
    GNode *newNode = new GNode();
    //Configure the new Node
    configureElement(newNode, elem);

    if (newNode->getName() == "Group")
    {
        //We can't use the parent node, as it is null
        if (!node) return NULL;
        newNode = node;
    }
    else
    {
        if (node)
        {
            //Add as a child
            addGNode(node,newNode,saveHistory);
        }
    }


    typedef xml::BaseElement::child_iterator<> elem_iterator;
    for (elem_iterator it=elem->begin(); it != elem->end(); ++it)
    {
        if (std::string(it->getClass()) == std::string("Node"))
        {
            buildNodeFromBaseElement(newNode, it,false); //Desactivate saving history
        }
        else
        {
            //Configure the new Component
            std::string templatename; std::string templateAttribute("template");
            templatename = it->getAttribute(templateAttribute, "");
            ClassInfo *info = getCreatorComponent(it->getType());
            BaseObject *newComponent=addComponent(newNode, info, templatename, saveHistory,displayWarning);
            configureElement(newComponent, it);
            Q3ListViewItem* itemGraph = graphListener->items[newComponent];

            std::string name=itemGraph->text(0).ascii();
            std::string::size_type pos = name.find(' ');
            if (pos != std::string::npos)  name.resize(pos);
            name += "  ";

            name+=newComponent->getName();
            itemGraph->setText(0,name.c_str());
        }
    }
    newNode->sendl.clearWarnings();

    return newNode;
}

ClassInfo *GraphModeler::getCreatorComponent(std::string name)
{

    ComponentMap::iterator it;
    for (it=library.begin(); it!=library.end(); it++)
    {
        if (it->second.first->className == name)
            return it->second.first;
    }
    return NULL;
}

void GraphModeler::configureElement(Base* b, xml::BaseElement *elem)
{
    //Init the Attributes of the object
    typedef xml::BaseElement::child_iterator<xml::AttributeElement> attr_iterator;
    typedef xml::BaseElement::child_iterator<xml::DataElement> data_iterator;
    for (attr_iterator itAttribute=elem->begin<xml::AttributeElement>(); itAttribute != elem->end<xml::AttributeElement>(); ++itAttribute)
    {
        for (data_iterator itData=itAttribute->begin<xml::DataElement>(); itData != itAttribute->end<xml::DataElement>(); ++itData)  itData->initNode();
        const std::string nameAttribute = itAttribute->getAttribute("type","");
        const std::string valueAttribute = itAttribute->getValue();
        elem->setAttribute(nameAttribute, valueAttribute.c_str());
    }

    std::vector< std::pair<std::string, BaseData*> > vecDatas=b->getFields();
    for (unsigned int i=0; i<vecDatas.size(); ++i)
    {
        std::string result = elem->getAttribute(vecDatas[i].first, "");
        if (!result.empty())
        {
            vecDatas[i].second->read(result);
        }
    }
}

GNode* GraphModeler::loadNode(GNode *node, std::string path)
{
    xml::BaseElement* newXML=NULL;

    newXML = xml::loadFromFile (path.c_str() );

    if (newXML == NULL) return NULL;

    //-----------------------------------------------------------------
    //Add the content of a xml file
    GNode *newNode = buildNodeFromBaseElement(node, newXML, true);
    //-----------------------------------------------------------------

    return newNode;
}

void GraphModeler::loadPreset(std::string presetName)
{


    xml::BaseElement* newXML = xml::loadFromFile (presetName.c_str() );
    if (newXML == NULL) return;

    xml::BaseElement::child_iterator<> it(newXML->begin());
    bool elementPresent[3]= {false,false,false};
    for (; it!=newXML->end(); ++it)
    {
        if (it->getName() == std::string("VisualNode") || it->getType() == std::string("OglModel")) elementPresent[1] = true;
        else if (it->getName() == std::string("CollisionNode")) elementPresent[2] = true;
        else if (it->getType() == std::string("MeshLoader") || it->getType() == std::string("SparseGrid")) elementPresent[0] = true;
    }

    if (!DialogAdd)
    {
        DialogAdd = new AddPreset(this,"AddPreset");
        DialogAdd->setPath(sofa::helper::system::DataRepository.getFirstPath());
    }

    DialogAdd->setRelativePath(sofa::helper::system::SetDirectory::GetParentDir(filenameXML.c_str()));

    DialogAdd->setElementPresent(elementPresent);
    DialogAdd->setPresetFile(presetName);
    GNode *node=getGNode(currentItem());
    DialogAdd->setParentNode(node);

    DialogAdd->show();
    DialogAdd->raise();
}


void GraphModeler::loadPreset(GNode *parent, std::string presetFile,
        std::string *filenames,
        std::string *translation,
        std::string *rotation,
        std::string scale)
{


    xml::BaseElement* newXML=NULL;

    newXML = xml::loadFromFile (presetFile.c_str() );
    if (newXML == NULL) return;

    bool collisionNodeFound=false;
    xml::BaseElement *meshMecha=NULL;
    xml::BaseElement::child_iterator<> it(newXML->begin());
    for (; it!=newXML->end(); ++it)
    {

        if (it->getType() == std::string("MechanicalObject"))
        {
            updatePresetNode(*it, std::string(), translation, rotation, scale);
        }
        if (it->getType() == std::string("MeshLoader") || it->getType() == std::string("SparseGrid"))
        {
            updatePresetNode(*it, filenames[0], translation, rotation, scale);
            meshMecha = it;
        }
        if (it->getType() == std::string("OglModel"))
        {
            updatePresetNode(*it, filenames[1], translation, rotation, scale);
        }

        if (it->getName() == std::string("VisualNode"))
        {
            xml::BaseElement* visualXML = it;

            xml::BaseElement::child_iterator<> it_visual(visualXML->begin());
            for (; it_visual!=visualXML->end(); ++it_visual)
            {
                if (it_visual->getType() == std::string("OglModel"))
                {
                    updatePresetNode(*it_visual, filenames[1], translation, rotation, scale);
                }
            }
        }
        if (it->getName() == std::string("CollisionNode"))
        {
            collisionNodeFound=true;
            xml::BaseElement* collisionXML = it;

            xml::BaseElement::child_iterator<> it_collision(collisionXML->begin());
            for (; it_collision!=collisionXML->end(); ++it_collision)
            {

                if (it_collision->getType() == std::string("MechanicalObject"))
                {
                    updatePresetNode(*it_collision, std::string(), translation, rotation, scale);
                }
                if (it_collision->getType() == std::string("MeshLoader"))
                {
                    updatePresetNode(*it_collision, filenames[2], translation, rotation, scale);
                }
            }
        }
    }



    if (!newXML->init()) std::cerr<< "Objects initialization failed.\n";
    GNode *presetNode = dynamic_cast<GNode*> ( newXML->getObject() );
    if (presetNode) addGNode(parent,presetNode);
}

void GraphModeler::updatePresetNode(xml::BaseElement &elem, std::string meshFile, std::string *translation, std::string *rotation, std::string scale)
{
    if (elem.presenceAttribute(std::string("filename")))     elem.setAttribute(std::string("filename"),     meshFile.c_str());
    if (elem.presenceAttribute(std::string("fileMesh")))     elem.setAttribute(std::string("fileMesh"),     meshFile.c_str());
    if (elem.presenceAttribute(std::string("fileTopology"))) elem.setAttribute(std::string("fileTopology"), meshFile.c_str());

    if (elem.presenceAttribute(std::string("dx"))) elem.setAttribute(std::string("dx"), translation[0].c_str());
    if (elem.presenceAttribute(std::string("dy"))) elem.setAttribute(std::string("dy"), translation[1].c_str());
    if (elem.presenceAttribute(std::string("dz"))) elem.setAttribute(std::string("dz"), translation[2].c_str());

    if (elem.presenceAttribute(std::string("rx"))) elem.setAttribute(std::string("rx"), rotation[0].c_str());
    if (elem.presenceAttribute(std::string("ry"))) elem.setAttribute(std::string("ry"), rotation[1].c_str());
    if (elem.presenceAttribute(std::string("rz"))) elem.setAttribute(std::string("rz"), rotation[2].c_str());

    if (elem.presenceAttribute(std::string("scale"))) elem.setAttribute(std::string("scale"), scale.c_str());
}


void GraphModeler::saveNode()
{
    saveNode(currentItem());
}

void GraphModeler::saveNode(Q3ListViewItem* item)
{
    if (!item) return;
    GNode *node = getGNode(item);
    if (!node)  return;

    QString s = sofa::gui::qt::getSaveFileName ( this, NULL, "Scenes (*.scn *.xml)", "save file dialog", "Choose where the scene will be saved" );
    if ( s.length() >0 )
        saveNode(node, s.ascii());
}

void GraphModeler::saveNode(GNode* node, std::string file)
{
    simulation::getSimulation()->printXML(node, file.c_str());
}

void GraphModeler::saveComponent(BaseObject* object, std::string file)
{
    std::ofstream out(file.c_str());
    simulation::XMLPrintVisitor print(out);
    out << "<Node name=\"Group\">\n";
    print.processBaseObject(object);
    out << "</Node>\n";
}

void GraphModeler::clearGraph(bool saveHistory)
{
    deleteComponent(firstChild(), saveHistory);
}

void GraphModeler::deleteComponent(Q3ListViewItem* item, bool saveHistory)
{
    if (!item) return;

    GNode *parent = getGNode(item->parent());
    bool isNode   = getObject(item)==NULL;
    if (!isNode && isObjectErasable(getObject(item)))
    {
        BaseObject* object = getObject(item);
        if (saveHistory)
        {
            Operation removal(getObject(item),Operation::DELETE_OBJECT);
            removal.parent=getGNode(item);
            removal.above=getComponentAbove(item);

            removal.info=std::string("Removing Object ") + object->getClassName();
            storeHistory(removal);
        }
        getGNode(item)->removeObject(getObject(item));
    }
    else
    {
        if (!isNodeErasable(getGNode(item))) return;
        GNode *node = getGNode(item);

        if (saveHistory)
        {
            Operation removal(node,Operation::DELETE_GNODE);
            removal.parent = parent;
            removal.above=getComponentAbove(item);
            removal.info=std::string("Removing GNode ") + node->getClassName();
            storeHistory(removal);
        }
        if (!parent)
            graphListener->removeChild(parent, node);
        else
            parent->removeChild((Node*)node);
        if (!parent && childCount() == 0) addGNode(NULL);
    }

}

Base *GraphModeler::getComponentAbove(Q3ListViewItem *item)
{
    if (!item) return NULL;
    Q3ListViewItem* itemAbove=item->itemAbove();
    while (itemAbove && itemAbove->parent() != item->parent() )
    {
        itemAbove = itemAbove->parent();
    }
    Base *result=getObject(itemAbove);
    if (!result) result=getGNode(itemAbove);
    return result;
}

void GraphModeler::deleteComponent()
{
    Q3ListViewItem *item = currentItem();
    deleteComponent(item);
}

void GraphModeler::modifyUnlock ( void *Id )
{
    map_modifyDialogOpened.erase( Id );
    map_modifyObjectWindow.erase( Id );
}



void GraphModeler::editUndo()
{
    Operation o=historyOperation.back();
    historyOperation.pop_back();

    processUndo(o);
    historyUndoOperation.push_back(o);

    emit( undo(historyOperation.size()));
    emit( redo(true));
}

void GraphModeler::editRedo()
{
    Operation o=historyUndoOperation.back();
    historyUndoOperation.pop_back();

    processUndo(o);
    historyOperation.push_back(o);

    emit( redo(historyUndoOperation.size()));
    emit( undo(true));

}


void GraphModeler::processUndo(Operation &o)
{
    switch(o.ID)
    {
    case Operation::DELETE_OBJECT:
        o.parent->addObject(dynamic_cast<BaseObject*>(o.sofaComponent));
        moveItem(graphListener->items[o.sofaComponent],graphListener->items[o.above]);
        o.ID = Operation::ADD_OBJECT;
        break;
    case Operation::DELETE_GNODE:
        o.parent->addChild(dynamic_cast<Node*>(o.sofaComponent));
        //If the node is not alone below another parent node
        moveItem(graphListener->items[o.sofaComponent],graphListener->items[o.above]);

        o.ID = Operation::ADD_GNODE;
        break;
    case Operation::ADD_OBJECT:
        o.parent=getGNode(graphListener->items[o.sofaComponent]);
        o.above=getComponentAbove(graphListener->items[o.sofaComponent]);

        o.parent->removeObject(dynamic_cast<BaseObject*>(o.sofaComponent));

        o.ID = Operation::DELETE_OBJECT;
        break;
    case Operation::ADD_GNODE:
        o.parent=getGNode(graphListener->items[o.sofaComponent]->parent());
        o.above=getComponentAbove(graphListener->items[o.sofaComponent]);
        o.parent->removeChild(dynamic_cast<Node*>(o.sofaComponent));
        o.ID = Operation::DELETE_GNODE;
        break;
    }
}

void GraphModeler::keyPressEvent ( QKeyEvent * e )
{
    switch ( e->key() )
    {
    case Qt::Key_Delete :
    {
        deleteComponent();
        break;
    }
    case Qt::Key_Return :
    case Qt::Key_Enter :
    {
        openModifyObject();
        break;
    }
    default:
    {
        Q3ListView::keyPressEvent(e);
        break;
    }
    }
}
/*****************************************************************************************************************/
// Test if a node can be erased in the graph : the condition is that none of its children has a menu modify opened
bool GraphModeler::isNodeErasable ( core::objectmodel::Base* element )
{
    std::map< void*, core::objectmodel::Base*>::iterator it;
    for (it = map_modifyDialogOpened.begin(); it != map_modifyDialogOpened.end(); it++)
    {

        if (dynamic_cast< BaseObject* >(it->second))
        {
            if (getGNode(graphListener->items[it->second]) == element) return false;
        }
        else if (it->second == element) return false;

    }

    std::map< core::objectmodel::Base*, Q3ListViewItem*>::iterator it_item;
    it_item = graphListener->items.find(element);

    Q3ListViewItem *child = it_item->second->firstChild();
    while (child != NULL)
    {
        for (it_item = graphListener->items.begin(); it_item != graphListener->items.end(); it_item++)
        {
            if  (it_item->second == child)
            {
                if (!isNodeErasable(it_item->first)) return false;
                break;
            }
        }
        child = child->nextSibling();
    }
    return true;
}

bool GraphModeler::isObjectErasable ( core::objectmodel::Base* element )
{
    std::map< void*, core::objectmodel::Base*>::iterator it;
    for (it = map_modifyDialogOpened.begin(); it != map_modifyDialogOpened.end(); it++)
    {
        if (it->second == element) return false;
    }

    return true;
}

void GraphModeler::initItem(Q3ListViewItem *item, Q3ListViewItem *above)
{
    moveItem(item, above);
    moveItem(above,item);
}

void GraphModeler::moveItem(Q3ListViewItem *item, Q3ListViewItem *above)
{
    if (item)
    {
        if (above)
        {
            item->moveItem(above);

            //Move the object in the Sofa Node.
            if (above && getObject(item))
            {
                GNode *n=getGNode(item);
                GNode::Sequence<BaseObject>::iterator A=n->object.end();
                GNode::Sequence<BaseObject>::iterator B=n->object.end();
                BaseObject* objectA=getObject(above);
                BaseObject* objectB=getObject(item);
                bool inversion=false;
                //Find the two objects in the Sequence of the GNode
                for (GNode::Sequence<BaseObject>::iterator it=n->object.begin(); it!=n->object.end(); ++it)
                {
                    if( *it == objectA)
                    {
                        A=it;
                        if (B!=n->object.end()) inversion=true;
                    }
                    else if ( *it == objectB) B=it;
                }
                //One has not been found: should not happen
                if (A==n->object.end() || B==n->object.end()) return;

                //Invert the elements
                GNode::Sequence<BaseObject>::iterator it;
                if (inversion) n->object.swap(A,B);
                else
                {
                    for (it=B; it!=A+1; --it) n->object.swap(it,it-1);
                }
            }
        }
        else
        {
            //Object
            if (getObject(item))
            {
                Q3ListViewItem *nodeQt = graphListener->items[getGNode(item)];
                Q3ListViewItem *firstComp=nodeQt->firstChild();
                if (firstComp != item) initItem(item, firstComp);
            }
            //GNode
            else
            {
                Q3ListViewItem *nodeQt = graphListener->items[getGNode(item->parent())];
                Q3ListViewItem *firstComp=nodeQt->firstChild();
                if (firstComp != item) initItem(item, firstComp);
            }
        }
    }
}


/// Drag Management
void GraphModeler::dragEnterEvent( QDragEnterEvent* event)
{
    event->accept(event->answerRect());
}

/// Drag Management
void GraphModeler::dragMoveEvent( QDragMoveEvent* event)
{
    QString text;
    Q3TextDrag::decode(event, text);
    if (!text.isEmpty())
    {
        std::string filename(text.ascii());
        std::string test = filename; test.resize(4);
        if (test == "file") {event->accept(event->answerRect());}
    }
    else
    {
        if ( getGNode(event->pos()))
            event->accept(event->answerRect());
        else
            event->ignore(event->answerRect());
    }
}

void GraphModeler::closeDialogs()
{
    std::map< void*, QDialog* >::iterator it;    ;
    for (it=map_modifyObjectWindow.begin();
            it!=map_modifyObjectWindow.end();
            it++)
    {
        delete it->second;
    }
}

/*****************************************************************************************************************/
//History of operations management
//TODO: not use the factory to create the elements!
bool GraphModeler::editCut(std::string path)
{
    if (selectedItem())
    {
        editCopy(path);
        deleteComponent(selectedItem(), true);
        return true;
    }
    return false;
}
bool GraphModeler::editCopy(std::string path)
{
    if (selectedItem())
    {
        BaseObject *object = getObject(selectedItem());
        if (!object)
        {
            GNode *node = getGNode(selectedItem());
            saveNode(node, path);
        }
        else
        {
            saveComponent(object,path);
        }
        return true;
    }
    return false;
}
bool GraphModeler::editPaste(std::string path)
{
    if (selectedItem())
    {
        GNode *node = getGNode(selectedItem());
        loadNode(node, path);
        Q3ListViewItem *pasteItem=selectedItem();
        Q3ListViewItem *insertedItem=selectedItem();
        while(insertedItem->nextSibling()) insertedItem=insertedItem->nextSibling();
        initItem(insertedItem, pasteItem);
    }
    return selectedItem();
}

///      void GraphModeler::addBaseElement(GNode *node,
/*****************************************************************************************************************/
//History of operations management

void GraphModeler::clearHistory()
{
    for ( int i=historyOperation.size()-1; i>=0; --i)
    {
        if (historyOperation[i].ID == Operation::DELETE_OBJECT)
            delete historyOperation[i].sofaComponent;
        else if (historyOperation[i].ID == Operation::DELETE_GNODE)
            simulation::getSimulation()->unload(dynamic_cast<GNode*>(historyOperation[i].sofaComponent));
    }
    historyOperation.clear();
    emit( undo(false) );
}

void GraphModeler::clearHistoryUndo()
{
    for ( int i=historyUndoOperation.size()-1; i>=0; --i)
    {
        if (historyUndoOperation[i].ID == Operation::DELETE_OBJECT)
            delete historyUndoOperation[i].sofaComponent;
        else if (historyUndoOperation[i].ID == Operation::DELETE_GNODE)
            simulation::getSimulation()->unload(dynamic_cast<GNode*>(historyUndoOperation[i].sofaComponent));
    }
    historyUndoOperation.clear();
    emit( redo(false) );
}

void GraphModeler::storeHistory(Operation &o)
{
    clearHistoryUndo();
    historyOperation.push_back(o);
    emit( undo(true) );
}


}
}
}
