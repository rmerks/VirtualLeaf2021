/*
 *
 *  This file is part of the Virtual Leaf.
 *
 *  VirtualLeaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VirtualLeaf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "warning.h"
#include "parse.h"
#include "output.h"
#include <string>

static const std::string _module_id("$Id$");

using namespace MyWarning;

char *ParsePar(FILE *fp, char *parameter, bool wrapflag) 
{
  char *line,*token;
  char *value;

  line=SearchToken(fp,parameter, wrapflag);
  if (line==NULL) {
    warning("Warning: Token %s not found.",parameter);
    value=0;
    return value;
  }

  /* parse the line on = sign */
  token=strtok(line,"=");
  if (token==NULL) {
    error("Parse error: no '=' sign found next to token %s, in line: \n %s.",
	  parameter,line);
  }

  // warning("Reading value for token %s...",token);
  fprintf(stderr, "[%s = ",token);

  token=strtok(NULL,"=");
  if (token==NULL)
    error("\nParse error: no value found after '=' sign, in line: \n %s",
	  line);

  value=strdup(token);
  free(line);

  return value;
}


int igetpar(FILE *fp,char *parameter, bool wrapflag) {

  // overloaded compatibility function. Doesn't need default parameter

  return igetpar(fp, parameter, 0, wrapflag);
}

int igetpar(FILE *fp,char *parameter, int default_val, bool wrapflag) 
{
  char *token;
  int value;

  /* Get token representing the value */
  token=ParsePar(fp,parameter, wrapflag);

  if (token==0) {
    /* default value */
    warning("No token %s found. Using default value %d.\n", parameter, default_val);
    return default_val;
  }
  /* read it */
  sscanf(token,"%d",&value);
  fprintf(stderr, "%d]\n",value);

  free(token);

  return value;
}

float fgetpar(FILE *fp,char *parameter, bool wrapflag) {

  // overloaded compatibility function. Doesn't need default parameter
  return fgetpar(fp, parameter, 0., wrapflag);
}

float fgetpar(FILE *fp, char *parameter, double default_val, bool wrapflag) 
{
  char *token;
  float value;

  /* Get token representing the value */
  token=ParsePar(fp,parameter, wrapflag);

  if (token==0) {
    /* default value */
    warning("No token %s found. Using default value %f.\n", parameter, default_val);
    return default_val;
  }

  /* read it */
  sscanf(token,"%e",&value);
  fprintf(stderr,"%e]\n",value);
  free(token);
  return value;
}


double *dgetparlist(FILE *fp,char *parameter, int n, bool wrapflag) 
{
  /* Get a list of n comma separated doubles */
  char *token;
  double *value;
  char *number;
  int i;

  value=(double *)malloc(n*sizeof(double));

  /* Get token representing the value */
  token=ParsePar(fp,parameter, wrapflag);

  if (token==0) {
    error("No token %s found.\n", parameter);
  }
  /* parse it */
  number=strtok(token,","); /* make a pointer to "token" */

  i=0;
  while (number!=NULL) {

    if (i>=n) {
      error("\nToo many values found for parameterlist '%s' (%d expected).",parameter,n);
    }

    sscanf(number,"%le",&value[i]);
    fprintf(stderr,"[%f]",value[i]);

    /* next value */
    number=strtok(NULL,",");
    i++;
  }

  fprintf(stderr,"]\n");

  if (i<n) {
    warning("Too few values found for parameterlist '%s' (%d expected).", parameter,n);
    warning("Padding with 0.");
    for (int j=i;j<n;j++) 
      value[j]=0.;

    fprintf(stderr, "Full array is ");
    for (int i=0;i<n;i++) {
      fprintf(stderr," %lf ",value[i]);
    }
    fprintf(stderr, "\n");
  }

  return value;
}

char *sgetpar(FILE *fp,char *parameter, bool wrapflag) 
{
  return sgetpar(fp, parameter, " ", wrapflag);
}

char *sgetpar(FILE *fp, char *parameter, const char *default_val, bool wrapflag) {

  char *token;
  char *value;
  int pos;

  /* Get token representing the value */
  token=ParsePar(fp,parameter, wrapflag);

  if (token==0) {
    /* default value */
    warning("No token %s found. Using default value '%s'.\n", parameter, default_val);
    value=strdup(default_val);
    return value;
  }

  /* strip leading spaces and duplicate string */
  pos=strspn(token," \t\n");
  value=(char *)malloc((strlen(&token[pos])+1)*sizeof(char));
  sprintf(value,"%s",&token[pos]);
  free(token);

  fprintf(stderr,"%s]\n",value);

  return value;
}

