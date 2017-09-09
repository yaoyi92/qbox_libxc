set cell 5 0 0  0 5 0  0 0 5
species oxygen O_HSCV_PBE-1.0.xml
species hydrogen H_HSCV_PBE-1.0.xml
atom O oxygen 0 0 0
atom H1 hydrogen  1.128  1.444  0.000
atom H2 hydrogen  1.128 -1.444  0.000

distance O H1
distance O H2
angle H1 O H2

set nspin 1
set xc LIBXC GGA_X_B88:1.0 GGA_C_LYP:1.0
set ecut 70
set wf_dyn PSDA
set ecutprec 5
randomize_wf
run 0 500 1
plot -kinetic_energy_density kin.cube
#save test.xml
