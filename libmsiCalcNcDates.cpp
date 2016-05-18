// =-=-=-=-=-=-=-
#include "apiHeaderAll.hpp"
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"

// =-=-=-=-=-=-=-
// STL/boost Includes
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>


int AddOffsetDate(char* intervalunits, struct tm* local, float partedecimale, int parteintera);

extern "C" {
    double get_plugin_interface_version() {
        return 1.0;
    }


    // =-=-=-=-=-=-=-
    // Returns startdate and enddate for NetCDF file in format YYYYMMDDHHMM, given a "date since", an offset for start and an offset for end.  
    // Example: date_since="days since 1990-01-01" , offset_start = "0.5", offset_end = "15.5" RETURN startdate = "199001011200" , enddate = "199001161200"   

    int msiCalcNcDates_impl(msParam_t* _inTimeInterval, msParam_t* _inOffsetStart, msParam_t* _inOffsetEnd, msParam_t* _outStartDate,  msParam_t* _outEndDate, ruleExecInfo_t* rei) {

        char* TimeInterval = parseMspForStr(_inTimeInterval);
        char* OffStart = parseMspForStr(_inOffsetStart);
        char* OffEnd = parseMspForStr(_inOffsetEnd);
	float OffsetStart = atof(OffStart);
	float OffsetEnd = atof(OffEnd);
	
	struct tm localStart = { 0 };
   	struct tm localEnd = { 0 };
   	struct tm local = { 0 };
   	int parteinteraStart, parteinteraEnd;
   	float partedecimaleStart, partedecimaleEnd, tmp;
   	char intervalunits[10];

   	//float OffsetStart = 0.0625;
   	//float OffsetEnd = 10.0;
   	//char* TimeInterval = "days since 1860-1-1";
  
   	parteinteraStart = (int) OffsetStart;
   	partedecimaleStart = OffsetStart - parteinteraStart;
  	parteinteraEnd = (int) OffsetEnd;
   	partedecimaleEnd = OffsetEnd - parteinteraEnd;

   	if ( sscanf(TimeInterval, "%s %*s %d-%d-%d",intervalunits, &local.tm_year,
                     &local.tm_mon, &local.tm_mday) == 4 )
   	{
	  if (strcmp(intervalunits,"years")!=0 && strcmp(intervalunits,"months")!=0 && strcmp(intervalunits,"days")!=0 
		&& strcmp(intervalunits,"hours")!=0) return 1;
      	  local.tm_mon  -= 1;
      	  local.tm_year -= 1900;
      	  localStart = local;
      	  localEnd = local;

      	  AddOffsetDate(intervalunits, &localStart, partedecimaleStart, parteinteraStart);
      	  mktime(&localStart);

      	  AddOffsetDate(intervalunits, &localEnd, partedecimaleEnd, parteinteraEnd);
      	  mktime(&localEnd);


      	  char bufferStart[13];
      	  char bufferEnd[13];
      	  strftime(bufferStart, 13, "%Y%m%d%H%M", &localStart);
      	  strftime(bufferEnd, 13, "%Y%m%d%H%M", &localEnd);
	
          fillStrInMsParam(_outStartDate, bufferStart);
          fillStrInMsParam(_outEndDate, bufferEnd);
          
  	  // Done
          return 0;
        }
	else
	{
	  //return -1;
	  return 1;
	}


    }

    irods::ms_table_entry* plugin_factory() {
        irods::ms_table_entry* msvc = new irods::ms_table_entry(5);

        msvc->add_operation("msiCalcNcDates_impl", "msiCalcNcDates");

        return msvc;
    }

} // extern "C"


int AddOffsetDate(char* intervalunits, struct tm* local, float partedecimale, int parteintera)
{
    float tmp, new_decimal;
    int new_int;	
    char new_units[10];

      if (strcmp(intervalunits, "years") == 0) 
      {
        local->tm_year += parteintera;
        if (partedecimale > 0)
        {
          tmp = partedecimale*12;
          new_int = (int) tmp;
          new_decimal = tmp - new_int;
	  strcpy(new_units,"months");
	  AddOffsetDate(new_units, local, new_decimal, new_int);
        }
      }	 
      else if (strcmp(intervalunits, "months") == 0)
      {
        local->tm_mon += parteintera;
        if (partedecimale > 0)
        {
          tmp = partedecimale*30;
          new_int = (int) tmp;
          new_decimal = tmp - new_int;
	  strcpy(new_units,"days");
	  AddOffsetDate(new_units, local, new_decimal, new_int);
        }
        printf("Months\n" );
      }
      else if (strcmp(intervalunits, "days") == 0)
      {
        local->tm_mday += parteintera;
        if (partedecimale > 0)
        {
          tmp = partedecimale*24;
          new_int = (int) tmp;
          new_decimal = tmp - new_int;
	  strcpy(new_units,"hours");
	  AddOffsetDate(new_units, local, new_decimal, new_int);
        }
      }
      else if (strcmp(intervalunits, "hours") == 0)
      {
        local->tm_hour += parteintera;
        if (partedecimale > 0)
        {
          tmp = partedecimale*60;
          new_int = (int) tmp;
	  local->tm_min += new_int;
        }
      }
  return 0;
}
