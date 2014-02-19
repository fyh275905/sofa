#ifndef AFFINEMULTIMAPPING_H
#define AFFINEMULTIMAPPING_H


#include "AssembledMultiMapping.h"
#include "../utils/map.h"

namespace sofa {
namespace component {
namespace mapping {

/** 
	a very general affine multi mapping: y = A x + b
	
	where x is the concatenation of input dofs, in the order specified in fromModel(). 

	A is given as rows, b as a vector.

	this is moslty useful to python scripts that need to compute arbitrary multimappings.
	
	@author Maxime Tournier
	
*/

// TODO make it work for any vector output dofs (only 1d dofs for now)

template<class TIn, class TOut>
class SOFA_Compliant_API AffineMultiMapping : public AssembledMultiMapping<TIn, TOut> {
	typedef AffineMultiMapping self;
	

  public:
	SOFA_CLASS(SOFA_TEMPLATE2(AffineMultiMapping,TIn,TOut), 
			   SOFA_TEMPLATE2(AssembledMultiMapping,TIn,TOut));
	
	typedef vector< typename TIn::Real > matrix_type;
	
	typedef vector< typename TOut::Real > value_type;

	Data<matrix_type> matrix;
	Data<value_type> value;
	Data<bool> hard_positions;
	
	AffineMultiMapping() :
		matrix(initData(&matrix, "matrix", "matrix for the mapping (row-major)")),
		value(initData(&value, "value", "offset value")),
		hard_positions(initData(&hard_positions, 
								false, 
								"hard_position", 
								"skip matrix multiplication in apply call: the output value will be hard set to @value")) {
		
		// hard positions allows to build arbitrary constraints

		assert( self::Nout == 1 );
	}


	virtual void assemble( const vector<typename self::in_pos_type>& in )  {
		// initialize jacobians

		typedef typename self::jacobian_type::CompressedMatrix jack_type;

		// each input mstate
		for(unsigned j = 0, m = in.size(); j < m; ++j) {
			jack_type& jack = this->jacobian(j).compressedMatrix;

			unsigned dim = this->from(j)->getMatrixSize();
			
			jack.resize(value.getValue().size(), dim );
			jack.setZero();
		}

		// each out dof
		unsigned off = 0;
			
		// each input mstate
		for(unsigned i = 0, n = value.getValue().size(); i < n; ++i) {
			
			// each input mstate
			for(unsigned j = 0, m = in.size(); j < m; ++j) {
				jack_type& jack = this->jacobian(j).compressedMatrix;
				
				unsigned dim = this->from(j)->getMatrixSize();
				
				unsigned r = i;
				jack.startVec(r);

				// each input mstate dof
				for(unsigned k = 0, p = in[j].size(); k < p; ++k) {
					
					// each dof dimension
					for(unsigned u = 0; u < self::Nin; ++u) {
						unsigned c = k * self::Nin + u;
						jack.insertBack(r, c) = matrix.getValue()[off + c];
					}					
				}
				off += dim;
			}
			
		}
		assert( off == matrix.getValue().size() );

		// each input mstate
		for(unsigned j = 0, m = in.size(); j < m; ++j) {
			jack_type& jack = this->jacobian(j).compressedMatrix;
			
			jack.finalize();
		}
		
		
	}


    virtual void apply(typename self::out_pos_type& out, 
					   const vector<typename self::in_pos_type>& in ) {
		
		// let's be paranoid
		assert( out.size() == value.getValue().size() );
		assert( matrix.getValue().size() % value.getValue().size() == 0 );
		
		// each out dof
		unsigned off = 0;
		for(unsigned i = 0, n = out.size(); i < n; ++i) {
			out[i] = value.getValue()[i];

			if( !hard_positions.getValue() ) {
				
				// each input mstate
				for(unsigned j = 0, m = in.size(); j < m; ++j) {
					unsigned dim = this->from(j)->getMatrixSize();
					
					// each input mstate dof
					for(unsigned k = 0, p = in[j].size(); k < p; ++k) {

						const typename TIn::Real* data = &matrix.getValue()[off] + k * self::Nin;
						
						using namespace utils;
						out[i][0] += map(in[j][k]).dot(map<self::Nin>(data));
					}
					
					off += dim;
				}
				
			}
		}
		
	}
	
};


}
}
}



#endif
