#ifndef COMPLIANT_HOLONOMICONSTRAINTVALUE_H
#define COMPLIANT_HOLONOMICONSTRAINTVALUE_H

#include "Stabilization.h"

namespace sofa {
namespace component {
namespace odesolver {

/// a constraint value that always enforces null relative velocity
/// Jv+C\lambda=0
class SOFA_Compliant_API HolonomicConstraintValue : public Stabilization {
  public:

    SOFA_CLASS(HolonomicConstraintValue, Stabilization);

    HolonomicConstraintValue( mstate_type* mstate = 0 );
	
	// value for stabilization
    virtual void correction(SReal* dst, unsigned n, unsigned dim, const core::MultiVecCoordId& posId = core::VecCoordId::position(), const core::MultiVecDerivId& velId = core::VecDerivId::velocity()) const;
	
	// value for dynamics
    virtual void dynamics(SReal* dst, unsigned n, unsigned dim, bool stabilization, const core::MultiVecCoordId& posId = core::VecCoordId::position(), const core::MultiVecDerivId& velId = core::VecDerivId::velocity()) const;
	
};

}
}
}



#endif
