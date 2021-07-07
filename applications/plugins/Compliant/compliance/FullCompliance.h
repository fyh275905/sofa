#ifndef SOFA_COMPONENT_COMPLIANCE_FULLCOMPLIANCE_H
#define SOFA_COMPONENT_COMPLIANCE_FULLCOMPLIANCE_H
#include <Compliant/config.h>
#include <sofa/core/behavior/ForceField.h>
#include <sofa/type/Mat.h>
#include <SofaEigen2Solver/EigenSparseMatrix.h>

namespace sofa
{
namespace component
{
namespace forcefield
{

/**
    use a full compliance or stiffness matrix provided by the user

    @author: Benjamin Gilles

  */
template<class TDataTypes>
class FullCompliance : public core::behavior::ForceField<TDataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(FullCompliance, TDataTypes), SOFA_TEMPLATE(core::behavior::ForceField, TDataTypes));

    typedef TDataTypes DataTypes;
    typedef core::behavior::ForceField<TDataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef core::objectmodel::Data<VecCoord> DataVecCoord;
    typedef core::objectmodel::Data<VecDeriv> DataVecDeriv;
    enum { N=DataTypes::deriv_total_size };

    typedef linearsolver::EigenSparseMatrix<TDataTypes,TDataTypes> block_matrix_type;
    Data< block_matrix_type > matC; ///< input compliance matrix (PSD)
    Data< block_matrix_type > matK; ///< input stiffness matrix (PSD)

    Data< type::vector<SReal> > damping; ///< Full damping

    virtual void init() override;

    /// Compute the compliance matrix
    virtual void reinit() override;

    virtual SReal getPotentialEnergy( const core::MechanicalParams* mparams, const DataVecCoord& x ) const override;

    /// Return a pointer to the compliance matrix
    virtual const sofa::defaulttype::BaseMatrix* getComplianceMatrix(const core::MechanicalParams*) override;

    virtual void addKToMatrix( sofa::defaulttype::BaseMatrix * matrix, SReal kFact, unsigned int &offset ) override;

    virtual void addBToMatrix( sofa::defaulttype::BaseMatrix * matrix, SReal bFact, unsigned int &offset ) override;

    /// addForce does nothing when this component is processed like a compliance.
    virtual void addForce(const core::MechanicalParams *, DataVecDeriv &, const DataVecCoord &, const DataVecDeriv &) override;

    /// addDForce does nothing when this component is processed like a compliance.
    virtual void addDForce(const core::MechanicalParams *, DataVecDeriv &, const DataVecDeriv &) override;

    /// unassembled API
    virtual void addClambda(const core::MechanicalParams *, DataVecDeriv &, const DataVecDeriv &, SReal) override;

protected:
    FullCompliance( core::behavior::MechanicalState<DataTypes> *mm = NULL);

    block_matrix_type matB; /// damping matrix (Negative S.D.)

    static const Real s_complianceEpsilon; /// threshold for which stiffness can be computed by inverting compliance
    bool invertMatrix(Eigen::SparseMatrix<Real>& Minv, const Eigen::SparseMatrix<Real>& M );
};

#if defined(SOFA_EXTERN_TEMPLATE) && !defined(SOFA_COMPONENT_COMPLIANCE_FULLCOMPLIANCE_CPP)
extern template class SOFA_Compliant_API FullCompliance<sofa::defaulttype::Vec1Types>;
extern template class SOFA_Compliant_API FullCompliance<sofa::defaulttype::Vec3Types>;
extern template class SOFA_Compliant_API FullCompliance<sofa::defaulttype::Vec6Types>;

#endif

}
}
}

#endif // SOFA_COMPONENT_COMPLIANCE_FULLCOMPLIANCE_H


