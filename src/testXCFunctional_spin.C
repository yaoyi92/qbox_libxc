////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008 The Regents of the University of California
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
// testXCFunctional.C
//
////////////////////////////////////////////////////////////////////////////////

// Test an XC functional by computing the xc energy of a gaussian
// of width 0.1 a.u. in a cube of side 1.0 a.u.
// With a cube of side 1.0 and 32x32x32 points,
// The LDA xc energy must be -2.8105 a.u.
// dExc/da must be 0.911682

#include <cstdlib>
#include <iostream>
#include <vector>
#include "LDAFunctional.h"
#include "VWNFunctional.h"
#include "PBEFunctional.h"
#include "BLYPFunctional.h"
#include "LIBXCFunctional.h"
#include "Timer.h"
#include <cassert>
#include <cmath>
using namespace std;

int main(int argc, char **argv)
{
  // use: testxcf alat np
  cout.precision(17);
  if ( argc != 3 )
  {
    cout << " use: testXCFunctional alat np" << endl;
    return 0;
  }
  assert(argc==3);
  double a = atof(argv[1]);
  double omega = a*a*a;
  int n = atoi(argv[2]);
  int n3 = n*n*n;
  vector<vector<double> > rh;
  rh.resize(2);
  rh[0].resize(n3);
  rh[1].resize(n3);

  XCFunctional *xcf_list[3];
  xcf_list[0] = new VWNFunctional(rh);
  xcf_list[1] = new LIBXCFunctional(rh, "LIBXC LDA_X LDA_C_VWN");
  xcf_list[2] = new BLYPFunctional(rh);
  //xcf_list[2] = new LIBXCFunctional(rh, "LIBXC LDA_X LDA_C_PZ_MOD");

  for ( int ixcf = 0; ixcf < 2; ixcf++ )
  {
    Timer tm;
    XCFunctional *xcf = xcf_list[ixcf];
    cout << endl << " Functional name: " << xcf->name() << endl;

    double *grad_rho_up[3], *grad_rho_dn[3];
    if ( xcf->isGGA() )
    {
      grad_rho_up[0] = xcf->grad_rho_up[0];
      grad_rho_up[1] = xcf->grad_rho_up[1];
      grad_rho_up[2] = xcf->grad_rho_up[2];
      grad_rho_dn[0] = xcf->grad_rho_dn[0];
      grad_rho_dn[1] = xcf->grad_rho_dn[1];
      grad_rho_dn[2] = xcf->grad_rho_dn[2];
    }

    const double rc = 0.1 * a;
    const double rc2 = rc * rc;
    const double pi = M_PI;
    const double fac = 1.0 / ( pow(pi,1.5) * rc*rc*rc );
    double sum = 0.0;

    for ( int i = 0; i < n; i++ )
    {
      double x = ( i * a ) / n - a/2;
      for ( int j = 0; j < n; j++ )
      {
        double y = ( j * a ) / n - a/2;
        for ( int k = 0; k < n; k++ )
        {
          double z = ( k * a ) / n - a/2;
          double r2 = x*x + y*y + z*z;
          int ii = i + n * ( j + n * k );
          rh[0][ii] = fac * exp( -r2 / rc2 );
          rh[1][ii] = 4.0 * fac * exp( -r2 / rc2 );
          sum += rh[0][ii];
          sum += rh[1][ii];

          if ( xcf->isGGA() )
          {
            grad_rho_up[0][ii] = - rh[0][ii] * 1.0 * x / rc2;
            grad_rho_up[1][ii] = - rh[0][ii] * 2.0 * y / rc2;
            grad_rho_up[2][ii] = - rh[0][ii] * 3.0 * z / rc2;
            grad_rho_dn[0][ii] = - rh[1][ii] * 3.0 * x / rc2;
            grad_rho_dn[1][ii] = - rh[1][ii] * 4.0 * y / rc2;
            grad_rho_dn[2][ii] = - rh[1][ii] * 5.0 * z / rc2;
          }
        }
      }
    }
    sum = sum * omega / n3;
    // the density should be normalized
    cout << " Integrated density: " << sum << endl;

    tm.start();
    xcf->setxc();
    tm.stop();

    //double *exc_up = xcf->exc_up;
    //double *exc_dn = xcf->exc_dn;
    //double *vxc1_up = xcf->vxc1_up;
    //double *vxc1_dn = xcf->vxc1_dn;
    //double *vxc2_upup = xcf->vxc2_upup;
    //double *vxc2_updn = xcf->vxc2_updn;
    //double *vxc2_dnup = xcf->vxc2_dnup;
    //double *vxc2_dndn = xcf->vxc2_dndn;
    //double excsum = 0.0;
    //for ( int i = 0; i < n3; i++ )
    //  {
    //  cout << "rho " << rh[0][i] << " " << rh[1][i] << endl;
    //  excsum += rh[0][i] * exc_up[i] + rh[1][i] * exc_dn[i];
    //  cout << "exc " << exc_up[i] << " " << exc_dn[i] << endl;
    //  cout << "vxc1 " << vxc1_up[i] << " " << vxc1_dn[i] << endl;
    //  cout << "vxc2 " << vxc2_upup[i] << " " << vxc2_updn[i] << " " << vxc2_dnup[i] << " " << vxc2_dndn[i] << endl;
    //  }

    //cout << " Total xc energy: " << excsum * omega / n3 << endl;
    double *exc = xcf->exc;
    double *vxc1_up = xcf->vxc1_up;
    double *vxc1_dn = xcf->vxc1_dn;
    double excsum = 0.0;
    for ( int i = 0; i < n3; i++ )
      {
      cout << "rho " << rh[0][i] << " " << rh[1][i] << endl;
      excsum += rh[0][i] * exc[i] + rh[1][i] * exc[i];
      cout << "exc " << exc[i] << endl;
      cout << "vxc1 " << vxc1_up[i] << " " << vxc1_dn[i] << endl;
      }

    cout << " Total xc energy: " << excsum * omega / n3 << endl;

    // if not a GGA, compute dExc/da
    if ( !xcf->isGGA() )
    {
    }
    else
    {
    }
    cout << " " << xcf->name() << " time: " << tm.real() << endl;
  } // ixcf
  return 0;
}
