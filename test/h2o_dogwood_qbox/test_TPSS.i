set cell 6 0 0  0 6 0  0 0 6
species oxygen O.blyp-mt.xml
species hydrogen H.blyp-vbc.xml
atom O oxygen 0 0 0
atom H1 hydrogen  1.128  1.444  0.000
atom H2 hydrogen  1.128 -1.444  0.000

distance O H1
distance O H2
angle H1 O H2

set nspin 1
#set xc LIBXC GGA_X_PBE:1.0 GGA_C_PBE:1.0
#set xc LIBXC MGGA_X_REVTPSS:1.0  MGGA_C_REVTPSS:1.0
set xc LIBXC MGGA_X_TPSS:1.0  MGGA_C_TPSS:1.0
#set xc LIBXC MGGA_X_M06_L:1.0  MGGA_C_M06_L:1.0
#set xc LIBXC MGGA_X_SCAN:1.0  MGGA_C_SCAN:1.0
set ecut 70
set wf_dyn PSDA
set ecutprec 5
randomize_wf
run 0 100 10
#run 0 100 10
#run 0 100 10
#save test.xml
