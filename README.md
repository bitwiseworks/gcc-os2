GCC for OS/2
============

This repository contains the GCC source code with patches needed to build it on
the OS/2 operating system. One day all patches will be merged upstream but
currently it is more convenient for the OS/2 developers to track them
separately.

Note that this repository is a successor of the previous repository located at
<https://github.com/psmedley/gcc> which is not maintained any more. All patches
from that repository have been applied here preserving their original authors
when possible.

Installation
------------

The easiest and the only officially supported way to install GCC for OS/2 is to
use binary builds provided by [bitwiseworks](https://www.bitwiseworks.com/).
This requires the [RPM/YUM environment for
OS/2](http://trac.netlabs.org/rpm/wiki) to be installed. Note that all recent
distributions of [ArcaOS](https://www.arcanoae.com/arcaos/) already have it, so
nothing needs to be done if you have one of these. Then simply type the
following on the command line prompt to install GCC:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
yum install gcc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you also need to install the C++ part of GCC, type the following:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
yum install gcc-c++
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Help and Support
----------------

You will find a lot of useful information about GCC for OS/2 on the [Wiki
pages](https://github.com/bitwiseworks/gcc-os2/wiki) of the project's  GitHub
repository. There you may also watch the progress of our work on this port.
Please read these pages carefully if you need any help or want to report a
problem.

Build Instructions
------------------

Please refer to [build
instructions](https://github.com/bitwiseworks/gcc-os2/wiki/Developers#building-gcc)
on GitHub.

Credits
-------

We thank Knut St. Osmundsen for maintaining the OS/2 port of GCC version 3 which
was used as a base for this port. We also thank Paul Smedley for his continued
OS/2 contribution to GCC 4 and further versions over these years.

*bww bitwiseworks GmbH*  
*January, 2020*
