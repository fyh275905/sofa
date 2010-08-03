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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_MAPPING_BEAMLINEARMAPPING_INL
#define SOFA_COMPONENT_MAPPING_BEAMLINEARMAPPING_INL

#include <sofa/component/mapping/BeamLinearMapping.h>
#include <sofa/simulation/common/Simulation.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/helper/io/MassSpringLoader.h>
#include <sofa/helper/io/SphereLoader.h>
#include <sofa/helper/io/Mesh.h>
#include <sofa/helper/gl/template.h>
#include <sofa/core/behavior/MechanicalMapping.inl>
#include <sofa/core/behavior/MechanicalState.h>
#include <string>



namespace sofa
{

namespace component
{

namespace mapping
{

using namespace sofa::defaulttype;

template <class BasicMapping>
void BeamLinearMapping<BasicMapping>::init()
{
    bool local = localCoord.getValue();
    if (this->points.empty() && this->toModel!=NULL)
    {
        typename In::VecCoord& xfrom = *this->fromModel->getX();
        beamLength.resize(xfrom.size());
        for (unsigned int i=0; i<xfrom.size()-1; i++)
            beamLength[i] = (Real)((xfrom[i]-xfrom[i+1]).norm());
        if (xfrom.size()>=2)
            beamLength[xfrom.size()-1] = beamLength[xfrom.size()-2];
        VecCoord& x = *this->toModel->getX();
        sout << "BeamLinearMapping: init "<<x.size()<<" points."<<sendl;
        points.resize(x.size());
        if (local)
        {
            for (unsigned int i=0; i<x.size(); i++)
                points[i] = x[i];
        }
        else
        {
            for (unsigned int i=0; i<x.size(); i++)
            {
                Coord p = xfrom[0].getOrientation().inverseRotate(x[i]-xfrom[0].getCenter());
                unsigned int j=0;
                while(j<beamLength.size() && p[0]>=beamLength[j])
                {
                    p[0] -= beamLength[j];
                    ++j;
                }
                p/=beamLength[j];
                p[0]+=j;
                points[i] = p;
            }
        }
    }
    this->BasicMapping::init();
}

template <class BasicMapping>
void BeamLinearMapping<BasicMapping>::apply( typename Out::VecCoord& out, const typename In::VecCoord& in )
{
    //translation = in[index.getValue()].getCenter();
    //in[index.getValue()].writeRotationMatrix(rotation);
    rotatedPoints0.resize(points.size());
    rotatedPoints1.resize(points.size());
    out.resize(points.size());
    for(unsigned int i=0; i<points.size(); i++)
    {
        Coord inpos = points[i];
        int in0 = helper::rfloor(inpos[0]);
        if (in0<0) in0 = 0; else if (in0 > (int)in.size()-2) in0 = in.size()-2;
        inpos[0] -= in0;
        rotatedPoints0[i] = in[in0].getOrientation().rotate(inpos) * beamLength[in0];
        Coord out0 = in[in0].getCenter() + rotatedPoints0[i];
        Coord inpos1 = inpos; inpos1[0] -= 1;
        rotatedPoints1[i] = in[in0+1].getOrientation().rotate(inpos1) * beamLength[in0];
        Coord out1 = in[in0+1].getCenter() + rotatedPoints1[i];
        Real fact = (Real)inpos[0];
        fact = 3*(fact*fact)-2*(fact*fact*fact);
        out[i] = out0 * (1-fact) + out1 * (fact);
    }
}

template <class BasicMapping>
void BeamLinearMapping<BasicMapping>::applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in )
{
    //const typename In::VecCoord& x = *this->fromModel->getX();
    //Deriv v,omega;
    //v = in[index.getValue()].getVCenter();
    //omega = in[index.getValue()].getVOrientation();
    out.resize(points.size());
    for(unsigned int i=0; i<points.size(); i++)
    {
        // out = J in
        // J = [ I -OM^ ]
        //out[i] =  v - cross(rotatedPoints[i],omega);

        defaulttype::Vec<N, typename In::Real> inpos = points[i];
        int in0 = helper::rfloor(inpos[0]);
        if (in0<0) in0 = 0; else if (in0 > (int)in.size()-2) in0 = in.size()-2;
        inpos[0] -= in0;
        Deriv omega0 = in[in0].getVOrientation();
        Deriv out0 = in[in0].getVCenter() - cross(rotatedPoints0[i], omega0);
        Deriv omega1 = in[in0+1].getVOrientation();
        Deriv out1 = in[in0+1].getVCenter() - cross(rotatedPoints1[i], omega1);
        Real fact = (Real)inpos[0];
        fact = 3*(fact*fact)-2*(fact*fact*fact);
        out[i] = out0 * (1-fact) + out1 * (fact);
    }
}

template <class BasicMapping>
void BeamLinearMapping<BasicMapping>::applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in )
{
    //Deriv v,omega;
    for(unsigned int i=0; i<points.size(); i++)
    {
        // out = Jt in
        // Jt = [ I     ]
        //      [ -OM^t ]
        // -OM^t = OM^

        //Deriv f = in[i];
        //v += f;
        //omega += cross(rotatedPoints[i],f);

        defaulttype::Vec<N, typename In::Real> inpos = points[i];
        int in0 = helper::rfloor(inpos[0]);
        if (in0<0) in0 = 0; else if (in0 > (int)out.size()-2) in0 = out.size()-2;
        inpos[0] -= in0;
        Deriv f = in[i];
        Real fact = (Real)inpos[0];
        fact = 3*(fact*fact)-2*(fact*fact*fact);
        out[in0].getVCenter() += f * (1-fact);
        out[in0].getVOrientation() += cross(rotatedPoints0[i], f) * (1-fact);
        out[in0+1].getVCenter() += f * (fact);
        out[in0+1].getVOrientation() += cross(rotatedPoints1[i], f) * (fact);
    }
    //out[index.getValue()].getVCenter() += v;
    //out[index.getValue()].getVOrientation() += omega;
}


// BeamLinearMapping::applyJT( typename In::VecConst& out, const typename Out::VecConst& in ) //
// this function propagate the constraint through the rigid mapping :
// if one constraint along (vector n) with a value (v) is applied on the childModel (like collision model)
// then this constraint is transformed by (Jt.n) with value (v) for the rigid model
// There is a specificity of this propagateConstraint: we have to find the application point on the childModel
// in order to compute the right constaint on the rigidModel.
template <class BasicMapping>
void BeamLinearMapping<BasicMapping>::applyJT( typename In::MatrixDeriv& out, const typename Out::MatrixDeriv& in )
{
    const typename In::VecCoord& x = *this->fromModel->getX();

    typename Out::MatrixDeriv::RowConstIterator rowItEnd = in.end();

    for (typename Out::MatrixDeriv::RowConstIterator rowIt = in.begin(); rowIt != rowItEnd; ++rowIt)
    {
        typename Out::MatrixDeriv::ColConstIterator colIt = rowIt.begin();
        typename Out::MatrixDeriv::ColConstIterator colItEnd = rowIt.end();

        if (colIt != colItEnd)
        {
            typename In::MatrixDeriv::RowIterator o = out.writeLine(rowIt.index());

            // computation of (Jt.n)
            for (typename Out::MatrixDeriv::ColConstIterator colIt = rowIt.begin(); colIt != colItEnd; ++colIt)
            {
                unsigned int indexIn = colIt.index();
                Deriv data = (Deriv) colIt.val();

                // interpolation
                Coord inpos = points[indexIn];
                int in0 = helper::rfloor(inpos[0]);
                if (in0<0)
                    in0 = 0;
                else if (in0 > (int)x.size()-2)
                    in0 = x.size()-2;
                inpos[0] -= in0;
                Real fact = (Real)inpos[0];
                fact = 3.0*(fact*fact)-2.0*(fact*fact*fact);

                // weighted value of the constraint direction
                Deriv w_n = data;

                // Compute the mapped Constraint on the beam nodes
                InDeriv direction0;
                direction0.getVCenter() = w_n * (1-fact);
                direction0.getVOrientation() = cross(rotatedPoints0[indexIn], w_n) * (1-fact);
                InDeriv direction1;
                direction1.getVCenter() = w_n * (fact);
                direction1.getVOrientation() = cross(rotatedPoints1[indexIn], w_n) * (fact);

                o.addCol(in0, direction0);
                o.addCol(in0+1, direction1);
            }
        }
    }
}

/*
template <class BasicMapping>
void BeamLinearMapping<BasicMapping>::applyJT( typename In::VecConst& out, const typename Out::VecConst& in )
{
	const typename In::VecCoord& x = *this->fromModel->getX();
	int outSize = out.size();
	out.resize(in.size() + outSize); // we can accumulate in "out" constraints from several mappings

	for(unsigned int i=0; i<in.size(); i++)
	{
		// computation of (Jt.n) //
		// in[i].size() = num node involved in the constraint

                OutConstraintIterator itOut;
                std::pair< OutConstraintIterator, OutConstraintIterator > iter=in[i].data();

                for (itOut=iter.first;itOut!=iter.second;itOut++)
                {
                        unsigned int indexIn = itOut->first;
                        Deriv data = (Deriv) itOut->second;
			// interpolation//////////
			defaulttype::Vec<N, typename In::Real> inpos = points[indexIn];
			int in0 = helper::rfloor(inpos[0]);
			if (in0<0) in0 = 0; else if (in0 > (int)x.size()-2) in0 = x.size()-2;
			inpos[0] -= in0;
                        Real fact = (Real)inpos[0];
                        fact = 3*(fact*fact)-2*(fact*fact*fact);
			/////////////////////////
			Deriv w_n = data;	// weighted value of the constraint direction

			// Compute the mapped Constraint on the beam nodes ///
			InDeriv direction0;
			direction0.getVCenter() = w_n * (1-fact);
			direction0.getVOrientation() = cross(rotatedPoints0[indexIn], w_n) * (1-fact);
			InDeriv direction1;
			direction1.getVCenter() = w_n * (fact);
			direction1.getVOrientation() = cross(rotatedPoints1[indexIn], w_n) * (fact);
                        out[outSize+i].add(in0, direction0);
                        out[outSize+i].add(in0+1, direction1);
		}

	}
}
*/

template <class BasicMapping>
void BeamLinearMapping<BasicMapping>::draw()
{
    if (!this->getShow()) return;
    std::vector< Vector3 > points;
    Vector3 point;

    const typename Out::VecCoord& x = *this->toModel->getX();
    for (unsigned int i=0; i<x.size(); i++)
    {
        point = OutDataTypes::getCPos(x[i]);
        points.push_back(point);
    }
    simulation::getSimulation()->DrawUtility.drawPoints(points, 7, Vec<4,float>(1,1,0,1));

}

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
