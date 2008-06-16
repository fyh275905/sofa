/****************************************************************************
*																			*
*		Copyright: See COPYING file that comes with this distribution		*
*																			*
****************************************************************************/
#ifndef SOFA_COMPONENT_MISC_MONITOR_H
#define SOFA_COMPONENT_MISC_MONITOR_H
#include <sofa/core/VisualModel.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>


namespace sofa
{

namespace component
{

namespace misc
{

using namespace core::objectmodel;
using namespace std;

template <class DataTypes>
class Monitor: public virtual core::VisualModel
{
public:

    typedef typename DataTypes::VecReal VecReal;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;

    class MonitorData
    {
    public:

        MonitorData()
        {
        };

        /*copy mechanical state vectors (forces, positions, velocities)
        *  into internal state vectors
        */
        void setValues ( VecDeriv* _vels, VecDeriv* _forces, VecCoord* _pos )
        {
            vels = _vels;
            forces = _forces;
            pos = _pos;
        }

        /////////////////Manipulate posIndices vector//////////////////////

        //copy the given vector into the posIndices vector
        void setIndPos ( sofa::helper::vector < int > &_IdxPos )
        {
            posIndices = _IdxPos;
        }

        //copy the given vector into the initPosIndices vector (used at initialization)
        void setIndPosInit ( sofa::helper::vector < int > &_IdxPos )
        {
            initPosIndices = _IdxPos;
        }

        //accessor to the indices vector of monitored positions
        const sofa::helper::vector < int > &getIndPos() const
        {
            return posIndices;
        }

        //accessor to the initial indices vector of monitored positions (used by reset())
        const sofa::helper::vector < int > &getIndPosInit () const
        {
            return initPosIndices;
        }

        ////////////////////Manipulate posVels vector//////////////////////

        //copy the given vector into the velsIndices vector
        void setIndVels ( sofa::helper::vector < int > &_IdxVels )
        {
            velsIndices = _IdxVels;
        }

        //copy the given vector into the initVelsIndices vector (used at initialization)
        void setIndVelsInit ( sofa::helper::vector < int > &_IdxVels )
        {
            initVelsIndices = _IdxVels;
        }

        //accessor to the indices vector of monitored velocities
        const sofa::helper::vector < int > &getIndVels() const
        {
            return velsIndices;
        }

        //accessor to the initial indices vector of monitored velocities (used by reset())
        const sofa::helper::vector < int > &getIndVelsInit () const
        {
            return initVelsIndices;
        }

        //////////////////Manipulate posForces vector//////////////////////

        //copy the given vector into the forcesIndices vector
        void setIndForces ( sofa::helper::vector < int > &_IdxForces )
        {
            forcesIndices = _IdxForces;
        }

        //copy the given vector into the initForcesIndices vector (used at initialization)
        void setIndForcesInit ( sofa::helper::vector < int > &_IdxForces )
        {
            initForcesIndices = _IdxForces;
        }

        //accessor to the indices vector of monitored forces
        const sofa::helper::vector < int > &getIndForces() const
        {
            return forcesIndices;
        }

        //accessor to the initial indices vector of monitored forces (used by reset())
        const sofa::helper::vector < int > &getIndForcesInit () const
        {
            return initForcesIndices;
        }

        ///////////////accessors to size of indices vectors////////////////

        //return the size of the indice vector of positions
        unsigned int sizeIdxPos() const
        {
            return posIndices.size();
        }

        //return the size of the indice vector of velocities
        unsigned int sizeIdxVels() const
        {
            return velsIndices.size();
        }

        //return the size of the indice vector of forces
        unsigned int sizeIdxForces() const
        {
            return forcesIndices.size();
        }

        /////////////accessors to size of mechancal state vectors//////////

        //return the size of mechanical state position vector
        unsigned int getSizeVecPos() const
        {
            return pos -> size();
        }

        //return the size of mechanical state velocity vector
        unsigned int getSizeVecVels() const
        {
            return vels -> size();
        }

        //return the size of mechanical state force vector
        unsigned int getSizeVecForces() const
        {
            return forces -> size();
        }

        ///////////////////////////////////////////////////////////////////

        //clear the indices vectors of monitored particles
        void clearVecIndices()
        {
            posIndices.clear();
            velsIndices.clear();
            forcesIndices.clear();
        }


