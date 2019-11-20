# h5q

> **H**DF5 **Q**uick View

A basic extension to Guile Scheme written in C, as a wrapper around some HDF5 functionality. Currently it offers only one simple function `read-attr` which can read a numeric attribute from an HDF5 file:

```
scheme@(guile-user)> (read-attr "/path/to/hdf5/file" "group" "attribute")
```

Note this was a first experiment in writing Guile extensions, so use at your own risk.

## Installation

Requires an installation of [Guile Scheme](https://www.gnu.org/software/guile/download/) and the [HDF5](https://www.hdfgroup.org/downloads/) C library. The shared C library can then be compiled with:

```
$ git clone https://github.com/lewis-weinberger/h5q.git
$ cd h5q
$ make
```

At this point the module can be loaded in the REPL and used with:

```
scheme@(guile-user)> (load-extension "./libh5q.so" "scm_init_h5q_module")
scheme@(guile-user)> (use-modules (h5q))
scheme@(guile-user)> (read-attr "/path/to/hdf5/file" "group" "attribute")
```

To install permanently you can use `make install` (may require root permissions depending on your Guile installation location). There is a small test file, `test.hdf5`, provided to check the compilation was successful:

```
scheme@(guile-user)> (use-modules (h5q))
scheme@(guile-user)> (read-attr "test.hdf5" "Header1" "test1")
Reading attribute 'test1' from group 'Header1' in file 'test.hdf5'.
$1 = 3.14159265359
scheme@(guile-user)> (read-attr "test.hdf5" "Header1" "test2")
Reading attribute 'test2' from group 'Header1' in file 'test.hdf5'.
$2 = 42.0
scheme@(guile-user)> (read-attr "test.hdf5" "Header2" "test3")
Reading attribute 'test3' from group 'Header2' in file 'test.hdf5'.
$3 = 5102934.0
scheme@(guile-user)> (read-attr "test.hdf5" "Header2" "test4")
Reading attribute 'test4' from group 'Header2' in file 'test.hdf5'.
$4 = 2.7182818284
```

## To Do

It would be nice to flesh out the features:

1. Procedures for discovering the HDF5 file structure, such as which groups, datasets and attributes are present in a file.
2. Procedures for reading compound and non-numeric data from attributes.

However going much further than this one might end up implementing a full-wrapper to the HDF5 C library (akin to [h5py](https://www.h5py.org/) in Python), which is well beyond the scope of this little extension.


## License

[GPLv3 License](./LICENSE)

