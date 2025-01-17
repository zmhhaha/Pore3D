// From C library:
#include <stdio.h>
#include <stdlib.h>

// Locals:
#include "_p3d_idlCommon.h"
#include "p3d_idlAdapter.h"

#include "p3dBlob.h"

// Textural statistics output struct in IDL format.
// NOTE: Order is crucial in reference to the C structure and 
// names should be in upper case format and alphabetical:
static IDL_STRUCT_TAG_DEF TextureStats_tags[] = {
    { "FD", 0, (void *) IDL_TYP_DOUBLE},
    { 0}
};

IDL_VPTR p3d_idlTextureAnalysis(int argc, IDL_VPTR argv[], char* argk) {
    // IDL array input and IDL struct output:
    IDL_VPTR idl_in_rev, idl_out_struct;
    unsigned char *in_rev;
    TextureStats stats;
    //TextureStats *stats = (TextureStats*) malloc(sizeof (TextureStats));
    
    IDL_MEMINT tmp_dims[IDL_MAX_ARRAY_DIM];
    IDL_MEMINT offset;
    double* d_tmp_ptr;
    char* s_data;

    int err_code;
    IDL_StructDefPtr s;


    // Get input data in IDL format:
    idl_in_rev = argv[0];

    IDL_ENSURE_SIMPLE(idl_in_rev);
    IDL_ENSURE_ARRAY(idl_in_rev); // Get input data in IDL format:		



    // Create the structure definition  
    s = IDL_MakeStruct(NULL, TextureStats_tags);

    // Import the data area into an IDL structure. This code performs an 1-to-1 
    // mapping of IDL struct with the C struct:
    //idl_out_struct = IDL_ImportArray(1, &one, IDL_TYP_STRUCT, (UCHAR *) stats, NULL, s);
    //stats = (TextureStats *) idl_out_struct->value.s.arr->data;

    if (idl_in_rev->value.arr->n_dim == 3) {
        // Extract input in C format
        if (idl_in_rev->type == IDL_TYP_BYTE)
            in_rev = (unsigned char *) idl_in_rev->value.arr->data;
        else
            _p3d_idlPrintNamedError("Input argument IMAGE must be of type BYTE.");

        // Call Pore3D:
        err_code = p3dTextureAnalysis(
                in_rev,
                &stats,
                (int) idl_in_rev->value.arr->dim[0],
                (int) idl_in_rev->value.arr->dim[1],
                (int) idl_in_rev->value.arr->dim[2],
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
    s = IDL_MakeStruct(NULL, TextureStats_tags);

    // Use temporary variable: doing so IDL will handle memory:
    tmp_dims[0] = 1;
    s_data = (char *) IDL_MakeTempStruct(s, 1, tmp_dims, &idl_out_struct, 0);


    // Get the field of the structure:
    offset = IDL_StructTagInfoByName(s, "FD", IDL_MSG_LONGJMP, NULL);
    d_tmp_ptr = (double *) (s_data + offset);
    *(d_tmp_ptr) = stats.FD;  
   

    // Return output in IDL Format:
    return idl_out_struct;
}

