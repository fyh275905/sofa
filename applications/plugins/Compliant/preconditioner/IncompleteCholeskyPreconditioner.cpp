#include "IncompleteCholeskyPreconditioner.h"

#include <sofa/core/ObjectFactory.h>


namespace sofa {
namespace component {
namespace linearsolver {


SOFA_DECL_CLASS(IncompleteCholeskyPreconditioner)
int IncompleteCholeskyPreconditionerClass = core::RegisterObject("Incomplete Cholesky preconditioner").add< IncompleteCholeskyPreconditioner >();


IncompleteCholeskyPreconditioner::IncompleteCholeskyPreconditioner()
    : BasePreconditioner()
    , d_constant( initData(&d_constant, false, "constant", "reuse first factorization"))
    , d_shift( initData(&d_shift, real(0), "shift", "initial shift"))
    , m_factorized(false)
{}


  template<class T, T, class R = void> struct sfinae {
    using type = R;
  };
  
  // sfinae for picking the right shift method
  template<class Preconditioner>
  static void set_shift(Preconditioner& preconditioner, SReal value,
                        typename sfinae<void (Preconditioner::*)(SReal) ,
                        &Preconditioner::setInitialShift>::type*) {
      preconditioner.setInitialShift( value );
  }


  template<class Preconditioner>
  static void set_shift(Preconditioner& preconditioner, SReal value,
                        typename sfinae<void (Preconditioner::*)(SReal) ,
                        &Preconditioner::setShift>::type*) {
      preconditioner.setShift( value );
  }
  

void IncompleteCholeskyPreconditioner::reinit()
{
    BasePreconditioner::reinit();
    m_factorized = false;

    set_shift(preconditioner, d_shift.getValue(), 0);
}

void IncompleteCholeskyPreconditioner::compute( const rmat& H )
{
    if( d_constant.getValue() )
    {
        if( m_factorized ) return;
        else m_factorized = true;
    }

    preconditioner.compute( H );

    if( preconditioner.info() != Eigen::Success )
    {
        serr<<"automatic regularization of a singular matrix"<<sendl;

        // if singular, try to regularize by adding a tiny diagonal matrix
        rmat identity(H.rows(),H.cols());
        identity.setIdentity();
        preconditioner.compute<rmat>( H + identity * std::numeric_limits<SReal>::epsilon() );

        if( preconditioner.info() != Eigen::Success )
        {
            serr << "non invertible response" << sendl;
            assert( false );
        }

    }
}

void IncompleteCholeskyPreconditioner::apply( vec& res, const vec& v )
{
    res.resize( v.size() );

    if( d_constant.getValue() )
    {
        res.head(preconditioner.rows()) = preconditioner.solve( v.head(preconditioner.rows()) );
        res.tail( v.size()-preconditioner.rows() ) = v.tail( v.size()-preconditioner.rows() ); // in case of dofs have been added, like mouse...
    }
    else
        res = preconditioner.solve( v );

}

}
}
}

 
