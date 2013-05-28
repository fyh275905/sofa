

#include "../initFlexible.h"


#include <sofa/component/container/MechanicalObject.h>


#include <sofa/component/mass/AddMToMatrixFunctor.h>
#include <sofa/component/mass/UniformMass.h>

#include <sofa/core/behavior/ForceField.h>


#include "ComponentSpecializationsDefines.h"



namespace sofa
{


// ==========================================================================
// Mechanical Object

namespace component
{
namespace container
{


template <> SOFA_Flexible_API
void MechanicalObject<defaulttype::TYPEABSTRACTNAME3Types>::draw(const core::visual::VisualParams* vparams);

#if defined(SOFA_EXTERN_TEMPLATE) && !defined(FLEXIBLE_TYPEABSTRACTNAMEComponents_CPP)
#ifndef SOFA_FLOAT
extern template class SOFA_Flexible_API MechanicalObjectInternalData<defaulttype::TYPEABSTRACTNAME3dTypes>;
extern template class SOFA_Flexible_API MechanicalObject<defaulttype::TYPEABSTRACTNAME3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_Flexible_API MechanicalObjectInternalData<defaulttype::TYPEABSTRACTNAME3fTypes>;
extern template class SOFA_Flexible_API MechanicalObject<defaulttype::TYPEABSTRACTNAME3fTypes>;
#endif
#endif


} // namespace container







// ==========================================================================
// Uniform Mass


namespace mass
{

template<int N, typename Real>
class AddMToMatrixFunctor< typename defaulttype::StdTYPEABSTRACTNAMETypes<N,Real>::Deriv, defaulttype::TYPEABSTRACTNAMEMass<N,Real> >
{
public:
    void operator()(defaulttype::BaseMatrix * mat, const defaulttype::TYPEABSTRACTNAMEMass<N,Real>& mass, int pos, double fact)
    {
        typedef defaulttype::TYPEABSTRACTNAMEMass<N,Real> TYPEABSTRACTNAMEMass;
        for( unsigned i=0; i<TYPEABSTRACTNAMEMass::VSize; ++i )
            for( unsigned j=0; j<TYPEABSTRACTNAMEMass::VSize; ++j )
            {
                mat->add(pos+i, pos+j, mass[i][j]*fact);
//            cerr<<"AddMToMatrixFunctor< defaulttype::Vec<N,Real>, defaulttype::Mat<N,N,Real> >::operator(), add "<< mass[i][j]*fact << " in " << pos+i <<","<< pos+j <<endl;
            }
    }
};


#ifndef SOFA_FLOAT
template <> SOFA_Flexible_API
void UniformMass<defaulttype::TYPEABSTRACTNAME3dTypes, defaulttype::TYPEABSTRACTNAME3dMass>::draw( const core::visual::VisualParams* vparams );
template <> SOFA_Flexible_API
double UniformMass<defaulttype::TYPEABSTRACTNAME3dTypes, defaulttype::TYPEABSTRACTNAME3dMass>::getPotentialEnergy( const core::MechanicalParams* /* PARAMS FIRST */, const DataVecCoord& vx ) const;
#endif
#ifndef SOFA_DOUBLE
template <> SOFA_Flexible_API
void UniformMass<defaulttype::TYPEABSTRACTNAME3fTypes, defaulttype::TYPEABSTRACTNAME3fMass>::draw( const core::visual::VisualParams* vparams );
template <> SOFA_Flexible_API
double UniformMass<defaulttype::TYPEABSTRACTNAME3fTypes, defaulttype::TYPEABSTRACTNAME3fMass>::getPotentialEnergy( const core::MechanicalParams* /* PARAMS FIRST */, const DataVecCoord& vx ) const;
#endif

} // namespace mass

} // namespace component



namespace core
{

namespace behavior
{

#if defined(SOFA_EXTERN_TEMPLATE) && !defined(FLEXIBLE_TYPEABSTRACTNAMEComponents_CPP)
#ifndef SOFA_FLOAT
extern template class SOFA_Flexible_API ForceField<defaulttype::TYPEABSTRACTNAME3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_Flexible_API ForceField<defaulttype::TYPEABSTRACTNAME3fTypes>;
#endif
#endif

} // namespace behavior

} // namespace core



} // namespace sofa


#include "ComponentSpecializationsUndef.h"

