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
#include <iostream>
#include <fstream>
#include <sofa/helper/ArgumentParser.h>
#include <sofa/simulation/tree/xml/initXml.h>
#include <sofa/simulation/tree/TreeSimulation.h>
#include <sofa/component/init.h>
#include <sofa/helper/Factory.h>
#include <sofa/helper/BackTrace.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/gui/SofaGUI.h>
#include <sofa/helper/system/gl.h>
#include <sofa/helper/system/glut.h>
#include <sofa/helper/system/atomic.h>

#ifndef WIN32
#include <dlfcn.h>
bool loadPlugin(const char* filename)
{
    void *handle;
    handle=dlopen(filename, RTLD_LAZY);
    if (!handle)
    {
        std::cerr<<"Error loading plugin "<<filename<<": "<<dlerror()<<std::endl;
        return false;
    }
    std::cerr<<"Plugin "<<filename<<" loaded."<<std::endl;
    return true;
}
#else
bool loadPlugin(const char* /*filename*/)
{
    std::cerr << "Plugin loading not supported on this platform.\n";
    return false;
}
#endif

// ---------------------------------------------------------------------
// ---
// ---------------------------------------------------------------------
int main(int argc, char** argv)
{
    std::cout << "Using " << sofa::helper::system::atomic<int>::getImplName()<<" atomics." << std::endl;

    sofa::helper::BackTrace::autodump();



    sofa::gui::SofaGUI::SetProgramName(argv[0]);
    std::string fileName ;
    bool        startAnim = false;
    bool        printFactory = false;
    bool        loadRecent = false;
    std::string gui = sofa::gui::SofaGUI::GetGUIName();
    std::vector<std::string> plugins;
    std::vector<std::string> files;

    std::string gui_help = "choose the UI (";
    gui_help += sofa::gui::SofaGUI::ListSupportedGUI('|');
    gui_help += ")";

    sofa::helper::parse(&files, "This is a SOFA application. Here are the command line arguments")
    .option(&startAnim,'s',"start","start the animation loop")
    .option(&printFactory,'p',"factory","print factory logs")
    .option(&gui,'g',"gui",gui_help.c_str())
    .option(&plugins,'l',"load","load given plugins")
    .option(&loadRecent,'r',"recent","load most recently opened file")
    (argc,argv);

    if(gui!="batch")
        glutInit(&argc,argv);
    sofa::component::init();
    sofa::simulation::setSimulation(new sofa::simulation::tree::TreeSimulation());
    sofa::simulation::tree::xml::initXml();

    if (!files.empty()) fileName = files[0];

    for (unsigned int i=0; i<plugins.size(); i++)
        loadPlugin(plugins[i].c_str());

    if (printFactory)
    {
        std::cout << "////////// FACTORY //////////" << std::endl;
        sofa::helper::printFactoryLog();
        std::cout << "//////// END FACTORY ////////" << std::endl;
    }

    if (int err=sofa::gui::SofaGUI::Init(argv[0],gui.c_str()))
        return err;

    sofa::simulation::tree::GNode* groot = NULL;


    if (fileName.empty())
    {
        fileName = "Demos/liver.scn";

        if (loadRecent) // try to reload the latest scene
        {
            std::string scenes = "config/Sofa.ini";
            scenes = sofa::helper::system::DataRepository.getFile( scenes );
            std::ifstream mrulist(scenes.c_str());
            std::getline(mrulist,fileName);
            mrulist.close();
        }
        fileName = sofa::helper::system::DataRepository.getFile(fileName);
    }
    if (groot==NULL)
    {
        groot = new sofa::simulation::tree::GNode;
        //return 1;
    }

    if (int err=sofa::gui::SofaGUI::createGUI(groot,fileName.c_str()))
        return err;

    std::string in_filename(fileName);
    if (in_filename.rfind(".simu") == std::string::npos)
    {
        sofa::simulation::tree::getSimulation()->unload ( groot);
        groot = dynamic_cast<sofa::simulation::tree::GNode*>( sofa::simulation::tree::getSimulation()->load(fileName.c_str()));
        sofa::simulation::tree::getSimulation()->init(groot);
        if(sofa::gui::SofaGUI::CurrentGUI())
            sofa::gui::SofaGUI::CurrentGUI()->setScene(groot,fileName.c_str());
    }


    if (startAnim)
        groot->setAnimate(true);



    //=======================================
    // Run the main loop

    if (gui=="none")
    {
        if (groot==NULL)
        {
            std::cerr<<"Could not load file "<<fileName<<std::endl;
            return 1;
        }
        std::cout << "Computing 1000 iterations." << std::endl;
        for (int i=0; i<1000; i++)
        {
            sofa::simulation::tree::getSimulation()->animate(groot);
        }
        std::cout << "1000 iterations done." << std::endl;
    }
    else
    {
        if (int err=sofa::gui::SofaGUI::MainLoop(groot,fileName.c_str()))
            return err;
        groot = dynamic_cast<sofa::simulation::tree::GNode*>( sofa::gui::SofaGUI::CurrentSimulation() );
    }

    if (groot!=NULL)
        sofa::simulation::tree::getSimulation()->unload(groot);
    return 0;
}
