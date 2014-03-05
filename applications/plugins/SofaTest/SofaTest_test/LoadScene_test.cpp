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

#include "Sofa_test.h"
#include <sofa/component/init.h>
#include <sofa/simulation/common/Simulation.h>
#include <sofa/simulation/graph/DAGSimulation.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/helper/system/SetDirectory.h>
#include <sofa/simulation/common/SceneLoaderXML.h>

namespace sofa {

/** Test a scene: load a given scene with the xml file contained in the sub-directories Scenes and init it.
To test a new scene add the xml file in the Scenes directory and add the TEST_F for your scene (see below the example for BilinearConstraint scene). 
 */
struct LoadScene_test: public Sofa_test<double>
{
    // root
   simulation::Node::SPtr root;

   bool LoadScene(std::string sceneName)
   {
       // Init Sofa
       sofa::component::init();
       simulation::Simulation* simulation;
       sofa::simulation::setSimulation(simulation = new sofa::simulation::graph::DAGSimulation());

       // Load the scene from the xml file
       std::string fileName = sofa::helper::system::DataRepository.getFile(sceneName);
       root = sofa::core::objectmodel::SPtr_dynamic_cast<sofa::simulation::Node>( sofa::simulation::getSimulation()->load(fileName.c_str()));

       // Test if load has succeeded
       sofa::simulation::SceneLoaderXML scene;
       
       if(!root || !scene.loadSucceed)
       {  
           ADD_FAILURE() << "Error while loading the scene: " << sceneName << std::endl;
           return false;   
       }

       return true;
   }

   bool initScene (std::string sceneName)
   {
       LoadScene(sceneName);
      
       // Init the scene
       sofa::simulation::getSimulation()->init(root.get());

       // Test if root is not null
       if(!root)
       {  
           ADD_FAILURE() << "Error in init for the scene: " << sceneName << std::endl;
           return false;   
       }

       return true;

   }
        

};

TEST_F( LoadScene_test,BilinearConstraint)
{
     ASSERT_TRUE(this->LoadScene("tests/SofaTest/BilinearConstraint.scn"));
     ASSERT_TRUE(this->initScene("tests/SofaTest/BilinearConstraint.scn"));
     ASSERT_NO_THROW(this->initScene("tests/SofaTest/BilinearConstraint.scn"));
}

}// namespace sofa







