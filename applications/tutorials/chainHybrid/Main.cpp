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
#include <sofa/helper/ArgumentParser.h>
#include <sofa/simulation/tree/Simulation.h>
#include <sofa/component/contextobject/CoordinateSystem.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/core/objectmodel/Context.h>
#include <sofa/gui/SofaGUI.h>

//Including components for collision detection
#include <sofa/component/collision/DefaultPipeline.h>
#include <sofa/component/collision/DefaultContactManager.h>
#include <sofa/component/collision/DefaultCollisionGroupManager.h>
#include <sofa/component/collision/BruteForceDetection.h>
#include <sofa/component/collision/NewProximityIntersection.h>
#include <sofa/component/collision/TriangleModel.h>

//Including component for topological description of the objects
#include <sofa/component/topology/MeshTopology.h>
#include <sofa/component/topology/RegularGridTopology.h>
#include <sofa/component/MeshLoader.h>

//Including Solvers
#include <sofa/component/odesolver/EulerImplicitSolver.h>
#include <sofa/component/linearsolver/CGLinearSolver.h>
#include <sofa/simulation/common/MatrixLinearSolver.h>

#include <sofa/component/visualmodel/OglModel.h>

#include <sofa/simulation/common/TransformationVisitor.h>
#include <sofa/helper/system/glut.h>

using sofa::component::visualmodel::OglModel;

using namespace sofa::simulation::tree;
using namespace sofa::component::collision;
using namespace sofa::component::topology;
using sofa::component::MeshLoader;
using sofa::component::odesolver::EulerImplicitSolver;
using sofa::component::linearsolver::CGLinearSolver;
using sofa::simulation::GraphScatteredMatrix;
using sofa::simulation::GraphScatteredVector;
typedef CGLinearSolver<GraphScatteredMatrix,GraphScatteredVector> CGLinearSolverGraph;

