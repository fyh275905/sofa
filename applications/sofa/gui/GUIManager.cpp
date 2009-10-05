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

#include "GUIManager.h"
#include <sofa/gui/SofaGUI.h>
#include <sofa/gui/BatchGUI.h>
#include <sofa/gui/qt/RealGUI.h>
#include <sofa/gui/glut/SimpleGUI.h>
#include <sofa/component/init.h>
#include <sofa/simulation/common/xml/initXml.h>

namespace sofa
{

namespace gui
{

/*STATIC FIELD DEFINITIONS */
SofaGUI* GUIManager::currentGUI = NULL;
std::list<GUIManager::GUICreator> GUIManager::guiCreators;
std::vector<std::string> GUIManager::guiOptions;

int BatchGUIClass = GUIManager::RegisterGUI("batch", &BatchGUI::CreateGUI, &BatchGUI::InitGUI, -1);

#ifdef SOFA_GUI_GLUT

int SimpleGUIClass = GUIManager::RegisterGUI("glut", &glut::SimpleGUI::CreateGUI, &glut::SimpleGUI::InitGUI, 0);
#endif

#ifdef SOFA_GUI_QGLVIEWER

int QGLViewerGUIClass = GUIManager::RegisterGUI ( "qglviewer", &qt::RealGUI::CreateGUI, &qt::RealGUI::InitGUI, 3 );
#endif

#ifdef SOFA_GUI_QTVIEWER

int QtGUIClass = GUIManager::RegisterGUI ( "qt", &qt::RealGUI::CreateGUI, &qt::RealGUI::InitGUI, 2 );
#endif

#ifdef SOFA_GUI_QTOGREVIEWER

int QtOGREGUIClass = GUIManager::RegisterGUI ( "ogre", &qt::RealGUI::CreateGUI, &qt::RealGUI::InitGUI, 1 );
#endif






void GUIManager::AddGUIOption(const char* option)
{
    guiOptions.push_back(option);
}


int GUIManager::RegisterGUI(const char* name, CreateGUIFn* creator, InitGUIFn* init, int priority)
{
    std::list<GUICreator>::iterator it = guiCreators.begin();
    std::list<GUICreator>::iterator itend = guiCreators.end();
    while (it != itend && strcmp(name, it->name))
        ++it;
    if (it != itend)
    {
        std::cerr << "ERROR(GUIManager): GUI "<<name<<" duplicate registration."<<std::endl;
        return 1;
    }
    GUICreator entry;
    entry.name = name;
    entry.creator = creator;
    entry.init = init;
    entry.priority = priority;
    guiCreators.push_back(entry);
    return 0;
}

std::vector<std::string> GUIManager::ListSupportedGUI()
{
    std::vector<std::string> names;
    for(std::list<GUICreator>::iterator it = guiCreators.begin(), itend = guiCreators.end(); it != itend; ++it)
    {
        names.push_back(it->name);
    }
    return names;
}

std::string GUIManager::ListSupportedGUI(char separator)
{
    std::string names;
    bool first = true;
    for(std::list<GUICreator>::iterator it =guiCreators.begin(), itend =guiCreators.end(); it != itend; ++it)
    {
        if (!first) names += separator; else first = false;
        names += it->name;
    }
    return names;
}

const char* GUIManager::GetValidGUIName(const char* name /* = NULL */)
{

    if (guiCreators.empty())
    {
        std::cerr << "ERROR(SofaGUI): No GUI registered."<<std::endl;
        return NULL;
    }
    else
    {
        std::list<GUICreator>::iterator it =guiCreators.begin();
        std::list<GUICreator>::iterator itend =guiCreators.end();
        name = it->name;
        int prio = it->priority;
        while (++it != itend)
        {
            if (it->priority > prio)
            {
                name = it->name;
                prio = it->priority;
            }
        }
    }
    return name;
}

GUIManager::GUICreator* GUIManager::GetGUICreator(const char* name)
{
    if (!name) name = GetValidGUIName();
    std::list<GUICreator>::iterator it =guiCreators.begin();
    std::list<GUICreator>::iterator itend =guiCreators.end();
    while (it != itend && strcmp(name, it->name))
        ++it;
    if (it == itend)
    {
        std::cerr << "ERROR(SofaGUI): GUI "<<name<<" not found."<<std::endl;
        std::cerr << "Available GUIs:" << ListSupportedGUI(' ') << std::endl;
        return NULL;
    }
    else
        return &(*it);
}

int GUIManager::Init(const char* argv0, const char* name)
{
    SofaGUI::SetProgramName(argv0);
    sofa::component::init();
    sofa::simulation::xml::initXml();
    if (guiCreators.empty())
    {
        std::cerr << "ERROR(SofaGUI): No GUI registered."<<std::endl;
        return 1;
    }
    const char* valid_name = GetValidGUIName(name);
    if (currentGUI)
        return 0; // already initialized

    GUICreator* creator = GetGUICreator(valid_name);
    if (!creator)
    {
        return 1;
    }
    if (creator->init)
        return (*creator->init)(valid_name, guiOptions);
    else
        return 0;
}


int GUIManager::createGUI(sofa::simulation::Node* groot, const char* filename)
{
    const char* name = GetValidGUIName();
    if (!currentGUI)
    {
        GUICreator* creator = GetGUICreator(name);
        if (!creator)
        {
            return 1;
        }
        currentGUI = (*creator->creator)(name, guiOptions, groot, filename);
        if (!currentGUI)
        {
            std::cerr << "ERROR(SofaGUI): GUI "<<name<<" creation failed."<<std::endl;
            return 1;
        }
    }
    return 0;
}
void GUIManager::Redraw()
{
    if (currentGUI) currentGUI->redraw();
}

sofa::simulation::Node* GUIManager::CurrentSimulation()
{
    if (currentGUI)
        return currentGUI->currentSimulation();
    else
        return NULL;
}

void GUIManager::SetScene(sofa::simulation::Node* groot, const char* filename /*=NULL*/, bool temporaryFile /*=false*/ )
{
    if (currentGUI) currentGUI->setScene(groot,filename,temporaryFile);

}

int GUIManager::MainLoop(sofa::simulation::Node* groot, const char* filename)
{
    int ret = 0;
    if (!currentGUI)
    {
        createGUI(groot, filename);
    }
    ret = currentGUI->mainLoop();
    if (ret)
    {
        std::cerr << "ERROR(SofaGUI): GUI "<<currentGUI->GetGUIName()<<" main loop failed (code "<<ret<<")."<<std::endl;
        return ret;
    }
    return ret;
}
void GUIManager::SetDimension(int  width , int  height )
{
    if (currentGUI) currentGUI->setDimension(width,height);
}
void GUIManager::SetFullScreen()
{
    if (currentGUI) currentGUI->setFullScreen();
}



}
// namespace gui

}
// namespace sofa
