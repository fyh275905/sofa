#include "CudaTypes.h"
#include "CudaMechanicalObject.inl"
#include <sofa/component/collision/SphereModel.inl>
#include <sofa/core/ObjectFactory.h>
#include <sofa/component/MappedObject.inl>

namespace sofa
{

namespace gpu
{

namespace cuda
{

SOFA_DECL_CLASS(CudaMechanicalObject)

int MechanicalObjectCudaClass = core::RegisterObject("Supports GPU-side computations using CUDA")
        .add< component::MechanicalObject<CudaVec3fTypes> >()
        .add< component::MechanicalObject<CudaVec3f1Types> >()
        .add< component::MechanicalObject<CudaRigid3fTypes> >()
#ifdef SOFA_DEV
#ifdef SOFA_GPU_CUDA_DOUBLE
        .add< component::MechanicalObject<CudaVec3dTypes> >()
        .add< component::MechanicalObject<CudaVec3d1Types> >()
        .add< component::MechanicalObject<CudaRigid3dTypes> >()
#endif // SOFA_GPU_CUDA_DOUBLE
#endif // SOFA_DEV
        ;

int CudaSphereModelClass = core::RegisterObject("Supports GPU-side computations using CUDA")
        .add< component::collision::TSphereModel<CudaVec3fTypes> >()
//.add< component::collision::TSphereModel<CudaVec3f1Types> >()
#ifdef SOFA_DEV
#ifdef SOFA_GPU_CUDA_DOUBLE
        .add< component::collision::TSphereModel<CudaVec3dTypes> >()
//.add< component::collision::TSphereModel<CudaVec3d1Types> >()
#endif // SOFA_GPU_CUDA_DOUBLE
#endif // SOFA_DEV
        ;

int MappedObjectCudaClass = core::RegisterObject("Supports GPU-side computations using CUDA")
        .add< component::MappedObject<CudaVec3fTypes> >()
        .add< component::MappedObject<CudaVec3f1Types> >()
        .add< component::MappedObject<CudaRigid3fTypes> >()
#ifdef SOFA_DEV
#ifdef SOFA_GPU_CUDA_DOUBLE
        .add< component::MappedObject<CudaVec3dTypes> >()
        .add< component::MappedObject<CudaVec3d1Types> >()
        .add< component::MappedObject<CudaRigid3dTypes> >()
#endif // SOFA_GPU_CUDA_DOUBLE
#endif // SOFA_DEV
        ;

} // namespace cuda

} // namespace gpu

} // namespace sofa
