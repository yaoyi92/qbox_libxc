////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2009 The Regents of the University of California
//
// This file is part of Qbox
//
// Qbox is distributed under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
// See the file COPYING in the root directory of this distribution
// or <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
//
// upf2qso.C: transform a UPF pseudopotential to QSO format
// the QSO format is defined at http://www.quantum-simulation.org
//
////////////////////////////////////////////////////////////////////////////////
//
// use: upf2qso rcut(a.u.) < pot.UPF > pot.xml
// The potentials in pot.xml are given on a radial linear grid up to rcut,
// with grid spacing of 0.01 (a.u.)
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include "spline.h"
#include "PeriodicTable.h"
using namespace std;

int seek_tag(string tag)
{
  // skip to line following tag
  bool done = false;
  string buf, s;
  istringstream is;

  while ( !done )
  {
    getline(cin,buf);
    is.clear();
    is.str(buf);
    is >> s;
    done = ( s == tag );
    if ( cin.eof() )
    {
      cerr << " found EOF before " << tag << endl;
      return 1;
    }
  }
  return 0;
}

int main(int argc, char** argv)
{
  cerr << " upf2qso v1.4 2014-02-24" << endl;
  if ( argc != 2 )
  {
    cerr << " usage: upf2qso rcut < file.upf > file.xml" << endl;
    return 1;
  }
  assert(argc==2);
  const double rcut = atof(argv[1]);

  PeriodicTable pt;
  string buf,s;
  istringstream is;
  int status;

  string upf_pp_info;
  status = seek_tag("<PP_INFO>");
  bool done = false;
  while (!done)
  {
    getline(cin,buf);
    is.clear();
    is.str(buf);
    is >> s;
    done = ( s == "</PP_INFO>" );
    if ( !done )
    {
      upf_pp_info += buf + '\n';
    }
  }

  // remove all '<' and '>' characters from the PP_INFO field
  // for XML compatibility
  string::size_type p = upf_pp_info.find_first_of("<>");
  while ( p != string::npos )
  {
    upf_pp_info[p] = ' ';
    p = upf_pp_info.find_first_of("<>");
  }

  status = seek_tag("<PP_HEADER>");

  // version number (ignore)
  getline(cin,buf);

  // element symbol
  string upf_symbol;
  getline(cin,buf);
  is.clear();
  is.str(buf);
  is >> upf_symbol;

  // get atomic number and mass
  const int atomic_number = pt.z(upf_symbol);
  const double mass = pt.mass(upf_symbol);

  // NC flag
  string upf_ncflag;
  getline(cin,buf);
  is.clear();
  is.str(buf);
  is >> upf_ncflag;
  if ( upf_ncflag != "NC" )
  {
    cerr << " not a Norm-conserving potential" << endl;
    cerr << " NC flag: " << upf_ncflag << endl;
    return 1;
  }

  // NLCC flag
  string upf_nlcc_flag;
  getline(cin,buf);
  is.clear();
  is.str(buf);
  is >> upf_nlcc_flag;
  if ( upf_nlcc_flag != "F" )
  {
    cerr << " Potential includes a non-linear core correction" << endl;
    return 1;
  }

  // XC functional (add in description)
  string upf_xcf[4];
  getline(cin,buf);
  is.clear();
  is.str(buf);
  is >> upf_xcf[0] >> upf_xcf[1] >> upf_xcf[2] >> upf_xcf[3];

  // add XC functional information to description
  upf_pp_info += upf_xcf[0] + ' ' + upf_xcf[1] + ' ' +
                 upf_xcf[2] + ' ' + upf_xcf[3] + '\n';

  // Z valence
  double upf_zval;
  getline(cin,buf);
  is.clear();
  is.str(buf);
  is >> upf_zval;

  // Total energy (ignore)
  getline(cin,buf);

  // suggested cutoff (ignore)
  getline(cin,buf);

  // max angular momentum
  int upf_lmax;
  getline(cin,buf);
  is.clear();
  is.str(buf);
  is >> upf_lmax;

  // number of points in mesh
  int upf_mesh_size;
  getline(cin,buf);
  is.clear();
  is.str(buf);
  is >> upf_mesh_size;

  // number of wavefunctions, number of projectors
  int upf_nwf, upf_nproj;
  getline(cin,buf);
  is.clear();
  is.str(buf);
  is >> upf_nwf >> upf_nproj;

  // Wavefunctions
  vector<string> upf_shell(upf_nwf);
  vector<int> upf_l(upf_nwf);
  vector<double> upf_occ(upf_nwf);
  // skip header
  getline(cin,buf);
  for ( int ip = 0; ip < upf_nwf; ip++ )
  {
    getline(cin,buf);
    is.clear();
    is.str(buf);
    is >> upf_shell[ip] >> upf_l[ip] >> upf_occ[ip];
  }
  status = seek_tag("</PP_HEADER>");
  if ( status != 0 ) return status;

  // read mesh
  status = seek_tag("<PP_MESH>");
  if ( status != 0 ) return status;
  status = seek_tag("<PP_R>");
  if ( status != 0 ) return status;
  vector<double> upf_r(upf_mesh_size);
  for ( int i = 0; i < upf_mesh_size; i++ )
   cin >> upf_r[i];
  status = seek_tag("</PP_R>");
  if ( status != 0 ) return status;
  status = seek_tag("<PP_RAB>");
  if ( status != 0 ) return status;
  vector<double> upf_rab(upf_mesh_size);
  for ( int i = 0; i < upf_mesh_size; i++ )
   cin >> upf_rab[i];
  status = seek_tag("</PP_RAB>");
  if ( status != 0 ) return status;
  status = seek_tag("</PP_MESH>");
  if ( status != 0 ) return status;

  status = seek_tag("<PP_LOCAL>");
  if ( status != 0 ) return status;
  vector<double> upf_vloc(upf_mesh_size);
  for ( int i = 0; i < upf_mesh_size; i++ )
    cin >> upf_vloc[i];
  status = seek_tag("</PP_LOCAL>");
  if ( status != 0 ) return status;

  status = seek_tag("<PP_NONLOCAL>");
  if ( status != 0 ) return status;
  vector<vector<double> > upf_vnl;
  upf_vnl.resize(upf_nproj);
  vector<int> upf_proj_l(upf_nproj);
  for ( int j = 0; j < upf_nproj; j++ )
  {
    status = seek_tag("<PP_BETA>");
    if ( status != 0 ) return status;
    int ip, l, np;
    cin >> ip >> l;
    while ( cin.get() != '\n' );
    assert(ip-1 < upf_nproj);
    assert(l <= upf_lmax);
    upf_proj_l[ip-1] = l;
    cin >> np;
    upf_vnl[j].resize(upf_mesh_size);
    for ( int i = 0; i < np; i++ )
      cin >> upf_vnl[j][i];
    status = seek_tag("</PP_BETA>");
    if ( status != 0 ) return status;
  }
  status = seek_tag("<PP_DIJ>");
  if ( status != 0 ) return status;
  int upf_ndij;
  cin >> upf_ndij;
  while ( cin.get() != '\n' );
  if ( upf_ndij != upf_nproj )
  {
    cerr << " Number of non-zero Dij differs from number of projectors" << endl;
    return 1;
  }

  vector<double> upf_d(upf_ndij);
  for ( int i = 0; i < upf_ndij; i++ )
  {
    int m,n;
    cin >> m >> n >> upf_d[i];
    if ( m != n )
    {
      cerr << " Non-local Dij has off-diagonal elements" << endl;
      cerr << " m=" << m << " n=" << n << endl;
      return 1;
    }
  }
  status = seek_tag("</PP_DIJ>");
  if ( status != 0 ) return status;

  status = seek_tag("</PP_NONLOCAL>");
  if ( status != 0 ) return status;

  // make table iproj[l] mapping l to iproj
  // vnl(l) is in vnl[iproj[l]] if iproj[l] > -1
  // vlocal if iproj[llocal] = -1
  vector<int> iproj(upf_lmax+2);
  for ( int l = 0; l <= upf_lmax+1; l++ )
    iproj[l] = -1;
  for ( int j = 0; j < upf_nproj; j++ )
    iproj[upf_proj_l[j]] = j;

  // determine angular momentum of local potential in UPF file
  int upf_llocal;
  // reverse loop to get correct upf_llocal when upf_nproj < upf_lmax
  for ( int l = upf_lmax+1; l >= 0; l-- )
    if ( iproj[l] == -1 )
      upf_llocal = l;
  // increase lmax if there are more projectors than wavefunctions
  int qso_lmax = upf_lmax;
  if (upf_lmax < upf_llocal)
  {
    qso_lmax = upf_lmax+1;
  }

  status = seek_tag("<PP_PSWFC>");
  vector<vector<double> > upf_wf;
  vector<int> upf_wf_l(upf_nwf);
  vector<double> upf_wf_occ(upf_nwf);
  upf_wf.resize(upf_nwf);
  for ( int j = 0; j < upf_nwf; j++ )
  {
    upf_wf[j].resize(upf_mesh_size);
    string label;
    cin >> label >> upf_wf_l[j] >> upf_wf_occ[j];
    while ( cin.get() != '\n' );
    for ( int i = 0; i < upf_mesh_size; i++ )
      cin >> upf_wf[j][i];
  }
  status = seek_tag("</PP_PSWFC>");
  if ( status != 0 ) return status;

  // output original data in file upf.dat
  ofstream upf("upf.dat");
  upf << "# vloc" << endl;
  for ( int i = 0; i < upf_vloc.size(); i++ )
    upf << upf_r[i] << " " << upf_vloc[i] << endl;
  upf << endl << endl;
  for ( int j = 0; j < upf_nproj; j++ )
  {
    upf << "# proj j=" << j << endl;
    for ( int i = 0; i < upf_vnl[j].size(); i++ )
      upf << upf_r[i] << " " << upf_vnl[j][i] << endl;
    upf << endl << endl;
  }
  for ( int j = 0; j < upf_nwf; j++ )
  {
    upf << "# wf j=" << j << endl;
    for ( int i = 0; i < upf_wf[j].size(); i++ )
      upf << upf_r[i] << " " << upf_wf[j][i] << endl;
    upf << endl << endl;
  }
  upf.close();


  // print summary
  cerr << "PP_INFO:" << endl << upf_pp_info << endl;
  cerr << "Element: " << upf_symbol << endl;
  cerr << "NC: " << upf_ncflag << endl;
  cerr << "NLCC: " << upf_nlcc_flag << endl;
  cerr << "XC: " << upf_xcf[0] << " " << upf_xcf[1] << " "
       << upf_xcf[2] << " " << upf_xcf[3] << endl;
  cerr << "Zv: " << upf_zval << endl;
  cerr << "lmax: " << qso_lmax << endl;
  cerr << "llocal: " << upf_llocal << endl;
  cerr << "nwf: " << upf_nwf << endl;
  cerr << "mesh_size: " << upf_mesh_size << endl;

  // interpolate functions on linear mesh

  // compute delta_vnl[l][i] on the upf log mesh

  // divide the projector function by the wavefunction, except if
  // the wavefunction amplitude is smaller than tol, outside of rcut_divide.
  const double tol = 1.e-5;
  const double rcut_divide = 1.0;
  vector<vector<double> > delta_vnl;
  delta_vnl.resize(upf_nproj);
  for ( int j = 0; j < upf_nproj; j++ )
  {
    delta_vnl[j].resize(upf_wf[j].size());
    for ( int i = 0; i < delta_vnl[j].size(); i++ )
    {
      double num = upf_vnl[j][i];
      double den = upf_wf[upf_proj_l[j]][i];

      delta_vnl[j][i] = 0.0;
      if ( upf_r[i] < rcut_divide )
      {
        // near the origin
        if ( i == 0 && fabs(den) < tol )
        {
          // i = 0 for linear mesh, r = 0.0: use nearest value
          delta_vnl[j][i] = upf_vnl[j][1] / upf_wf[upf_proj_l[j]][1];
        }
        else
        {
          // other points near the origin
          delta_vnl[j][i] = num / den;
        }
      }
      else
      {
        // wavefunction gets small at large r.
        // Assume that delta_vnl is zero when that happens
        if ( fabs(den) > tol )
          delta_vnl[j][i] = num / den;
      }
    }
  }

  vector<vector<double> > vps;
  vps.resize(upf_nproj+1);
  for ( int j = 0; j < upf_nproj; j++ )
  {
    vps[j].resize(upf_mesh_size);
    for ( int i = 0; i < delta_vnl[j].size(); i++ )
      vps[j][i] = upf_vloc[i] + delta_vnl[j][i];
  }

  // interpolate vloc
  vector<double> f(upf_mesh_size), fspl(upf_mesh_size);

  // factor 0.5: convert from Ry in UPF to Hartree in QSO
  for ( int i = 0; i < upf_vloc.size(); i++ )
    f[i] = 0.5 * upf_vloc[i];

  int n = upf_vloc.size();
  int bcnat_left = 0;
  double yp_left = 0.0;
  int bcnat_right = 1;
  double yp_right = 0.0;
  spline(n,&upf_r[0],&f[0],yp_left,yp_right,
         bcnat_left,bcnat_right,&fspl[0]);

  const double mesh_spacing = 0.01;
  int nplin = (int) (rcut / mesh_spacing);

  vector<double> vloc_lin(nplin);
  for ( int i = 0; i < nplin; i++ )
  {
    double r = i * mesh_spacing;
    if ( r >= upf_r[0] )
      splint(n,&upf_r[0],&f[0],&fspl[0],r,&vloc_lin[i]);
    else
      // use value closest to the origin for r=0
      vloc_lin[i] = 0.5 * upf_vloc[0];
  }

  // interpolate vps[j], j=0, nproj-1
  vector<vector<double> > vps_lin;
  vps_lin.resize(vps.size());
  for ( int j = 0; j < vps.size(); j++ )
  {
    vps_lin[j].resize(nplin);
  }

  for ( int j = 0; j < upf_nproj; j++ )
  {
    // factor 0.5: convert from Ry in UPF to Hartree in QSO
    for ( int i = 0; i < upf_vloc.size(); i++ )
      f[i] = 0.5 * vps[j][i];

    int n = upf_vloc.size();
    int bcnat_left = 0;
    double yp_left = 0.0;
    int bcnat_right = 1;
    double yp_right = 0.0;
    spline(n,&upf_r[0],&f[0],yp_left,yp_right,
           bcnat_left,bcnat_right,&fspl[0]);

    for ( int i = 0; i < nplin; i++ )
    {
      double r = i * mesh_spacing;
      if ( r >= upf_r[0] )
        splint(n,&upf_r[0],&f[0],&fspl[0],r,&vps_lin[j][i]);
      else
        vps_lin[j][i] = 0.5 * vps[j][0];
    }
  }

  // write potentials in gnuplot format on file vlin.dat
  ofstream vlin("vlin.dat");
  for ( int l = 0; l <= qso_lmax; l++ )
  {
    vlin << "# v, l=" << l << endl;
    if ( iproj[l] == -1 )
    {
      // l == llocal
      for ( int i = 0; i < nplin; i++ )
        vlin << i*mesh_spacing << " " << vloc_lin[i] << endl;
      vlin << endl << endl;
    }
    else
    {
      for ( int i = 0; i < nplin; i++ )
        vlin << i*mesh_spacing << " " << vps_lin[iproj[l]][i] << endl;
      vlin << endl << endl;
    }
  }

  // interpolate wavefunctions on the linear mesh

  vector<vector<double> > wf_lin;
  wf_lin.resize(upf_nwf);
  for ( int j = 0; j < upf_nwf; j++ )
  {
    wf_lin[j].resize(nplin);
    assert(upf_wf[j].size()<=f.size());
    for ( int i = 0; i < upf_wf[j].size(); i++ )
    {
      if ( upf_r[i] > 0.0 )
        f[i] = upf_wf[j][i] / upf_r[i];
      else
      {
        // value at origin, depending on angular momentum
        if ( upf_wf_l[j] == 0 )
        {
          // l=0: take value closest to r=0
          f[i] = upf_wf[j][1]/upf_r[1];
        }
        else
        {
          // l>0:
          f[i] = 0.0;
        }
      }
    }

    int n = upf_wf[j].size();
    // choose boundary condition at origin depending on angular momentum
    int bcnat_left = 0;
    double yp_left = 0.0;
    if ( upf_wf_l[j] == 1 )
    {
      bcnat_left = 1; // use natural bc
      double yp_left = 0.0; // not used
    }
    int bcnat_right = 1;
    double yp_right = 0.0;
    spline(n,&upf_r[0],&f[0],yp_left,yp_right,
           bcnat_left,bcnat_right,&fspl[0]);

    for ( int i = 0; i < nplin; i++ )
    {
      double r = i * mesh_spacing;
      if ( r >= upf_r[0] )
        splint(n,&upf_r[0],&f[0],&fspl[0],r,&wf_lin[j][i]);
      else
      {
        // r < upf_r[0]
        assert(upf_r[0]>0.0);
        // compute value near origin, depending on angular momentum
        if ( upf_wf_l[j] == 0 )
        {
          // l=0: take value closest to r=0
          wf_lin[j][i] = upf_wf[j][0]/upf_r[0];
        }
        else
        {
          // l>0:
          wf_lin[j][i] = upf_wf[j][0] * r / ( upf_r[0] * upf_r[0] );
        }
      }
    }

    vlin << "# phi, l=" << upf_l[j] << endl;
    for ( int i = 0; i < nplin; i++ )
      vlin << i*mesh_spacing << " " << wf_lin[j][i] << endl;
    vlin << endl << endl;
  }

  cerr << " interpolation done" << endl;

#if 1
  // output potential on log mesh
  ofstream vout("v.dat");
  for ( int l = 0; l <= qso_lmax; l++ )
  {
    vout << "# v, l=" << l << endl;
    if ( iproj[l] == -1 )
    {
      // l == llocal
      for ( int i = 0; i < upf_vloc.size(); i++ )
        vout << upf_r[i] << " " << 0.5*upf_vloc[i] << endl;
      vout << endl << endl;
    }
    else
    {
      for ( int i = 0; i < vps[iproj[l]].size(); i++ )
        vout << upf_r[i] << " " << 0.5*vps[iproj[l]][i] << endl;
      vout << endl << endl;
    }
  }
  vout << endl << endl;
  vout.close();
#endif

  // Generate QSO file

  // output potential in QSO format
  cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  cout << "<fpmd:species xmlns:fpmd=\"http://www.quantum-simulation.org/ns/fpmd/fpmd-1.0\"" << endl;
  cout << "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << endl;
  cout << "  xsi:schemaLocation=\"http://www.quantum-simulation.org/ns/fpmd/fpmd-1.0"  << endl;
  cout << "  species.xsd\">" << endl;
  cout << "<description>" << endl;
  cout << "Translated from UPF format by upf2qso" << endl;
  cout << upf_pp_info;
  cout << "</description>" << endl;
  cout << "<symbol>" << upf_symbol << "</symbol>" << endl;
  cout << "<atomic_number>" << atomic_number << "</atomic_number>" << endl;
  cout << "<mass>" << mass << "</mass>" << endl;
  cout << "<norm_conserving_pseudopotential>" << endl;
  cout << "<valence_charge>" << upf_zval << "</valence_charge>" << endl;
  cout << "<lmax>" << qso_lmax << "</lmax>" << endl;
  cout << "<llocal>" << upf_llocal << "</llocal>" << endl;
  cout << "<nquad>0</nquad>" << endl;
  cout << "<rquad>0.0</rquad>" << endl;
  cout << "<mesh_spacing>" << mesh_spacing << "</mesh_spacing>" << endl;

  for ( int l = 0; l <= qso_lmax; l++ )
  {
    cout << "<projector l=\"" << l << "\" size=\"" << nplin << "\">"
         << endl;
    cout << "<radial_potential>" << endl;
    if ( iproj[l] == -1 )
    {
      // l == llocal
      for ( int i = 0; i < nplin; i++ )
        cout << setprecision(12) << vloc_lin[i] << endl;
    }
    else
    {
      for ( int i = 0; i < nplin; i++ )
        cout << setprecision(12) << vps_lin[iproj[l]][i] << endl;
    }
    cout << "</radial_potential>" << endl;
    // find index j corresponding to angular momentum l
    int j = 0;
    while ( upf_wf_l[j] != l && j < upf_nwf ) j++;
    // check if found
    const bool found = ( j != upf_nwf );
    // print wf only if found
    if ( found )
    {
      cout << "<radial_function>" << endl;
      for ( int i = 0; i < nplin; i++ )
        cout << setprecision(12) << wf_lin[j][i] << endl;
      cout << "</radial_function>" << endl;
    }
    cout << "</projector>" << endl;
  }
  cout << "</norm_conserving_pseudopotential>" << endl;
  cout << "</fpmd:species>" << endl;
  return 0;
}

