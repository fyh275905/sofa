/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2018 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_PARTIALLINEARMOVEMENTCONSTRAINT_INL
#define SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_PARTIALLINEARMOVEMENTCONSTRAINT_INL

#include <SofaBoundaryCondition/PartialLinearMovementConstraint.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/simulation/Simulation.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/RGBAColor.h>
#include <iostream>
#include <SofaBaseTopology/TopologySubsetData.inl>



namespace sofa
{

namespace component
{

namespace projectiveconstraintset
{


// Define TestNewPointFunction
template< class DataTypes>
bool PartialLinearMovementConstraint<DataTypes>::FCPointHandler::applyTestCreateFunction(unsigned int, const sofa::helper::vector<unsigned int> &, const sofa::helper::vector<double> &)
{
    return lc != 0;
}

// Define RemovalFunction
template< class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::FCPointHandler::applyDestroyFunction(unsigned int pointIndex, value_type &)
{
    if (lc)
    {
        lc->removeIndex((unsigned int) pointIndex);
    }
}

template <class DataTypes>
PartialLinearMovementConstraint<DataTypes>::PartialLinearMovementConstraint()
    : core::behavior::ProjectiveConstraintSet<DataTypes>(NULL)
    , data(new PartialLinearMovementConstraintInternalData<DataTypes>)
    , m_indices( initData(&m_indices,"indices","Indices of the constrained points") )
    , m_keyTimes(  initData(&m_keyTimes,"keyTimes","key times for the movements") )
    , m_keyMovements(  initData(&m_keyMovements,"movements","movements corresponding to the key times") )
    , showMovement( initData(&showMovement, (bool)false, "showMovement", "Visualization of the movement to be applied to constrained dofs."))
    , linearMovementBetweenNodesInIndices( initData(&linearMovementBetweenNodesInIndices, (bool)false, "linearMovementBetweenNodesInIndices", "Take into account the linear movement between the constrained points"))
    , mainIndice( initData(&mainIndice, "mainIndice", "The main indice node in the list of constrained nodes, it defines how to apply the linear movement between this constrained nodes "))
    , minDepIndice( initData(&minDepIndice, "minDepIndice", "The indice node in the list of constrained nodes, which is imposed the minimum displacment "))
    , maxDepIndice( initData(&maxDepIndice, "maxDepIndice", "The indice node in the list of constrained nodes, which is imposed the maximum displacment "))
    , m_imposedDisplacmentOnMacroNodes(  initData(&m_imposedDisplacmentOnMacroNodes,"imposedDisplacmentOnMacroNodes","The imposed displacment on macro nodes") )
    , X0 ( initData ( &X0, Real(0.0),"X0","Size of specimen in X-direction" ) )
    , Y0 ( initData ( &Y0, Real(0.0),"Y0","Size of specimen in Y-direction" ) )
    , Z0 ( initData ( &Z0, Real(0.0),"Z0","Size of specimen in Z-direction" ) )
    , movedDirections( initData(&movedDirections,"movedDirections","for each direction, 1 if moved, 0 if free") )
{
    // default to indice 0
    m_indices.beginEdit()->push_back(0);
    m_indices.endEdit();

    //default valueEvent to 0
    m_keyTimes.beginEdit()->push_back( 0.0 );
    m_keyTimes.endEdit();
    m_keyMovements.beginEdit()->push_back( Deriv() );
    m_keyMovements.endEdit();
    VecBool movedDirection;
    for( unsigned i=0; i<NumDimensions; i++)
        movedDirection[i] = true;
    movedDirections.setValue(movedDirection);

    pointHandler = new FCPointHandler(this, &m_indices);
}


template <class DataTypes>
PartialLinearMovementConstraint<DataTypes>::~PartialLinearMovementConstraint()
{
    if (pointHandler)
        delete pointHandler;
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::clearIndices()
{
    m_indices.beginEdit()->clear();
    m_indices.endEdit();
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::addIndex(unsigned int index)
{
    m_indices.beginEdit()->push_back(index);
    m_indices.endEdit();
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::removeIndex(unsigned int index)
{
    removeValue(*m_indices.beginEdit(),index);
    m_indices.endEdit();
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::clearKeyMovements()
{
    m_keyTimes.beginEdit()->clear();
    m_keyTimes.endEdit();
    m_keyMovements.beginEdit()->clear();
    m_keyMovements.endEdit();
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::addKeyMovement(Real time, Deriv movement)
{
    m_keyTimes.beginEdit()->push_back( time );
    m_keyTimes.endEdit();
    m_keyMovements.beginEdit()->push_back( movement );
    m_keyMovements.endEdit();
}

// -- Constraint interface


template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::init()
{
    this->core::behavior::ProjectiveConstraintSet<DataTypes>::init();

    topology = this->getContext()->getMeshTopology();

    // Initialize functions and parameters
    m_indices.createTopologicalEngine(topology, pointHandler);
    m_indices.registerTopologicalData();

    x0.resize(0);
    nextM = prevM = Deriv();

    currentTime = -1.0;
    finished = false;
}


template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::reset()
{
    nextT = prevT = 0.0;
    nextM = prevM = Deriv();

    currentTime = -1.0;
    finished = false;
}


template <class DataTypes>
template <class DataDeriv>
void PartialLinearMovementConstraint<DataTypes>::projectResponseT(const core::MechanicalParams* /*mparams*/, DataDeriv& dx)
{
    Real cT = (Real) this->getContext()->getTime();
    VecBool movedDirection = movedDirections.getValue();
    if ((cT != currentTime) || !finished)
    {
        findKeyTimes();
    }

    if (finished && nextT != prevT)
    {
        const SetIndexArray & indices = m_indices.getValue();

        //set the motion to the Dofs
        for (unsigned int indice : indices)
        {
            for( unsigned j=0; j<NumDimensions; j++)
                if(movedDirection[j]) dx[indice][j] = (Real) 0.0;

        }
    }
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::projectResponse(const core::MechanicalParams* mparams, DataVecDeriv& resData)
{
    helper::WriteAccessor<DataVecDeriv> res = resData;
    projectResponseT<VecDeriv>(mparams, res.wref());
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::projectVelocity(const core::MechanicalParams* /*mparams*/, DataVecDeriv& vData)
{
    helper::WriteAccessor<DataVecDeriv> dx = vData;
    Real cT = (Real) this->getContext()->getTime();
    if ((cT != currentTime) || !finished)
    {
        findKeyTimes();
    }

    if (finished && nextT != prevT)
    {
        const SetIndexArray & indices = m_indices.getValue();

        //set the motion to the Dofs
        for (unsigned int indice : indices)
        {
            dx[indice] = (nextM - prevM)*(1.0 / (nextT - prevT));
        }
    }
}


template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::projectPosition(const core::MechanicalParams* /*mparams*/, DataVecCoord& xData)
{
    helper::WriteAccessor<DataVecCoord> x = xData;
    Real cT = (Real) this->getContext()->getTime();

    //initialize initial Dofs positions, if it's not done
    if (x0.size() == 0)
    {
        const SetIndexArray & indices = m_indices.getValue();
        x0.resize(x.size());
        for (unsigned int indice : indices)
        {
            x0[indice] = x[indice];
        }
    }

    if ((cT != currentTime) || !finished)
    {
        findKeyTimes();
    }

    //if we found 2 keyTimes, we have to interpolate a velocity (linear interpolation)
    if(finished && nextT != prevT)
    {
        interpolatePosition<Coord>(cT, x.wref());
    }
}

template <class DataTypes>
template <class MyCoord>
void PartialLinearMovementConstraint<DataTypes>::interpolatePosition(Real cT, typename std::enable_if<!std::is_same<MyCoord, defaulttype::RigidCoord<3, Real> >::value, VecCoord>::type& x)
{
    const SetIndexArray & indices = m_indices.getValue();
    Real dt = (cT - prevT) / (nextT - prevT);
    Deriv m = prevM + (nextM-prevM)*dt;
    VecBool movedDirection = movedDirections.getValue();
    //set the motion to the Dofs
    if(linearMovementBetweenNodesInIndices.getValue())
    {

        const helper::vector<Real> &imposedDisplacmentOnMacroNodes = this->m_imposedDisplacmentOnMacroNodes.getValue();
        Real a = X0.getValue();
        Real b = Y0.getValue();
        Real c = Z0.getValue();
        bool case2d=false;
        if((a==0.0)||(b==0.0)||(c==0.0)) case2d=true;
        if(a==0.0) {a=b; b=c;}
        if(b==0.0) {b=c;}

        for (unsigned int indice : indices)
        {
            for( unsigned j=0; j< NumDimensions; j++)
            {
                if(movedDirection[j])
                {
                    if(case2d)
                    {
                        x[indice][j] = x0[indice][j] + ((Real)1.0/(a*b))*((a-x0[indice][0])*(b-x0[indice][1])*imposedDisplacmentOnMacroNodes[0]+   ///< N1
                                x0[indice][0]*(b-x0[indice][1])*imposedDisplacmentOnMacroNodes[1]+         ///< N2
                                x0[indice][0]*x0[indice][1]*imposedDisplacmentOnMacroNodes[2]+              ///< N3
                                (a-x0[indice][0])*x0[indice][1]*imposedDisplacmentOnMacroNodes[3])*m[j];    ///< N4
                        //                             4|----------------|3
                        //                              |                |
                        //                              |                |
                        //                              |                |
                        //                             1|----------------|2
                    }
                    else ///< case3d
                    {
                        //        |Y
                        // 	      5---------8
                        //       /|	       /|
                        //      / |	      / |
                        //     6--|------7  |
                        //     |  |/	 |  |
                        //     |  1------|--4--->X
                        //     | / 	     | /
                        //     |/	     |/
                        //     2---------3
                        //   Z/
                        //

                        x[indice][j] = x0[indice][j] + ((Real)1.0/(a*b*c))*(
                                    (a-x0[indice][0])*(b-x0[indice][1])*(c-x0[indice][2])*imposedDisplacmentOnMacroNodes[0]+    ///< N1
                                (a-x0[indice][0])*(b-x0[indice][1])*x0[indice][2]*imposedDisplacmentOnMacroNodes[1]+        ///< N2
                                x0[indice][0]*(b-x0[indice][1])*x0[indice][2]*imposedDisplacmentOnMacroNodes[2]+            ///< N3
                                x0[indice][0]*(b-x0[indice][1])*(c-x0[indice][2])*imposedDisplacmentOnMacroNodes[3]+        ///< N4
                                (a-x0[indice][0])*x0[indice][1]*(c-x0[indice][2])*imposedDisplacmentOnMacroNodes[4]+        ///< N5
                                (a-x0[indice][0])*x0[indice][1]*x0[indice][2]*imposedDisplacmentOnMacroNodes[5]+            ///< N6
                                x0[indice][0]*x0[indice][1]*x0[indice][2]*imposedDisplacmentOnMacroNodes[6]+                ///< N7
                                x0[indice][0]*x0[indice][1]*(c-x0[indice][2])*imposedDisplacmentOnMacroNodes[7]             ///< N8

                                )*m[j];

                    }
                }
            }
        }

    }
    else
    {
        for (unsigned int indice : indices)
        {
            for( unsigned j=0; j< NumDimensions; j++)
                if(movedDirection[j]) x[indice][j] = x0[indice][j] + m[j] ;
        }
    }

}

template <class DataTypes>
template <class MyCoord>
void PartialLinearMovementConstraint<DataTypes>::interpolatePosition(Real cT, typename std::enable_if<std::is_same<MyCoord, defaulttype::RigidCoord<3, Real> >::value, VecCoord>::type& x)
{
    const SetIndexArray & indices = m_indices.getValue();

    Real dt = (cT - prevT) / (nextT - prevT);
    Deriv m = prevM + (nextM-prevM)*dt;
    helper::Quater<Real> prevOrientation = helper::Quater<Real>::createQuaterFromEuler(getVOrientation(prevM));
    helper::Quater<Real> nextOrientation = helper::Quater<Real>::createQuaterFromEuler(getVOrientation(nextM));

    //set the motion to the Dofs
    for (unsigned int indice : indices)
    {
        x[indice].getCenter() = x0[indice].getCenter() + getVCenter(m) ;
        x[indice].getOrientation() = x0[indice].getOrientation() * prevOrientation.slerp2(nextOrientation, dt);
    }
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::projectJacobianMatrix(const core::MechanicalParams* mparams, DataMatrixDeriv& cData)
{

    helper::WriteAccessor<DataMatrixDeriv> c = cData;

    MatrixDerivRowIterator rowIt = c->begin();
    MatrixDerivRowIterator rowItEnd = c->end();

    while (rowIt != rowItEnd)
    {
        projectResponseT<MatrixDerivRowType>(mparams, rowIt.row());
        ++rowIt;
    }
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::findKeyTimes()
{
    Real cT = (Real) this->getContext()->getTime();
    finished = false;

    if(m_keyTimes.getValue().size() != 0 && cT >= *m_keyTimes.getValue().begin() && cT <= *m_keyTimes.getValue().rbegin())
    {
        nextT = *m_keyTimes.getValue().begin();
        prevT = nextT;

        typename helper::vector<Real>::const_iterator it_t = m_keyTimes.getValue().begin();
        typename VecDeriv::const_iterator it_m = m_keyMovements.getValue().begin();

        //WARNING : we consider that the key-events are in chronological order
        //here we search between which keyTimes we are, to know which are the motion to interpolate
        while( it_t != m_keyTimes.getValue().end() && !finished)
        {
            if( *it_t <= cT)
            {
                prevT = *it_t;
                prevM = *it_m;
            }
            else
            {
                nextT = *it_t;
                nextM = *it_m;
                finished = true;
            }
            it_t++;
            it_m++;
        }
    }
}

// Matrix Integration interface
template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::applyConstraint(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix)
{
    const SetIndexArray & indices = m_indices.getValue();
    core::behavior::MultiMatrixAccessor::MatrixRef r = matrix->getMatrix(this->mstate.get(mparams));

    if (r) {
        VecBool movedDirection = movedDirections.getValue();
        for (unsigned int indice : indices)
        {
            // Reset Fixed Row and Col
            for (unsigned int c=0; c<NumDimensions; ++c)
            {
                if( movedDirection[c] ) r.matrix->clearRowCol(r.offset + NumDimensions * indice + c);
            }
            // Set Fixed Vertex
            for (unsigned int c=0; c<NumDimensions; ++c)
            {
                if( movedDirection[c] ) r.matrix->set(r.offset + NumDimensions * indice + c, r.offset + NumDimensions * indice + c, 1.0);
            }
        }
    }
}

template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::applyConstraint(const core::MechanicalParams* mparams, defaulttype::BaseVector* vector, const sofa::core::behavior::MultiMatrixAccessor* matrix)
{
    int o = matrix->getGlobalOffset(this->mstate.get(mparams));
    if (o >= 0) {
        unsigned int offset = (unsigned int)o;
        VecBool movedDirection = movedDirections.getValue();
        const SetIndexArray & indices = m_indices.getValue();
        for (unsigned int indice : indices)
        {
            for (unsigned int c = 0; c < NumDimensions; ++c)
            {
                if (movedDirection[c])
                {
                    vector->clear(offset + NumDimensions * indice + c);
                }
            }
        }
    }
}

//display the path the constrained dofs will go through
template <class DataTypes>
void PartialLinearMovementConstraint<DataTypes>::draw(const core::visual::VisualParams* vparams)
{
    vparams->drawTool()->saveLastState();

    if (!vparams->displayFlags().getShowBehaviorModels() || m_keyTimes.getValue().size() == 0)
        return;

    sofa::helper::vector<defaulttype::Vector3> vertices;
    sofa::defaulttype::RGBAColor color(1, 0.5, 0.5, 1);

    if (showMovement.getValue())
    {
        vparams->drawTool()->disableLighting();
        defaulttype::Vector3 v0, v1;

        const SetIndexArray & indices = m_indices.getValue();
        const VecDeriv& keyMovements = m_keyMovements.getValue();
        for (unsigned int i = 0; i < keyMovements.size() - 1; i++)
        {
            for (unsigned int indice : indices)
            {
                v0 = DataTypes::getCPos(x0[indice]) + DataTypes::getDPos(keyMovements[i]);
                v1 = DataTypes::getCPos(x0[indice]) + DataTypes::getDPos(keyMovements[i + 1]);

                vertices.push_back(v0);
                vertices.push_back(v1);
            }
        }
        vparams->drawTool()->drawLines(vertices, 1, color);
    }
    else
    {
        const VecCoord& x = this->mstate->read(core::ConstVecCoordId::position())->getValue();

        defaulttype::Vector3 point;
        const SetIndexArray & indices = m_indices.getValue();
        for (unsigned int indice : indices)
        {
            point = DataTypes::getCPos(x[indice]);
            vertices.push_back(point);
        }
        vparams->drawTool()->drawPoints(vertices, 10, color);
    }

    vparams->drawTool()->restoreLastState();
}

} // namespace constraint

} // namespace component

} // namespace sofa

#endif

