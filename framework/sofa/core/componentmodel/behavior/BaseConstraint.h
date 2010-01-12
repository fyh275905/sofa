/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_BASECONSTRAINT_H
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_BASECONSTRAINT_H

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>

#include <sofa/defaulttype/BaseMatrix.h>
#include <sofa/defaulttype/BaseVector.h>

#include <vector>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{
#ifdef SOFA_DEV

/**
 *  \brief Object computing a constraint resolution within a Gauss-Seidel algorithm
 */

class ConstraintResolution
{
public:
    ConstraintResolution()
        : nbLines(1) {}

    virtual ~ConstraintResolution() {}

    /// The resolution object can do precomputation with the compliance matrix, and give an initial guess.
    virtual void init(int /*line*/, double** /*w*/, double* /*force*/) {}
    /// Resolution of the constraint for one Gauss-Seidel iteration
    virtual void resolution(int line, double** w, double* d, double* force) = 0;
    /// Called after Gauss-Seidel last iteration, in order to store last computed forces for the inital guess
    virtual void store(int /*line*/, double* /*force*/, bool /*convergence*/) {}

    /// Number of dof used by this particular constraint. To be modified in the object's constructor.
    unsigned char nbLines;
};
#endif // SOFA_DEV

/**
 *  \brief Component computing constraints within a simulated body.
 *
 *  This class define the abstract API common to all constraints.
 *  A BaseConstraint computes constraints applied to one or more simulated body
 *  given its current position and velocity.
 *
 *  Constraints can be internal to a given body (attached to one MechanicalState,
 *  see the Constraint class), or link several bodies together (such as contacts,
 *  see the InteractionConstraint class).
 *
 */
class SOFA_CORE_API BaseConstraint : public virtual objectmodel::BaseObject
{
public:
    SOFA_CLASS(BaseConstraint, objectmodel::BaseObject);

    BaseConstraint()
        : group(initData(&group, 0, "group", "ID of the group containing this constraint. This ID is used to specify which constraints are solved by which solver, by specifying in each solver which groups of constraints it should handle."))
    {
    }

    virtual ~BaseConstraint() { }

    /// Get the ID of the group containing this constraint. This ID is used to specify which constraints are solved by which solver, by specifying in each solver which groups of constraints it should handle.
    int getGroup() const { return group.getValue(); }

    /// Set the ID of the group containing this constraint. This ID is used to specify which constraints are solved by which solver, by specifying in each solver which groups of constraints it should handle.
    void setGroup(int g) { group.setValue(g); }

    /// @name Vector operations
    /// @{

    /// Project dx to constrained space (dx models an acceleration).
    virtual void projectResponse() = 0;

    /// Project v to constrained space (v models a velocity).
    virtual void projectVelocity() = 0;

    /// Project x to constrained space (x models a position).
    virtual void projectPosition() = 0;

    /// Project vFree to constrained space (vFree models a velocity).
    virtual void projectFreeVelocity() = 0;

    /// Project xFree to constrained space (xFree models a position).
    virtual void projectFreePosition() = 0;

    /// @}

    /// @name Matrix operations
    /// @{

    /// Project the compliance Matrix to constrained space.
    virtual void projectResponse(double **);

    /// Project to constrained space using offset parameter
    virtual void applyConstraint(unsigned int&);

    /// Project the global Mechanical Matrix to constrained space using offset parameter
    virtual void applyConstraint(defaulttype::BaseMatrix *, unsigned int & /*offset*/);

    /// Project the global Mechanical Vector to constrained space using offset parameter
    virtual void applyConstraint(defaulttype::BaseVector *, unsigned int & /*offset*/);

    /// Set the violation of each constraint
    virtual void getConstraintValue(defaulttype::BaseVector *, bool /* freeMotion */ = true ) {}

    /// Set an Id for each constraint
    virtual void getConstraintId(long * /*id*/, unsigned int & /*offset*/) {}

    typedef long long PersistentID;
    typedef helper::vector<PersistentID> VecPersistentID;
    typedef defaulttype::Vec<3,int> ConstCoord;
    typedef helper::vector<ConstCoord> VecConstCoord;
    typedef defaulttype::Vec<3,double> ConstDeriv;
    typedef helper::vector<ConstDeriv> VecConstDeriv;
    typedef double ConstArea;
    typedef helper::vector<ConstArea> VecConstArea;

    class ConstraintBlockInfo
    {
    public:
        BaseConstraint* parent;
        int const0; ///< index of first constraint
        int nbLines; ///< how many dofs (i.e. lines in the matrix) are used by each constraint
        int nbGroups; ///< how many groups of constraints are active
        bool hasId; ///< true if this constraint has persistent ID information
        bool hasPosition; ///< true if this constraint has coordinates information
        bool hasDirection; ///< true if this constraint has direction information
        bool hasArea; ///< true if this constraint has area information
        int offsetId; ///< index of first constraint group info in vector of persistent ids and coordinates
        int offsetPosition; ///< index of first constraint group info in vector of coordinates
        int offsetDirection; ///< index of first constraint info in vector of directions
        int offsetArea; ///< index of first constraint group info in vector of areas
        ConstraintBlockInfo() : parent(NULL), const0(0), nbLines(1), nbGroups(0), hasId(false), hasPosition(false), hasDirection(false), hasArea(false), offsetId(0), offsetPosition(0), offsetDirection(0), offsetArea(0)
        {}
    };
    typedef helper::vector<ConstraintBlockInfo> VecConstraintBlockInfo;

    /// Get information for each constraint: pointer to parent BaseConstraint, unique persistent ID, 3D position
    virtual void getConstraintInfo(VecConstraintBlockInfo& /*blocks*/, VecPersistentID& /*ids*/, VecConstCoord& /*positions*/, VecConstDeriv& /*directions*/, VecConstArea& /*areas*/) {}

#ifdef SOFA_DEV
    /// Add the corresponding ConstraintResolution using the offset parameter
    virtual void getConstraintResolution(std::vector<ConstraintResolution*>& /*resTab*/, unsigned int& /*offset*/) {};
#endif //SOFA_DEV

    /// says if the constraint is holonomic or not
    /// holonomic constraints can be processed using different methods such as :
    /// projection - reducing the degrees of freedom - simple lagrange multiplier process
    /// Non-holonomic constraints (like contact, friction...) need more specific treatments
    virtual bool isHolonomic() {return false; }

    /// Test if the constraint is satisfied: return the error.
    virtual double getError() { return 0.0;}

    /// If the constraint is applied only on a subset of particles.
    /// That way, we can optimize the time spent traversing the mappings
    /// Desactivated by default. The constraints using only a subset of particles should activate the mask,
    /// and during projectResponse(), insert the indices of the particles modified
    virtual bool useMask() {return false;}
protected:
    Data<int> group;
};

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
