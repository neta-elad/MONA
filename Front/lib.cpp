/*
 * MONA
 * Copyright (C) 1997-2013 Aarhus University.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the  Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335,
 * USA.
 */

#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include "lib.h"
#include "symboltable.h"
#include "env.h"
#include "../GTA/gta.h"

using std::cout;

extern SymbolTable symbolTable;
extern Options options;
extern Guide guide;

int 
AutLib::Dir::compare(AutLib::Dir::File *a, AutLib::Dir::File *b)
{
  if (a->hashvalue < b->hashvalue)
    return -1;
  else if (a->hashvalue > b->hashvalue)
    return 1;

  return strcmp(a->descriptor, b->descriptor);
}

// AUTOMATON FILE

AutLib::Dir::File::File(char *name, Signature *sign, Deque<SSSet> *statespaces)
{
  const size_t x_size = 10000;
  char *x = new char[x_size];
  char tmp[1000];
  unsigned len;

  // add name and signature to descriptor
  snprintf(x, x_size, "%s^", name);
  const size_t x_len = strlen(x);
  sign->dump(x+x_len, x_size - x_len);
  len = strlen(x);

  if (options.mode == TREE) {
    // add guide
    snprintf(tmp, sizeof(tmp), "^%i", guide.numSs);
    strcpy(x+len, tmp);
    len += strlen(tmp);
    for (unsigned i = 0; i < guide.numSs; i++) {
      snprintf(tmp, sizeof(tmp), "_%i_%i", guide.muLeft[i], guide.muRight[i]);
      strcpy(x+len, tmp);
      len += strlen(tmp);
    }

    // add universes
    snprintf(tmp, sizeof(tmp), "^%i", guide.numUnivs);
    strcpy(x+len, tmp);
    len += strlen(tmp);
    for (unsigned i = 0; i < guide.numUnivs; i++) {
      snprintf(tmp, sizeof(tmp), "_%s_%i", guide.univPos[i], guide.numUnivSS[i]);
      strcpy(x+len, tmp);
      len += strlen(tmp);
      for (unsigned j = 0; j < guide.numUnivSS[i]; j++) {
	    snprintf(tmp, sizeof(tmp), "_%i", guide.univSS[i][j]);
      strcpy(x+len, tmp);
      len += strlen(tmp);
      }
    }

    // add state spaces
    for (Deque<SSSet>::iterator i = statespaces->begin();
	 i != statespaces->end(); i++) {
      snprintf(x+len, x_size - len, "^");
      ++len;
      for (unsigned j = 0; j < guide.numSs; j++)
	if ((*i)[j]) {
	  snprintf(tmp, sizeof(tmp), "_%d", j);
	  strcpy(x+len, tmp);
	  len += strlen(tmp);
	}
    }
  }
  
  // make hash value
  hashvalue = 0;
  char *t = x;
  while (*t)
    hashvalue = (hashvalue << 1) + *t++;

  descriptor = new char[len+1];
  strcpy(descriptor, x);

  delete[] x;
  filenumber = 0; // set later
}

AutLib::Dir::File::File(std::istream &s)
{
  char x[1000];
  s >> x >> filenumber;
  hashvalue = 0;
  char *t = x;
  while (*t)
    hashvalue = (hashvalue << 1) + *t++;
  descriptor = new char[strlen(x)+1];
  strcpy(descriptor, x);
}

void
AutLib::Dir::File::store(std::ostream &s)
{
  s << descriptor << " " << filenumber << "\n";
}

AutLib::Dir::File::~File()
{
  delete[] descriptor;
}

// DIRECTORY

