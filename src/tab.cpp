/**---------------------------------------------------------------------------
 *
 * @file       CWarehouseTab.cpp
 * @brief      Single CWarehouseTab for a CWarehouseTable of database
 *
 * See README.md for further information
 *
 * @date      20250729
 * @author    Maximilian Seesslen <dev@seesslen.net>
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 *--------------------------------------------------------------------------*/


/*--- Includes -------------------------------------------------------------*/


#include <tab.hpp>
#include "ui_tab.h"
#include <QSqlError>
#include <QDataWidgetMapper>
#include <QLabel>
#include <QTextEdit>
#include <QTextBrowser>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSpinBox>
#include <QSqlRelationalDelegate>


/*--- Implementation -------------------------------------------------------*/


CWarehouseTab::CWarehouseTab(const QString &table, QWidget *parent)
   :QWidget(parent)
   ,ui(new Ui::Tab)
   ,m_table(table)
{
   ui->setupUi(this);

   // Create the data model:
   model = new QSqlRelationalTableModel(ui->tableRows);
   // Vs.: QSqlCWarehouseTableModel::OnManualSubmit);
   model->setEditStrategy( QSqlTableModel::OnFieldChange );

   model->setTable(m_table);

   // Set the model, hide ID column:
   ui->tableRows->setModel(model);

   // Vs.: QAbstractItemView::SingleSelection
   ui->tableRows->setSelectionMode(QAbstractItemView::ExtendedSelection);

   // Lock and prohibit resizing of the width of the rating column:
   ui->tableRows->horizontalHeader()->setSectionResizeMode(
               0, QHeaderView::ResizeToContents);

   buildFormular(ui->groupBox, model, ui->tableRows);
   if (!model->select()) {
       showError(model->lastError());
       return;
   }
   updateCount();

   ui->tableRows->setCurrentIndex(model->index(0, 0));

   adjustCWarehouseTable();

   connect(ui->lineSearch, SIGNAL( textChanged( const QString & ) ), this, SLOT(searchChanged( const QString & )));
   connect(ui->lineSearchId, SIGNAL( textChanged( const QString & ) ), this, SLOT(searchChangedId( const QString & )));
   connect(ui->pushAdd, SIGNAL(pressed()), this, SLOT(addPressed()));
   connect(ui->pushRemove, SIGNAL(pressed()), this, SLOT(removePressed()));

}


CWarehouseTab::~CWarehouseTab()
{
   delete ui;
}


int CWarehouseTab::adjustCWarehouseTable()
{
   int nameIndex=0;

   //qWarning() << "Adjusting " << model->columnCount() << " fields";
   for(int i1=0; i1<model->columnCount(); i1++)
   {
      QString fieldName=model->record().field(i1).name();
      if( fieldName != "Name" )
      {
         // qWarning() << "Hide " << model->fieldIndex( fieldName );
         ui->tableRows->setColumnHidden(model->fieldIndex( fieldName ), true );
      }
      else
      {
         nameIndex=i1;
      }
   }

   ui->tableRows->setColumnWidth(nameIndex, 512*10 );

   return(nameIndex);
}


void CWarehouseTab::buildFormular(QGroupBox *groupBox
         , QSqlRelationalTableModel *model, QTableView *CWarehouseTable)
{
   m_gridLayout=new QGridLayout(groupBox);
   m_gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

   m_mapper = new QDataWidgetMapper(this);
   m_mapper->setModel(model);
   m_mapper->setItemDelegate( new QSqlRelationalDelegate(this) );
   int yPos=0;

   for(int i1=0; i1<model->columnCount(); i1++)
   {
      QSqlRecord record = model->database().record(model->tableName());
      QSqlField field = record.field(i1);

      if(field.name() == "id")
      {
         // We may continue if we dont want to see the id
         //continue;
      }
      QLabel *label=new QLabel(groupBox);
      label->setObjectName(QString::fromUtf8("label"));
      m_gridLayout->addWidget(label, yPos, 0, 1, 1);

      QWidget *editElement=nullptr;
      QString fieldName=field.name();

      QModelIndex modelIndex = model->index(0, yPos);

      int fieldIndex=model->fieldIndex( fieldName );

      if(fieldIndex<0)
      {
         qWarning("Could not find field index for '%s'", qPrintable( fieldName ));
         continue;
      }

      label->setText( fieldName.split("_")[0] );
      editElement=createFormularWidget(groupBox, field, modelIndex );

      if(editElement)
      {
         m_gridLayout->addWidget(editElement, yPos, 1, 1, 1);
      }

      if(!editElement)
      {
         qFatal("no element");
      }
      yPos++;
   }

   connect(CWarehouseTable->selectionModel(), &QItemSelectionModel::currentRowChanged,
           m_mapper, &QDataWidgetMapper::setCurrentModelIndex );

   QSpacerItem *verticalSpacer = new QSpacerItem(358, 182, QSizePolicy::Minimum, QSizePolicy::Expanding);
   m_gridLayout->addItem(verticalSpacer, 10, 0, 1, 2);

   updateRelation();
}


