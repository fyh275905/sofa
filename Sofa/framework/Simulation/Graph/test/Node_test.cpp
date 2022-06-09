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
#include <sofa/testing/BaseSimulationTest.h>
using sofa::testing::BaseSimulationTest ;

#include <sofa/simulation/graph/SimpleApi.h>
using namespace sofa::simpleapi ;

#include "Node_test.h"

namespace sofa
{

TEST( Node_test, getPathName)
{
    /* create trivial DAG :
     *
     * A
     * |\
     * B C
     * |
     * D
     *
     */
    EXPECT_MSG_NOEMIT(Error, Warning);

    Node::SPtr root = sofa::simpleapi::createNode("A");
    Node::SPtr B = createChild(root, "B");
    Node::SPtr D = createChild(B, "D");
    BaseObject::SPtr C = core::objectmodel::New<Dummy>("C");
    root->addObject(C);

    EXPECT_STREQ(root->getPathName().c_str(), "/");
    EXPECT_STREQ(B->getPathName().c_str(), "/B");
    EXPECT_STREQ(C->getPathName().c_str(), "/C");
    EXPECT_STREQ(D->getPathName().c_str(), "/B/D");
}

TEST(Node_test, addObject)
{
    sofa::core::sptr<Node> root = sofa::simpleapi::createNode("root");
    BaseObject::SPtr A = core::objectmodel::New<Dummy>("A");
    BaseObject::SPtr B = core::objectmodel::New<Dummy>("B");

    root->addObject(A);

    // adds a second object after the last one.
    root->addObject(B);
    auto b = dynamic_cast< sofa::core::objectmodel::BaseContext*>(root.get());
    ASSERT_NE(b, nullptr);
    ASSERT_EQ(b->getObjects()[0]->getName(), "A");
    ASSERT_EQ(b->getObjects()[1]->getName(), "B");
}

TEST(Node_test, addObjectAtFront)
{
    sofa::core::sptr<Node> root = sofa::simpleapi::createNode("root");
    BaseObject::SPtr A = core::objectmodel::New<Dummy>("A");
    BaseObject::SPtr B = core::objectmodel::New<Dummy>("B");

    root->addObject(A);

    // adds a second object before the first one.
    root->addObject(B, sofa::core::objectmodel::TypeOfInsertion::AtBegin);
    auto b = dynamic_cast< sofa::core::objectmodel::BaseContext*>(root.get());
    ASSERT_NE(b, nullptr);
    ASSERT_EQ(b->getObjects()[0]->getName(), "B");
    ASSERT_EQ(b->getObjects()[1]->getName(), "A");
}

TEST(Node_test, addObjectPreservingContext)
{
    sofa::core::sptr<Node> root = sofa::simpleapi::createNode("root");

    BaseObject::SPtr A = core::objectmodel::New<Dummy>("A");
    BaseObject::SPtr B = core::objectmodel::New<Dummy>("B");

    auto child1 = sofa::simpleapi::createChild(root, "child1");
    auto child2 = sofa::simpleapi::createChild(root, "child2");

    // add the created object into the node named 'child1'
    child1->addObject(A);
    child1->addObject(B);

    // check that the two objects are in node1
    ASSERT_NE(child1->getObject(A->getName()), nullptr);
    ASSERT_NE(child1->getObject(B->getName()), nullptr);

    ASSERT_NE(A->getPathName(), "/root/child1/A");
    ASSERT_NE(B->getPathName(), "/root/child1/B");

    // add the created object into the node named 'child1'
    child2->addObject(A);
    ASSERT_EQ(child1->getObject(A->getName()), nullptr); // not in child1 anymore
    ASSERT_NE(child2->getObject(A->getName()), nullptr); // but in child2

    // check that the pathname reflects the change
    ASSERT_NE(A->getPathName(), "/root/child2/A");
    ASSERT_NE(B->getPathName(), "/root/child1/B");

    // check that the linkpath returned is reflecting the changes
    ASSERT_EQ(A->name.getLinkPath(), "@/child2/A.name");
    ASSERT_EQ(B->name.getLinkPath(), "@/child1/B.name");

    // check that the linkpath returned for links is reflecting the changes
    ASSERT_EQ(A->findLink("context")->getLinkedPath(), "@./");
    ASSERT_EQ(B->findLink("context")->getLinkedPath(), "@./");
}


}// namespace sofa







