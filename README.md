To open in Visual Studio Community 2019:

1. Start the Visual Studio Community 2019 program.
2. Click the open folder button.
3. Navigate to the directory containing this file and open it.

You should be able to build, debug, run tests etc.

CMakeLists.txt projects are also supported in IntelliJ IDEs (e.g. CLion) and
with the cmake CLI utility available for all platforms.

The program expects to be run in the following ways:

- `./imp1.exe ls 2 path_to_file.txt`
- `./imp1.exe lpt 1 path_to_file.txt`

See help menu by running `./imp1.exe help` for more detailed instructions.

See the `test/inputs` directory for examples of input files.
See the `test/*.c` files for examples used to test the rules.
