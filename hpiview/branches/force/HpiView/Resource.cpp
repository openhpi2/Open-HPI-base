/***************************************************************************
                          Resources.cpp  -  description
                             -------------------
    begin                : Mon Dec  8 15:36:21 CET 2003
    copyright            : (C) 2003 by Thomas Kanngieser
    email                : Thomas.Kanngieser@fci.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "HpiView.h"


//////////////////////////////////////////////////
//                  cHpiItem
//////////////////////////////////////////////////


cHpiItem::cHpiItem( QListView *parent, const char *name )
  : QListViewItem( parent, name ), m_mark( true )
{
  strcpy( m_name, name );
}


cHpiItem::cHpiItem( cHpiItem *parent, const char *name )
  : QListViewItem( parent, name ), m_mark( true )
{
  strcpy( m_name, name );
}


cHpiItem *
cHpiItem::findChild( const char *name )
{
  cHpiItem *child = (cHpiItem *)firstChild();

  while( child )
     {
       if ( child->isName( name ) )
            return child;

       child = (cHpiItem *)child->nextSibling();
     }

  return 0;
}


void
cHpiItem::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 0 );
}


void
cHpiItem::mark( bool m, bool rec )
{
  m_mark = m;
  
  if ( !rec )
       return;

  cHpiItem *child = (cHpiItem *)firstChild();

  while( child )
     {
       child->mark( m );
       child = (cHpiItem *)child->nextSibling();
     }
}


void
cHpiItem::deleteUnmark()
{
  if ( !m_mark )
     {
       char str[1024];
       sprintf( str, "deleting %s", m_name );
       theApp->Log( str );
       delete this;
       return;
     }

  cHpiItem *child = (cHpiItem *)firstChild();

  while( child )
     {
       cHpiItem *next = (cHpiItem *)child->nextSibling();

       child->deleteUnmark();

       child = next;
     }
}


//////////////////////////////////////////////////
//                  cHpiResourceRoot
//////////////////////////////////////////////////

cHpiResourceRoot::cHpiResourceRoot( QListView *parent, const char *name )
  : cHpiItem( parent, name )
{
}


cHpiItem *
cHpiResourceRoot::createEntityPath( const SaHpiEntityPathT &path, int max )
{
  int idx = SAHPI_MAX_ENTITY_PATH;

  cHpiItem *parent = this;

  while ( idx > max )
    {
      idx = idx - 1;

      const SaHpiEntityT *e = &path.Entry[idx];

      if ( e->EntityType == SAHPI_ENT_UNSPECIFIED )
           continue;

      QString str = QString( "%1 %2" )
        .arg( hpiEntityType2String( e->EntityType ) )
        .arg( e->EntityInstance );

      cHpiItem *item = parent->findChild( str );

      if ( !item )
           item = new cHpiResourceEntity( parent, str, *e );

      item->mark( true );
      parent = item;
    }

  return parent;
}


void
cHpiResourceRoot::discoverResources()
{
  SaErrorT err = saHpiResourcesDiscover( theApp->m_session_id );

  if ( err != SA_OK )
     {
       theApp->LogError( "saHpiResourcesDiscover: ", err );
       return;
     }

  SaHpiEntryIdT next = SAHPI_FIRST_ENTRY;

  do
     {
       SaHpiEntryIdT current = next;
       SaHpiRptEntryT entry;

       err = saHpiRptEntryGet( theApp->m_session_id, current, &next, &entry );

       if ( err != SA_OK )
          {
            if ( current != SAHPI_FIRST_ENTRY )
               {
                 theApp->LogError( "saHpiRptEntryGet: ", err );
                 return;
               }

            theApp->Log( "Empty RPT !\n" );

            break;
          }

       // create entity path
       cHpiItem *parent = createEntityPath( entry.ResourceEntity );

       SaHpiEntityT *e = &entry.ResourceEntity.Entry[0];

       QString name = QString( "%1 %2" )
         .arg( hpiEntityType2String( e->EntityType ) )
         .arg( e->EntityInstance );

       // create rpt entry
       cHpiItem *item = parent->findChild( name );

       if ( !item )
            item = new cHpiResourceRptItem( parent, name, *e, entry );

       item->mark( true );

       parent = item;

       // create rdr
       if ( entry.ResourceCapabilities & SAHPI_CAPABILITY_RDR )
          {
            SaHpiEntryIdT next_rdr = SAHPI_FIRST_ENTRY;

            while ( next_rdr != SAHPI_LAST_ENTRY )
               {
                 SaHpiEntryIdT current_rdr = next_rdr;
                 SaHpiRdrT rdr;

                 err = saHpiRdrGet( theApp->m_session_id, 
                                    entry.ResourceId,
                                    current_rdr,
                                    &next_rdr, &rdr );

                 if ( err != SA_OK )
                    {
                      if ( current_rdr == SAHPI_FIRST_ENTRY )
                           theApp->Log( "empty RDR table !\n" );
                      else
                           theApp->LogError( "saHpiRdrGet: ", err );

                      break;
                    }

                 parent = createEntityPath( rdr.Entity, 0 );

                 name = QString( "%1 %2" )
                   .arg( (char *)rdr.IdString.Data )
                   .arg( rdr.RecordId );

                 item = parent->findChild( name );

                 if ( !item )
                    {
                      if ( rdr.RdrType == SAHPI_INVENTORY_RDR )
                         {
                           cHpiResourceRdrInventory *inv = new cHpiResourceRdrInventory( parent, name, rdr );
                           item = inv;

                           SaHpiInventoryDataT *d = (SaHpiInventoryDataT *)inv->m_inventory;

                           err = saHpiEntityInventoryDataRead( theApp->m_session_id, entry.ResourceId,
                                                               rdr.RdrTypeUnion.InventoryRec.EirId,
                                                               dMaxInventorySize, d,
                                                               &inv->m_size );

                           if ( err )
                                theApp->LogError( "saHpiEntityInventoryDataRead: ", err );
                           else if ( d->Validity != SAHPI_INVENT_DATA_VALID )
                                theApp->Log( "saHpiEntityInventoryDataRead: data not valid" );
                           else
                              {
                                for( int i = 0; d->DataRecords[i]; i++ )
                                   {
                                     SaHpiInventDataRecordT *r = d->DataRecords[i];

                                     switch( r->RecordType )
                                        {
                                          case SAHPI_INVENT_RECTYPE_INTERNAL_USE:
                                               new cHpiResourceInventoryInternalUse( item, "InternalUse", &r->RecordData.InternalUse );
                                               break;

                                          case SAHPI_INVENT_RECTYPE_CHASSIS_INFO:
                                               new cHpiResourceInventoryChassis( item, "Chassis", &r->RecordData.ChassisInfo );
                                               break;

                                          case SAHPI_INVENT_RECTYPE_BOARD_INFO:
                                               new cHpiResourceInventoryBoard( item, "Board", &r->RecordData.BoardInfo );
                                               break;

                                          case SAHPI_INVENT_RECTYPE_PRODUCT_INFO:
                                               new cHpiResourceInventoryProduct( item, "Product", &r->RecordData.ProductInfo );
                                               break;

                                          case SAHPI_INVENT_RECTYPE_OEM:
                                               new cHpiResourceInventoryOem( item, "Oem", &r->RecordData.OemData );
                                               break;
                                        }
                                   }
                              }
                         }
                      else
                           item = new cHpiResourceRdr( parent, name, rdr );
                    }

                 item->mark( true, true );
               }
          }
     }
  while( next != SAHPI_LAST_ENTRY );

  mark( true );

  deleteUnmark();

  mark( false, true );
}


//////////////////////////////////////////////////
//                  cHpiResourceRptItem
//////////////////////////////////////////////////


void
cHpiResourceRptItem::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 2 );

  int idx = 0;
  tableRptEntry( theApp->m_data, idx, m_rpt_entry );
  theApp->m_data->adjustColumn( 0 );
  theApp->m_data->adjustColumn( 1 );
}


//////////////////////////////////////////////////
//                  cHpiResourceRdr
//////////////////////////////////////////////////


void
cHpiResourceRdr::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 2 );

  int idx = 0;
  tableRdr( theApp->m_data, idx, m_rdr );

  theApp->m_data->adjustColumn( 0 );
  theApp->m_data->adjustColumn( 1 );
}


//////////////////////////////////////////////////
//                  cHpiResourceInventoryInternalUse
//////////////////////////////////////////////////


void
cHpiResourceInventoryInternalUse::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 2 );

  theApp->m_data->adjustColumn( 0 );
  theApp->m_data->adjustColumn( 1 );
  
}


//////////////////////////////////////////////////
//                  cHpiResourceInventoryInternalUse
//////////////////////////////////////////////////

void
cHpiResourceInventoryChassis::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 2 );

  int idx = 0;
  tableInventoryChassis( theApp->m_data, idx, *m_chassis );

  theApp->m_data->adjustColumn( 0 );
  theApp->m_data->adjustColumn( 1 );
}


//////////////////////////////////////////////////
//                  cHpiResourceInventoryBoard
//////////////////////////////////////////////////

void
cHpiResourceInventoryBoard::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 2 );

  int idx = 0;
  tableInventoryGeneral( theApp->m_data, idx, *m_board );

  theApp->m_data->adjustColumn( 0 );
  theApp->m_data->adjustColumn( 1 );
}


//////////////////////////////////////////////////
//                  cHpiResourceInventoryProduct
//////////////////////////////////////////////////

void
cHpiResourceInventoryProduct::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 2 );

  int idx = 0;
  tableInventoryGeneral( theApp->m_data, idx, *m_product );

  theApp->m_data->adjustColumn( 0 );
  theApp->m_data->adjustColumn( 1 );
}


//////////////////////////////////////////////////
//                  cHpiResourceInventoryOem
//////////////////////////////////////////////////

void
cHpiResourceInventoryOem::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 2 );

  theApp->m_data->adjustColumn( 0 );
  theApp->m_data->adjustColumn( 1 );
}


//////////////////////////////////////////////////
//                  cHpiEventRoot
//////////////////////////////////////////////////


void
cHpiEventRoot::getEvents()
{
  while ( getEvent() )
       ;
}


bool
cHpiEventRoot::getEvent()
{
  SaHpiEventT event;
  SaHpiRdrT rdr;
  SaHpiRptEntryT rpt_entry;

  SaErrorT err = saHpiEventGet( theApp->m_session_id, 0, &event, &rdr, &rpt_entry );

  if ( err != SA_OK )
     {
       if ( err != SA_ERR_HPI_TIMEOUT )
            theApp->LogError( "saHpiEventGet: ", err );

       return false;
     }

  theApp->Log( "new event.\n" );

  new cHpiEvent( this, QString( "%1 %2" )
                 .arg( m_current++ )
                 .arg( hpiEventType2String( event.EventType ) ), event, rdr, rpt_entry );

  return false;
}


//////////////////////////////////////////////////
//                  cHpiEvent
//////////////////////////////////////////////////

void
cHpiEvent::activate()
{
  theApp->m_data->setNumRows( 0 );
  theApp->m_data->setNumCols( 2 );

  int idx = 0;

  tableEvent( theApp->m_data, idx, m_event );

  if ( WithRdr() )
       tableRdr( theApp->m_data, idx, m_rdr );

  if ( WithRptEntry() )
       tableRptEntry( theApp->m_data, idx, m_rpt_entry );

  theApp->m_data->adjustColumn( 0 );
  theApp->m_data->adjustColumn( 1 );
}


//////////////////////////////////////////////////
//                  cHpiViewResource
//////////////////////////////////////////////////


cHpiViewResource::cHpiViewResource( QWidget *parent, const char* name )
  : QListView( parent, name )
{
  setRootIsDecorated( true );
  addColumn( "Resource", 300 );
  addColumn( "Type", 100 );

  // resources
  m_resource = new cHpiResourceRoot( this, "Resources" );
  m_resource->discoverResources();

  m_resource_timer = new QTimer( this );
  connect( m_resource_timer, SIGNAL( timeout() ), SLOT( resourceTimeout() ) );
  m_resource_timer->start( 10000 );

  // events
  m_event = new cHpiEventRoot( this, "Events" );
  m_event->getEvents();

  m_event_timer = new QTimer( this );
  connect( m_event_timer, SIGNAL( timeout() ), SLOT( eventTimeout() ) );
  m_event_timer->start( 1000 );
}


void
cHpiViewResource::resourceTimeout()
{
  m_resource->discoverResources();
}


void
cHpiViewResource::eventTimeout()
{
  m_event->getEvents();
}

