/******************************************************************************
 *       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
 *                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
 *                                                                             *
 * This library is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU Lesser General Public License as published by    *
 * the Free Software Foundation; either version 2.1 of the License, or (at     *
 * your option) any later version.                                             *
 *                                                                             *
 * This library is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
 * for more details.                                                           *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this library; if not, write to the Free Software Foundation,     *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
 *******************************************************************************
 *                               SOFA :: Modules                               *
 *                                                                             *
 * Authors: The SOFA Team and external contributors (see Authors.txt)          *
 *                                                                             *
 * Contact information: contact@sofa-framework.org                             *
 ******************************************************************************/

#include <sofa/component/collision/FixParticlePerformer.h>
#include <sofa/component/constraint/FixedConstraint.h>

#include <sofa/simulation/common/Simulation.h>

#include <sofa/simulation/common/InitVisitor.h>
#include <sofa/simulation/common/DeleteVisitor.h>

#include <sofa/component/collision/SphereModel.h>
#include <sofa/component/collision/TriangleModel.h>

namespace sofa
{

namespace component
{

namespace collision
{

template <class DataTypes>
void FixParticlePerformer<DataTypes>::start()
{
    const BodyPicked &picked=this->interactor->getBodyPicked();

    helper::vector<unsigned int > points;
    typename DataTypes::Coord fixPoint;
    MouseContainer* mstateCollision=getFixationPoints(picked, points, fixPoint);

    if (!mstateCollision || points.empty())
    {
        std::cerr << "Model not supported!" << std::endl;
        return;
    }


    simulation::Node* nodeCollision = static_cast<simulation::Node*>(mstateCollision->getContext());
    simulation::Node* nodeFixation = simulation::getSimulation()->newNode("FixationPoint");
    fixations.push_back( nodeFixation );

    //Create the Container of points
    MouseContainer* mstateFixation = new MouseContainer();
    mstateFixation->setIgnoreLoader(true);

    mstateFixation->resize(1);
    (*mstateFixation->getX())[0] = fixPoint;
    nodeFixation->addObject(mstateFixation);


    //Fix all the points
    constraint::FixedConstraint<DataTypes> *fixFixation = new constraint::FixedConstraint<DataTypes>();
    fixFixation->f_fixAll.setValue(true);
    nodeFixation->addObject(fixFixation);

    //Add Interaction ForceField
    MouseForceField *distanceForceField = new MouseForceField(mstateFixation, mstateCollision);
    const double friction=0.0;
    const double coeffStiffness=1/(double)points.size();
    for (unsigned int i=0; i<points.size(); ++i)
        distanceForceField->addSpring(0,points[i], stiffness*coeffStiffness, friction, 0);
    nodeFixation->addObject(distanceForceField);

    //Add the nodes
    nodeCollision->addChild(nodeFixation);
    nodeFixation->updateContext();
    nodeFixation->execute<simulation::InitVisitor>();
}

template <class DataTypes>
void FixParticlePerformer<DataTypes>::execute()
{
};




template <class DataTypes>
void FixParticlePerformer<DataTypes>::draw()
{
    for (unsigned int i=0; i<fixations.size(); ++i)
    {
        bool b = fixations[i]->getContext()->getShowBehaviorModels();
        fixations[i]->getContext()->setShowBehaviorModels(true);
        simulation::getSimulation()->draw(fixations[i]);
        fixations[i]->getContext()->setShowBehaviorModels(b);
    }
}


template <class DataTypes>
FixParticlePerformer<DataTypes>::FixParticlePerformer(BaseMouseInteractor *i):TInteractionPerformer<DataTypes>(i)
{
}


template <class DataTypes>
sofa::component::container::MechanicalObject< DataTypes >* FixParticlePerformer<DataTypes>::getFixationPoints(const BodyPicked &b, helper::vector<unsigned int> &points, typename DataTypes::Coord &fixPoint)
{
    const int idx=b.indexCollisionElement;
    MouseContainer* collisionState=0;

    if (b.body)
    {
        collisionState = dynamic_cast<MouseContainer*>(b.body->getContext()->getMechanicalState());

        if (SphereModel *sphere = dynamic_cast<SphereModel*>(b.body))
        {
            Sphere s(sphere, idx);
            fixPoint = s.p();
            points.push_back(s.getIndex());
        }
        else if(TriangleModel *triangle = dynamic_cast<TriangleModel*>(b.body))
        {
            Triangle t(triangle, idx);
            fixPoint = (t.p1()+t.p2()+t.p3())/3.0;
            points.push_back(t.p1Index());
            points.push_back(t.p2Index());
            points.push_back(t.p3Index());
        }

    }
    else if (b.mstate)
    {
        collisionState = dynamic_cast<MouseContainer*>(b.mstate);
        fixPoint = (*(collisionState->getX()))[idx];
        points.push_back(idx);
    }

    return collisionState;
}

}
}
}
