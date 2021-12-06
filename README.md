# STAF
Forked from the Software Testing Automation Framework (STAF), which is a framework designed to improve the level of reuse and automation in test cases and test environments. The goal of STAF is to provide a complete end-to-end automation solution for testers.
# Build
###Core
```
export PROJECTS="staf connprov*"
export OS_NAME=win32 (See above for other valid OS_NAME values)
export BUILD_TYPE=retail
export MSVCDIR=d:/MVStudio (Only needed if on Windows)
export VSCOMMONDIR=d:/MVStudio/Common (Only needed if on Windows)
export MSSDKDIR=d:/MicrosoftSDK (Only needed if on Windows and building IA-64 code)
export STAF_USE_IPV6= (Make sure not set to build TCP IPv4 libraries)
export OPENSSL_ROOT=c:/tools/openssl-0.9.8e-root (Needed if STAF_USE_SSL is set to 1)
make
```
###Java
```
export PROJECTS=java
export JAVA_DRIVE=d
export JAVA_LIBS=jvm
export JAVA_VERSION=1.4
export JAVA_BUILD_V11=0
export JAVA_BUILD_V12=1
export JAVA_V12_ROOT=/cygdrive/d/ibmjdk1.4.2
export JAVA_V12_LIBDIRS=/cygdrive/d/ibmjdk1.4.2/lib
export JAVA_V12_BIN_DIR=/usr/java_dev2/sh (this is only required on AIX, when using Java 1.2+ to build)
make
```
###Package
```
export OS_NAME=linux
make PROJECTS=pkg
```