//Using double by default, if you have SOFA_FLOAT in use in you sofa-default.cfg, then it will be FLOAT.
#include <sofa/component/typedef/Sofa_typedef.h>
// ---------------------------------------------------------------------
// ---
// ---------------------------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    sofa::helper::parse("This is a SOFA application.")
    (argc,argv);
    sofa::gui::SofaGUI::Init(argv[0]);

    // The graph root node
    GNode* groot = new GNode;
    groot->setName( "root" );
    groot->setGravityInWorld( Coord3(0,0,-10) );


    //Components for collision management
    //------------------------------------
    //--> adding collision pipeline
    DefaultPipeline* collisionPipeline = new DefaultPipeline;
    collisionPipeline->setName("Collision Pipeline");
    groot->addObject(collisionPipeline);

    //--> adding collision detection system
    BruteForceDetection* detection = new BruteForceDetection;
    detection->setName("Detection");
    groot->addObject(detection);

    //--> adding component to detection intersection of elements
    NewProximityIntersection* detectionProximity = new NewProximityIntersection;
    detectionProximity->setName("Proximity");
    detectionProximity->setAlarmDistance(0.3);   //warning distance
    detectionProximity->setContactDistance(0.2); //min distance before setting a spring to create a repulsion
    groot->addObject(detectionProximity);


    //--> adding contact manager
    DefaultContactManager* contactManager = new DefaultContactManager;
    contactManager->setName("Contact Manager");
    groot->addObject(contactManager);

    //--> adding component to handle groups of collision.
    DefaultCollisionGroupManager* collisionGroupManager = new DefaultCollisionGroupManager;
    collisionGroupManager->setName("Collision Group Manager");
    groot->addObject(collisionGroupManager);



    //Elements of the scene
    //------------------------------------
    GNode* chain = new GNode;
    chain->setName("Chain");
    groot->addChild(chain);

    //************************************
    //Torus Fixed
    GNode* torusFixed = new GNode;
    torusFixed->setName("Fixed");
    chain->addChild(torusFixed);

    MeshLoader* loaderFixed = new MeshLoader;
    loaderFixed->load(sofa::helper::system::DataRepository.getFile("mesh/torus_for_collision.obj").c_str());
    torusFixed->addObject(loaderFixed);

    MeshTopology* meshTorusFixed = new MeshTopology;
    torusFixed->addObject(meshTorusFixed);

    MechanicalObject3* dofFixed = new MechanicalObject3; dofFixed->setName("Fixed Object");
    torusFixed->addObject(dofFixed);

    TriangleModel* triangleFixed = new TriangleModel; triangleFixed->setName("Collision Fixed");
    triangleFixed->setSimulated(false); //Not simulated, fixed object
    triangleFixed->setMoving(false);    //No extern events
    torusFixed->addObject(triangleFixed);

    OglModel* visualFixed = new OglModel;
    visualFixed->setName("visual");
    visualFixed->load(sofa::helper::system::DataRepository.getFile("mesh/torus.obj"),"","");
    visualFixed->setColor("gray");
    torusFixed->addObject(visualFixed);


    //************************************
    //Torus FEM
    GNode* torusFEM = new GNode;
    torusFEM->setName("FEM");
    chain->addChild(torusFEM);

    EulerImplicitSolver* solverFEM = new EulerImplicitSolver;
    CGLinearSolverGraph* linearFEM = new CGLinearSolverGraph;
    solverFEM->setName("Euler Implicit");
    solverFEM->f_rayleighStiffness.setValue(0.01);
    solverFEM->f_rayleighMass.setValue(1);

    solverFEM->setName("Conjugate Gradient");
    linearFEM->f_maxIter.setValue(20); //iteration maxi for the CG
    linearFEM->f_smallDenominatorThreshold.setValue(0.000001);
    linearFEM->f_tolerance.setValue(0.001);

    torusFEM->addObject(solverFEM);
    torusFEM->addObject(linearFEM);


    MeshLoader* loaderFEM = new MeshLoader;
    loaderFEM->load(sofa::helper::system::DataRepository.getFile("mesh/torus_low_res.msh").c_str());
    torusFEM->addObject(loaderFEM);

    MeshTopology* meshTorusFEM = new MeshTopology;
    torusFEM->addObject(meshTorusFEM);

    MechanicalObject3* dofFEM = new MechanicalObject3; dofFEM->setName("FEM Object");
    torusFEM->addObject(dofFEM);

    UniformMass3* uniMassFEM = new UniformMass3;
    uniMassFEM->setTotalMass(5); //the whole object will have 5 as given mass
    torusFEM->addObject(uniMassFEM);

    TetrahedronFEMForceField3* tetraFEMFF = new TetrahedronFEMForceField3;
    tetraFEMFF->setName("FEM");
    tetraFEMFF->setComputeGlobalMatrix(false);
    tetraFEMFF->setMethod("large");
    tetraFEMFF->setPoissonRatio(0.3);
    tetraFEMFF->setYoungModulus(1000);
    torusFEM->addObject(tetraFEMFF);

    //Node VISUAL
    GNode* FEMVisualNode = new GNode;
    FEMVisualNode->setName("Visu");
    torusFEM->addChild(FEMVisualNode);

    OglModel* visualFEM = new OglModel;
    visualFEM->setName("visual");
    visualFEM->load(sofa::helper::system::DataRepository.getFile("mesh/torus.obj"),"","");
    visualFEM->setColor("red");
    FEMVisualNode->addObject(visualFEM);

    BarycentricMapping3_to_Ext3* mappingFEM = new BarycentricMapping3_to_Ext3(dofFEM, visualFEM);
    mappingFEM->setName("Mapping Visual");
    FEMVisualNode->addObject(mappingFEM);


    //Node COLLISION
    GNode* FEMCollisionNode = new GNode;
    FEMCollisionNode->setName("Collision");
    torusFEM->addChild(FEMCollisionNode);

    MeshLoader* loaderFEM_surf = new MeshLoader;
    loaderFEM_surf->load(sofa::helper::system::DataRepository.getFile("mesh/torus_for_collision.obj").c_str());
    FEMCollisionNode->addObject(loaderFEM_surf);

    MeshTopology* meshTorusFEM_surf= new MeshTopology;
    FEMCollisionNode->addObject(meshTorusFEM_surf);

    MechanicalObject3* dofFEM_surf = new MechanicalObject3;  dofFEM_surf->setName("Collision Object FEM");
    FEMCollisionNode->addObject(dofFEM_surf);

    TriangleModel* triangleFEM = new TriangleModel; triangleFEM->setName("TriangleCollision FEM");
    FEMCollisionNode->addObject(triangleFEM);

    BarycentricMechanicalMapping3_to_3* mechaMappingFEM = new BarycentricMechanicalMapping3_to_3(dofFEM, dofFEM_surf);
    FEMCollisionNode->addObject(mechaMappingFEM);


    //************************************
    //Torus Spring
    GNode* torusSpring = new GNode;
    torusSpring->setName("Spring");
    chain->addChild(torusSpring);

    EulerImplicitSolver* solverSpring = new EulerImplicitSolver;
    CGLinearSolverGraph* linearSpring = new CGLinearSolverGraph;
    solverSpring->setName("Euler Implicit");
    solverSpring->f_rayleighStiffness.setValue(0.01);
    solverSpring->f_rayleighMass.setValue(1);

    linearSpring->setName("Conjugate Gradient");
    linearSpring->f_maxIter.setValue(20); //iteration maxi for the CG
    linearSpring->f_smallDenominatorThreshold.setValue(0.000001);
    linearSpring->f_tolerance.setValue(0.001);

    torusSpring->addObject(solverSpring);
    torusSpring->addObject(linearSpring);

    MeshLoader* loaderSpring = new MeshLoader;
    loaderSpring->load(sofa::helper::system::DataRepository.getFile("mesh/torus_low_res.msh").c_str());
    torusSpring->addObject(loaderSpring);
    loaderSpring->init();

    MeshTopology* meshTorusSpring = new MeshTopology;
    torusSpring->addObject(meshTorusSpring);

    MechanicalObject3* dofSpring = new MechanicalObject3; dofSpring->setName("Spring Object");
    torusSpring->addObject(dofSpring);

    UniformMass3* uniMassSpring = new UniformMass3;
    uniMassSpring->setTotalMass(5); //the whole object will have 5 as given mass
    torusSpring->addObject(uniMassSpring);

    MeshSpringForceField3* springFF = new MeshSpringForceField3;
    springFF->setName("Springs");
    springFF->setStiffness(400);
    springFF->setDamping(0);
    torusSpring->addObject(springFF);

    //Node VISUAL
    GNode* SpringVisualNode = new GNode;
    SpringVisualNode->setName("Visu");
    torusSpring->addChild(SpringVisualNode);

    OglModel* visualSpring = new OglModel;
    visualSpring->setName("visual");
    visualSpring->load(sofa::helper::system::DataRepository.getFile("mesh/torus.obj"),"","");
    visualSpring->setColor("green");
    SpringVisualNode->addObject(visualSpring);

    BarycentricMapping3_to_Ext3* mappingSpring = new BarycentricMapping3_to_Ext3(dofSpring, visualSpring);
    mappingSpring->setName("Mapping Visual");
    SpringVisualNode->addObject(mappingSpring);


    //Node COLLISION
    GNode* SpringCollisionNode = new GNode;
    SpringCollisionNode->setName("Collision");
    torusSpring->addChild(SpringCollisionNode);

    MeshLoader* loaderSpring_surf = new MeshLoader;
    loaderSpring_surf->load(sofa::helper::system::DataRepository.getFile("mesh/torus_for_collision.obj").c_str());
    SpringCollisionNode->addObject(loaderSpring_surf);

    MeshTopology* meshTorusSpring_surf= new MeshTopology;
    SpringCollisionNode->addObject(meshTorusSpring_surf);

    MechanicalObject3* dofSpring_surf = new MechanicalObject3; dofSpring_surf->setName("Collision Object Spring");
    SpringCollisionNode->addObject(dofSpring_surf);

    TriangleModel* triangleSpring = new TriangleModel; triangleSpring->setName("TriangleCollision Spring");
    SpringCollisionNode->addObject(triangleSpring);

    BarycentricMechanicalMapping3_to_3* mechaMappingSpring = new BarycentricMechanicalMapping3_to_3(dofSpring, dofSpring_surf);
    SpringCollisionNode->addObject(mechaMappingSpring);


    //************************************
    //Torus FFD
    GNode* torusFFD = new GNode;
    torusFFD->setName("FFD");
    chain->addChild(torusFFD);

    EulerImplicitSolver* solverFFD = new EulerImplicitSolver;
    CGLinearSolverGraph* linearFFD = new CGLinearSolverGraph;
    solverFFD->setName("Euler Implicit");
    solverFFD->f_rayleighStiffness.setValue(0.01);
    solverFFD->f_rayleighMass.setValue(1);

    linearFFD->setName("Conjugate Gradient");
    linearFFD->f_maxIter.setValue(20); //iteration maxi for the CG
    linearFFD->f_smallDenominatorThreshold.setValue(0.000001);
    linearFFD->f_tolerance.setValue(0.001);

    torusFFD->addObject(solverFFD);
    torusFFD->addObject(linearFFD);

    MechanicalObject3* dofFFD = new MechanicalObject3; dofFFD->setName("FFD Object");
    torusFFD->addObject(dofFFD);

    UniformMass3* uniMassFFD = new UniformMass3;
    uniMassFFD->setTotalMass(5); //the whole object will have 5 as given mass
    torusFFD->addObject(uniMassFFD);

    RegularGridTopology* gridTopo = new RegularGridTopology(6,2,5); //dimension of the grid
    gridTopo->setPos(
        -2.5,2.5,  //Xmin, Xmax
        -0.5,0.5,  //Ymin, Ymax
        -2,2       //Zmin, Zmax
    );
    torusFFD->addObject(gridTopo);

    RegularGridSpringForceField3* FFDFF = new RegularGridSpringForceField3;
    FFDFF->setName("Springs FFD");
    FFDFF->setStiffness(200);
    FFDFF->setDamping(0);
    torusFFD->addObject(FFDFF);

    //Node VISUAL
    GNode* FFDVisualNode = new GNode;
    FFDVisualNode->setName("Visu");
    torusFFD->addChild(FFDVisualNode);

    OglModel* visualFFD = new OglModel;
    visualFFD->setName("visual");
    visualFFD->load(sofa::helper::system::DataRepository.getFile("mesh/torus.obj"),"","");
    visualFFD->setColor("yellow");
    FFDVisualNode->addObject(visualFFD);

    BarycentricMapping3_to_Ext3* mappingFFD = new BarycentricMapping3_to_Ext3(dofFFD, visualFFD);
    mappingFFD->setName("Mapping Visual");
    FFDVisualNode->addObject(mappingFFD);


    //Node COLLISION
    GNode* FFDCollisionNode = new GNode;
    FFDCollisionNode->setName("Collision");
    torusFFD->addChild(FFDCollisionNode);


    MeshLoader* loaderFFD_surf = new MeshLoader;
    loaderFFD_surf->load(sofa::helper::system::DataRepository.getFile("mesh/torus_for_collision.obj").c_str());
    FFDCollisionNode->addObject(loaderFFD_surf);

    MeshTopology* meshTorusFFD_surf= new MeshTopology;
    FFDCollisionNode->addObject(meshTorusFFD_surf);

    MechanicalObject3* dofFFD_surf = new MechanicalObject3; dofFFD_surf->setName("Collision Object FFD");
    FFDCollisionNode->addObject(dofFFD_surf);

    TriangleModel* triangleFFD = new TriangleModel;  triangleFFD->setName("TriangleCollision FFD");
    FFDCollisionNode->addObject(triangleFFD);

    BarycentricMechanicalMapping3_to_3* mechaMappingFFD = new BarycentricMechanicalMapping3_to_3(dofFFD, dofFFD_surf);
    FFDCollisionNode->addObject(mechaMappingFFD);

    //************************************
    //Torus Rigid
    GNode* torusRigid = new GNode;
    torusRigid->setName("Rigid");
    chain->addChild(torusRigid);


    EulerImplicitSolver* solverRigid = new EulerImplicitSolver;
    CGLinearSolverGraph* linearRigid = new CGLinearSolverGraph;
    solverRigid->setName("Euler Implicit");
    solverRigid->f_rayleighStiffness.setValue(0.01);
    solverRigid->f_rayleighMass.setValue(1);

    solverRigid->setName("Conjugate Gradient");
    linearRigid->f_maxIter.setValue(20); //iteration maxi for the CG
    linearRigid->f_smallDenominatorThreshold.setValue(0.000001);
    linearRigid->f_tolerance.setValue(0.001);

    torusRigid->addObject(solverRigid);
    torusRigid->addObject(linearRigid);

    MechanicalObjectRigid3* dofRigid = new MechanicalObjectRigid3; dofRigid->setName("Rigid Object");
    torusRigid->addObject(dofRigid);

    UniformMassRigid3* uniMassRigid = new UniformMassRigid3;
    uniMassRigid->setTotalMass(1); //the whole object will have 5 as given mass
    torusRigid->addObject(uniMassRigid);

    //Node VISUAL
    GNode* RigidVisualNode = new GNode;
    RigidVisualNode->setName("Visu");
    torusRigid->addChild(RigidVisualNode);

    OglModel* visualRigid = new OglModel;
    visualRigid->setName("visual");
    visualRigid->load(sofa::helper::system::DataRepository.getFile("mesh/torus.obj"),"","");
    visualRigid->setColor("gray");
    RigidVisualNode->addObject(visualRigid);

    RigidMappingRigid3_to_Ext3* mappingRigid = new RigidMappingRigid3_to_Ext3(dofRigid, visualRigid);
    mappingRigid->setName("Mapping Visual");
    RigidVisualNode->addObject(mappingRigid);


    //Node COLLISION
    GNode* RigidCollisionNode = new GNode;
    RigidCollisionNode->setName("Collision");
    torusRigid->addChild(RigidCollisionNode);

    MeshLoader* loaderRigid_surf = new MeshLoader;
    loaderRigid_surf->load(sofa::helper::system::DataRepository.getFile("mesh/torus_for_collision.obj").c_str());
    RigidCollisionNode->addObject(loaderRigid_surf);

    MeshTopology* meshTorusRigid_surf= new MeshTopology;
    RigidCollisionNode->addObject(meshTorusRigid_surf);

    MechanicalObject3* dofRigid_surf = new MechanicalObject3; dofRigid_surf->setName("Collision Object Rigid");
    RigidCollisionNode->addObject(dofRigid_surf);

    TriangleModel* triangleRigid = new TriangleModel;  triangleRigid->setName("TriangleCollision Rigid");
    RigidCollisionNode->addObject(triangleRigid);

    RigidMechanicalMappingRigid3_to_3* mechaMappingRigid = new RigidMechanicalMappingRigid3_to_3(dofRigid, dofRigid_surf);
    RigidCollisionNode->addObject(mechaMappingRigid);









    getSimulation()->init(groot);

    //Set the initial position: must be done AFTER the initialization of the scene
    sofa::simulation::TransformationVisitor transform;
    transform.setTranslation(2.5,0.0,0.0);
    transform.setRotation(90,0,0);
    transform.execute(torusFEM);

    transform.setTranslation(5.0,0.0,0.0);
    transform.setRotation(0,0,0);
    transform.execute(torusSpring);

    transform.setTranslation(7.5,0.0,0.0);
    transform.setRotation(90,0,0);
    transform.execute(torusFFD);

    transform.setTranslation(10.0,0.0,0.0);
    transform.setRotation(00,0,0);
    transform.execute(torusRigid);


    groot->setAnimate(false);

    //=======================================
    // Run the main loop
    sofa::gui::SofaGUI::MainLoop(groot);

    return 0;
}
