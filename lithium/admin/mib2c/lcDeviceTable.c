/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.table_data.conf 15999 2007-03-25 22:32:02Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "lcDeviceTable.h"

static netsnmp_tdata *static_lcDeviceTable = NULL;

/** Pointer retrieval */
netsnmp_tdata* lcDeviceTable ()
{ return static_lcDeviceTable; }

/** Initializes the lcDeviceTable module */
netsnmp_tdata*
init_lcDeviceTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    return initialize_table_lcDeviceTable();
}

/** Initialize the lcDeviceTable table by defining its contents and how it's structured */
netsnmp_tdata*
initialize_table_lcDeviceTable(void)
{
    static oid lcDeviceTable_oid[] = {1,3,6,1,4,1,20038,1,1,3};
    size_t lcDeviceTable_oid_len   = OID_LENGTH(lcDeviceTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_tdata                   *table_data;
    netsnmp_table_registration_info *table_info;

    reg = netsnmp_create_handler_registration(
              "lcDeviceTable",     lcDeviceTable_handler,
              lcDeviceTable_oid, lcDeviceTable_oid_len,
              HANDLER_CAN_RONLY
              );

    table_data = netsnmp_tdata_create_table( "lcDeviceTable", 0 );
    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_UNSIGNED,  /* index: lcCustomerIndex */
                           ASN_UNSIGNED,  /* index: lcSiteIndex */
                           ASN_UNSIGNED,  /* index: lcDeviceIndex */
                           0);

    table_info->min_column = COLUMN_LCDEVICENAME;
    table_info->max_column = COLUMN_LCDEVICEOPERSTATUS;
    
    netsnmp_tdata_register( reg, table_data, table_info );

    static_lcDeviceTable = table_data;

    return table_data;
}

/* create a new row in the table */
netsnmp_tdata_row *
lcDeviceTable_createEntry(netsnmp_tdata *table_data
                 , u_long  lcCustomerIndex
                 , u_long  lcSiteIndex
                 , u_long  lcDeviceIndex
                ) {
    struct lcDeviceTable_entry *entry;
    netsnmp_tdata_row *row;

    entry = SNMP_MALLOC_TYPEDEF(struct lcDeviceTable_entry);
    if (!entry)
        return NULL;

    row = netsnmp_tdata_create_row();
    if (!row) {
        SNMP_FREE(entry);
        return NULL;
    }
    row->data = entry;
    entry->lcCustomerIndex = lcCustomerIndex;
    netsnmp_tdata_row_add_index( row, ASN_UNSIGNED,
                                 &(entry->lcCustomerIndex),
                                 sizeof(entry->lcCustomerIndex));
    entry->lcSiteIndex = lcSiteIndex;
    netsnmp_tdata_row_add_index( row, ASN_UNSIGNED,
                                 &(entry->lcSiteIndex),
                                 sizeof(entry->lcSiteIndex));
    entry->lcDeviceIndex = lcDeviceIndex;
    netsnmp_tdata_row_add_index( row, ASN_UNSIGNED,
                                 &(entry->lcDeviceIndex),
                                 sizeof(entry->lcDeviceIndex));
    netsnmp_tdata_add_row( table_data, row );
    return row;
}

/* remove a row from the table */
void
lcDeviceTable_removeEntry(netsnmp_tdata     *table_data, 
                 netsnmp_tdata_row *row) {
    struct lcDeviceTable_entry *entry;

    if (!row)
        return;    /* Nothing to remove */
    entry = (struct lcDeviceTable_entry *)
        netsnmp_tdata_remove_and_delete_row( table_data, row );
    if (entry)
        SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/** handles requests for the lcDeviceTable table */
int
lcDeviceTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct lcDeviceTable_entry          *table_entry;

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct lcDeviceTable_entry *)
                              netsnmp_tdata_extract_entry(request);
            table_info  =     netsnmp_extract_table_info( request);
    
            switch (table_info->colnum) {
            case COLUMN_LCDEVICENAME:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->lcDeviceName,
                                          table_entry->lcDeviceName_len);
                break;
            case COLUMN_LCDEVICEDESC:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->lcDeviceDesc,
                                          table_entry->lcDeviceDesc_len);
                break;
            case COLUMN_LCDEVICEADMINSTATUS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->lcDeviceAdminStatus);
                break;
            case COLUMN_LCDEVICEOPERSTATUS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->lcDeviceOperStatus);
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

    }
    return SNMP_ERR_NOERROR;
}
