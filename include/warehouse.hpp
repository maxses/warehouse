#ifndef WAREHOUSE_HPP
#define WAREHOUSE_HPP
/**---------------------------------------------------------------------------
 *
 * @file       warehouse.hpp
 * @brief      Gui for database form
 *
 * See README.md for further information
 *
 * @date      20250729
 * @author    Maximilian Seesslen <dev@seesslen.net>
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 *--------------------------------------------------------------------------*/


/*--- Includes -------------------------------------------------------------*/


#include <QtWidgets>
#include <QtSql>

#include "ui_warehouse.h"


/*--- Declaration ----------------------------------------------------------*/


/** \brief  The main application window
 */
class CWarehouse: public QMainWindow
{
    Q_OBJECT
public:
    CWarehouse(const QString &databaseFile);
    
private:
    /** @brief initializes the database by loading file
     */
    QSqlError initDb(const QString &databaseFile) const;
        
private slots:
    /** @brief Show the "About"-Window
     */
    void about();

private:
    void showError(const QSqlError &err);
    void fillFormular(QGroupBox *groupBox, QSqlRelationalTableModel *model, QTableView *table);
    Ui::Warehouse ui;
    void addTab(const QString &table);

    void createMenuBar();

public:
    void dump(const QString name);
};


/*--- Fin ------------------------------------------------------------------*/
#endif // ? ! WAREHOUSE_HPP
