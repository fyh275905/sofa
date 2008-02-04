#include "CudaTypes.h"
#include <CudaBarycentricMapping.inl>
//#include <sofa/component/mapping/BarycentricMapping.inl>
#include <sofa/core/componentmodel/behavior/MappedModel.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/defaulttype/VecTypes.h>

namespace sofa
{

namespace gpu
{

namespace cuda
{
using namespace sofa::defaulttype;
using namespace sofa::core;
using namespace sofa::core::componentmodel::behavior;
using namespace sofa::component::mapping;

SOFA_DECL_CLASS(CudaBarycentricMapping)

int BarycentricMappingCudaClass = core::RegisterObject("Supports GPU-side computations using CUDA")
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<CudaVec3fTypes>, MechanicalState<CudaVec3fTypes> > > >()
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<CudaVec3fTypes> > > >()
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<CudaVec3fTypes> > > >()
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<CudaVec3fTypes>, MechanicalState<Vec3fTypes> > > >()
        .add< BarycentricMapping< MechanicalMapping< MechanicalState<CudaVec3fTypes>, MechanicalState<Vec3dTypes> > > >()
//.add< BarycentricMapping< Mapping< State<CudaVec3fTypes>, MechanicalState<CudaVec3fTypes> > > >()
        .add< BarycentricMapping< Mapping< State<CudaVec3fTypes>, MappedModel<CudaVec3fTypes> > > >()
        .add< BarycentricMapping< Mapping< State<CudaVec3fTypes>, MappedModel<ExtVec3fTypes> > > >()
        .add< BarycentricMapping< Mapping< State<CudaVec3fTypes>, MappedModel<ExtVec3dTypes> > > >()
        .add< BarycentricMapping< Mapping< State<CudaVec3fTypes>, MappedModel<Vec3fTypes> > > >()
        .add< BarycentricMapping< Mapping< State<CudaVec3fTypes>, MappedModel<Vec3dTypes> > > >()
        ;

} // namespace cuda

} // namespace gpu

} // namespace sofa
