////////////////////////////////////////////////////////////////////////////////
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
// XCPotential.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef XCPOTENTIAL_H
#define XCPOTENTIAL_H

#include "Control.h"
#include "ChargeDensity.h"
#include <string>
#include <vector>
#include <valarray>
#include <complex>

class Basis;
class FourierTransform;
class XCFunctional;

class XCPotential
{
  private:

  const ChargeDensity& cd_;
  XCFunctional* xcf_;

  std::vector<std::vector<double> > vxctmp;          // vxctmp[ispin][ir]
  std::vector<std::complex<double> > tmpr;           // tmpr[ir]
  std::vector<std::complex<double> > tmp1, tmp2;     // tmp1[ig], tmp2[ig]

  double exc_, dxc_;
  int nspin_;
  int ngloc_;
  int np012loc_;

  FourierTransform& vft_;
  Basis& vbasis_;

  public:

  const XCFunctional* xcf() { return xcf_; }
  bool isGGA(void);
  XCPotential(const ChargeDensity& cd, const std::string functional_name,
    const Control& ctrl);
  ~XCPotential();
  void update(std::vector<std::vector<double> >& vr);
  void compute_stress(std::valarray<double>& sigma_exc);
  double exc(void) { return exc_; }
  double dxc(void) { return dxc_; }
};

class XCPotentialException
{
  public:
  std::string msg;
  XCPotentialException(std::string s) : msg(s) {}
};
#endif