        inline friend std::ostream& operator << ( std::ostream& out, const MonitorData& m )
        {
            out << "P";
            out << m.sizeIdxPos();
            out << " [";
            for (unsigned int i = 0; i < m.sizeIdxPos(); i++)
                out << (m.getIndPos())[i] << " ";

            out << "] ";
            out << "V";
            out << m.sizeIdxVels();
            out << " [";
            for (unsigned int i = 0; i < m.sizeIdxVels(); i++)
                out << (m.getIndVels())[i] << " ";

            out << "] ";
            out << "F";
            out << m.sizeIdxForces();
            out << " [";
            for (unsigned int i = 0; i < m.sizeIdxForces(); i++)
                out << (m.getIndForces())[i] << " ";

            return out;
        }

        /**Indices vector initialization must be written like :
        *	P"Number of particles to monitor" [ "indices of the particles" ]
        *	V"Number of particles to monitor" [ "indices of the particles" ]
        *	F"Number of particles to monitor" [ "indices of the particles" ]
        *	Example : P2 [0 1] V4 [3 2 1 0] F0 []
        **/
        inline friend std::istream& operator >> ( std::istream& in, MonitorData &m )
        {
            char temp;
            int nbParticles = 0;
            int number = 0;

            in >> temp; // skip the "P"
            in >> nbParticles; //number of positions to monitor
            in >> temp; //skip the "["
            for (int i = 0; i < nbParticles; i++)
            {
                in >> number;
                m.posIndices.push_back(number);
            }

            in >> temp; //skip the "]"
            in >> temp; //skip the "V"
            in >> nbParticles; //number of velocities to monitor
            in >> temp; //skip the "["
            for (int i = 0; i < nbParticles; i++)
            {
                in >> number;
                m.velsIndices.push_back(number);
            }

            in >> temp; //skip the "]"
            in >> temp; //skip the "F"
            in >> nbParticles; //number of forces to monitor
            in >> temp; //skip the "["
            for (int i = 0; i < nbParticles; i++)
            {
                in >> number;
                m.forcesIndices.push_back(number);
            }
            return in;
        }

        //return the position vector of particle number "index"
        const Coord getPos ( unsigned int index ) const
        {
            return pos[0][posIndices[index]];
        }

        //return the velocity vector of particle number "index"
        const Deriv getVel ( unsigned int index ) const
        {
            return vels[0][velsIndices[index]];
        }

        //return the force vector of particle number "index"
        const Deriv getForce ( unsigned int index ) const
        {
            return forces[0][forcesIndices[index]];
        }

    protected:

        ///indices of the particles which we want to monitor positions
        sofa::helper::vector < int > posIndices;
        ///indices of the particles which we want to monitor velocities
        sofa::helper::vector < int > velsIndices;
        ///indices of the particles which we want to monitor forces
        sofa::helper::vector < int > forcesIndices;

        ///indices of the original particles positions wanted (used by reset function)
        sofa::helper::vector < int > initPosIndices;
        ///indices of the original particles velocities wanted (used by reset function)
        sofa::helper::vector < int > initVelsIndices;
        ///indices of the original particles forces wanted (used by reset function)
        sofa::helper::vector < int > initForcesIndices;

        VecDeriv* vels;
        VecDeriv* forces;
        VecCoord* pos;
    };

    Data < bool > saveXToGnuplot;
    Data < bool > saveVToGnuplot;
    Data < bool > saveFToGnuplot;

    Data < MonitorData > monitoring;

    Monitor ();
    ~Monitor ();

    ///set the indices of particles of which we want to monitor positions
    void setIndForces ( sofa::helper::vector < int > &_IdxForces );

    ///set the indices of particles of which we want to monitor velocities
    void setIndVels ( sofa::helper::vector < int > &_IdxVels );

    ///set the indices of particles of which we want to monitor forces
    void setIndPos ( sofa::helper::vector < int > &_IdxPos );

    //init data
    virtual void init ();

    //reset monitored values
    virtual void reset ();

    /**initialize gnuplot files
    *called when ExportGnuplot box is checked
    */
    virtual void reinit();

    /**function called at every step of simulation;
    *store mechanical state vectors (forces, positions, velocities) into
    *the MonitorData nested class. The filter (which position(s), velocity(ies) or *force(s) are displayed) is made in the gui
    */
    virtual void fwdDraw ( Pass );

    ///create gnuplot files
    virtual void initGnuplot ( const std::string path );

    ///write in gnuplot files the monitored desired data (velocities,positions,forces)
    virtual void exportGnuplot ( Real time );


protected:

    std::ofstream* saveGnuplotX;
    std::ofstream* saveGnuplotV;
    std::ofstream* saveGnuplotF;

    core::componentmodel::behavior::MechanicalState<DataTypes>* mmodel;
};

} // namespace misc

} // namespace component

} // namespace sofa

#endif
