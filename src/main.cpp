/**---------------------------------------------------------------------------
 *
 * @file       main.cpp
 * @brief      Start the warehouse application
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
#include <QtWidgets>


/*--- Implementation -------------------------------------------------------*/


int main(int argc, char * argv[])
{
   Q_INIT_RESOURCE( warehouse );
   QApplication app(argc, argv);

   QCoreApplication::setApplicationName("warehouse");
   QCoreApplication::setApplicationVersion("1.0");

   QCommandLineParser parser;
   parser.setApplicationDescription("Automatic form generator for SQLite databases");
   parser.addHelpOption();
   parser.addVersionOption();
   parser.addPositionalArgument("database", "SQLite database file");

   QCommandLineOption oDump("d", "Dump table", "table" );
   parser.addOption( oDump );

   parser.process(app);

   if(parser.positionalArguments().count() < 1)
   {
      qFatal("Database has to be given.");
   }

   CWarehouse warehouse(parser.positionalArguments()[0]);

   if( parser.isSet( oDump ) )
   {
      warehouse.dump( parser.value(oDump) );
      return(0);
   }

   warehouse.show();

   return app.exec();
}


/*--- Fin ------------------------------------------------------------------*/
