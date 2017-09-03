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
// Xc.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef XC_H
#define XC_H

#include<iostream>
#include<iomanip>
#include<sstream>
#include<stdlib.h>

#include "Sample.h"
#include "xc.h"

class Xc : public Var
{
  Sample *s;

  public:

  const char *name ( void ) const { return "xc"; };

  int set ( int argc, char **argv )
  {
    string v;
    v = argv[1];
    if ( (argc > 2) && (v == "LIBXC") ) {
      for ( int n = 2; n < argc ; n ++ ) {
        v = v + " " + argv[n];
        }
    }
    else if ( argc != 2 )
    {
      if ( ui->onpe0() )
      cout << " xc takes only one value" << endl;
      return 1;
    }
    else {
      v = argv[1];
      if ( !( v == "LDA" ||
              v == "VWN" ||
              v == "PBE" ||
              v == "BLYP" ||
              v == "HF" ||
              v == "PBE0" ||
              v == "B3LYP" ) )
      {
        if ( ui->onpe0() )
          cout << " xc must be LDA, VWN, PBE, BLYP, HF, PBE0 or B3LYP" << endl;
        return 1;
      }
    }
    s->ctrl.xc= v;

    return 0;
  }

  string print (void) const
  {
     ostringstream st;
     st.setf(ios::left,ios::adjustfield);
     st << setw(50) << name() << " = ";
     st.setf(ios::right,ios::adjustfield);
     st << setw(50) << s->ctrl.xc;
     return st.str();
  }

  Xc(Sample *sample) : s(sample) { s->ctrl.xc = "LDA"; };
};
#endif
