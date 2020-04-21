/**---------------------------------------------------------------------------
 *
 * @file       warehouse.cpp
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


#include <warehouse.hpp>
//#include <warehousedelegate.hpp>
#include <tab.hpp>
#include <QtSql>


/*--- Implementation -------------------------------------------------------*/


CWarehouse::CWarehouse(const QString &databaseFile)
{
    ui.setupUi(this);

    setWindowTitle( "Warehouse " + databaseFile );

    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(
                    this,
                    "Unable to load database driver",
                    "This application needs the SQLITE driver"
                    );

    // Initialize the database:
    QSqlError err = initDb(databaseFile);
    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }

    createMenuBar();

    QStringList tables=QSqlDatabase::database().tables();

    for(QString table :tables)
    {
      addTab(table);
    }

}


QSqlError CWarehouse::initDb(const QString &databaseFile) const
{
   QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName(databaseFile);
   
   if (!db.open())
      return db.lastError();

   QStringList tables = db.tables();
   
   return QSqlError();
}


void CWarehouse::addTab(const QString &table)
{
   CWarehouseTab *tab=new CWarehouseTab(table);
   ui.tabWidget->addTab(tab, QString());
   tab->setObjectName(QString::fromUtf8("tab"));
   ui.tabWidget->setTabText(ui.tabWidget->indexOf(tab), table);
   
   return;
}


void CWarehouse::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}


void CWarehouse::createMenuBar()
{
    QAction *quitAction = new QAction(tr("&Quit"), this);
    QAction *aboutAction = new QAction(tr("&About"), this);
    QAction *aboutQtAction = new QAction(tr("&About Qt"), this);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(quitAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    connect(quitAction, &QAction::triggered, this, &CWarehouse::close);
    connect(aboutAction, &QAction::triggered, this, &CWarehouse::about);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}


void CWarehouse::about()
{
    QMessageBox::about(this, tr("About 'Warehouse'"),
            tr("<p>Dynamic form creation from SQLdatabases.</p>"));
}


void CWarehouse::dump(const QString name)
{
   qWarning("Dumping %s", qPrintable(name) );
   for(int i1=0; i1< ui.tabWidget->count(); i1++)
   {
      if( ui.tabWidget->tabText(i1) == name )
      {
         CWarehouseTab *tab=dynamic_cast<CWarehouseTab *>( ui.tabWidget->widget(i1) );
         if( !tab )
         {
            qWarning("Could not cast widget to CTab");
         }
         else
         {
            tab->dump();
         }
         return;
      }
   }
   qWarning("Could not find table '%s'.", qPrintable(name));
}


/*--- Fin ------------------------------------------------------------------*/
