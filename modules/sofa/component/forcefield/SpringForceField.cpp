// Author: François Faure, INRIA-UJF, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/forcefield/SpringForceField.inl>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/ObjectFactory.h>
//#include <typeinfo>


namespace sofa
{

namespace component
{

namespace forcefield
{

SOFA_DECL_CLASS(SpringForceField)

using namespace sofa::defaulttype;


// Register in the Factory
int SpringForceFieldClass = core::RegisterObject("Springs")
#ifndef SOFA_FLOAT
        .add< SpringForceField<Vec3dTypes> >()
        .add< SpringForceField<Vec2dTypes> >()
        .add< SpringForceField<Vec1dTypes> >()
        .add< SpringForceField<Vec6dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< SpringForceField<Vec3fTypes> >()
        .add< SpringForceField<Vec2fTypes> >()
        .add< SpringForceField<Vec1fTypes> >()
        .add< SpringForceField<Vec6fTypes> >()
#endif
        ;
#ifndef SOFA_FLOAT
template class SpringForceField<Vec3dTypes>;
template class SpringForceField<Vec2dTypes>;
template class SpringForceField<Vec1dTypes>;
template class SpringForceField<Vec6dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SpringForceField<Vec3fTypes>;
template class SpringForceField<Vec2fTypes>;
template class SpringForceField<Vec1fTypes>;
template class SpringForceField<Vec6fTypes>;
#endif


} // namespace forcefield

} // namespace component

} // namespace sofa

