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

#include "../../../../tutorials/objectCreator/ObjectCreator.h"

#include <sofa/helper/ArgumentParser.h>
#include <sofa/gui/GUIManager.h>


#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/SetDirectory.h>
#include <sofa/gui/SofaGUI.h>


//Including component for topological description of the objects
#include <sofa/component/topology/MeshTopology.h>
#include <sofa/component/topology/RegularGridTopology.h>


//SOFA_HAS_BOOST_KERNEL to define in chainHybrid.pro

#include <sofa/simulation/bgl/BglSimulation.h>
#include <sofa/component/visualModel/OglModel.h>

//Using double by default, if you have SOFA_FLOAT in use in you sofa-default.cfg, then it will be FLOAT.
#include <sofa/component/typedef/Sofa_typedef.h>



using sofa::component::visualmodel::OglModel;
using namespace sofa::simulation;
using namespace sofa::component::forcefield;
using namespace sofa::component::collision;
using namespace sofa::component::topology;
using sofa::component::container::MeshLoader;
// ---------------------------------------------------------------------
// ---
// ---------------------------------------------------------------------

Node* createRegularGrid(double x, double y, double z)
{
    static unsigned int i = 1;
    std::ostringstream oss;
    oss << "regularGrid_" << i;

    Node* node = sofa::simulation::getSimulation()->newNode(oss.str());

    RegularGridTopology* grid = new RegularGridTopology(3,3,3);
    grid->setPos(-1+x,1+x,-1+y,1+y,-1+z,1+z);
    MechanicalObject3* dof = new MechanicalObject3;

    UniformMass3* mass = new UniformMass3;
    mass->setTotalMass(10);

    HexahedronFEMForceField3* ff = new HexahedronFEMForceField3();
    ff->setYoungModulus(400);
    ff->setPoissonRatio(0.3);
    ff->setMethod(1);

    node->addObject(dof);
    node->addObject(mass);
    node->addObject(grid);
    node->addObject(ff);

    const Deriv3 translation(x,y,z);

    //Node VISUAL
    Node* VisualNode = sofa::ObjectCreator::CreateVisualNodeVec3(dof,"mesh/ball.obj", "red", translation);
    node->addChild(VisualNode);

    node->setShowBehaviorModels(true);
    return node;
}


int main(int argc, char** argv)
{
    glutInit(&argc,argv);

    std::vector<std::string> files;

    sofa::helper::parse("This is a SOFA application. Here are the command line arguments")
//       .option(&simulationType,'s',"simulation","type of the simulation(bgl,tree)")
    (argc,argv);

    sofa::simulation::setSimulation(new sofa::simulation::bgl::BglSimulation());
    sofa::gui::GUIManager::Init(argv[0]);

    Node* solverNode = sofa::ObjectCreator::CreateEulerSolverNode("Solver");


    // The graph root node
    Node* root = sofa::ObjectCreator::CreateRootWithCollisionPipeline("bgl");
    root->setGravityInWorld( Coord3(0,0,0) );

    root->addChild(solverNode);

    Node* grid1 = createRegularGrid(-1.5,0,0);
    Node* grid2 = createRegularGrid(1.5,0,0);
    solverNode->addChild(grid1);
    solverNode->addChild(grid2);


    MechanicalObject3* subsetDof = new MechanicalObject3;
    SubsetMultiMappingVec3d_to_Vec3d* subsetMultiMapping = new SubsetMultiMappingVec3d_to_Vec3d();
    MechanicalObject3* input1 = dynamic_cast<MechanicalObject3*>(grid2->getMechanicalState());
    MechanicalObject3* input2 = dynamic_cast<MechanicalObject3*>(grid1->getMechanicalState());

    input1->f_printLog.setValue(true);
    input1->setName("input1");
    input2->f_printLog.setValue(true);
    input2->setName("input2");
    subsetDof->f_printLog.setValue(true);
    subsetDof->setName("subsetDof");

    subsetMultiMapping->addInputModel( input1 );
    subsetMultiMapping->addInputModel( input2 );
    subsetMultiMapping->addOutputModel( subsetDof );

    subsetMultiMapping->addPoint( input1, 21);
    subsetMultiMapping->addPoint( input1, 18);
    subsetMultiMapping->addPoint( input1, 9);
    subsetMultiMapping->addPoint( input1, 12);

    subsetMultiMapping->addPoint( input2, 11);
    subsetMultiMapping->addPoint( input2, 20);
    subsetMultiMapping->addPoint( input2, 14);
    subsetMultiMapping->addPoint( input2, 23);


    MeshTopology* topology = new MeshTopology;

    topology->addHexa(4,2,3,6,5,1,0,7);

    HexahedronFEMForceField3* ff = new HexahedronFEMForceField3();
    ff->setYoungModulus(400);
    ff->setPoissonRatio(0.3);
    ff->setMethod(1);


    Node* multiParentsNode = getSimulation()->newNode("MultiParents");
    multiParentsNode->addObject(topology);
    multiParentsNode->addObject(subsetDof);
    multiParentsNode->addObject(subsetMultiMapping);

    multiParentsNode->addObject(ff);

    multiParentsNode->setShowForceFields(true);


    grid1->addChild(multiParentsNode);
    grid2->addChild(multiParentsNode);
    root->setAnimate(false);

    getSimulation()->init(root);


    //=======================================
    // Run the main loop
    sofa::gui::GUIManager::MainLoop(root);

    return 0;
}
