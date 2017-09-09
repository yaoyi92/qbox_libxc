#-------------------------------------------------------------------------------
#
#  icc_mkl.mk
#
#-------------------------------------------------------------------------------
#
 PLT=x86_64
#-------------------------------------------------------------------------------
 LIBXCDIR = /nas/longleaf/home/yiy/softwares-dogwood/cp2k/libxc_svn/libxc/install-yiy-intel
 MPIDIR=/usr/mpi/intel/mvapich2-2.3a/
 XERCESCDIR=/nas/longleaf/home/yiy/softwares-dogwood/qbox/xerces-c-src_2_8_0
 PLTOBJECTS = readTSC.o
 MKLROOT=/nas/longleaf/apps/intel/17.2/intel/compilers_and_libraries_2017.2.174/linux/mkl/
 INCLUDE = -I$(MKLROOT)/include

 CXX=icc
 LD=mpicxx

 PLTFLAGS += -DIA32 -D_LARGEFILE_SOURCE \
             -D_FILE_OFFSET_BITS=64 -DUSE_MPI -DSCALAPACK -DADD_ \
             -DAPP_NO_THREADS -DXML_USE_NO_THREADS -DUSE_XERCES

# FFT must be FFTW2, FFTW3, ESSL or NOLIB
  FFT=FFTW3

ifeq ($(FFT),FFTW2)
 PLTFLAGS += -DUSE_FFTW2
 PLTFLAGS += -DFFTWMEASURE
 FFTWDIR=/share/apps/fftw/fftw-2.1.5/fftw
 FFTWINCLUDEDIR=$(FFTWDIR)
 FFTWLIBDIR=$(FFTWDIR)/.libs
 INCLUDE += -I$(FFTWINCLUDEDIR)
 LIBPATH += -L$(FFTWLIBDIR)
 LIBS += -lfftw
endif

ifeq ($(FFT),FFTW3)
 PLTFLAGS += -DUSE_FFTW3
 PLTFLAGS += -DFFTWMEASURE
#PLTFLAGS += -DFFTW_TRANSPOSE
 PLTFLAGS += -DFFTW3_2D
 FFTWDIR=/nas/longleaf/home/yiy/softwares-dogwood/qbox/fftw-3.3.6-pl2/install-yiy
 FFTWINCLUDEDIR=$(FFTWDIR)/include
 FFTWLIBDIR=$(FFTWDIR)/lib
 INCLUDE += -I$(FFTWINCLUDEDIR)
 LIBPATH += -L$(FFTWLIBDIR)
 LIBS += -lfftw3
endif

ifeq ($(FFT),ESSL)
$(error ESSL library not available)
endif

ifeq ($(FFT),NOLIB)
 PLTFLAGS += -DFFT_NOLIB
endif


 INCLUDE += -I$(MPIDIR)/include -I$(XERCESCDIR)/include -I$(LIBXCDIR)/include

 CXXFLAGS=  -g -O0 -vec-report1 -D$(PLT) $(INCLUDE) $(PLTFLAGS) $(DFLAGS)

 LIBPATH += -L$(MPIDIR)/lib64 \
            -L$(MKLDIR)/lib/intel64  \
            -L$(XERCESCDIR)/lib \
            -L/nas/longleaf/home/yiy/softwares-dogwood/qbox/libuuid-1.0.3/install-yiy/lib \
            -L$(LIBXCDIR)/lib

 LIBS +=  $(PLIBS) \
          -lmkl_intel_lp64 \
          -lmkl_lapack95_lp64 -lmkl_sequential -lmkl_core \
          -lirc -lifcore -lsvml \
          -luuid $(XERCESCDIR)/lib/libxerces-c.a -lpthread \
          -lxc

# Parallel libraries
 PLIBS = -lmkl_scalapack_lp64 -lmkl_blacs_intelmpi_lp64

 LDFLAGS = $(LIBPATH) $(LIBS)
#-------------------------------------------------------------------------------
