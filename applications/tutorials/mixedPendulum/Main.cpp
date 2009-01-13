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
/** A sample program. Laure Heigeas, Francois Faure, 2007. */
// scene data structure
#include <sofa/simulation/tree/Simulation.h>
#include <sofa/component/contextobject/Gravity.h>
#include <sofa/component/odesolver/CGImplicitSolver.h>
#include <sofa/component/odesolver/EulerSolver.h>
#include <sofa/component/odesolver/StaticSolver.h>
#include <sofa/component/visualmodel/OglModel.h>
// gui
#include <sofa/gui/SofaGUI.h>

#include <sofa/component/typedef/Sofa_typedef.h>

using namespace sofa::simulation::tree;
//typedef sofa::component::odesolver::EulerSolver Solver;
typedef sofa::component::odesolver::CGImplicitSolver Solver;

int main(int, char** argv)
{
    sofa::gui::SofaGUI::Init(argv[0]);
    //=========================== Build the scene
    double endPos = 1.;
    double attach = -1.;
    double splength = 1.;

    //-------------------- The graph root node
    GNode* groot = new GNode;
    groot->setName( "root" );
    groot->setGravityInWorld( Coord3(0,-10,0) );

    // One solver for all the graph
    Solver* solver = new Solver;
    groot->addObject(solver);
    solver->setName("S");

    //-------------------- Deformable body
    GNode* deformableBody = new GNode;
    groot->addChild(deformableBody);
    deformableBody->setName( "deformableBody" );

    // degrees of freedom
    MechanicalObject3* DOF = new MechanicalObject3;
    deformableBody->addObject(DOF);
    DOF->resize(2);
    DOF->setName("Dof1");
    VecCoord3& x = *DOF->getX();
    x[0] = Coord3(0,0,0);
    x[1] = Coord3(endPos,0,0);

    // mass
    //    ParticleMasses* mass = new ParticleMasses;
    UniformMass3* mass = new UniformMass3;
    deformableBody->addObject(mass);
    mass->setMass(1);
    mass->setName("M1");

    // Fixed point
    FixedConstraint3* constraints = new FixedConstraint3;
    deformableBody->addObject(constraints);
    constraints->setName("C");
    constraints->addConstraint(0);


    // force field
    StiffSpringForceField3* spring = new StiffSpringForceField3;
    deformableBody->addObject(spring);
    spring->setName("F1");
    spring->addSpring( 1,0, 100., 1, splength );


    //-------------------- Rigid body
    GNode* rigidBody = new GNode;
    groot->addChild(rigidBody);
    rigidBody->setName( "rigidBody" );

    // degrees of freedom
    MechanicalObjectRigid3* rigidDOF = new MechanicalObjectRigid3;
    rigidBody->addObject(rigidDOF);
    rigidDOF->resize(1);
    rigidDOF->setName("Dof2");
    VecCoordRigid3& rigid_x = *rigidDOF->getX();
    rigid_x[0] = CoordRigid3( Coord3(endPos-attach+splength,0,0),
            Quat3::identity() );

    // mass
    UniformMassRigid3* rigidMass = new UniformMassRigid3;
    rigidBody->addObject(rigidMass);
    rigidMass->setName("M2");
    UniformMassRigid3::MassType* m = rigidMass->mass.beginEdit();
    m->mass=0.3;
    UniformMassRigid3::MassType::Mat3x3 inertia;
    inertia.fill(0.0);
    float in = 0.1f;
    inertia[0][0] = in;
    inertia[1][1] = in;
    inertia[2][2] = in;
    m->inertiaMatrix = inertia;
    m->recalc();
    rigidMass->mass.endEdit();


    //-------------------- the particles attached to the rigid body
    GNode* rigidParticles = new GNode;
    rigidParticles->setName( "rigidParticles" );
    rigidBody->addChild(rigidParticles);

    // degrees of freedom of the skin
    MechanicalObject3* rigidParticleDOF = new MechanicalObject3;
    rigidParticles->addObject(rigidParticleDOF);
    rigidParticleDOF->resize(1);
    rigidParticleDOF->setName("Dof3");
    VecCoord3& rp_x = *rigidParticleDOF->getX();
    rp_x[0] = Coord3(attach,0,0);

    // mapping from the rigid body DOF to the skin DOF, to rigidly attach the skin to the body
    RigidMechanicalMappingRigid3_to_3* rigidMapping = new RigidMechanicalMappingRigid3_to_3(rigidDOF,rigidParticleDOF);
    rigidParticles->addObject( rigidMapping );
    rigidMapping->setName("Map23");


    // ---------------- Interaction force between the deformable and the rigid body
    StiffSpringForceField3* iff = new StiffSpringForceField3( DOF, rigidParticleDOF );
    groot->addObject(iff);
    iff->setName("F13");
    iff->addSpring( 1,0, 100., 1., splength );




    //=========================== Init the scene
    getSimulation()->init(groot);
    groot->setAnimate(false);
    groot->setShowNormals(false);
    groot->setShowInteractionForceFields(true);
    groot->setShowMechanicalMappings(true);
    groot->setShowCollisionModels(false);
    groot->setShowBoundingCollisionModels(false);
    groot->setShowMappings(false);
    groot->setShowForceFields(true);
    groot->setShowWireFrame(false);
    groot->setShowVisualModels(true);
    groot->setShowBehaviorModels(true);



    //=========================== Run the main loop

    sofa::gui::SofaGUI::MainLoop(groot);
}

