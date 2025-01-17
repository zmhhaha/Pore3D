// From C library:
#include <stdio.h>
#include <stdlib.h>

// Locals:
#include "_p3d_idlCommon.h"
#include "p3d_idlAdapter.h"

#include "p3dBlob.h"

// Connected components statistics output struct in IDL format.
// NOTE: Order is crucial in reference to the C structure and 
// names should be in upper case format and alphabetical:
// Basic characteristic statistics output struct in IDL format.
// NOTE: Order is crucial in reference to the C structure and 
// names should be in upper case format and alphabetical:
static IDL_STRUCT_TAG_DEF BasicStats_tags[] = {
    { "CV", 0, (void *) IDL_TYP_DOUBLE},
    { "MV", 0, (void *) IDL_TYP_DOUBLE},
    { "SV", 0, (void *) IDL_TYP_DOUBLE},
    { "VV", 0, (void *) IDL_TYP_DOUBLE},
    { 0}
};

IDL_VPTR p3d_idlBasicAnalysis(int argc, IDL_VPTR argv[], char* argk) {

    typedef struct {
        IDL_KW_RESULT_FIRST_FIELD; // Must be first entry in structure
        double resolution;
        int rs_there;
    } KW_RESULT;

    // Alphabetical order is crucial:
    static IDL_KW_PAR kw_pars[] = {
        IDL_KW_FAST_SCAN,
        { "VOXELSIZE", IDL_TYP_DOUBLE, 1, 0, (int*) IDL_KW_OFFSETOF(rs_there), (char*) IDL_KW_OFFSETOF(resolution)},
        { NULL}
    };

    KW_RESULT kw;

    // IDL array input and IDL struct output:
    IDL_VPTR idl_in_rev, idl_out_struct;
    unsigned char *in_rev;
    BasicStats stats;
    //BasicStats *stats = (BasicStats*) malloc(sizeof (BasicStats));
    int keywords_ct = 0;

    IDL_MEMINT tmp_dims[IDL_MAX_ARRAY_DIM];
    IDL_MEMINT offset;
    double* d_tmp_ptr;
    char* s_data;

    double resolution = 1.0; // default value

    int err_code;
    IDL_StructDefPtr s;

    // Get input data in IDL format:
    idl_in_rev = argv[0];

    IDL_ENSURE_SIMPLE(idl_in_rev);
    IDL_ENSURE_ARRAY(idl_in_rev); // Get input data in IDL format:		


    // Process keywords:
    IDL_KWProcessByOffset(argc, argv, argk, kw_pars, NULL, 1, &kw);


    // Get the RESOLUTION input keyword:
    if (kw.rs_there) {
        // Check values:
        if (kw.resolution <= 0)
            _p3d_idlPrintNamedError("VOXELSIZE must be greater than 0.");

        // Get value:
        resolution = (double) kw.resolution;

        keywords_ct++;
    }



    // Create the structure definition  
    s = IDL_MakeStruct(NULL, BasicStats_tags);

    // Import the data area into an IDL structure. This code performs an 1-to-1 
    // mapping of IDL struct with the C struct:
    //idl_out_struct = IDL_ImportArray(1, &one, IDL_TYP_STRUCT, (UCHAR *) stats, NULL, s);
    //stats = (BasicStats *) idl_out_struct->value.s.arr->data;

    if (idl_in_rev->value.arr->n_dim == 3) {
        // Extract input in C format
        if (idl_in_rev->type == IDL_TYP_BYTE)
            in_rev = (unsigned char *) idl_in_rev->value.arr->data;
        else
            _p3d_idlPrintNamedError("Input argument IMAGE must be of type BYTE.");

        // Call Pore3D:
        err_code = p3dBasicAnalysis(
                in_rev,
                &stats,
                (int) idl_in_rev->value.arr->dim[0],
                (int) idl_in_rev->value.arr->dim[1],
                (int) idl_in_rev->value.arr->dim[2],
                resolution,
                _p3d_idlPrintInfo
                );

        if (err_code == P3D_ERROR) {
            // Print error:
            _p3d_idlPrintNamedError("Error on internal code execution.");
        }

    } else {
        _p3d_idlPrintNamedError("Input argument IMAGE must be a 3D matrix.");
    }

    // -------------------------------------------------------------------------
    s = IDL_MakeStruct(NULL, BasicStats_tags);

    // Use temporary variable: doing so IDL will handle memory:
    tmp_dims[0] = 1;
    s_data = (char *) IDL_MakeTempStruct(s, 1, tmp_dims, &idl_out_struct, 0);



    // Get the field of the structure:
    offset = IDL_StructTagInfoByName(s, "CV", IDL_MSG_LONGJMP, NULL);
    d_tmp_ptr = (double *) (s_data + offset);
    *(d_tmp_ptr) = stats.Cv;

    offset = IDL_StructTagInfoByName(s, "MV", IDL_MSG_LONGJMP, NULL);
    d_tmp_ptr = (double *) (s_data + offset);
    *(d_tmp_ptr) = stats.Mv;
    
    offset = IDL_StructTagInfoByName(s, "SV", IDL_MSG_LONGJMP, NULL);
    d_tmp_ptr = (double *) (s_data + offset);
    *(d_tmp_ptr) = stats.Sv;

    offset = IDL_StructTagInfoByName(s, "VV", IDL_MSG_LONGJMP, NULL);
    d_tmp_ptr = (double *) (s_data + offset);
    *(d_tmp_ptr) = stats.Vv;


    // Release keyword resources:
    IDL_KW_FREE;

    // Return output in IDL Format:
    return idl_out_struct;
}