void CWarehouseTab::updateRelation()
{
   QSqlRelationalTableModel *model=
         dynamic_cast<QSqlRelationalTableModel *> ( m_mapper->model() );
   int yPos=0;

   for(int i1=0; i1<model->columnCount(); i1++)
   {
      QSqlRecord record = model->database().record(model->tableName());
      QSqlField field = record.field(i1);
      QWidget *editElement=nullptr;

      if(field.name() == "id")
      {
         // We may continue if we dont want to see the id
         //continue;
      }
      QString fieldName=field.name();
      QModelIndex modelIndex = model->index(0, yPos);
      int fieldIndex=model->fieldIndex( fieldName );

      editElement=m_gridLayout->itemAtPosition(yPos, 1)->widget();

      if( (field.name() != "id") && isForeignKey( field.name() ))
      {
         model->setRelation(fieldIndex, QSqlRelation( foreignKeyTable(fieldName), "id", "Name"));
         QComboBox *comboBox=dynamic_cast<QComboBox *>(editElement);
         if(comboBox == nullptr)
         {
            qFatal("Could not get Widget");
         }
         comboBox->setModel(model->relationModel(fieldIndex));
         qWarning("Model! %s - %d", qPrintable(foreignKeyTable(fieldName))
                  , fieldIndex);
         comboBox->setModelColumn(
                     model->relationModel(fieldIndex)->fieldIndex("Name"));
      }

      m_mapper->addMapping(editElement, model->fieldIndex( field.name() ));

      yPos++;
   }
}


void CWarehouseTab::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize database",
                "Error initializing database: " + err.text());
}


void CWarehouseTab::addPressed()
{
   QSqlRecord record = model->database().record(model->tableName());

   for(int i1=0; i1<record.count(); i1++)
   {
      QSqlField field = record.field(i1);
      QString fieldName=field.name();

      // Need to be done or record is invalid
      if(isForeignKey(fieldName))
      {
         record.setValue(i1, 1);
      }
   }
   bool sta=model->insertRecord(-1, record);

   model->submitAll();
   // model->database().commit();
   model->select();
   updateCount();
}


void CWarehouseTab::removePressed()
{
   model->removeRow( ui->tableRows->currentIndex().row() );
   model->select();
   updateCount();

   qWarning("Removing");
}


void CWarehouseTab::searchChanged(const QString &line)
{
   if ( line.size() || false )
   {
      QString search;
      QSqlRecord record = model->database().record(model->tableName());
      
      for(int i1=0; i1<model->columnCount(); i1++)
      {
         QSqlField field = record.field(i1);
         if(i1)
         {
            search+= " OR ";
         }
         search += model->tableName() + ".'" + field.name() + "' LIKE '%%" + line + "%%'";
      }
      
      model->setFilter( search );
      bool ret=false;
      ret=model->select();
   }
   else
   {
      model->setFilter( QString() );
      model->select();
   }
   updateCount();
}


void CWarehouseTab::searchChangedId(const QString &line)
{
   if ( line.size() || false )
   {
      /* Take care that the CWarehouseTable name is given on views with CWarehouseTable relations */
      QString search=QString(
                           model->tableName() + ".id=" + line
                           );
      model->setFilter( search );
      bool ret=false;
      ret=model->select();
   }
   else
   {
      model->setFilter( QString() );
      model->select();
   }
}


