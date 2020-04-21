#ifndef WAREHOUSE_TAB_H
#define WAREHOUSE_TAB_H
/**---------------------------------------------------------------------------
 *
 * @file       CWarehouseTab.hpp
 * @brief      Single tab for a table of database
 *
 * See README.md for further information
 *
 * @date      20250729
 * @author    Maximilian Seesslen <dev@seesslen.net>
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 *--------------------------------------------------------------------------*/


/*--- Includes -------------------------------------------------------------*/


#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QGroupBox>
#include <QTableView>
#include <QGridLayout>
#include <QDataWidgetMapper>
#include <QItemDelegate>


/*--- Declaration ----------------------------------------------------------*/


namespace Ui {
class Tab;
}

class CWarehouseTab : public QWidget
{
   Q_OBJECT

public:
   explicit CWarehouseTab(const QString &table, QWidget *parent = nullptr);
   ~CWarehouseTab();
   int adjustCWarehouseTable();
   void buildFormular(QGroupBox *groupBox
            , QSqlRelationalTableModel *model, QTableView *CWarehouseTable);
   
   /** @bried Show error in dialog box
    */
   void showError(const QSqlError &err);

private slots:
   
   /** @bried Slot for signal when Add/'+' was pressed
    */
   void addPressed();
    
    /** @bried Slot for signal when Del/'-' was pressed
    */
   void removePressed();
   void searchChanged( const QString &line );
   void searchChangedId(const QString &line);

private:
   Ui::Tab *ui;
   QSqlRelationalTableModel *model;
   QString m_table;
   QDataWidgetMapper *m_mapper;
   QGridLayout *m_gridLayout;
   
   
   /** @bried Create an Qt widget depending on the data type of 'field'
    */
   QWidget *createFormularWidget(QGroupBox *groupBox
                              , QSqlField &field, QModelIndex &index ) const;
   bool isForeignKey(const QString &name) const;
   QString foreignKeyTable(const QString &name) const;
   void updateCount() const;
   bool isMultiLine(const QString &name) const;
   void updateRelation();

public:
   void dump();

};


/*--- Fin ------------------------------------------------------------------*/
#endif // ? ! WAREHOUSE_TAB_H
