/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
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
* Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/core/DataEngine.h>

#include <gtest/gtest.h>

namespace sofa {


/// to test tracked Data
class TestEngine : public core::DataEngine
{

public:

    SOFA_CLASS(TestEngine,core::DataEngine);


    Data< bool > input;
    Data< int > output;

    enum { UNDEFINED=0, CHANGED, NO_CHANGED };

    TestEngine()
        : Inherit1()
        , input(initData(&input,false,"input","input"))
        , output(initData(&output,(int)UNDEFINED,"output","output"))
    {}

    ~TestEngine() {}

    void init()
    {
        addInput(&input);
        trackData(input);

        addOutput(&output);

        setDirtyValue();
    }

    void reinit()
    {
        update();
    }

    void update()
    {
        if( didTrackedDataChanged( input ) )
            output.setValue(CHANGED);
        else
            output.setValue(NO_CHANGED);

        cleanDirty();
    }

};


struct DataEngine_test: public ::testing::Test
{
    TestEngine engine;

    void SetUp()
    {
        engine.init();
    }

    /// to test tracked Data
    void testTrackedData()
    {
        // input did not change
        ASSERT_TRUE(engine.output.getValue()==TestEngine::NO_CHANGED);

        engine.input.setValue(true);
        ASSERT_TRUE(engine.output.getValue()==TestEngine::CHANGED);

        engine.update();
        ASSERT_TRUE(engine.output.getValue()==TestEngine::NO_CHANGED);
    }

};

// Test
TEST_F(DataEngine_test, testTrackedData )
{
    this->testTrackedData();
}


}// namespace sofa