QWidget *CWarehouseTab::createFormularWidget(QGroupBox *groupBox
                           , QSqlField &field, QModelIndex &index ) const
{
   QWidget *widget=nullptr;
   QVariant::Type type=field.type();

   if( isForeignKey(field.name()))
   {
      type=QVariant::Type::Map;
   }

   if( isMultiLine(field.name()))
   {
      type=QVariant::Type::StringList;
   }

   switch ( type )
   {
      case QVariant::Type::String:
      {
         QLineEdit *lineEdit;
         lineEdit = new QLineEdit(groupBox);
         lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
         lineEdit->setEnabled(true);
         lineEdit->setText( index.data().toString() );
         widget=lineEdit;
         break;
      }
      case QVariant::Type::Int:
      {
         if(field.name()=="id")
         {
            QLineEdit *line = new QLineEdit(groupBox);
            line->setObjectName(QString::fromUtf8("label"));
            line->setEnabled(false);
            line->setAlignment(Qt::AlignRight);
            widget=line;
         }
         else
         {
            QSpinBox *spinBox = new QSpinBox(groupBox);
            spinBox->setObjectName(QString::fromUtf8("spinBox"));
            spinBox->setEnabled(true);
            spinBox->setMaximum( 1<<30 );
            widget=spinBox;
         }
         break;
      }
      case QVariant::Type::Map:
      {
         QComboBox *comboBox;
         comboBox = new QComboBox(groupBox);
         comboBox->setObjectName(QString::fromUtf8("comboBox"));
         comboBox->setEnabled(true);
         widget=comboBox;
         break;
      }
      case QVariant::Type::StringList:
      {
         /*
         QTextBrowser *textEdit;
         textEdit = new QTextBrowser(groupBox);
         */
         QTextEdit *textEdit;
         textEdit = new QTextEdit(groupBox);
         textEdit->setObjectName(QString::fromUtf8("lineEdit"));
         textEdit->setEnabled(true);
         textEdit->setAcceptRichText(true);
         //QTextDocument
         //textEdit->setDocument()
         //textEdit->setHtml( index.data().toString() );
         //textEdit->set
         //textEdit->setMarkdown( index.data().toString() );
         widget=textEdit;
         break;
      }
      default:
      {
         qWarning("unknonw data type: %d", (int)field.type() );
         break;
      }
   }

   return(widget);
}


bool CWarehouseTab::isForeignKey(const QString &name) const
{
   bool foreign=false;
   QStringList list=name.split("_");

   if( ( list.size() >= 3 ) && ( list[1].toLower() == "id" ) )
   {
      foreign=true;
   }

   return(foreign);
}


QString CWarehouseTab::foreignKeyTable(const QString &name) const
{
   QString foreignCWarehouseTable;
   QStringList list=name.split("_");

   if( ( list.size() >= 3 ) && ( list[1].toLower() == "id" ) )
   {
      foreignCWarehouseTable=list[0];
   }

   return(foreignCWarehouseTable);
}

void CWarehouseTab::updateCount() const
{
   int modelCount;
   QSqlQuery query( QString("SELECT id, Name FROM %1").arg(m_table) );
   int databaseCount = query.size();
   databaseCount=0;

   qWarning() << "Checking CWarehouseTable '" << m_table << "'";

   // model->rowCount() is limitted to 256 for some reason. Nevertheless all
   // items are shown in the GUI. We have to iterate manually over the records.
   modelCount=0;
   QSqlQuery modelQuery(model->query());
   if(modelQuery.first())
   {
      modelCount++;
      while (modelQuery.next()) {
         modelCount++;
      }
   }

   while (query.next()) {
       int id = query.value(0).toInt();
       bool found=false;
       databaseCount++;

       for(int i1=0; i1<modelCount; i1++)
       {
         if( model->record(i1).value(0).toInt() == id)
         {
            found=true;
            break;
         }
       }

       if(!found)
       {
          qWarning() << "   Record not available due to missing keys: "
                     << id << ": "
                     << query.value(1).toString();
       }
   }

   QString line=QString("%1/%2").arg(modelCount).arg(databaseCount);
   ui->labelCount->setText(line);
   qWarning() << "Updating " << m_table << ": " << line;


   QPalette palette = ui->labelCount->palette();
   palette.setColor(QPalette::WindowText, Qt::black);
   if(model->filter().isNull())
   {
      if( modelCount != databaseCount)
      {
         palette.setColor(QPalette::WindowText, Qt::red);
      }
   }

   ui->labelCount->setPalette(palette);
}


bool CWarehouseTab::isMultiLine(const QString &name) const
{
   bool multiLine=false;

   if( (name=="Description")  )
   {
      multiLine=true;
   }

   return(multiLine);
}


void CWarehouseTab::dump()
{
   QSqlQuery query( QString("SELECT id, Name FROM %1").arg(m_table) );
   int databaseCount = query.size();

   printf( "|_. Id |_. Name |\n" );
   while (query.next()) {
       int id = query.value(0).toInt();
       printf("| %d | %s |\n", id, qPrintable( query.value(1).toString() ) );
   }
}


/*--- Fin ------------------------------------------------------------------*/
