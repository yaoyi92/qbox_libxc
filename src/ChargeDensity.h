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
// ChargeDensity.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARGEDENSITY_H
#define CHARGEDENSITY_H

#include <vector>
#include <valarray>
#include <complex>
#include <string>
#include <map>
#include "Timer.h"
#include "Context.h"

class Wavefunction;
class FourierTransform;
class Basis;

typedef std::map<std::string,Timer> TimerMap;

class ChargeDensity
{
  private:

  const Context& ctxt_;
  MPI_Comm vcomm_;
  const Wavefunction& wf_;
  Basis* vbasis_;
  FourierTransform* vft_;
  std::vector<FourierTransform*> ft_; // ft_[ikp];
  std::valarray<std::complex<double> > rhotmp;

  public:

  mutable TimerMap tmap;

  std::vector<std::vector<double> > rhor; // rhor[ispin][i]
  std::vector<std::vector<std::complex<double> > > rhog; // rhog[ispin][ig]
  // core density ptr. If non-zero, contains the real-space core density
  double* rhocore_r;
  void update_density(void);
  void update_rhor(void);

  const Context& context(void) const { return ctxt_; }
  MPI_Comm vcomm(void) const { return vcomm_; }
  Basis* vbasis(void) const { return vbasis_; }
  FourierTransform* vft(void) const { return vft_; }
  FourierTransform* ft(int ikp) const { return ft_[ikp]; }

  ChargeDensity(const Wavefunction& wf);
  ~ChargeDensity();
};
#endif
