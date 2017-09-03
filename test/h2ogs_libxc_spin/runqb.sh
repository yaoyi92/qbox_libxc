export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/home/yiy/Codes/qbox/xerces-c-src_2_8_0/lib
QB=/home/yiy/Codes/qbox/qbox-1.63.7/src/qb
#mpirun.openmpi -n 1 $QB test.i > test.out
#$QB test_LDA.i > test_LDA.out
#$QB test_LIBXC_LDA.i > test_LIBXC_LDA.out
$QB test_VWN.i > test_VWN.out
$QB test_LIBXC_VWN.i > test_LIBXC_VWN.out
#$QB test_PBE.i > test_PBE.out
#$QB test_LIBXC_PBE.i > test_LIBXC_PBE.out
#$QB test_BLYP.i > test_BLYP.out
#$QB test_LIBXC_BLYP.i > test_LIBXC_BLYP.out