AutLib::Dir::Dir(char *name, char *src, Deque<char*> *dependencies)
{
  dirname = name;
  sourcename = src;
  const size_t bufsize = strlen(dirname)+5;
  libname = new char[bufsize];
  snprintf(libname, bufsize, "%s/LIB", dirname);

  // make sure directory is created
  struct stat buf;
  if (stat(dirname, &buf))
    if (mkdir(dirname, S_IWUSR | S_IRUSR | S_IXUSR)) {
      cout << "Unable to create directory '" << dirname << "'\n"
	   << "Execution aborted\n";
      exit(-1);
    }

  // if src newer than LIB then remove all files
  if (stat(libname, &buf) == 0) {
    struct stat buf2;
    for (Deque<char *>::iterator i = dependencies->begin();
	 i != dependencies->end(); i++) {
      stat(*i, &buf2);
      if (difftime(buf.st_mtime, buf2.st_mtime) <= 0) {
	char t[500];
	snprintf(t, sizeof(t), "/bin/rm %s/*%s %s/LIB", 
		dirname, 
		(options.mode == TREE) ? ".gta" : ".dfa",
		dirname);
	if (system(t)) {
	  cout << "Unable to create directory '" << dirname << "'\n"
	       << "Execution aborted\n";
	  exit(-1);
	}
	break;
      }
    }
  }

  // read LIB file
  std::ifstream s(libname);
  if (s) {
    unsigned n;
    s >> nextFilenumber;
    s >> n;
    while (n-- && !s.eof())
      files.push_back(new File(s));
  }
  else
    nextFilenumber = 1;
}

char *
AutLib::Dir::getFileName(char *name, Signature *sign, Deque<SSSet> *statespaces)
{
  // find/create filename + number
  File *a = new File(name, sign, statespaces);
  Deque<File *>::iterator b;
  for (b = files.begin(); b != files.end(); b++)
    if (compare(a, *b) == 0) {
      delete a;
      a = *b;
      break;
    }
  if (b == files.end()) {
    files.push_back(a);
    a->filenumber = nextFilenumber++;
  }
  const size_t bufsize = strlen(dirname)+20;
  char *t = new char[bufsize];
  snprintf(t, bufsize, "%s/%i%s", dirname, a->filenumber, 
	  (options.mode == TREE) ? ".gta" : ".dfa");
  return t;
}

AutLib::Dir::~Dir()
{
  // store to $MONALIB/source/LIB
  std::ofstream s(libname);
  s << nextFilenumber << "\n"
    << files.size() << "\n";
  Deque<File *>::iterator i;
  for (i = files.begin(); i != files.end(); i++) {
    (*i)->store(s);
    delete *i;
  }
  delete[] dirname;
  delete[] libname;
}

// AUTOMATON LIBRARY

AutLib::AutLib()
{
  char *s = getenv("MONALIB");
  if (s) {
    monalib = new char[strlen(s)+2];
    strcpy(monalib, s);
  }
  else {
    monalib = new char[3];
    strcpy(monalib, ".");
  }
  strcat(monalib, "/");
}

AutLib::~AutLib()
{
  // store changes + clean up
  Deque<Dir *>::iterator i;
  for (i = dirs.begin(); i != dirs.end(); i++)
    delete *i;
  delete[] monalib;
}

void 
AutLib::openDir(char *src, Deque<char *> *dependencies)
{
  // read <$MONALIB>/<src>.lib/LIB if not already done
  Deque<Dir *>::iterator i;
  for (i = dirs.begin(); i != dirs.end(); i++)
    if ((*i)->sourcename == src)
      return;

  char *s = src + strlen(src);
  while (s > src && *(s-1) != '/')
    s--;
  char *d = new char[strlen(s)+strlen(monalib)+1];
  strcpy(d, monalib);
  strcat(d, s);
  if (strlen(s) > 5 && strcmp(s+strlen(s)-5, ".mona") == 0)
    d[strlen(d)-5] = 0;
  strcat(d, ".lib");
  dirs.push_back(new Dir(d, src, dependencies));
}

char *
AutLib::getFileName(char *name, char *origin, Signature *sign, 
		    Deque<SSSet> *statespaces)
{
  // get or make filenumber + create filename
  Deque<Dir *>::iterator i;
  for (i = dirs.begin(); i != dirs.end(); i++)
    if ((*i)->sourcename == origin)
      return (*i)->getFileName(name, sign, statespaces);
  invariant(false);
  return 0;
}

bool 
AutLib::fileExists(char *filename)
{
  // check file exists
  std::ifstream s(filename);
  return s.good();
}
