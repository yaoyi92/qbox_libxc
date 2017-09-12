#QB=/home/yiy/Codes/qbox/qbox-1.63.7/src/qb
QB="mpirun -n 4 ../../src/qb"
$QB test_LDA.i > test_LDA.out
