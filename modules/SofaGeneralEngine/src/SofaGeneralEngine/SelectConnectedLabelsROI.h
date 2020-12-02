/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <SofaGeneralEngine/config.h>

#include <sofa/core/DataEngine.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/helper/vector.h>
#include <sofa/helper/vector.h>
#include <sofa/helper/vectorData.h>
#include <sofa/defaulttype/typeinfo/DataTypeInfo[Integer].h>



namespace sofa::component::engine
{

/**
 * Select a subset of points or cells labeled from different sources, that are connected given a list of connection pairs
 */

template <class _T>
class SelectConnectedLabelsROI : public sofa::core::DataEngine
{
public:
    typedef core::DataEngine Inherited;
    SOFA_CLASS(SOFA_TEMPLATE(SelectConnectedLabelsROI,_T),Inherited);

    typedef _T T;
    typedef unsigned int Index;

    //Input
    Data<unsigned int> d_nbLabels; ///< number of label lists
    typedef helper::vector<helper::vector<T> > VecVLabels;
    helper::vectorData<VecVLabels> d_labels;
    Data<helper::vector<T> > d_connectLabels; ///< Pairs of label to be connected accross different label lists

    //Output
    Data<helper::vector<Index> > d_indices; ///< selected point/cell indices

    /// Implementing the GetCustomTemplateName is mandatory to have a custom template name paremters
    /// instead of the default one generated automatically by the SOFA_CLASS() macro.
    static std::string GetCustomTemplateName(){       return sofa::defaulttype::DataTypeName<T>::name();    }

    SelectConnectedLabelsROI(): Inherited()
      , d_nbLabels ( initData ( &d_nbLabels,(unsigned int)0,"nbLabels","number of label lists" ) )
      , d_labels(this, "labels", "lists of labels associated to each point/cell", helper::DataEngineInput)
      , d_connectLabels ( initData ( &d_connectLabels,"connectLabels","Pairs of label to be connected accross different label lists" ) )
      , d_indices ( initData ( &d_indices,"indices","selected point/cell indices" ) )
    {
        d_labels.resize(d_nbLabels.getValue());
    }

    ~SelectConnectedLabelsROI() override {}

    void init() override
    {
        addInput(&d_nbLabels);
        d_labels.resize(d_nbLabels.getValue());
        addInput(&d_connectLabels);
        addOutput(&d_indices);
        setDirtyValue();
    }

    void reinit() override
    {
        d_labels.resize(d_nbLabels.getValue());
        update();
    }


    /// Parse the given description to assign values to this object's fields and potentially other parameters
    void parse ( sofa::core::objectmodel::BaseObjectDescription* arg ) override
    {
        d_labels.parseSizeData(arg, d_nbLabels);
        Inherit1::parse(arg);
    }

    /// Assign the field values stored in the given map of name -> value pairs
    void parseFields ( const std::map<std::string,std::string*>& str ) override
    {
        d_labels.parseFieldsSizeData(str, d_nbLabels);
        Inherit1::parseFields(str);
    }

protected:


    void doUpdate() override
    {
        helper::WriteOnlyAccessor< Data< helper::vector<Index> > > indices = d_indices;
        indices.clear();

        unsigned int nb = d_nbLabels.getValue();
        if(nb<2) return;

        // convert connectLabels to set for efficient look-up
        helper::ReadAccessor<Data< helper::vector<T> > > connectL(this->d_connectLabels);
        typedef std::pair<T,T>  TPair;
        std::set<TPair> connectS;
        for(unsigned int i=0;i<connectL.size()/2;i++)
        {
            connectS.insert(TPair(connectL[2*i],connectL[2*i+1]));
            connectS.insert(TPair(connectL[2*i+1],connectL[2*i]));
        }

        // read input label lists
        std::vector<const VecVLabels* > labels;
        for(unsigned int l=0; l<nb ; ++l)
        {
            helper::ReadAccessor< Data<VecVLabels> > rlab(d_labels[l]);
            labels.push_back(&rlab.ref());
        }

        size_t nbp = (*labels[0]).size();
        for(size_t i=0; i<nbp;i++)
        {
            bool connected = false;
            for(unsigned int l1=0; l1<nb && !connected;l1++)
                for(size_t i1=0; i1<(*labels[l1])[i].size() && !connected;i1++)
                    for(unsigned int l2=0; l2<nb && !connected;l2++)
                        if(l1!=l2)
                            for(size_t i2=0; i2<(*labels[l2])[i].size() && !connected;i2++)
                                if(connectS.find(TPair((*labels[l1])[i][i1],(*labels[l2])[i][i2]))!=connectS.end())
                                    connected=true;
            if(connected)
                indices.push_back((Index)i);
        }
    }

};


} //namespace sofa::component::engine
