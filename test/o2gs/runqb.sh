export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/home/yiy/Codes/qbox/xerces-c-src_2_8_0/lib
QB=/home/yiy/Codes/qbox/qbox-1.63.7/src/qb
mpirun.openmpi -n 4 $QB test.i > test.out
