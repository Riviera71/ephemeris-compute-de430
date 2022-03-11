// errorReport.c
// 
// -------------------------------------------------
// Copyright 2015-2022 Dominic Ford
//
// This file is part of EphemerisCompute.
//
// EphemerisCompute is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// EphemerisCompute is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EphemerisCompute.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "asciiDouble.h"
#include "strConstants.h"

#include "errorReport.h"

static char temp_stringA[LSTR_LENGTH], temp_stringB[LSTR_LENGTH], temp_stringC[LSTR_LENGTH], temp_stringD[LSTR_LENGTH], temp_stringE[LSTR_LENGTH];
char temp_err_string[FNAME_LENGTH];

//! ephem_error - Log an error message
//! \param msg The error message

void ephem_error(char *msg) {
    if ((msg != temp_stringA) && (msg != temp_stringB)) {
        strcpy(temp_stringA, msg);
        msg = temp_stringA;
    }
    if (DEBUG) {
        snprintf(temp_stringC, FNAME_LENGTH, "%s%s", "Error: ", msg);
        ephem_log(temp_stringC);
    }
    snprintf(temp_stringC, FNAME_LENGTH, "Error: %s\n", msg);
    fputs(temp_stringC, stderr);
}

//! ephem_fatal - Log a fatal error message, and exit with status 1
//! \param msg The error message

void ephem_fatal(char *file, int line, char *msg) {
    char introline[FNAME_LENGTH];
    if (msg != temp_stringE) strcpy(temp_stringE, msg);
    snprintf(introline, FNAME_LENGTH, "Fatal Error encountered in %s at line %d:", file, line);
    ephem_error(introline);
    ephem_error(temp_stringE);
    if (DEBUG) ephem_log("Terminating with error condition 1.");
    exit(1);
}

//! ephem_warning - Log a warning message
//! \param msg The warning message

void ephem_warning(char *msg) {
    if (msg != temp_stringA) strcpy(temp_stringA, msg);
    if (DEBUG) {
        snprintf(temp_stringC, FNAME_LENGTH, "%s%s", "Warning: ", temp_stringA);
        ephem_log(temp_stringC);
    }
    snprintf(temp_stringC, FNAME_LENGTH, "Warning: %s\n", temp_stringA);
    fputs(temp_stringC, stderr);
}

//! ephem_report - Log a report message
//! \param msg The report message

void ephem_report(char *msg) {
    if (msg != temp_stringA) strcpy(temp_stringA, msg);
    if (DEBUG) {
        snprintf(temp_stringC, FNAME_LENGTH, "%s%s", "Reporting: ", temp_stringA);
        ephem_log(temp_stringC);
    }
    snprintf(temp_stringC, FNAME_LENGTH, "%s\n", temp_stringA);
    fputs(temp_stringC, stdout);
}

//! ephem_log - Log an information message
//! \param msg The information message

void ephem_log(char *msg) {
    static FILE *logfile = NULL;
    static int latch = 0;
    char linebuffer[LSTR_LENGTH];

    if (latch) return; // Do not allow recursive calls, which might be generated by the call to ephem_fatal below
    latch = 1;
    if (logfile == NULL) {
        if ((logfile = fopen("ephem.log", "w")) == NULL) {
            ephem_fatal(__FILE__, __LINE__, "Could not open log file to write.");
            exit(1);
        }
        setvbuf(logfile, NULL, _IOLBF, 0); // Set log file to be line-buffered, so that log file is always up-to-date
    }

    if (msg != temp_stringD) strcpy(temp_stringD, msg);
    fprintf(logfile, "[%s] %s\n", str_strip(friendly_time_string(), linebuffer), temp_stringD);
    latch = 0;
}

void dcffread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (fread(ptr, size, nmemb, stream) != nmemb) ephem_fatal(__FILE__, __LINE__, "file read fail");
}