char *bool_str(bool bool_var) {

  /* Return string "true" if bool_var=true */
  /* Return string "false" if bool_var=false */

  static char t[5] = "true";
  static char f[6] = "false";

  if (bool_var) {
    return t;
  } else {
    return f;
  }
}

bool bgetpar(FILE *fp, char *parameter,  bool wrapflag) {

  // overloaded compatibility function. Doesn't need default parameter
  // default = false

  return bgetpar(fp, parameter, 0, wrapflag);
}

bool bgetpar(FILE *fp, char *parameter, int default_val, bool wrapflag) {

  /* Get boolean parameter. */
  /* if "true" or "yes", return 1 */
  /* if "false" or "no", return 0 */
  /* else complain */

  char *token;
  int value=0;
  int pos;

  /* Get token representing the value */
  token=ParsePar(fp,parameter, wrapflag);

  if (token==0) {
    /* default value */
    warning("No token %s found. Using default value %s.\n", parameter, bool_str(default_val));
    return default_val;
  }


  /* strip leading spaces */
  pos=strspn(token," \t\n");

  if (!strcmp(&token[pos],"yes") || !strcmp(&token[pos],"true") || !strcmp(&token[pos],"1"))
    value=1;
  else 
    if (!strcmp(&token[pos],"no") || !strcmp(&token[pos],"false") || !strcmp(&token[pos],"0"))
      value=0;
    else
      error("Keyword '%s' not recognized. Try yes/no or true/false.\n",&token[pos]);

  fprintf(stderr, "%s]\n",bool_str(value));

  return value;
}


char *SearchToken(FILE *fp, char *token,bool wrapflag) 
{
  /* This function returns the next line of FILE *fp that contains
     the string stored in the null terminated string token */

  /* remember to free the memory allocated for line */

  unsigned int len;
  char *line;
  int wrapped=false;
  long initial_position;

  char *tokenplusspace = (char *)malloc( (strlen(token)+3)*sizeof(char));
  strcpy(tokenplusspace, token);
  strcat(tokenplusspace," ");

  initial_position=ftell(fp);
  if (ferror(fp)) /* error occured */
    {
      error("%s",strerror(errno));
    }


  if (feof(fp)) {
    warning("End of file\n");
  }


  while (!(wrapped && ftell(fp)>=initial_position)) {

    /* As long as the search was not wrapped and we are not
     * back to where we were, continue searching */

    /* Read a line, and check whether an EOF was found */
    if ((line=ReadLine(fp))==NULL) {
      /* Yes? wrapflag on? => Wrap. */
      if (wrapflag) {
	wrapped=true;
	fseek(fp,0L,SEEK_SET);
	continue;
      } else 
	break;
    }

    /* strip leading spaces */
    int pos=strspn(line," \t\n");

    if (line[pos]=='#') {

      continue;
    } 

    len=strlen(line);
    if (strlen(tokenplusspace)<=len) {

      /* only if the line is longer than the token, it might be found */
      // if (strstr(line,tokenplusspace)!=NULL) /* FOUND */
      if (strstr(line,tokenplusspace)==(&line[pos])) /* FOUND */
	{
	  free(tokenplusspace);
	  return line;
	}
    }

    free(line);
  }
  free(tokenplusspace);
  return NULL; /* Token Not Found in the file */
}

int TokenInLineP(char *line,char *token) 
{
  if (strstr(token, line)!=NULL)
    return true;
  else
    return false;
}


void SkipLine(FILE *fp) {

  /* Just skips a line in FILE *fp */
  char *tmpstring;
  tmpstring=ReadLine(fp);
  free(tmpstring);
}

void SkipToken(FILE *fp,char *token, bool wrapflag)
{
  /* A very simple function:
     call SearchToken() and get rid of the memory returned by
     it.
     Also, return an error if the desired token was not found in the file.
  */
  char *tmppointer;

  tmppointer=SearchToken(fp,token, wrapflag);

  if (tmppointer==NULL) {
    error("Token `%s' not found by function SkipToken.\n",token);
  }

  free(tmppointer);
}

/* finis */
