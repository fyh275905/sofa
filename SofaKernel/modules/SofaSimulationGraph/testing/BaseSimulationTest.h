/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2017 INRIA, USTL, UJF, CNRS, MGH                    *
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
/******************************************************************************
 * Contributors:
 *    - damien.marchal@univ-lille1.fr
 ******************************************************************************/
#ifndef SOFA_HELPER_TESTING_BASESIMULATIONTEST_H
#define SOFA_HELPER_TESTING_BASESIMULATIONTEST_H

#include <sofa/helper/testing/BaseTest.h>
#include <sofa/simulation/Node.h>
#include <sofa/simulation/Simulation.h>

namespace sofa
{
namespace helper
{
namespace testing
{
using sofa::simulation::Node ;
using sofa::simulation::Simulation ;

class SOFA_SIMULATION_GRAPH_API BaseSimulationTest : public virtual BaseTest
{
public:
    BaseSimulationTest() ;

    bool importPlugin(const std::string& name) ;

    class SceneInstance
    {
    public:

         SceneInstance(const std::string& rootname="root") ;
         SceneInstance(const std::string& type, const std::string& memory) ;
         ~SceneInstance() ;

        Node::SPtr root ;
        Simulation* simulation {nullptr} ;

        void initScene() ;
    } ;
};

} ///testing
} ///helper
} ///sofa

#endif /// SOFA_HELPER_TESTING_BASESIMULATIONTEST_H
