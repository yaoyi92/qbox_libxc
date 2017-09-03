set cell 11 0 0  0 11 0  0 0 11
species oxygen O_HSCV_PBE-1.0.xml
atom O1 oxygen  1.16  0  0
atom O2 oxygen -1.16  0  0
set xc LIBXC GGA_X_PBE GGA_C_PBE
#set xc PBE
#set xc LIBXC GGA_X_B88 GGA_C_LYP
#set xc BLYP
set nspin 2
set delta_spin 1
set nempty 4
set ecut 70
set wf_dyn PSDA
set ecutprec 5
randomize_wf
run 0 40 5 
