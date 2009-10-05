/******************************************************************************
 *       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
 *                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the Free  *
 * Software Foundation; either version 2 of the License, or (at your option)   *
 * any later version.                                                          *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details.                                                               *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program; if not, write to the Free Software Foundation, Inc., 51  *
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
 *******************************************************************************
 *                            SOFA :: Applications                             *
 *                                                                             *
 * Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
 * H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
 * M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
 *                                                                             *
 * Contact information: contact@sofa-framework.org                             *
 ******************************************************************************/
#ifndef SOFA_GUI_OPERATIONFACTORY_H
#define SOFA_GUI_OPERATIONFACTORY_H
#include "SofaGUI.h"
#include <sofa/gui/MouseOperations.h>

#include <iostream>
#include <map>

namespace sofa
{

namespace gui
{


class OperationCreator
{
public:
    virtual ~OperationCreator() {};
    virtual Operation* create() const =0;
    virtual std::string getDescription() const=0;
    virtual bool isModifiable() const=0;
};

template<class RealOperation>
class TOperationCreator: public OperationCreator
{
public:
    Operation* create() const {return new RealOperation();};
    std::string getDescription() const { return RealOperation::getDescription();};
    bool isModifiable() const { return RealOperation::isModifiable();};
};



class SOFA_SOFAGUI_API OperationFactory
{
public:
    typedef std::map< std::string, OperationCreator* > RegisterStorage;
    RegisterStorage registry;

    static OperationFactory* getInstance()
    {
        static OperationFactory instance;
        return &instance;
    };

    static bool IsModifiable(const std::string &name)
    {
        const RegisterStorage &reg = getInstance()->registry;
        const RegisterStorage::const_iterator it = reg.find(name);
        if (it != reg.end())
        {
            return it->second->isModifiable();
        }
        else return false;

    }
    static std::string GetDescription(const std::string &name)
    {
        const RegisterStorage &reg = getInstance()->registry;
        const RegisterStorage::const_iterator it = reg.find(name);
        if (it != reg.end())
        {
            return it->second->getDescription();
        }
        else return std::string();

    }

    static Operation* Instanciate(const std::string &name)
    {
        const RegisterStorage &reg = getInstance()->registry;
        RegisterStorage::const_iterator it = reg.find(name);
        if (it != reg.end())
        {
            const OperationCreator *creator=it->second;
            return creator->create();
        }
        else return NULL;
    }

};

class RegisterOperation
{
public:
    std::string name;
    OperationCreator *creator;

    RegisterOperation(const std::string &n)
    {
        name = n;
    }

    template <class TOperation>
    void add()
    {
        creator = new TOperationCreator< TOperation >();
        OperationFactory::getInstance()->registry.insert(std::make_pair(name, creator));
    }
};


}
}

#endif
