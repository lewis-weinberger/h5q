/******************************************************************************/
/* Simple C extension for Guile Scheme to read attributes from HDF5 files.    
   Copyright (C) 2019 Lewis Weinberger

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.    */
/******************************************************************************/

#include <hdf5.h>
#include <libguile.h>

void h5fclose(void *fid) {
    hid_t hdf5_file = *( (hid_t *) fid);
    H5Fclose(hdf5_file);
}

void h5gclose(void *gid) {
    hid_t hdf5_grp = *( (hid_t *) gid);
    H5Gclose(hdf5_grp);
}

void h5aclose(void *aid) {
    hid_t hdf5_attr = *( (hid_t *) aid);
    H5Aclose(hdf5_attr);
}

void h5tclose(void *tid) {
    hid_t hdf5_type = *( (hid_t *) tid);
    H5Tclose(hdf5_type);
}

SCM scm_read_attr(SCM scm_filename, SCM scm_group, SCM scm_attribute) {
    char *filename, *group, *attribute;
    hid_t hdf5_file, hdf5_grp, hdf5_attr, hdf5_type;

    unsigned long long ull_buf;
    long long ll_buf;
    double d_buf;
    /* Note that HDF5 converts between similar datatypes and so to be safe
       we use the largest datatypes here to try to ensure all (atomic) types
       are represented */

    SCM scm_attr, args;

    /* Start the dynwind context */
    scm_dynwind_begin(0);
    
    /* Convert Guile strings to C strings according to current locale's encoding */
    filename = scm_to_locale_string(scm_filename);
    scm_dynwind_free(filename);

    group = scm_to_locale_string(scm_group);
    scm_dynwind_free(group);

    attribute = scm_to_locale_string(scm_attribute);
    scm_dynwind_free(attribute);

    /* Print input */
    args = scm_list_3(scm_attribute, scm_group, scm_filename);
    scm_simple_format(SCM_BOOL_T,
		      scm_from_locale_string("Reading attribute '~A' from group '~A' in file '~A'.\n"),
		      args);

    /* Open HDF5 file */
    hdf5_file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (hdf5_file < 0) {
	scm_misc_error("read-attr",
		       "Unable to open file '~A'.",
		       scm_list_1(scm_filename));
    }
    scm_dynwind_unwind_handler(h5fclose, &hdf5_file, SCM_F_WIND_EXPLICITLY);

    /* Open group*/
    hdf5_grp = H5Gopen(hdf5_file, group, H5P_DEFAULT);
    if (hdf5_grp < 0) {
	scm_misc_error("read-attr",
		       "Unable to open group '~A'.",
		       scm_list_1(scm_group));
    }
    scm_dynwind_unwind_handler(h5gclose, &hdf5_grp, SCM_F_WIND_EXPLICITLY);

    /* Open attribute */
    hdf5_attr = H5Aopen_name(hdf5_grp, attribute);
    if (hdf5_attr < 0) {
	scm_misc_error("read-attr",
		       "Unable to open attribute '~A'.",
		       scm_list_1(scm_attribute));
    }
    scm_dynwind_unwind_handler(h5aclose, &hdf5_attr, SCM_F_WIND_EXPLICITLY);

    /* Determine datatype */
    hdf5_type = H5Aget_type(hdf5_attr);
    if (hdf5_type < 0) {
	scm_misc_error("read-attr",
		       "Unable to determine datatype for attribute '~A'.",
		       scm_list_1(scm_attribute));
    }
    scm_dynwind_unwind_handler(h5tclose, &hdf5_type, SCM_F_WIND_EXPLICITLY);

    /* Read the attribute based on datatype */
    if (H5Tequal(hdf5_type, H5T_NATIVE_SHORT)
	|| H5Tequal(hdf5_type, H5T_NATIVE_INT)
	|| H5Tequal(hdf5_type, H5T_NATIVE_LONG)
	|| H5Tequal(hdf5_type, H5T_NATIVE_LLONG)) {
	
        H5Aread(hdf5_attr, hdf5_type, &ll_buf);
	scm_attr = scm_from_long_long(ll_buf);
	
    } else if (H5Tequal(hdf5_type, H5T_NATIVE_USHORT)
	       || H5Tequal(hdf5_type, H5T_NATIVE_UINT)
	       || H5Tequal(hdf5_type, H5T_NATIVE_ULONG)
	       || H5Tequal(hdf5_type, H5T_NATIVE_ULLONG)) {
	
        H5Aread(hdf5_attr, hdf5_type, &ull_buf);
	scm_attr = scm_from_ulong_long(ull_buf);
	
    } else if (H5Tequal(hdf5_type, H5T_NATIVE_FLOAT)
	       || H5Tequal(hdf5_type, H5T_NATIVE_DOUBLE)) {
	
        H5Aread(hdf5_attr, hdf5_type, &d_buf);
	scm_attr = scm_from_double(d_buf);
	
    } else {
	scm_misc_error("read-attr",
		       "Attribute '~A' not an atomic datatype.",
		       scm_list_1(scm_attribute));
    }

    /* End dynwind context */
    scm_dynwind_end();

    return scm_attr;
}

void init_h5q () {
    /* Define primitive procedure */
    scm_c_define_gsubr("read-attr", 3, 0, 0, scm_read_attr);

    /* Export binding to public interface */
    scm_c_export("read-attr", NULL);
}

void scm_init_h5q_module () {
    /* Define Scheme module */
    scm_c_define_module("h5q", init_h5q, NULL);
}
