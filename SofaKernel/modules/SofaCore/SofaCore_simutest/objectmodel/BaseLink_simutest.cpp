/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/core/objectmodel/Base.h>
using sofa::core::objectmodel::Base ;
using sofa::core::objectmodel::ComponentState;

#include <SofaSimulationGraph/testing/BaseSimulationTest.h>
using sofa::helper::testing::BaseSimulationTest ;
using sofa::simulation::Node ;

#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject;

#include <sofa/core/PathResolver.h>

using sofa::defaulttype::Rigid3Types;
using sofa::defaulttype::Vec3Types;

namespace
{

class BaseLink_test : public BaseSimulationTest,
        public ::testing::WithParamInterface<std::vector<std::string>>
{
public:
    SceneInstance* c;
    Node* node {nullptr};
    BaseLink_test()
    {
        importPlugin("SofaComponentAll") ;
        std::stringstream scene ;
        scene << "<?xml version='1.0'?>"
                 "<Node name='Root' gravity='0 -9.81 0' time='0' animate='0' >               \n"
                 "   <MechanicalObject name='mstate0'/>                                      \n"
                 "   <InfoComponent name='obj'/>                                             \n"
                 "   <Node name='child1'>                                                    \n"
                 "      <MechanicalObject name='mstate1'/>                                   \n"
                 "      <Node name='child2'>                                                 \n"
                 "      </Node>                                                              \n"
                 "   </Node>                                                                 \n"
                 "</Node>                                                                    \n" ;
        c = new SceneInstance("xml", scene.str()) ;
        c->initScene() ;
        Node* root = c->root.get() ;
        Base* b = sofa::core::PathResolver::FindBaseFromPath(root, "@/child1/child2");
        node = dynamic_cast<Node*>(b);
    }

    ~BaseLink_test() override
    {
        delete c;
    }
};


//////////////////////// Testing valid path //////////////////////////////////////
class MultiLink_simutest : public BaseLink_test {};

TEST_P(MultiLink_simutest, checkPaths)
{
    ASSERT_NE(node,nullptr);
    auto& t = GetParam();
    if(t[2]=="true")
        ASSERT_TRUE(node->object.CheckPaths(t[0], node)) << t[1] << " " << t[2];
    else
        ASSERT_FALSE(node->object.CheckPaths(t[0], node)) << t[1] << " " << t[2];
}

std::vector<std::vector<std::string>> multiLinkValues={
    {"@/child1/mstate1", "The mstate exists, CheckPaths should returns", "true"},
    {"@/child1/mstate1 @/mstate0", "There is multiple path, the link is a multilink and both pointed object exists. CheckPath should return ", "true"},
    {"@../mstate1 @../../mstate0", "There is multiple path, the link is a multilink and both pointed object exists. CheckPath should return ", "true"}
};

INSTANTIATE_TEST_SUITE_P(CheckPaths,
                        MultiLink_simutest,
                        ::testing::ValuesIn(multiLinkValues));

//////////////////////// Testing invalid path //////////////////////////////////////
class SingleLink_simutest : public BaseLink_test {};

TEST_P(SingleLink_simutest, CheckPath)
{
    ASSERT_NE(node,nullptr);
    auto& t = GetParam();
    if(t[2]=="true")
        ASSERT_TRUE(node->mechanicalState.CheckPath(t[0], node)) << t[1] << " " << t[2];
    else
        ASSERT_FALSE(node->mechanicalState.CheckPath(t[0], node)) << t[1] << " " << t[2];
}

std::vector<std::vector<std::string>> singleLinkValues={
    {"@/child1", "The linked type is a node, while the link should point to an object. Using two different types should return", "false"},
    {"@/obj", "The linked type is an InfoComponent type while the link should point to a  object. CheckPath should return", "false"},
    {"@/child1/mstate1 @/child1/mstate1", "Using multiple link in a SingleLink::CheckPath function should fail and return", "false"},
    {"@../mstate", "The path is not pointing to a valid mstate. CheckLink shoud return", "false"},
    {"@../mstate1", "The link's target exists and is of same type. CheckPath should return", "true"}
};

INSTANTIATE_TEST_SUITE_P(CheckPath,
                        SingleLink_simutest,
                        ::testing::ValuesIn(singleLinkValues));


} /// namespace
