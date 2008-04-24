#include <sofa/component/constraint/LagrangianMultiplierContactConstraint.inl>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/component/MechanicalObject.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace constraint
{

SOFA_DECL_CLASS(LagrangianMultiplierContactConstraint)

using namespace sofa::defaulttype;
using namespace sofa::helper;

int LagrangianMultiplierContactConstraintClass = core::RegisterObject("TODO-LagrangianMultiplierContactConstraintClass")
#ifndef SOFA_FLOAT
        .add< LagrangianMultiplierContactConstraint<Vec3dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< LagrangianMultiplierContactConstraint<Vec3fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class LagrangianMultiplierContactConstraint<Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class LagrangianMultiplierContactConstraint<Vec3fTypes>;
#endif


} // namespace constraint

} // namespace component

} // namespace sofa

