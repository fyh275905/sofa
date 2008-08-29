
/******************************************************************************
 *       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
 *                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef ADDPRESET_H
#define ADDPRESET_H

#include "DialogAddPreset.h"
#include <sofa/simulation/tree/GNode.h>

namespace sofa
{

namespace gui
{

namespace qt
{
using sofa::simulation::tree::GNode;

class AddPreset : public DialogAddPreset
{
    Q_OBJECT
public:

    AddPreset(  QWidget* parent, const char* name= 0, bool  modal= FALSE, Qt::WFlags f= 0 );
    void setElementPresent(bool *elementPresent);
    void setParentNode(GNode* parentNode) {node=parentNode;}
    void setPresetFile(std::string p) {presetFile=p;}
    void setPath(std::string p) {fileName=p;}
    void clear();

public slots:
    void fileOpen();
    void accept();

signals:
    void loadPreset(GNode*,std::string,std::string*, std::string*,std::string*,std::string);



protected:
    std::string fileName;
    std::string presetFile;
    GNode *node;
};

} // namespace qt

} // namespace gui

} // namespace sofa

#endif
