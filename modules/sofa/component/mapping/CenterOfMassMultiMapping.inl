#ifndef SOFA_COMPONENT_MAPPING_CENTEROFMASSMULTIMAPPING_INL
#define SOFA_COMPONENT_MAPPING_CENTEROFMASSMULTIMAPPING_INL

#include <sofa/component/mapping/CenterOfMassMultiMapping.h>
#include <sofa/defaulttype/Vec.h>
//#include <sofa/helper/gl/template.h>
#include <sofa/simulation/common/Simulation.h>
#include <algorithm>
#include <functional>

namespace sofa
{

namespace component
{

namespace mapping
{

using namespace core;
using namespace core::behavior;

template < typename Model >
struct Operation
{

    typedef typename Model::VecCoord VecCoord;
    typedef typename Model::Coord    Coord;
    typedef typename Model::Deriv    Deriv;
    typedef typename Model::VecDeriv VecDeriv;

public :
    static inline const VecCoord* getVecCoord( const Model* m, const VecId& id) { return m->getVecCoord(id.index); };
    static inline VecDeriv* getVecDeriv( Model* m, const VecId& id) { return m->getVecDeriv(id.index);};

    static inline const BaseMass* fetchMass  ( const Model* m)
    {
        BaseMass* mass = dynamic_cast<BaseMass*> (m->getContext()->getMass());
        return mass;
    }
    static inline double computeTotalMass( const Model* model, const BaseMass* mass )
    {
        double result = 0.0;
        for ( unsigned int i = 0; i < model->getX()->size(); i++)
        {
            result += mass->getElementMass(i);
        }
        return result;
    }

    static inline Coord WeightedCoord( const VecCoord* v, const BaseMass* m)
    {
        Coord c;
        for (unsigned int i=0 ; i< v->size() ; i++)
        {
            c += (*v)[i] * m->getElementMass(i);
        }
        return c;
    }

    static inline Deriv WeightedDeriv( const VecDeriv* v, const BaseMass* m)
    {
        Deriv d;
        for (unsigned int i=0 ; i< v->size() ; i++)
        {
            d += (*v)[i] * m->getElementMass(i);
        }
        return d;
    }
};

template< class BasicMultiMapping  >
void CenterOfMassMultiMapping< BasicMultiMapping >::apply(const helper::vector<OutVecCoord*>& outPos, const helper::vector<const InVecCoord*>& inPos )
{
    typedef typename InVecCoord::iterator iter_coord;
    assert( outPos.size() == 1); // we are dealing with a many to one mapping.
    InCoord COM;
    std::transform(inPos.begin(), inPos.end(), inputBaseMass.begin(), inputWeightedCOM.begin(), Operation<In>::WeightedCoord );

    for( iter_coord iter = inputWeightedCOM.begin() ; iter != inputWeightedCOM.end(); ++iter ) COM += *iter;
    COM *= invTotalMass;

    OutVecCoord* outVecCoord = outPos[0];

    SReal x,y,z;
    InDataTypes::get(x,y,z,COM);
    OutDataTypes::set((*outVecCoord)[0], x,y,z);
}

template< class BasicMultiMapping >
void CenterOfMassMultiMapping< BasicMultiMapping >::applyJ(const helper::vector< OutVecDeriv*>& outDeriv, const helper::vector<const InVecDeriv*>& inDeriv)
{
    typedef typename InVecDeriv::iterator iter_deriv;
    assert( outDeriv.size() == 1 );

    InDeriv Velocity;
    std::transform(inDeriv.begin(), inDeriv.end(), inputBaseMass.begin(), inputWeightedForce.begin(), Operation<In>::WeightedDeriv );

    for ( iter_deriv iter = inputWeightedForce.begin() ; iter != inputWeightedForce.end() ; ++iter ) Velocity += *iter;
    Velocity *= invTotalMass;

    OutVecDeriv* outVecDeriv =  outDeriv[0];

    SReal x,y,z;
    InDataTypes::get(x,y,z,Velocity);
    OutDataTypes::set((*outVecDeriv)[0], x,y,z);
}



template < class BasicMultiMapping >
void CenterOfMassMultiMapping< BasicMultiMapping >::applyJT( const helper::vector<InVecDeriv*>& outDeriv , const helper::vector<const OutVecDeriv*>& inDeriv )
{
    assert( inDeriv.size() == 1 );


    OutDeriv gravityCenterForce;
    const OutVecDeriv* inForce = inDeriv[0];
    if( !inForce->empty() )
    {
        gravityCenterForce = (* inForce) [0];
        gravityCenterForce *= invTotalMass;

        SReal x,y,z;
        OutDataTypes::get(x,y,z,gravityCenterForce);

        InDeriv f;
        InDataTypes::set(f,x,y,z);

        for (unsigned int i=0; i<outDeriv.size(); ++i)
        {
            InVecDeriv& v=*(outDeriv[i]);
            const core::behavior::BaseMass* m=inputBaseMass[i];
            for (unsigned int p=0; p<v.size(); ++p)
            {
                v[p] += f*m->getElementMass(p);
            }
        }
    }
}

template< class BasicMultiMapping >
void CenterOfMassMultiMapping< BasicMultiMapping>::init()
{
    typedef helper::vector<double>::iterator iter_double;

    inputBaseMass.resize ( this->getFromModels().size()  );
    inputTotalMass.resize( this->getFromModels().size()  );
    inputWeightedCOM.resize( this->getFromModels().size() );
    inputWeightedForce.resize( this->getFromModels().size() );

    std::transform(this->getFromModels().begin(), this->getFromModels().end(), inputBaseMass.begin(), Operation<In>::fetchMass );

    std::transform(this->getFromModels().begin(), this->getFromModels().end(), inputBaseMass.begin(), inputTotalMass.begin(), Operation<In>::computeTotalMass );

    invTotalMass = 0.0;
    for ( iter_double iter = inputTotalMass.begin() ; iter != inputTotalMass.end() ; ++ iter )
    {
        invTotalMass += *iter;
    }
    invTotalMass = 1.0/invTotalMass;
    Inherit::init();

    if (this->getToModels()[0]) this->getToModels()[0]->resize(1);
}

template< class BasicMultiMapping >
void CenterOfMassMultiMapping< BasicMultiMapping >::draw()
{
    assert( this->toModels.size() == 1 );
    OutVecCoord* X = this->getToModels()[0]->getVecCoord( VecId::position().index );

    std::vector< Vector3 > points;
    Vector3 point1,point2;

    unsigned int sizePoints= (OutCoord::static_size <=3)?OutCoord::static_size:3;
    for(unsigned int i=0 ; i<OutCoord::static_size ; i++)
    {
        OutCoord v;
        v[i] = (Real)0.1;
        for (unsigned int s=0; s<sizePoints; ++s)
        {
            point1[s] = ( (*X)[0] -v)[s];
            point2[s] = ( (*X)[0] +v)[s];
        }
        points.push_back(point1);
        points.push_back(point2);
    }
    simulation::getSimulation()->DrawUtility.drawLines(points, 1, Vec<4,float>(1,1,0,1));

}


} // namespace mapping

} // namespace component

} // namespace sofa

#endif //SOFA_COMPONENT_MAPPING_CENTEROFMASSMULTIMAPPING_INL
