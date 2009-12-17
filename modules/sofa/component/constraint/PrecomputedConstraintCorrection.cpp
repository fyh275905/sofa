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
#define SOFA_COMPONENT_CONSTRAINT_PRECOMPUTEDCONSTRAINTCORRECTION_CPP
#include "PrecomputedConstraintCorrection.inl"
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{
namespace component
{
namespace constraint
{

#ifndef SOFA_FLOAT

template<>
void PrecomputedConstraintCorrection<defaulttype::Vec3dTypes>::draw()
{
    if (!getContext()->getShowBehaviorModels() || !_rotations) return;

    // we draw the rotations associated to each node //

    simulation::Node *node = dynamic_cast<simulation::Node *>(getContext());

    sofa::component::forcefield::TetrahedronFEMForceField<defaulttype::Vec3dTypes>* forceField = NULL;
    sofa::component::container::RotationFinder<defaulttype::Vec3dTypes>* rotationFinder = NULL;

    if (node != NULL)
    {
        //		core::componentmodel::behavior::BaseForceField* _forceField = node->forceField[1];
        forceField = node->get<component::forcefield::TetrahedronFEMForceField<defaulttype::Vec3dTypes> > ();
        if (forceField == NULL)
        {
            rotationFinder = node->get<component::container::RotationFinder<defaulttype::Vec3dTypes> > ();
            if (rotationFinder == NULL)
            {
                sout << "No rotation defined : only defined for TetrahedronFEMForceField and RotationFinder!";
                return;
            }
        }
    }

    VecCoord& x = *mstate->getX();
    for (unsigned int i=0; i< x.size(); i++)
    {
        Transformation Ri;
        if (forceField != NULL)
        {
            forceField->getRotation(Ri, i);
        }
        else // rotationFinder has been defined
        {
            Ri = rotationFinder->getRotations()[i];
        }


        sofa::defaulttype::Matrix3 RotMat;

        for (unsigned int a=0; a<3; a++)
        {
            for (unsigned int b=0; b<3; b++)
            {
                RotMat[a][b] = Ri(a,b);
            }
        }

        sofa::defaulttype::Quat q;
        q.fromMatrix(RotMat);


        helper::gl::Axis::draw(x[i], q  , 10.0);

    }
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Rigid3dTypes>::rotateConstraints()
{
    VecCoord& x = *mstate->getX();
    VecConst& constraints = *mstate->getC();
    VecCoord& x0 = *mstate->getX0();

    unsigned int numConstraints = constraints.size();
    //	int sizemax=0;
    //	int index_const = -1;
    // on fait tourner les normales (en les ramenant dans le "pseudo" repere initial) //
    for(unsigned int curRowConst = 0; curRowConst < numConstraints; curRowConst++)
    {
        ConstraintIterator itConstraint;
        std::pair< ConstraintIterator, ConstraintIterator > iter=constraints[curRowConst].data();
        for (itConstraint=iter.first; itConstraint!=iter.second; itConstraint++)
        {
            unsigned int dof = itConstraint->first;
            Deriv& n = itConstraint->second;
            const int localRowNodeIdx = dof;
            Quat q;
            if (_restRotations)
                q = x[localRowNodeIdx].getOrientation() * x0[localRowNodeIdx].getOrientation().inverse();
            else
                q = x[localRowNodeIdx].getOrientation();


            Vec3d n_i = q.inverseRotate(n.getVCenter());
            Vec3d wn_i= q.inverseRotate(n.getVOrientation());

            // on passe les normales du repere global au repere local
            n.getVCenter() = n_i;
            n.getVOrientation() = wn_i;

        }
    }
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Vec1dTypes>::rotateConstraints()
{
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Rigid3dTypes>::rotateResponse()
{

    VecDeriv& dx = *mstate->getDx();
    VecCoord& x = *mstate->getX();
    VecCoord& x0 = *mstate->getX0();
    for(unsigned int j = 0; j < dx.size(); j++)
    {
        // on passe les deplacements du repere local (au repos) au repere global
        Deriv temp ;
        Quat q;
        if (_restRotations)
            q = x[j].getOrientation() * x0[j].getOrientation().inverse();
        else
            q = x[j].getOrientation();

        temp.getVCenter()		= q.rotate(dx[j].getVCenter());
        temp.getVOrientation()  = q.rotate(dx[j].getVOrientation());
        dx[j] = temp;
    }
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Vec1dTypes>::rotateResponse()
{
}


#endif
#ifndef SOFA_DOUBLE


template<>
void PrecomputedConstraintCorrection<defaulttype::Vec3fTypes>::rotateConstraints()
{
    VecConst& constraints = *mstate->getC();
    unsigned int numConstraints = constraints.size();

    simulation::Node *node = dynamic_cast<simulation::Node *>(getContext());

    sofa::component::forcefield::TetrahedronFEMForceField<defaulttype::Vec3fTypes>* forceField = NULL;
    sofa::component::container::RotationFinder<defaulttype::Vec3fTypes>* rotationFinder = NULL;

    if (node != NULL)
    {
        forceField = node->get<component::forcefield::TetrahedronFEMForceField<defaulttype::Vec3fTypes> > ();
        if (forceField == NULL)
        {
            rotationFinder = node->get<component::container::RotationFinder<defaulttype::Vec3fTypes> > ();
            if (rotationFinder == NULL)
            {
                sout << "No rotation defined : only defined for TetrahedronFEMForceField and RotationFinder!";
                return;
            }
        }
    }
    else
    {
        sout << "Error getting context in method: PrecomputedConstraintCorrection<defaulttype::Vec3dTypes>::rotateConstraints()";
        return;
    }

    //sout << "start rotating normals " << g_timer_elapsed(timer, &micro) << sendl;
    //	int sizemax=0;
    //	int index_const = -1;
    // on fait tourner les normales (en les ramenant dans le "pseudo" repere initial) //
    for(unsigned int curRowConst = 0; curRowConst < numConstraints; curRowConst++)
    {
        ConstraintIterator itConstraint;

        std::pair< ConstraintIterator, ConstraintIterator > iter=constraints[curRowConst].data();
        for (itConstraint=iter.first; itConstraint!=iter.second; itConstraint++)
        {
            unsigned int dof = itConstraint->first;
            Deriv& n = itConstraint->second;
            const int localRowNodeIdx = dof;
            Transformation Ri;
            if (forceField != NULL)
            {
                forceField->getRotation(Ri, localRowNodeIdx);
            }
            else // rotationFinder has been defined
            {
                Ri = rotationFinder->getRotations()[localRowNodeIdx];
            }
            Ri.transpose();
            // on passe les normales du repere global au repere local
            Deriv n_i = Ri * n;
            n.x() =  n_i.x();
            n.y() =  n_i.y();
            n.z() =  n_i.z();
        }
        /*
        // test pour voir si on peut reduire le nombre de contrainte
        if (sizeCurRowConst > sizemax)
        {
        sizemax = sizeCurRowConst;
        index_const = curRowConst;
        }
        */
    }
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Rigid3fTypes>::rotateConstraints()
{
    VecCoord& x = *mstate->getX();
    VecConst& constraints = *mstate->getC();
    VecCoord& x0 = *mstate->getX0();

    unsigned int numConstraints = constraints.size();
    //	int sizemax=0;
    //	int index_const = -1;
    // on fait tourner les normales (en les ramenant dans le "pseudo" repere initial) //
    for(unsigned int curRowConst = 0; curRowConst < numConstraints; curRowConst++)
    {
        ConstraintIterator itConstraint;

        std::pair< ConstraintIterator, ConstraintIterator > iter=constraints[curRowConst].data();
        for (itConstraint=iter.first; itConstraint!=iter.second; itConstraint++)
        {
            unsigned int dof = itConstraint->first;
            Deriv& n = itConstraint->second;
            const int localRowNodeIdx = dof;
            Quat q;
            if (_restRotations)
                q = x[localRowNodeIdx].getOrientation() * x0[localRowNodeIdx].getOrientation().inverse();
            else
                q = x[localRowNodeIdx].getOrientation();


            Vec3d n_i = n.getVCenter();
            Vec3d wn_i= n.getVOrientation();

            // on passe les normales du repere global au repere local
            n.getVCenter() = n_i;
            n.getVOrientation() = wn_i;

        }
    }
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Vec1fTypes>::rotateConstraints()
{
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Vec3fTypes>::rotateResponse()
{
    simulation::Node *node = dynamic_cast<simulation::Node *>(getContext());

    sofa::component::forcefield::TetrahedronFEMForceField<defaulttype::Vec3fTypes>* forceField = NULL;
    sofa::component::container::RotationFinder<defaulttype::Vec3fTypes>* rotationFinder = NULL;

    if (node != NULL)
    {
        //		core::componentmodel::behavior::BaseForceField* _forceField = node->forceField[1];
        forceField = node->get<component::forcefield::TetrahedronFEMForceField<defaulttype::Vec3fTypes> > ();
        if (forceField == NULL)
        {
            rotationFinder = node->get<component::container::RotationFinder<defaulttype::Vec3fTypes> > ();
            if (rotationFinder == NULL)
            {
                sout << "No rotation defined : only defined for TetrahedronFEMForceField and RotationFinder!";
                return;
            }
        }
    }
    else
    {
        sout << "Error getting context in method: PrecomputedConstraintCorrection<defaulttype::Vec3dTypes>::rotateConstraints()";
        return;
    }

    VecDeriv& dx = *mstate->getDx();
    for(unsigned int j = 0; j < dx.size(); j++)
    {
        Transformation Rj;
        if (forceField != NULL)
        {
            forceField->getRotation(Rj, j);
        }
        else // rotationFinder has been defined
        {
            Rj = rotationFinder->getRotations()[j];
        }
        // on passe les deplacements du repere local au repere global
        const Deriv& temp = Rj * dx[j];
        dx[j] = temp;
    }
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Rigid3fTypes>::rotateResponse()
{

    VecDeriv& dx = *mstate->getDx();
    VecCoord& x = *mstate->getX();
    VecCoord& x0 = *mstate->getX0();
    for(unsigned int j = 0; j < dx.size(); j++)
    {
        // on passe les deplacements du repere local (au repos) au repere global
        Deriv temp ;
        Quat q;
        if (_restRotations)
            q = x[j].getOrientation() * x0[j].getOrientation().inverse();
        else
            q = x[j].getOrientation();

        temp.getVCenter()		= q.rotate(dx[j].getVCenter());
        temp.getVOrientation()  = q.rotate(dx[j].getVOrientation());
        dx[j] = temp;
    }
}



template<>
void PrecomputedConstraintCorrection<defaulttype::Vec1fTypes>::rotateResponse()
{
}


#endif

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(PrecomputedConstraintCorrection)

int ContactCorrectionClass = core::RegisterObject("Component computing contact forces within a simulated body using the compliance method.")
#ifndef SOFA_FLOAT
        .add< PrecomputedConstraintCorrection<Vec3dTypes> >()
//     .add< PrecomputedConstraintCorrection<Vec2dTypes> >()
        .add< PrecomputedConstraintCorrection<Vec1dTypes> >()
        .add< PrecomputedConstraintCorrection<Rigid3dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< PrecomputedConstraintCorrection<Vec3fTypes> >()
//     .add< PrecomputedConstraintCorrection<Vec2fTypes> >()
        .add< PrecomputedConstraintCorrection<Vec1fTypes> >()
        .add< PrecomputedConstraintCorrection<Rigid3fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Vec3dTypes>;
//template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Vec2dTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Vec1dTypes>;
//template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Vec6dTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Rigid3dTypes>;
//template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Rigid2dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Vec3fTypes>;
//template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Vec2fTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Vec1fTypes>;
//template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Vec6fTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Rigid3fTypes>;
//template class SOFA_COMPONENT_CONSTRAINT_API PrecomputedConstraintCorrection<Rigid2fTypes>;
#endif

} // namespace collision

} // namespace component

} // namespace sofa
