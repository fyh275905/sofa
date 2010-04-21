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
#include <sofa/helper/OptionsGroup.h>


namespace sofa
{

namespace helper
{

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
OptionsGroup::OptionsGroup() : textItems()
{
    selectedItem=0;
}
///////////////////////////////////////
OptionsGroup::OptionsGroup(const OptionsGroup & m_radiotrick) : textItems(m_radiotrick)
{
    selectedItem=m_radiotrick.getSelectedId();
}
///////////////////////////////////////
OptionsGroup & OptionsGroup::operator=(const OptionsGroup & m_radiotrick)
{
    textItems::resize(m_radiotrick.size());
    for(unsigned int i=0; i<textItems::size(); i++)
    {
        textItems::operator[](i)=m_radiotrick[i];
    }
    selectedItem=m_radiotrick.selectedItem;
    return *this;
}
///////////////////////////////////////
void OptionsGroup::setNames(int nbofRadioButton,...)
{
    textItems::resize(nbofRadioButton);
    va_list vl;
    va_start(vl,nbofRadioButton);
    for(unsigned int i=0; i<textItems::size(); i++)
    {
        const char * tempochar=va_arg(vl,char *);
        std::string  tempostring(tempochar);
        textItems::operator[](i)=tempostring;
    }
    va_end(vl);
    selectedItem=0;
}
///////////////////////////////////////
int OptionsGroup::isInButtonList(const std::string & tempostring) const
{
    for(unsigned int i=0; i<textItems::size(); i++)
    {
        if (textItems::operator[](i)==tempostring) return i;
    }
    return -1;
}
///////////////////////////////////////
void OptionsGroup::setSelectedItem(unsigned int id_item)
{
    if (id_item<textItems::size())
        selectedItem=id_item;
    //std::cout<<"OptionsGroup:: ==============================setted :"<< this->selectedItem << std::endl;
}
///////////////////////////////////////
void OptionsGroup::setSelectedItem(const std::string & m_string)
{
    int id_stringinButtonList = isInButtonList(m_string);
    if (id_stringinButtonList == -1)
    {
        std::cout<<"WARNING(OptionsGroup) : \""<< m_string <<"\" is not a parameter in button list :\" "<<(*this)<<"\""<< std::endl;
    }
    else
    {
        setSelectedItem(id_stringinButtonList);
    }
}
///////////////////////////////////////
unsigned int OptionsGroup::getSelectedId() const
{
    return selectedItem;
}
///////////////////////////////////////
std::string  OptionsGroup::getSelectedItem() const
{
    std::string checkedString;
    checkedString = textItems::operator[](selectedItem);
    return checkedString;
}
///////////////////////////////////////
void OptionsGroup::readFromStream(std::istream & stream)
{
    std::string tempostring;
    stream >> tempostring;
    int id_stringinButtonList = isInButtonList(tempostring);
    if (id_stringinButtonList == -1)
    {
        std::cout<<"WARNING(OptionsGroup) : \""<< tempostring <<"\" is not a parameter in button list :\" "<<(*this)<<"\""<< std::endl;
    }
    else
    {
        setSelectedItem(id_stringinButtonList);
    }
}
///////////////////////////////////////
void OptionsGroup::writeToStream(std::ostream & stream) const
{

    for(unsigned int i=0; i<textItems::size()-1; i++)
    {
        std::string tempostring= textItems::operator[](i);
        stream<< tempostring << " ";
    }
}
///////////////////////////////////////
void OptionsGroup::TestOptionsGroup() const
{
    sofa::helper::OptionsGroup m_radiotrick; m_radiotrick.setNames(3,"hello1","hello2","hello3");
    std::cout<<"Radio button :"<<m_radiotrick<<"    selectedId :"<<m_radiotrick.getSelectedId()<<"   getSelectedItem() :"<<m_radiotrick.getSelectedItem()<<std::endl;
    std::cin>>m_radiotrick;
    std::cout<<"Radio button :"<<m_radiotrick<<"    selectedId :"<<m_radiotrick.getSelectedId()<<"   getSelectedItem() :"<<m_radiotrick.getSelectedItem()<<std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace helper

} // namespace sofa
