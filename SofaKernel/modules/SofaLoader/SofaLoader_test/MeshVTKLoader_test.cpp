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

#include <gtest/gtest.h>

#include "SofaLoader/MeshVTKLoader.h"
using sofa::component::loader::MeshVTKLoader ;

#include <sofa/helper/system/FileRepository.h>
using sofa::helper::system::DataRepository ;

#include <sofa/helper/BackTrace.h>
using sofa::helper::BackTrace ;

#include <SofaTest/TestMessageHandler.h>
using sofa::helper::logging::ExpectMessage ;
using sofa::helper::logging::Message ;

namespace sofa
{
namespace meshvtkloader_test
{

int initTestEnvironment()
{
    BackTrace::autodump() ;
    return 0;
}
int s_autodump = initTestEnvironment() ;


struct MeshVTKLoaderTest : public ::testing::Test,
                            public MeshVTKLoader
{

    MeshVTKLoaderTest()
    {}

    void testLoad(std::string const& filename, unsigned nbPoints, unsigned nbEdges, unsigned nbTriangles, unsigned nbQuads, unsigned nbPolygons, unsigned nbTetrahedra, unsigned nbHexahedra)
    {
        setFilename(filename);
        EXPECT_TRUE(load());
        EXPECT_EQ(nbPoints, d_positions.getValue().size());
        EXPECT_EQ(nbEdges, d_edges.getValue().size());
        EXPECT_EQ(nbTriangles, d_triangles.getValue().size());
        EXPECT_EQ(nbQuads, d_quads.getValue().size());
        EXPECT_EQ(nbPolygons, d_polygons.getValue().size());
        EXPECT_EQ(nbTetrahedra, d_tetrahedra.getValue().size());
        EXPECT_EQ(nbHexahedra, d_hexahedra.getValue().size());
    }

};

TEST_F(MeshVTKLoaderTest, detectFileType)
{
    ASSERT_EQ(MeshVTKLoader::LEGACY, detectFileType(DataRepository.getFile("mesh/liver.vtk").c_str()));
    ASSERT_EQ(MeshVTKLoader::XML, detectFileType(DataRepository.getFile("mesh/Armadillo_Tetra_4406.vtu").c_str()));
}

TEST_F(MeshVTKLoaderTest, loadLegacy)
{
    testLoad(DataRepository.getFile("mesh/liver.vtk"), 5008, 0, 10000, 0, 0, 0, 0);
}

TEST_F(MeshVTKLoaderTest, loadXML)
{
    testLoad(DataRepository.getFile("mesh/Armadillo_Tetra_4406.vtu"), 1446, 0, 0, 0, 0, 4406, 0);
}

TEST_F(MeshVTKLoaderTest, loadInvalidFilenames)
{
    ExpectMessage errmsg(Message::Error) ;

    setFilename("");
    EXPECT_FALSE(load());

    setFilename("/home/test/thisisnotavalidpath");
    EXPECT_FALSE(load());

    setFilename(DataRepository.getFile("test.vtu"));
    EXPECT_FALSE(load());

    setFilename(DataRepository.getFile("test.vtk"));
    EXPECT_FALSE(load());
}

//TODO(dmarchal): Remove this tests until we can fix them.
#if 0
TEST_F(MeshVTKLoaderTest, loadBrokenVtkFile_OpenIssue)
{
    setFilename(DataRepository.getFile("mesh/liver_for_test_broken.vtk"));
    EXPECT_FALSE(load());
}

TEST_F(MeshVTKLoaderTest, loadBrokenVtuFile_OpenIssue)
{
    setFilename(DataRepository.getFile("mesh/Armadillo_Tetra_4406_for_test_broken.vtu"));
    EXPECT_FALSE(load());
}
#endif

}// namespace meshvtkloader_test
}// namespace sofa
