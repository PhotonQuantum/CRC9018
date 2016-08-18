# CRC 9018
This is a **DEPRECATED** program for FRC Strongshold Competition. You should **NEVER** use this code for deploy environment.
```
cmake .
make # make your robot code
make deploy # deploy your robot code, configure team number in CMakeLists.txt
make doxygen # build doxygen docs
make ci-test # run tests, see test/
```

The ci-test target depends on Boost.Test.

The skeleton is Travis CI-ready and will automatically install the FRC toolchain, WPILib, and Boost.Test on the Travis instance.  The default script is:
```
cmake .
make
make ci-test
```  
