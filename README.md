To open in Visual Studio Community 2019:

1. Start the Visual Studio Community 2019 program.
2. Click the open folder button.
3. Navigate to the directory containing this file and open it.

You should be able to build, debug, run tests etc.

CMakeLists.txt projects are also supported in IntelliJ IDEs (e.g. CLion) and
with the cmake CLI utility available for all platforms.

The program expects to be run in the following ways:

- `./imp3.exe mcnaughton 2 path_to_file.txt`
- `./imp3.exe spt 1 path_to_file.txt`

See help menu by running `./imp3.exe help` for more detailed instructions.

See the `test/inputs` directory for examples of input files.
See the `test/*.c` files for examples used to test the rules.


I seem to be getting double free / corruption error on Linux but not on Windows with the same code. Weird.
