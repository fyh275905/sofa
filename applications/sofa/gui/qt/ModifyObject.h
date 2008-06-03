/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This program is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU General Public License as published by the Free   *
* Software Foundation; either version 2 of the License, or (at your option)    *
* any later version.                                                           *
*                                                                              *
* This program is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
* more details.                                                                *
*                                                                              *
* You should have received a copy of the GNU General Public License along with *
* this program; if not, write to the Free Software Foundation, Inc., 51        *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                    *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef MODIFYOBJECT_H
#define MODIFYOBJECT_H


#include <sofa/core/objectmodel/BaseObject.h>
#include "RealGUI.h"

#include <sofa/component/topology/PointData.h>
#include <sofa/component/forcefield/SpringForceField.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/simulation/common/Node.h>

#ifdef QT_MODULE_QT3SUPPORT
#include <QDialog>
#include <Q3ListViewItem>
#include <Q3ListView>
#include <Q3Table>
#include <Q3GroupBox>
#include <Q3TextEdit>
#else
#include <qdialog.h>
#include <qlistview.h>
#include <qtable.h>
#include <qgroupbox.h>
#include <qtextedit.h>
#endif


#include <qwt_plot.h>

#include <qwt_plot_curve.h>

class QPushButton;

namespace sofa
{

namespace gui
{

namespace qt
{

using sofa::helper::Quater;
using sofa::defaulttype::Vec;
using sofa::simulation::Node;

#ifndef QT_MODULE_QT3SUPPORT
typedef QListViewItem Q3ListViewItem;
typedef QTable    Q3Table;
typedef QGroupBox Q3GroupBox;
typedef QTextEdit   Q3TextEdit;
#endif
class ModifyObject : public QDialog
{
    Q_OBJECT
public:

    ModifyObject( void *Id, core::objectmodel::Base* node, Q3ListViewItem* item_clicked, QWidget* parent, const char* name= 0, bool  modal= FALSE, Qt::WFlags f= 0 );
    ~ModifyObject()
    {
        delete buttonUpdate;
    }

    void setNode(core::objectmodel::Base* node, Q3ListViewItem* item_clicked=NULL); //create all the widgets of the dialog window

public slots:
    void updateValues();              //update the node with the values of the field
    void updateTextEdit();            //update the text fields due to unknown data field
    void updateTables();              //update the tables of value at each step of the simulation
    void saveTables();                //Save in datafield the content of a QTalbe
    void saveTextEdit();                //Save in datafield the content of a QTextEdit
    void changeValue();               //each time a field is modified
    void changeVisualValue();               //each time a field of the Visualization tab is modified
    void changeNumberPoint();         //used to dynamically add points in an object of type pointSubset
    void closeNow () {emit(reject());} //called from outside to close the current widget
    void reject   () {                 emit(dialogClosed(Id)); deleteLater(); QDialog::reject();} //When closing a window, inform the parent.
    void accept   () { updateValues(); emit(dialogClosed(Id)); deleteLater(); QDialog::accept();} //if closing by using Ok button, update the values

signals:
    void objectUpdated();              //update done
    void dialogClosed(void *);            //the current window has been closed: we give the Id of the current window
    void transformObject(Node * current_node, double translationX, double translationY, double translationZ,
            double rotationX, double rotationY, double rotationZ,
            double scale);


protected:

    virtual void closeEvent ( QCloseEvent * ) {emit(reject());}
    void updateContext( Node *node );

    void createGraphMass(QTabWidget *);
    void updateHistory();
    void updateEnergy();

    bool createTable(core::objectmodel::BaseData* field, Q3GroupBox *box=NULL, Q3Table* vectorTable=NULL, Q3Table* vectorTable2=NULL );
    void storeTable(Q3Table* table, core::objectmodel::BaseData* field);

    void createVector(const Quater<double> &value, Q3GroupBox *box); //will be created as a Vec<4,double>
    void createVector(const Quater<float>  &value, Q3GroupBox *box); //will be created as a Vec<4,float>

    //*********************************************************
    template< int N, class T>
    void createVector(const Vec<N,T> &value, Q3GroupBox *box);
    template< int N, class T>
    void storeVector(std::list< QObject *>::iterator &list_it, Data< Vec<N,T> > *ff);
    template<class T>
    void storeVector(std::list< QObject *>::iterator &list_it, Data< Quater<T> > *ff);
    template< int N, class T>
    void storeVector(std::list< QObject *>::iterator &list_it, DataPtr< Vec<N,T> > *ff);
    template<class T>
    void storeVector(std::list< QObject *>::iterator &list_it, DataPtr< Quater<T> > *ff);
    template< int N, class T>
    void storeVector(std::list< QObject *>::iterator &list_it, Vec<N,T> *ff);
    template<class T>
    void storeVector(std::list< QObject *>::iterator &list_it, Quater<T> *ff);
    //*********************************************************
    template< class T>
    bool createQtTable(Data< sofa::helper::vector< T > > *ff, Q3GroupBox *box, Q3Table* vectorTable );
    template<class T>
    void storeQtTable( Q3Table* table, Data< sofa::helper::vector< T > >* ff );

    void storeQtTable( Q3Table* table, Data< sofa::helper::vector< std::string > >* ff );
    //*********************************************************
    template< class T>
    bool createQtTable(DataPtr< sofa::helper::vector< T > > *ff, Q3GroupBox *box, Q3Table* vectorTable );
    template< class T>
    void storeQtTable( Q3Table* table, DataPtr< sofa::helper::vector< T > >* ff );
    //*********************************************************
    template< int N, class T>
    bool createQtTable(Data< sofa::helper::vector< Vec<N,T> > > *ff, Q3GroupBox *box, Q3Table* vectorTable );
    template< int N, class T>
    void storeQtTable( Q3Table* table, Data< sofa::helper::vector< Vec<N,T> > >* ff );
    //*********************************************************
    template< int N, class T>
    bool createQtTable(DataPtr< sofa::helper::vector< Vec<N,T> > > *ff, Q3GroupBox *box, Q3Table* vectorTable );
    template< int N, class T>
    void storeQtTable( Q3Table* table, DataPtr< sofa::helper::vector< Vec<N,T> > >* ff );
    //*********************************************************
    template< class T>
    bool createQtTable(Data< sofa::component::topology::PointData< T > > *ff, Q3GroupBox *box, Q3Table* vectorTable );
    template< class T>
    void storeQtTable( Q3Table* table, Data< sofa::component::topology::PointData< T > >* ff );
    //*********************************************************


    QWidget *parent;
    QTabWidget *dialogTab;
    core::objectmodel::Base* node;
    Q3ListViewItem * item;
    QPushButton *buttonUpdate;
    std::list< QObject* >                         list_Object;
    std::list< std::list< QObject* > * >          list_PointSubset;
    std::list< std::pair< Q3Table*, core::objectmodel::BaseData*> > list_Table;
    std::list< std::pair< Q3TextEdit*, core::objectmodel::BaseData*> > list_TextEdit;
    std::map< core::objectmodel::BaseData*, int > dataIndexTab;
    void *Id;
    bool visualContentModified;
    std::vector< double > history;
    std::vector< double > energy_history[3];
    QwtPlot *graphEnergy;
    QwtPlotCurve *energy_curve[3];
    unsigned int counterWidget;
};

} // namespace qt

} // namespace gui

} // namespace sofa

#endif
