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
// SpeciesCmd.C
//
////////////////////////////////////////////////////////////////////////////////

#include "SpeciesCmd.h"
#include "SpeciesReader.h"
#include "Species.h"
using namespace std;

class Species;

////////////////////////////////////////////////////////////////////////////////
int SpeciesCmd::action(int argc, char **argv)
{
  if ( argc != 3 )
  {
    if ( ui->onpe0() )
      cout << "  Use: species name uri" << endl;
    return 1;
  }

  if ( ui->onpe0() )
  {
    cout << "  SpeciesCmd: defining species " << argv[1]
         << " as " << argv[2] << endl;
  }

  string xmlstr;
  SpeciesReader sp_reader;
  if ( ui->onpe0() )
    sp_reader.uri_to_string(argv[2], argv[1], xmlstr);

  s->ctxt_.string_bcast(xmlstr,0);
  Species* sp = new Species("argv[1]");
  sp_reader.string_to_species(xmlstr,*sp);
  s->atoms.addSpecies(sp,argv[1]);

  return 0;
}
