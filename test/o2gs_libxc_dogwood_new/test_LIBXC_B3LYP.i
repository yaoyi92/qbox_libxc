set cell 11 0 0  0 11 0  0 0 11
species oxygen O_HSCV_PBE-1.0.xml
atom O1 oxygen  1.16  0  0
atom O2 oxygen -1.16  0  0
set xc LIBXC LDA_X:0.08 HF:0.2 GGA_X_B88:0.72 LDA_C_VWN:0.19 GGA_C_LYP:0.81
set nspin 2
set delta_spin 1
set nempty 4
set ecut 70
set wf_dyn PSDA
set ecutprec 5
randomize_wf
run 0 40 5 
