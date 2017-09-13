#QB=/home/yiy/Codes/qbox/qbox-1.63.7/src/qb
QB="mpirun -n 4 ../../src/qb"
#$QB test_LDA_PZ.i > test_LDA_PZ.out
#$QB test_LDA_PW.i > test_LDA_PW.out
#$QB test_PBE.i > test_PBE.out
#$QB test_BLYP.i > test_BLYP.out
$QB test_TPSS.i > test_TPSS.out
#$QB test_REVTPSS.i > test_REVTPSS.out
#$QB test_M06L.i > test_M06L.out
#$QB test_SCAN.i > test_SCAN.out
