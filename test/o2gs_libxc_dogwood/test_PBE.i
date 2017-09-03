set cell 11 0 0  0 11 0  0 0 11
species oxygen O_HSCV_PBE-1.0.xml
atom O1 oxygen  1.16  0  0
atom O2 oxygen -1.16  0  0
set xc PBE
set nspin 2
set delta_spin 1
set nempty 4
set ecut 70
set wf_dyn PSDA
set ecutprec 5
randomize_wf
run 0 40 5 
