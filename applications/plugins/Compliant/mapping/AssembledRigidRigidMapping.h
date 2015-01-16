/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_MAPPING_AssembledRigidRigidMapping_H
#define SOFA_COMPONENT_MAPPING_AssembledRigidRigidMapping_H

#include "AssembledMapping.h"
#include "initCompliant.h"

#include "utils/se3.h" 
#include "utils/pair.h"

#include <sofa/core/ObjectFactory.h>

namespace sofa
{


namespace component
{

namespace mapping
{


/**

   Adaptation of RigidRigidMapping for sparse jacobian matrices.

   Computes a right-translated rigid-frame:    g |-> g.h

   where h is a fixed local joint frame. multiple joint frames may be
   given for one dof using member data @source

   obseletes Flexible/deformationMapping/JointRigidMapping.h

   TODO .inl

   @author maxime.tournier@inria.fr
   
*/
 


template <class TIn, class TOut>
class SOFA_Compliant_API AssembledRigidRigidMapping : public AssembledMapping<TIn, TOut> {
  public:
	SOFA_CLASS(SOFA_TEMPLATE2(AssembledRigidRigidMapping,TIn,TOut), 
               SOFA_TEMPLATE2(AssembledMapping,TIn,TOut));

	
	AssembledRigidRigidMapping() 
		: source(initData(&source, "source", "input dof and rigid offset for each output dof" )),
        geometricStiffness(initData(&geometricStiffness,
                               0,
                               "geometricStiffness",
                               "assemble (and use) geometric stiffness (0=no GS, 1=non symmetric, 2=symmetrized)")) {
                
    }

	typedef defaulttype::SerializablePair<unsigned, typename TIn::Coord> source_type;
	Data< vector< source_type > > source;

    Data<int> geometricStiffness;

    typedef typename TIn::Real Real;

  protected:
	typedef SE3< typename TIn::Real > se3;
  
	typedef AssembledRigidRigidMapping self;


    virtual void assemble_geometric(const typename self::in_pos_type& in_pos,
                                    const typename self::out_force_type& out_force) {

        // we're done lol
        if( ! geometricStiffness.getValue() ) return;

        // sorted in-out
        typedef std::map<unsigned, unsigned> in_out_type;
        in_out_type in_out;

        for(unsigned i = 0, n = source.getValue().size(); i < n; ++i) {
            const source_type& s = source.getValue()[i];
            in_out[ s.first() ] = i;
        }
        
        typename self::jacobian_type::CompressedMatrix& dJ = this->geometric.compressedMatrix;

        dJ.resize( 6 * in_pos.size(),
                   6 * in_pos.size() );

        dJ.setZero();

        for(in_out_type::const_iterator it = in_out.begin(), end = in_out.end();
            it != end; ++it) {

            const unsigned i = it->second;
            const source_type& s = source.getValue()[i];
            assert( it->first == s.first() );
            
			const typename TOut::Deriv& lambda = out_force[i];
            const typename TOut::Deriv::Vec3& f = lambda.getLinear();

            const typename TOut::Deriv::Quat& R = in_pos[ s.first() ].getOrientation();
            const typename TOut::Deriv::Vec3& t = s.second().getCenter();

            const typename TOut::Deriv::Vec3& Rt = R.rotate( t );

            typename se3::mat33 block = se3::hat( se3::map(f) ) * se3::hat( se3::map(Rt));

			for(unsigned j = 0; j < 3; ++j) {
                
				const unsigned row = 6 * s.first() + 3 + j;
                
				dJ.startVec( row );
				
				for(unsigned k = 0; k < 3; ++k) {
                    const unsigned col = 6 * s.first() + 3 + k;
                    
					if( block(j, k) ) dJ.insertBack(row, col) = block(j, k);
				}
			}			
 		}

        dJ.finalize();

        if( geometricStiffness.getValue() == 2 )
        {
            // is there a more efficient way to symmetrize?
            dJ += typename self::jacobian_type::CompressedMatrix( dJ.transpose() );
            dJ /= 2.0;
        }

    }

    
    // virtual void applyDJT(const core::MechanicalParams* mparams,
    //                       core::MultiVecDerivId inForce,
    //                       core::ConstMultiVecDerivId /* inDx */ ) {
    //     std::cout << "PARANOID TEST YO" << std::endl;
        
    //     const Data<typename self::InVecDeriv>& inDx =
    //         *mparams->readDx(this->fromModel);
            
    //     const core::State<TIn>* from_read = this->getFromModel();
    //     core::State<TIn>* from_write = this->getFromModel();

    //     typename self::in_vel_type lvalue( *inForce[from_write].write() );

    //     typename self::in_pos_type in_pos = this->in_pos();
    //     typename self::out_force_type out_force = this->out_force();

    //     for(unsigned i = 0, n = source.getValue().size(); i < n; ++i) {
    //         const source_type& s = source.getValue()[i];

    //         const typename TOut::Deriv& lambda = out_force[i];
    //         const typename TOut::Deriv::Vec3& f = lambda.getLinear();

    //         const typename TOut::Deriv::Quat& R = in_pos[ s.first() ].getOrientation();
    //         const typename TOut::Deriv::Vec3& t = s.second().getCenter();

    //         const typename TOut::Deriv::Vec3& Rt = R.rotate( t );
    //         const typename TIn::Deriv::Vec3& omega = inDx.getValue()[ s.first() ].getAngular();
            
    //         lvalue[s.first()].getAngular() -= TIn::crosscross(f, omega, Rt) * mparams->kFactor();
    //     }
      
    // }



	virtual void assemble( const typename self::in_pos_type& in_pos ) {

		typename self::jacobian_type::CompressedMatrix& J = this->jacobian.compressedMatrix;

		assert( in_pos.size() );
        assert( source.getValue().size() );
		
        J.resize(6 * source.getValue().size(),
		         6 * in_pos.size() );
		J.setZero();
		
        for(unsigned i = 0, n = source.getValue().size(); i < n; ++i) {
            const source_type& s = source.getValue()[i];
			
            typename se3::mat66 block = se3::dR(s.second(), in_pos[ s.first() ] );
			
			for(unsigned j = 0; j < 6; ++j) {
				unsigned row = 6 * i + j;
				
				J.startVec( row );
				
				for(unsigned k = 0; k < 6; ++k) {
                    unsigned col = 6 * s.first() + k;
					if( block(j, k) ) {
                        J.insertBack(row, col) = block(j, k);
                    }
				}
			}			
 		}

		J.finalize();

	}


	
	virtual void apply(typename self::out_pos_type& out,
	                   const typename self::in_pos_type& in ) {
        assert( out.size() == source.getValue().size() );
		
        for(unsigned i = 0, n = source.getValue().size(); i < n; ++i) {
            const source_type& s = source.getValue()[i];
            out[ i ] = se3::prod( in[ s.first() ], s.second() );
		}
		
	}

};


} // namespace mapping

} // namespace component

} // namespace sofa

#endif
