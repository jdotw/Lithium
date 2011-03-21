#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "document.h"

/*
 * Documents -- A generic handler for core-side XML documents
 *
 * Includes Scenes and VRacks
 */

int l_document_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *pgres;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_document_enable failed to connect to postgres database"); return -1; }

  /* Check documents table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'documents' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE documents (id serial, type varchar, descr varchar, state integer, editor varchar, version integer)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_document_enable failed to create documents table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Get next case ID */
  pgres = PQexec (pgconn, "SELECT nextval('documents_id_seq')");
  if (pgres && PQresultStatus(pgres) == PGRES_TUPLES_OK && PQntuples(pgres) > 0)
  {
    char *curid_str = PQgetvalue (pgres, 0, 0);
    if (curid_str) l_document_id_setcurrent (atol(curid_str));
  } 
  else
  { i_printf (1, "l_document_enable warning, failed to retrieve next document ID from SQL (%s)", PQresultErrorMessage (pgres)); }   
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Close SQL */
  i_pg_close (pgconn);
  
  return 0;
}

l_document* l_document_create ()
{
  l_document *doc = (l_document *) malloc (sizeof(l_document));
  memset (doc, 0, sizeof(l_document));
  return doc;
}

void l_document_free (void *docptr)
{
  l_document *doc = docptr;

  if (doc->type_str) free (doc->type_str);
  if (doc->desc_str) free (doc->desc_str);
  if (doc->editor_str) free (doc->editor_str);

  free (doc);
}

