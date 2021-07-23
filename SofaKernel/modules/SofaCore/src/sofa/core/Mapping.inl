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
#ifndef SOFA_CORE_MAPPING_INL
#define SOFA_CORE_MAPPING_INL

#include <sofa/core/State.h>
#include <sofa/core/behavior/BaseMechanicalState.h>
#include <sofa/core/StateVecAccessor.h>
#include <sofa/core/Mapping.h>
#include <iostream>

namespace sofa
{

namespace core
{

template <class In, class Out>
Mapping<In,Out>::Mapping(State<In>* from, State<Out>* to)
    : BaseMapping()
    , fromModel(initLink("input", "Input object to map"), from)
    , toModel(initLink("output", "Output object to map"), to)
    , f_applyRestPosition( initData( &f_applyRestPosition, false, "applyRestPosition", "set to true to apply this mapping to restPosition at init"))
{
    if(to != nullptr && !testMechanicalState(to))
        setNonMechanical();
}

template <class In, class Out>
Mapping<In,Out>::~Mapping()
{
}

template <class In, class Out>
State<In>* Mapping<In,Out>::getFromModel()
{
    return this->fromModel;
}

template <class In, class Out>
State<Out>* Mapping<In,Out>::getToModel()
{
    return this->toModel;
}

template <class In, class Out>
type::vector<BaseState*> Mapping<In,Out>::getFrom()
{
    type::vector<BaseState*> vec(1,this->fromModel.get());
    return  vec;
}

template <class In, class Out>
type::vector<BaseState*> Mapping<In,Out>::getTo()
{
    type::vector<BaseState*> vec(1,this->toModel.get());
    return vec;
}

///<TO REMOVE>
///Necessary ?
template <class In, class Out>
type::vector<behavior::BaseMechanicalState*> Mapping<In,Out>::getMechFrom()
{
    type::vector<behavior::BaseMechanicalState*> vec;
    behavior::BaseMechanicalState* meshFrom = this->fromModel.get()->toBaseMechanicalState();
    if(meshFrom)
        vec.push_back(meshFrom);

    return vec;
}

template <class In, class Out>
type::vector<behavior::BaseMechanicalState*> Mapping<In,Out>::getMechTo()
{
    type::vector<behavior::BaseMechanicalState*> vec;
    behavior::BaseMechanicalState* meshTo = this->toModel.get()->toBaseMechanicalState();
    if(meshTo)
        vec.push_back(meshTo);

    return vec;
}

template <class In, class Out>
void Mapping<In,Out>::init()
{
    if(toModel && !testMechanicalState(toModel.get()))
    {
        setNonMechanical();
        maskFrom = nullptr;
        maskTo = nullptr;
    }
    else
    {
        core::behavior::BaseMechanicalState *state;
        if( (state = this->fromModel.get()->toBaseMechanicalState()) )
            maskFrom = &state->forceMask;
        if( (state = this->toModel.get()->toBaseMechanicalState()) )
            maskTo = &state->forceMask;
    }

    apply(mechanicalparams::defaultInstance(), VecCoordId::position(), ConstVecCoordId::position());
    applyJ(mechanicalparams::defaultInstance(), VecDerivId::velocity(), ConstVecDerivId::velocity());
    if (f_applyRestPosition.getValue())
        apply(mechanicalparams::defaultInstance(), VecCoordId::restPosition(), ConstVecCoordId::restPosition());
}

template <class In, class Out>
sofa::defaulttype::BaseMatrix* Mapping<In,Out>::createMappedMatrix(const behavior::BaseMechanicalState* state1, const behavior::BaseMechanicalState* state2, func_createMappedMatrix m_createMappedMatrix)
{
    sofa::defaulttype::BaseMatrix* result;
    if( !this->areMatricesMapped() )
    {
        msg_warning() << "Mapping::createMappedMatrix() this mapping do not support matrices building. Set mapMatrices to true" << getClassName();
        return nullptr;
    }

    result = (*m_createMappedMatrix)(state1,state2);

    return result;

}






template <class In, class Out>
void Mapping<In,Out>::apply(const MechanicalParams* mparams, MultiVecCoordId outPos, ConstMultiVecCoordId inPos)
{
    State<In>* fromModel = this->fromModel.get();
    State<Out>*  toModel = this->toModel.get();
    if(fromModel && toModel)
    {
        OutDataVecCoord* out = sofa::core::getWrite(toModel, outPos);
        const InDataVecCoord* in = sofa::core::getRead(fromModel,inPos);
        if(out && in)
        {

                this->apply(mparams, *out, *in);
#ifdef SOFA_USE_MASK
            this->m_forceMaskNewStep = true;
#endif
        }
    }
}// Mapping::apply

template <class In, class Out>
void Mapping<In,Out>::applyJ(const MechanicalParams* mparams, MultiVecDerivId outVel, ConstMultiVecDerivId inVel)
{
    State<In>* fromModel = this->fromModel.get();
    State<Out>*  toModel = this->toModel.get();
    if(fromModel && toModel)
    {
        OutDataVecDeriv* out = sofa::core::getWrite(toModel, outVel);
        const InDataVecDeriv* in = sofa::core::getRead(fromModel, inVel);
        if(out && in)
        {
                this->applyJ(mparams, *out, *in);
        }
    }
}// Mapping::applyJ

template <class In, class Out>
void Mapping<In,Out>::applyJT(const MechanicalParams *mparams, MultiVecDerivId inForce, ConstMultiVecDerivId outForce)
{
    State<In>* fromModel = this->fromModel.get();
    State<Out>*  toModel = this->toModel.get();
    if(fromModel && toModel)
    {
        InDataVecDeriv* out = sofa::core::getWrite(fromModel, inForce);
        const OutDataVecDeriv* in = sofa::core::getRead(toModel, outForce);
        if(out && in)
        {
            this->applyJT(mparams, *out, *in);

#ifdef SOFA_USE_MASK
            if( this->m_forceMaskNewStep )
            {
                this->m_forceMaskNewStep = false;
                updateForceMask();
            }
#endif /*SOFA_USE_MASK*/

        }
    }
}// Mapping::applyJT

/// ApplyJT (Constraint)///
template <class In, class Out>
void Mapping<In,Out>::applyJT(const ConstraintParams* cparams, MultiMatrixDerivId inConst, ConstMultiMatrixDerivId outConst )
{
    State<In>* fromModel = this->fromModel.get();
    State<Out>*  toModel = this->toModel.get();
    if(fromModel && toModel)
    {
        InDataMatrixDeriv* out = sofa::core::getWrite(fromModel,inConst);
        const OutDataMatrixDeriv* in = sofa::core::getRead(toModel,outConst);
        if(out && in)
        {
            this->applyJT(cparams, *out, *in);
        }
    }
}// Mapping::applyJT (Constraint)


template <class In, class Out>
void Mapping<In,Out>::applyDJT(const MechanicalParams* /*mparams */ , MultiVecDerivId /*parentForce*/, ConstMultiVecDerivId  /*childForce*/ )
{
    //applyDJT
}


template <class In, class Out>
void Mapping<In,Out>::computeAccFromMapping(const MechanicalParams* mparams, MultiVecDerivId outAcc, ConstMultiVecDerivId inVel, ConstMultiVecDerivId inAcc )
{
    State<In>* fromModel = this->fromModel.get();
    State<Out>*  toModel = this->toModel.get();
    if(fromModel && toModel)
    {
        OutDataVecDeriv* out = sofa::core::getWrite(toModel, outAcc);
        const InDataVecDeriv* inV = sofa::core::getRead(fromModel, inVel);
        const InDataVecDeriv* inA = sofa::core::getRead(fromModel, inAcc);
        if(out && inV && inA)
            this->computeAccFromMapping(mparams, *out, *inV, *inA);
    }
}// Mapping::computeAccFromMapping

template <class In, class Out>
void Mapping<In,Out>::disable()
{
}

template <class In, class Out>
void Mapping<In,Out>::setModels(State<In>* from, State<Out>* to)
{
    this->fromModel.set( from );
    this->toModel.set( to );
    if(to != nullptr && !testMechanicalState(to))
        setNonMechanical();
}

template <class In, class Out>
bool Mapping<In,Out>::setFrom(BaseState* from)
{
    if( !from ) return false;

    State<In>* in = dynamic_cast< State<In>* >(from);
    if( !in )
    {
        msg_error() << "setFrom " << from->getName() << " should be of type " << sofa::helper::NameDecoder::getTypeName<State<In>>();
        return false;
    }

    this->fromModel.set( in );
    return true;
}

template <class In, class Out>
bool Mapping<In,Out>::setTo(BaseState* to)
{
    if( !to ) return false;

    State<Out>* out = dynamic_cast< State<Out>* >(to);
    if( !out )
    {
        msg_error() << "setTo " << to->getName() << " should be of type " << sofa::helper::NameDecoder::getTypeName< State<Out> >();
        return false;
    }

    this->toModel.set( out );

    if( !testMechanicalState(out))
        setNonMechanical();

    return true;
}

template <class In, class Out>
void Mapping<In,Out>::updateForceMask()
{
    assert( maskFrom /*&& SOFA_CLASS_METHOD*/ );
    // the default implementation adds every dofs to the parent mask
    // this sould be overloaded by each mapping to only add the implicated parent dofs to the mask
    maskFrom->assign( fromModel->getSize(), true );
}


} // namespace core

} // namespace sofa

#endif
