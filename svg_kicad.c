/*
This file is part of SVG2Kicad.

    SVG2Kicad is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SVG2Kicad is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SVG2Kicad.  If not, see <http://www.gnu.org/licenses/>.

	Copyright © 2011 Robert Latham
*/

/*

  To compile the command line version compile this file (and its header) only

*/

// SVG to Kicad functions

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "svg_kicad.h"

#ifndef MFC
int main(int argc, char *argv[ ]) {
    int nArg, nArgSVG, nArgKicad, nArgType, nArgScale, nArgSmooth, nArgName;
    char cType;
    float fScale;
    unsigned int unSmoothness;
    char *pszSVGPathname;
    char *pszName;
    int nRet, nLen, nChar;
    char cScaleType;
    char cUnits1, cUnits2;
    boolean bExpectingParam;

    nArgSVG = nArgKicad = nArgType = nArgScale = nArgSmooth = nArgName = 0;
    cScaleType = -1;
    cUnits1 = cUnits2 = 0;
    bExpectingParam = false;

    for(nArg = 0; nArg < argc; nArg++) {
        if (stricmp(argv[nArg], "-i") == 0) {
            nArgSVG = nArg + 1;
            bExpectingParam = true;
        } else if (stricmp(argv[nArg], "-o") == 0) {
            nArgKicad = nArg + 1;
            bExpectingParam = true;
        } else if (stricmp(argv[nArg], "-t") == 0) {
            nArgType = nArg + 1;
            bExpectingParam = true;
        } else if (stricmp(argv[nArg], "-r") == 0) {
            nArgScale = nArg + 1;
            if (cScaleType == -1) {
                cScaleType = 0;
            } else {
                // Too many scale types specified

                cScaleType = -2;
            }
            bExpectingParam = true;
        } else if (stricmp(argv[nArg], "-w") == 0) {
            nArgScale = nArg + 1;
            if (cScaleType == -1) {
                cScaleType = 1;
            } else {
                // Too many scale types specified

                cScaleType = -2;
            }
            bExpectingParam = true;
        } else if (stricmp(argv[nArg], "-h") == 0) {
            nArgScale = nArg + 1;
            if (cScaleType == -1) {
                cScaleType = 2;
            } else {
                // Too many scale types specified

                cScaleType = -2;
            }
            bExpectingParam = true;
        } else if (stricmp(argv[nArg], "-s") == 0) {
            nArgSmooth = nArg + 1;
            bExpectingParam = true;
        } else if (stricmp(argv[nArg], "-n") == 0) {
            nArgName = nArg + 1;
            bExpectingParam = true;
        } else if (*argv[nArg] == '-') {
            if (!bExpectingParam) {
                PrintUsage();
                return -1;
            }
        }

        if (bExpectingParam) {
            bExpectingParam = false;
        }
    }

    if (cScaleType == -1) {
        // Select default

        cScaleType = 0;
    }

    if ((nArgSVG > 0) && (nArgKicad > 0) && (cScaleType > -1)) {
        if (nArgType == 0) {
            cType = 'e';
            pszSVGPathname = argv[nArgKicad];
            pszSVGPathname += strlen(pszSVGPathname);
            while ((pszSVGPathname != argv[nArgKicad]) && (*pszSVGPathname != '.')) {
                pszSVGPathname--;
            }

            if (*pszSVGPathname++ == '.') {
                if (stricmp(pszSVGPathname, "mod") == 0) {
                    cType = 'm';
                } else if (stricmp(pszSVGPathname, "lib") == 0) {
                    cType = 'l';
                }
            }

            printf("Output type not specified; using '%c'\r\n", cType);
        } else {
            cType = argv[nArgType][0];
        }

        if (nArgScale == 0) {
            fScale = 50;
            printf("Scale not specified; using %fx\r\n", fScale);
        } else {
            if (sscanf(argv[nArgScale], "%f%c%c", &fScale, &cUnits1, &cUnits2) > 1) {
                if (((cUnits1 == 'i') || (cUnits1 == 'I')) && ((cUnits2 == 'n') || (cUnits2 == 'N'))) {
                    // Using inches
                } else if (((cUnits1 == 'm') || (cUnits1 == 'M')) && ((cUnits2 == 'm') || (cUnits2 == 'M'))) {
                    // Using metric

                    fScale = fScale / 25.4f;
                } else {
                    PrintUsage();
                    return -1;
                }
            }
        }

        if (nArgSmooth == 0) {
            unSmoothness = 20;
            printf("Smoothness not specified; using %u\r\n", unSmoothness);
        } else {
            sscanf(argv[nArgSmooth], "%u", &unSmoothness);
            if (unSmoothness < 1) {
                PrintUsage();
                return -1;
            }
        }

        if (nArgName == 0) {
            pszName = malloc(8);
            if (pszName == NULL) {
                Log("Out of memory.");
            } else {
                strcpy(pszName, "FROMSVG");
                printf("Name not specified; using '%s'\r\n", pszName);
            }
        } else {
            nLen = strlen(argv[nArgName]);
            for (nChar = 0; nChar < nLen; nChar++) {
                if (isspace(argv[nArgName][nChar])) {
                    printf("Name must not include whitespace.\r\n");
                    return -1;
                }
            }
            pszName = malloc(strlen(argv[nArgName]) + 1);
            if (pszName == NULL) {
                Log("Out of memory.");
            } else {
                strcpy(pszName, argv[nArgName]);
            }
        }

        nRet = SVG2Kicad(argv[nArgSVG], argv[nArgKicad], pszName, cType, fScale, cScaleType, unSmoothness);
        if (GetLog() != NULL) {
            printf("\r\nResults:\r\n%s", GetLog());
        }
    
        if (pszName) {
            free(pszName);
        }

    } else {
        PrintUsage();
        nRet = -1;
    }

    DestroyLog();

    return nRet;
}

void PrintUsage(void) {
    printf("\r\nSVG2Kicad format\r\n"
        "\r\n"
        "SVG2Kicad -i <SVG Pathname>   # Pathname to SVG file\r\n"
        "          -o <Kicad pathname> # Pathname to Kicad file to be created\r\n"
        "          [-t e|m|l]          # Kicad file type\r\n"
        "          [-n name]           # Module or component name\r\n"
        "          [-r|w|h <scale>]    # Scaling (see below)\r\n"
        "          [-s <smoothness>]   # Lines used for PWL approximation (>= 1)\r\n"
        "\r\n"
        "Scaling can be relative (r), absolute width (w), or absolute height (h).\r\n"
        "If absolute, the <scale> figure can be suffixed 'mm' or 'in'.\r\n"
        "The default absolute scale is 'mm' (ie metric).\r\n\r\n"
        "Bug reports to: kicad-users@yahoogroups.com (patches welcome!)\r\n\r\n");
}
#endif // _MFC

int SVG2Kicad(const char *pszSVGPathname, const char *pszKicadPathname, const char* pszName, char cType, float fScale, char m_cScaleType, unsigned int unSmoothness) {
    FILE            *pfileSVG;
    FILE            *pfileKicad;
    char            pcContent[5];
    int                nRet;
    char            *szTag;
    int                nLenReqd;
    char            szTimestamp[100] = "";
    time_t            timeNow;
    boolean            bInDefs;
    char            *szDefs;
    float            fXTranslate, fYTranslate, fRotation;
    float            fWidth, fHeight;

    memset(pcContent, 0, 5);
    nRet = -1;
    pfileSVG = pfileKicad = NULL;
    bInDefs = false;
    szDefs = NULL;
    fXTranslate = fYTranslate = fRotation = 0;

    pfileSVG = fopen(pszSVGPathname, "rb");
    if (pfileSVG == NULL) {
        Log("Unable to open SVG file for reading.");
    } else {
        pfileKicad = fopen(pszKicadPathname, "wb");
        if (pfileKicad == NULL) {
            Log("Unable to open SVG file for writing.");
        } else {
            // Write the appropriate header

            time(&timeNow);
            if (cType == 'l') {
                strftime(szTimestamp, 99, " Date: %c", localtime(&timeNow));
                fprintf(pfileKicad, "EESchema-LIBRARY Version 2.3%s\r\n"
                            "#\r\n"
                            "# %s\r\n"
                            "#\r\n"
                            "DEF %s DWG 0 0 Y N 1 F N\r\n"
                            "F0 \"DWG\" 0 100 60 H V C C\r\n"
                            "F1 \"%s\" 0 -100 60 H V C C\r\n"
                            "DRAW\r\n", szTimestamp, pszName, pszName, pszName);
            } else {
                strftime(szTimestamp, 99, " %c", localtime(&timeNow));
                fprintf(pfileKicad, "PCBNEW-LibModule-V%s\r\n"
                            "# encoding utf-8\r\n"
                            "$INDEX\r\n"
                            "%s\r\n"
                            "$EndINDEX\r\n"
                            "$MODULE %s\r\n"
                            "Po 0 0 0 15 4EA584B9 00000000 ~~\r\n"
                            "Li %s\r\n"
                            "Sc 00000000\r\n"
                            "AR %s\r\n"
                            "Op 0 0 0\r\n"
                            "T0 0 -177 295 295 0 59 N V 21 N \"%s\"\r\n"
                            "T1 0 217 295 295 0 59 N I 21 N \"VAL**\"\r\n", szTimestamp, pszName, pszName, pszName, pszName, pszName);
            }

            // Look for start of tag

            do {
                nLenReqd = GetNextTagLen(pfileSVG);
                if (nLenReqd > 0) {
                    szTag = (char*) malloc(nLenReqd);
                    if (szTag == NULL) {
                        Log("Out of memory.");
                    } else {
                        // http://www.w3.org/TR/SVG/shapes.html

                        *szTag = 0;
                        ReadNextTag(pfileSVG, szTag, nLenReqd);
                        if (bInDefs) {
                            if (IsTag(szTag, "/defs")) {
                                bInDefs = false;
                            } else {
                                // Save for later use

                                if (szDefs == NULL) {
                                    szDefs = malloc(strlen(szTag) + 3);
                                    if (szDefs == NULL) {
                                        Log("Out of memory.");
                                    } else {
                                        *szDefs = 0;
                                    }
                                } else {
                                    int nLen;

                                    nLen = strlen(szDefs);
                                    szDefs = realloc(szDefs, nLen + strlen(szTag) + 3);
                                    if (szDefs == NULL) {
                                        Log("Out of memory.");
                                    } else {
                                        *(szDefs + nLen) = 0;
                                    }
                                }
                                if (szDefs != NULL) {
                                    strcat(szDefs, "<");
                                    strcat(szDefs, szTag);
                                    strcat(szDefs, ">");
                                }
                            }
                            if (IsTag(szTag, "text")) {
                                char szText[MAX_TEXT + 1];
                                char *pszText;
                                int nLen, nChar;
                                boolean bInTag;

                                // Save the text

                                pszText = szText;
                                memset(pszText, 0, 255);
                                do {
                                    nChar = fgetc(pfileSVG);
                                    if (nChar != EOF) {
                                        *pszText++ = (char) nChar;
                                        if (nChar == '<') {
                                            bInTag = true;
                                        } else if (nChar == '>') {
                                            bInTag = false;
                                        }
                                    }
                                } while ((strlen(szText) < 255) && (nChar != EOF) && (!bInTag || (strcmp(szText + strlen(szText) - 5, "/text") != 0)));

                                nLen = strlen(szDefs);
                                szDefs = realloc(szDefs, nLen + strlen(szText) + 4);
                                if (szDefs == NULL) {
                                    Log("Out of memory.");
                                } else {
                                    *(szDefs + nLen) = 0;
                                }
                                strcat(szDefs, szText);
                                strcat(szDefs, ">");
                            }
                        } else {
                            if (IsTag(szTag, "g")) {
                                GetTransform(szTag, &fXTranslate, &fYTranslate, &fRotation);
                            } else if (IsTag(szTag, "/g")) {
                                fXTranslate = fYTranslate = fRotation = 0;
                            } else if (IsTag(szTag, "defs")) {
                                bInDefs = true;
                            } else if (IsTag(szTag, "svg")) {
                                GetSize(szTag, &fWidth, &fHeight);
                                switch (m_cScaleType) {
                                case 0:

                                    // Nothing to do

                                    break;

                                case 1:

                                    // Scale by width

                                    if (fWidth > 0) {
                                        fScale = fScale * 10000.0f / fWidth;
                                        if (cType == 'l') {
                                            fScale = fScale / 10;
                                        }
                                    } else {
                                        Log("SVG file does not specify a width; using relative scaling.");
                                    }
                                    break;

                                case 2:

                                    // Scale by height

                                    if (fHeight > 0) {
                                        fScale = fScale * 10000.0f / fHeight;
                                        if (cType == 'l') {
                                            fScale = fScale / 10;
                                        }
                                    } else {
                                        Log("SVG file does not specify a height; using relative scaling.");
                                    }
                                    break;
                                }
                            } else {
                                ProcessTag(pfileSVG, NULL, pfileKicad, cType, szTag, fXTranslate, fYTranslate, fRotation, fScale, unSmoothness, szDefs);
                            }
                        }
                        free(szTag);
                    }
                }
            } while (nLenReqd > 0);
        }
    }

    if (pfileSVG != NULL) {
        fclose(pfileSVG);
    }
    if (pfileKicad != NULL) {
        if (cType == 'l') {
            fprintf(pfileKicad, "ENDDRAW\r\nENDDEF\r\n#\r\n#End Library");
        } else {
            fprintf(pfileKicad, "$EndMODULE %s\r\n$EndLIBRARY\r\n", pszName);
        }
        fclose(pfileKicad);
    }

    if (szDefs != NULL) {
        free(szDefs);
    }

    Log("Processing complete.");

    return 0;
}

void GetTransform(char *pszTag, float *pfXTranslate, float *pfYTranslate, float *pfRotation) {
    char *pszTranslate;
    char *pszRotate;
    char *pszTerminator;

    *pfXTranslate = *pfYTranslate = *pfRotation = 0;
    pszRotate = pszTranslate = GetVar(pszTag, " transform=");
    if (pszTranslate != NULL) {
        pszTerminator = pszTranslate + strlen(pszTranslate);

        // Look for a translation

        pszTranslate = strstr(pszTranslate, "translate");
        if (pszTranslate != NULL) {
            do {
                if (*pszTranslate++ == '(') {
                    if (sscanf(pszTranslate, "%f,%f", pfXTranslate, pfYTranslate) == 1) {
                        // No comma?

                        if (sscanf(pszTranslate, "%f%f", pfXTranslate, pfYTranslate) != 2) {
                            *pfXTranslate = *pfYTranslate = 0;
                        }
                    }
                    pszTranslate = NULL;
                }
            } while ((pszTranslate != NULL) && (*pszTranslate != ')') && (*pszTranslate != '>') && (*pszTranslate != 0));
        }

        // Look for a rotation

        pszRotate = strstr(pszRotate, "rotate");
        if (pszRotate != NULL) {
            do {
                if (*pszRotate++ == '(') {
                    sscanf(pszRotate, "%f", pfRotation);
                    *pfRotation = 2 * PI * *pfRotation / 360;
                    pszRotate = NULL;
                }
            } while ((pszRotate != NULL) && (*pszRotate != ')') && (*pszRotate != '>') && (*pszRotate != 0));
        }

        // Remove temporary zero terminator

        *pszTerminator = ' ';
    }
}

void GetSize(char *szTag, float *pfWidth, float *pfHeight) {
    char *pszWidth;
    char *pszHeight;
    char *pszViewbox;
    float fXStart, fYStart, fXEnd, fYEnd;

    // Get the variables, which requires replacing the temporary zero terminator

    *pfWidth = *pfHeight = 0;
    pszWidth = GetVar(szTag, " width=");
    if (pszWidth != NULL) {
        *pfWidth = (float) atof(pszWidth);
        *(pszWidth + strlen(pszWidth)) = ' ';
    }

    pszHeight = GetVar(szTag, " height=");
    if (pszHeight != NULL) {
        *pfHeight = (float) atof(pszHeight);
        *(pszHeight + strlen(pszHeight)) = ' ';
    }

    pszViewbox = GetVar(szTag, " viewBox=");
    if (pszViewbox != NULL) {
        if (sscanf(pszViewbox, "%f%f%f%f", &fXStart, &fYStart, &fXEnd, &fYEnd) == 4) {
            *pfWidth = fXEnd - fXStart;
            *pfHeight = fYEnd - fYStart;
        }
        *(pszHeight + strlen(pszHeight)) = ' ';
    }

    if (*pfWidth < 0) {
        *pfWidth = 0;
    }

    if (*pfHeight < 0) {
        *pfHeight = 0;
    }
}

void ProcessTag(FILE *pfileSVG, char **ppszDefs, FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale, unsigned int unSmoothness, char *szDefs) {
    char *pszLink;
    char *pszEndLink;
    char *pszDefs;
    char *pszID;
    char *szDefsTag;
    int nLen;
    boolean bInDef;

    if (IsTag(szTag, "use") && (szDefs != NULL)) {
        // Look for the definition

        bInDef = false;
        pszLink = strstr(szTag, "xlink:href");
        if (pszLink != NULL) {
            pszLink = strstr(pszLink, "#");
            if (pszLink != NULL) {
                pszLink++;
                pszEndLink = strstr(pszLink, "\"");
                if (pszEndLink != NULL) {
                    // Temporarily zero terminate

                    *pszEndLink = 0;

                    pszDefs = szDefs;
                    do {
                        nLen = ReadNextTagFromDefs(&pszDefs, NULL, 0);
                        if (nLen > 0) {
                            szDefsTag = malloc(nLen);
                            if (szDefsTag == NULL) {
                                Log("Out of memory.");
                            } else {
                                ReadNextTagFromDefs(&pszDefs, szDefsTag, nLen);
                                if (IsTag(szDefsTag, "g")) {
                                    pszID = GetVar(szDefsTag, " id=");
                                    if (pszID != NULL) {
                                        if (strcmp(pszLink, pszID) == 0) {
                                            // We've found it in the definitions

                                            bInDef = true;
                                        }

                                        // Reverse the zero terminator created to delimit the variable

                                        *(pszID + strlen(pszID)) = ' ';
                                    }
                                } else if (IsTag(szDefsTag, "/g")) {
                                    // End of definition

                                    bInDef = false;
                                } else if (bInDef) {
                                    // Process like any other tag

                                    ProcessTag(NULL, &pszDefs, pfileKicad, cType, szDefsTag, fXTranslate, fYTranslate, fRotation, fScale, unSmoothness, szDefs);
                                }

                                free(szDefsTag);
                            }
                        }
                    } while (nLen > 0);

                    // Remove the temporary zero terminator

                    *pszEndLink = '\"';
                }
            }
        }
    } else if (IsTag(szTag, "text")) {
        ProcessText(pfileSVG, ppszDefs, pfileKicad, cType, szTag, fXTranslate, fYTranslate, fRotation, fScale);
    } else if (IsTag(szTag, "path")) {
        ProcessPath(pfileKicad, cType, szTag, fXTranslate, fYTranslate, fRotation, fScale, unSmoothness);
    } else if (IsTag(szTag, "rect")) {
        ProcessRect(pfileKicad, cType, szTag, fXTranslate, fYTranslate, fRotation, fScale);
    } else if (IsTag(szTag, "line")) {
        ProcessLine(pfileKicad, cType, szTag, fXTranslate, fYTranslate, fRotation, fScale);
    } else if (IsTag(szTag, "circle")) {
        ProcessCircle(pfileKicad, cType, szTag, fXTranslate, fYTranslate, fScale);
    } else if (IsTag(szTag, "ellipse")) {
        ProcessEllipse(pfileKicad, cType, szTag, fXTranslate, fYTranslate, fRotation, fScale, unSmoothness);
    } else if (IsTag(szTag, "polygon")) {
        ProcessPoly(pfileKicad, cType, szTag, fXTranslate, fYTranslate, fRotation, fScale, true);
    } else if (IsTag(szTag, "polyline")) {
        ProcessPoly(pfileKicad, cType, szTag, fXTranslate, fYTranslate, fRotation, fScale, false);
    }
}

void ProcessText(FILE *pfileSVG, char **ppszDefs, FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale) {
    char *pszX;
    char *pszY;
    float fXStart, fYStart;
    char szText[MAX_TEXT + 1];
    char *pszText;
    int nChar;
    boolean bInTag;
    float fXObjectTranslate, fYObjectTranslate, fObjectRotation;

    pszText = szText;
    memset(pszText, 0, 255);
    bInTag = false;

    // Adjust for any transform

    GetTransform(szTag, &fXObjectTranslate, &fYObjectTranslate, &fObjectRotation);
    fXTranslate += fXObjectTranslate;
    fYTranslate += fYObjectTranslate;
    fRotation += fObjectRotation;

    // Get the variables, which requires replacing the temporary zero terminator

    pszX = GetVar(szTag, " x=");
    if (pszX != NULL) {
        fXStart = (float) atof(pszX);
        *(pszX + strlen(pszX)) = ' ';
    } else {
        fXStart = 0;
    }

    pszY = GetVar(szTag, " y=");
    if (pszY != NULL) {
        fYStart = (float) atof(pszY);
        *(pszY + strlen(pszY)) = ' ';
    } else {
        fYStart = 0;
    }

    if (ppszDefs != NULL) {
        *ppszDefs += strlen(szTag);
        while (*(*ppszDefs)++ != '>') {}
    }
    do {
        if (pfileSVG == NULL) {
            nChar = **ppszDefs;
            (*ppszDefs)++;
            if (nChar == 0) {
                nChar = EOF;
            }
        } else {
            nChar = fgetc(pfileSVG);
        }
        if (nChar != EOF) {
            *pszText++ = (char) nChar;
            if (nChar == '<') {
                bInTag = true;
            } else if (nChar == '>') {
                bInTag = false;
            }
        }
    } while ((strlen(szText) < 255) && (nChar != EOF) && (!bInTag || (strcmp(szText + strlen(szText) - 5, "/text") != 0)));

    // Set the zero terminator

    pszText = strrchr(szText, '<');
    if (pszText != NULL) {
        *pszText = 0;
    }

    WriteKicadText(pfileKicad, cType, fXStart, fYStart, szText, fXTranslate, fYTranslate, fRotation, fScale);
}

void ProcessRect(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale) {
    char *pszWidth;
    char *pszHeight;
    char *pszX;
    char *pszY;
    char *pszStrokeWidth;
    float fXStart, fYStart, fHeight, fWidth;
    float fXObjectTranslate, fYObjectTranslate, fObjectRotation;
    float fStrokeWidth;

    // Adjust for any transform

    GetTransform(szTag, &fXObjectTranslate, &fYObjectTranslate, &fObjectRotation);
    fXTranslate += fXObjectTranslate;
    fYTranslate += fYObjectTranslate;
    fRotation += fObjectRotation;

    // Get the variables, which requires replacing the temporary zero terminator

    pszWidth = GetVar(szTag, " width=");
    if (pszWidth != NULL) {
        fWidth = (float) atof(pszWidth);
        *(pszWidth + strlen(pszWidth)) = ' ';
    }

    pszHeight = GetVar(szTag, " height=");
    if (pszHeight != NULL) {
        fHeight = (float) atof(pszHeight);
        *(pszHeight + strlen(pszHeight)) = ' ';
    }

    pszX = GetVar(szTag, " x=");
    if (pszX != NULL) {
        fXStart = (float) atof(pszX);
        *(pszX + strlen(pszX)) = ' ';
    } else {
        fXStart = 0;
    }

    pszY = GetVar(szTag, " y=");
    if (pszY != NULL) {
        fYStart = (float) atof(pszY);
        *(pszY + strlen(pszY)) = ' ';
    } else {
        fYStart = 0;
    }

    pszStrokeWidth = GetVar(szTag, " stroke-width=");
    if (pszStrokeWidth != NULL) {
        fStrokeWidth = (float) atof(pszStrokeWidth);
        *(pszStrokeWidth + strlen(pszStrokeWidth)) = ' ';
    } else {
        fStrokeWidth = 0;
    }

    if ((pszWidth != NULL) && (pszHeight != NULL)) {
        if ((fWidth >=0) && (fHeight >= 0)) {
            // Draw the rectangle

            WriteKicadRect(pfileKicad, cType,  fXStart, fYStart, fXStart + fWidth, fYStart + fHeight, fXTranslate, fYTranslate, fRotation, fStrokeWidth, fScale);
        }
    } else {
        Log("Invalid rectangle in SVG file (width or height not specified).");
    }
}

void ProcessCircle(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fScale) {
    char *pszCx;
    char *pszCy;
    char *pszRadius;
    char *pszStrokeWidth;
    float fXCentre, fYCentre, fRadius;
    float fXObjectTranslate, fYObjectTranslate, fObjectRotation;
    float fStrokeWidth;

    // Adjust for any transform

    GetTransform(szTag, &fXObjectTranslate, &fYObjectTranslate, &fObjectRotation);
    fXTranslate += fXObjectTranslate;
    fYTranslate += fYObjectTranslate;

    // Get the variables, which requires replacing the temporary zero terminator

    pszCx = GetVar(szTag, " cx=");
    if (pszCx != NULL) {
        fXCentre = (float) atof(pszCx);
        *(pszCx + strlen(pszCx)) = ' ';
    } else {
        fXCentre = 0;
    }

    pszCy = GetVar(szTag, " cy=");
    if (pszCy != NULL) {
        fYCentre = (float) atof(pszCy);
        *(pszCy + strlen(pszCy)) = ' ';
    } else {
        fYCentre = 0;
    }

    pszStrokeWidth = GetVar(szTag, " stroke-width=");
    if (pszStrokeWidth != NULL) {
        fStrokeWidth = (float) atof(pszStrokeWidth);
        *(pszStrokeWidth + strlen(pszStrokeWidth)) = ' ';
    } else {
        fStrokeWidth = 0;
    }

    pszRadius = GetVar(szTag, " r=");
    if (pszRadius != NULL) {
        fRadius = (float) atof(pszRadius);
        *(pszRadius + strlen(pszRadius)) = ' ';
    }

    if (pszRadius != NULL) {
        if (fRadius > 0) {
            // Draw the circle

            WriteKicadCircle(pfileKicad, cType, fXCentre, fYCentre, fRadius, fXTranslate, fYTranslate, fStrokeWidth, fScale);
        }
    } else {
        Log("Invalid circle in SVG file (radius not specified).");
    }
}

void ProcessEllipse(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale, unsigned int unSmoothness) {
    char *pszCx;
    char *pszCy;
    char *pszRadiusX;
    char *pszRadiusY;
    char *pszStrokeWidth;
    float fXCentre, fYCentre, fXRadius, fYRadius;
    float fXStart, fYStart, fXEnd, fYEnd;
    unsigned int unSegment;
    float fXObjectTranslate, fYObjectTranslate, fObjectRotation;
    float fStrokeWidth;

    // Adjust for any transform

    GetTransform(szTag, &fXObjectTranslate, &fYObjectTranslate, &fObjectRotation);
    fXTranslate += fXObjectTranslate;
    fYTranslate += fYObjectTranslate;
    fRotation += fObjectRotation;

    // Get the variables, which requires replacing the temporary zero terminator

    pszCx = GetVar(szTag, " cx=");
    if (pszCx) {
        fXCentre = (float) atof(pszCx);
        *(pszCx + strlen(pszCx)) = ' ';
    } else {
        fXCentre = 0;
    }

    pszCy = GetVar(szTag, " cy=");
    if (pszCy) {
        fYCentre = (float) atof(pszCy);
        *(pszCy + strlen(pszCy)) = ' ';
    } else {
        fYCentre = 0;
    }

    pszRadiusX = GetVar(szTag, " rx=");
    if (pszRadiusX) {
        fXRadius = (float) atof(pszRadiusX);
        *(pszRadiusX + strlen(pszRadiusX)) = ' ';
    }
    pszRadiusY = GetVar(szTag, " ry=");
    if (pszRadiusY) {
        fYRadius = (float) atof(pszRadiusY);
        *(pszRadiusY + strlen(pszRadiusY)) = ' ';
    }

    pszStrokeWidth = GetVar(szTag, " stroke-width=");
    if (pszStrokeWidth != NULL) {
        fStrokeWidth = (float) atof(pszStrokeWidth);
        *(pszStrokeWidth + strlen(pszStrokeWidth)) = ' ';
    } else {
        fStrokeWidth = 0;
    }

    if ((pszRadiusX != NULL) && (pszRadiusY != NULL)) {
        if ((fXRadius > 0) && (fYRadius > 0)) {
            // Draw the ellipse

            EllipticArc(fXRadius, fYRadius, fRotation, fXCentre, fYCentre, 0, &fXStart, &fYStart);
            for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                // Piece-wise linear approximation

                EllipticArc(fXRadius, fYRadius, fRotation, fXCentre, fYCentre, 2 * PI * (float) unSegment / (float) unSmoothness, &fXEnd, &fYEnd);
                WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                fXStart = fXEnd;
                fYStart = fYEnd;
            }
        }
    } else {
        Log("Invalid ellipse in SVG file (one or both radii not specified).");
    }
}

void ProcessLine(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale) {
    char *pszX1;
    char *pszY1;
    char *pszX2;
    char *pszY2;
    char *pszStrokeWidth;
    float fXStart, fYStart, fXEnd, fYEnd;
    float fXObjectTranslate, fYObjectTranslate, fObjectRotation;
    float fStrokeWidth;

    // Adjust for any transform

    GetTransform(szTag, &fXObjectTranslate, &fYObjectTranslate, &fObjectRotation);
    fXTranslate += fXObjectTranslate;
    fYTranslate += fYObjectTranslate;
    fRotation += fObjectRotation;
    if (fRotation != 0) {
        Log("Note: Rotation of lines is currently not supported.");
    }

    // Get the variables, which requires replacing the temporary zero terminator

    fXStart = fYStart = fXEnd = fYEnd = 0;
    pszX1 = GetVar(szTag, " x1=");
    if (pszX1 != NULL) {
        fXStart = (float) atof(pszX1);
        *(pszX1 + strlen(pszX1)) = ' ';
    }
    pszY1 = GetVar(szTag, " y1=");
    if (pszY1 != NULL) {
        fYStart = (float) atof(pszY1);
        *(pszY1 + strlen(pszY1)) = ' ';
    }
    pszX2 = GetVar(szTag, " x2=");
    if (pszX2 != NULL) {
        fXEnd = (float) atof(pszX2);
        *(pszX2 + strlen(pszX2)) = ' ';
    }
    pszY2 = GetVar(szTag, " y2=");
    if (pszY2 != NULL) {
        fYEnd = (float) atof(pszY2);
        *(pszY2 + strlen(pszY2)) = ' ';
    }

    pszStrokeWidth = GetVar(szTag, " stroke-width=");
    if (pszStrokeWidth != NULL) {
        fStrokeWidth = (float) atof(pszStrokeWidth);
        *(pszStrokeWidth + strlen(pszStrokeWidth)) = ' ';
    } else {
        fStrokeWidth = 0;
    }

    if (!((fXStart == fXEnd) && (fYStart == fYEnd))) {
        // Draw the line

        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);
    } else {
        Log("Invalid line in SVG file (line has zero length).");
    }
}

void ProcessPoly(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale, boolean bClose) {
    char *pszPline;
    char *pszStrokeWidth;
    float fXInit, fYInit;
    float fXStart, fXEnd, fYStart, fYEnd;
    int nVars;
    float fXObjectTranslate, fYObjectTranslate, fObjectRotation;
    float fStrokeWidth;

    // Adjust for any transform

    GetTransform(szTag, &fXObjectTranslate, &fYObjectTranslate, &fObjectRotation);
    fXTranslate += fXObjectTranslate;
    fYTranslate += fYObjectTranslate;
    fRotation += fObjectRotation;
    if (fRotation != 0) {
        Log("Note: Rotation of polylines and polygons is currently not supported.");
    }

    // Stroke width

    pszStrokeWidth = GetVar(szTag, " stroke-width=");
    if (pszStrokeWidth != NULL) {
        fStrokeWidth = (float) atof(pszStrokeWidth);
        *(pszStrokeWidth + strlen(pszStrokeWidth)) = ' ';
    } else {
        fStrokeWidth = 0;
    }

    // Get the line start

    pszPline = GetVar(szTag, " points=");

    if (pszPline != NULL) {
        nVars = ReadLinear(pszPline, &fXStart, &fYStart);
        fXInit = fXStart;
        fYInit = fYStart;
        while ((nVars == 2) && (pszPline != NULL) && (*pszPline != 0)) {
            pszPline = GetNextPoint(pszPline);
            if (pszPline != NULL) {
                nVars = ReadLinear(pszPline, &fXEnd, &fYEnd);
                if (nVars == 2) {
                    // Draw line

                    WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);
                    fXStart = fXEnd;
                    fYStart = fYEnd;
                } else if (bClose) {
                    // Close polygon

                    fXStart = fXEnd;
                    fYStart = fYEnd;
                    fXEnd = fXInit;
                    fYEnd = fYInit;
                    WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);
                }
            }
        }
    } else {
        Log("Invalid polyline or polygon in SVG file (no points specified).");
    }
}

void ProcessPath(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale, unsigned int unSmoothness) {
    float fXStart, fYStart, fXEnd, fYEnd;
    float fXCurveStart, fYCurveStart, fXCurveEnd, fYCurveEnd;
    float fXControl1, fYControl1, fXControl2, fYControl2;
    float fXRadius, fYRadius, fXRot;
    float fXDelta, fYDelta, fInitX, fInitY;
    boolean bLargeArc, bSweep;
    char cLastCommand;
    boolean bEndOfLine;
    unsigned int unSegment;
    char *pszVar;
    float fXObjectTranslate, fYObjectTranslate, fObjectRotation;
    char *pszStrokeWidth;
    float fStrokeWidth;

    // Adjust for any transform

    GetTransform(szTag, &fXObjectTranslate, &fYObjectTranslate, &fObjectRotation);
    fXTranslate += fXObjectTranslate;
    fYTranslate += fYObjectTranslate;
    fRotation += fObjectRotation;
    if (fRotation != 0) {
        Log("Note: Rotation of paths is currently supported for elliptical arcs only.");
    }

    // Stroke width

    pszStrokeWidth = GetVar(szTag, " stroke-width=");
    if (pszStrokeWidth != NULL) {
        fStrokeWidth = (float) atof(pszStrokeWidth);
        *(pszStrokeWidth + strlen(pszStrokeWidth)) = ' ';
    } else {
        fStrokeWidth = 0;
    }

    // http://www.w3.org/TR/SVG/paths.html#PathDataMovetoCommands

    pszVar = GetVar(szTag, " d=");
    if (pszVar != NULL) {
        fXStart = fYStart = fXEnd = fYEnd = 0;
        bEndOfLine = false;
        cLastCommand = 'Z';

        do {
            switch (*pszVar++) {
            case 'M':
                if (ReadLinear(pszVar, &fXStart, &fYStart) == 2) {
                    if (cLastCommand == 'Z') {
                        fInitX = fXStart;
                        fInitY = fYStart;
                    }
                    bEndOfLine = false;
                } else {
                    pszVar = NULL;
                    Log("Invalid path move in SVG file (too few parameters).");
                }
                cLastCommand = *(pszVar - 1);
                break;

            case 'm':
                if (ReadLinear(pszVar, &fXDelta, &fYDelta) == 2) {
                    fXStart = fXEnd + fXDelta;
                    fYStart = fYEnd + fYDelta;
                    if (cLastCommand == 'Z') {
                        fInitX = fXStart;
                        fInitY = fYStart;
                    }
                    bEndOfLine = false;
                } else {
                    pszVar = NULL;
                    Log("Invalid path move in SVG file (too few parameters).");
                }
                cLastCommand = *(pszVar - 1);
                break;

            case 'L':
                if (ReadLinear(pszVar, &fXEnd, &fYEnd) == 2) {
                    bEndOfLine = true;
                } else {
                    pszVar = NULL;
                    Log("Invalid path line in SVG file (too few parameters).");
                }
                cLastCommand = *(pszVar - 1);
                break;

            case 'l':
                if (ReadLinear(pszVar, &fXDelta, &fYDelta) == 2) {
                    fXEnd = fXStart + fXDelta;
                    fYEnd = fYStart + fYDelta;
                    bEndOfLine = true;
                } else {
                    pszVar = NULL;
                    Log("Invalid path line in SVG file (too few parameters).");
                }
                cLastCommand = *(pszVar - 1);
                break;

            case 'H':
                if (ReadLinearOffset(pszVar, &fXEnd) == 1) {
                    bEndOfLine = true;
                } else {
                    pszVar = NULL;
                    Log("Invalid path horizontal line in SVG file (too few parameters).");
                }
                cLastCommand = *(pszVar - 1);
                break;

            case 'h':
                if (ReadLinearOffset(pszVar, &fXDelta) == 1) {
                    fXEnd = fXStart + fXDelta;
                    fYEnd = fYStart;
                    bEndOfLine = true;
                } else {
                    pszVar = NULL;
                    Log("Invalid path horizontal line in SVG file (too few parameters).");
                }
                cLastCommand = *(pszVar - 1);
                break;

            case 'V':
                if (ReadLinearOffset(pszVar, &fYEnd) == 1) {
                    bEndOfLine = true;
                } else {
                    pszVar = NULL;
                    Log("Invalid path vertical line in SVG file (too few parameters).");
                }
                cLastCommand = *(pszVar - 1);
                break;

            case 'v':
                if (ReadLinearOffset(pszVar, &fYDelta) == 1) {
                    fXEnd = fXStart;
                    fYEnd = fYStart + fYDelta;
                    bEndOfLine = true;
                } else {
                    pszVar = NULL;
                    Log("Invalid path vertical line in SVG file (too few parameters).");
                }
                cLastCommand = *(pszVar - 1);
                break;

            case 'C':
                cLastCommand = *(pszVar - 1);
                while (ReadCubic(&pszVar, &fXControl1, &fYControl1, &fXControl2, &fYControl2, &fXEnd, &fYEnd) == 6) {
                    fXCurveStart = fXStart;
                    fYCurveStart = fYStart;
                    fXCurveEnd = fXEnd;
                    fYCurveEnd = fYEnd;

                    for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                        // Piece-wise linear approximation

                        fXEnd = fXCurveEnd;
                        fYEnd = fYCurveEnd;
                        CubicBezier(fXCurveStart, fYCurveStart, &fXEnd, &fYEnd, fXControl1, fYControl1, fXControl2, fYControl2, ((float) unSegment) / unSmoothness);
                        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                        fXStart = fXEnd;
                        fYStart = fYEnd;
                    }
                }
                bEndOfLine = false;
                break;

            case 'c':
                cLastCommand = *(pszVar - 1);
                while (ReadCubic(&pszVar, &fXControl1, &fYControl1, &fXControl2, &fYControl2, &fXDelta, &fYDelta) == 6) {
                    fXControl1 = fXStart + fXControl1;
                    fYControl1 = fYStart + fYControl1;
                    fXControl2 = fXStart + fXControl2;
                    fYControl2 = fYStart + fYControl2;
                    fXCurveStart = fXStart;
                    fYCurveStart = fYStart;
                    fXCurveEnd = fXStart + fXDelta;
                    fYCurveEnd = fYStart + fYDelta;

                    for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                        // Piece-wise linear approximation

                        fXEnd = fXCurveEnd;
                        fYEnd = fYCurveEnd;
                        CubicBezier(fXCurveStart, fYCurveStart, &fXEnd, &fYEnd, fXControl1, fYControl1, fXControl2, fYControl2, ((float) unSegment) / unSmoothness);
                        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                        fXStart = fXEnd;
                        fYStart = fYEnd;
                    }
                }
                bEndOfLine = false;
                break;

            case 'S':
                cLastCommand = *(pszVar - 1);
                fXControl1 = (2 * fXEnd) - fXControl2;
                fYControl1 = (2 * fYEnd) - fYControl2;
                if (ReadShortCubic(&pszVar, &fXControl2, &fYControl2, &fXEnd, &fYEnd) == 4) {
                    fXCurveStart = fXStart;
                    fYCurveStart = fYStart;
                    fXCurveEnd = fXEnd;
                    fYCurveEnd = fYEnd;

                    for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                        // Piece-wise linear approximation

                        fXEnd = fXCurveEnd;
                        fYEnd = fYCurveEnd;
                        CubicBezier(fXCurveStart, fYCurveStart, &fXEnd, &fYEnd, fXControl1, fYControl1, fXControl2, fYControl2, ((float) unSegment) / unSmoothness);
                        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                        fXStart = fXEnd;
                        fYStart = fYEnd;
                    }
                } else {
                    Log("Invalid short cubic Bezier curve in SVG file (too few parameters).");
                }
                bEndOfLine = false;
                break;

            case 's':
                cLastCommand = *(pszVar - 1);
                fXControl1 = (2 * fXEnd) - fXControl2;
                fYControl1 = (2 * fYEnd) - fYControl2;
                if (ReadShortCubic(&pszVar, &fXControl2, &fYControl2, &fXDelta, &fYDelta) == 4) {
                    fXControl2 = fXStart + fXControl2;
                    fYControl2 = fYStart + fYControl2;
                    fXCurveStart = fXStart;
                    fYCurveStart = fYStart;
                    fXCurveEnd = fXStart + fXDelta;
                    fYCurveEnd = fYStart + fYDelta;

                    for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                        // Piece-wise linear approximation

                        fXEnd = fXCurveEnd;
                        fYEnd = fYCurveEnd;
                        CubicBezier(fXCurveStart, fYCurveStart, &fXEnd, &fYEnd, fXControl1, fYControl1, fXControl2, fYControl2, ((float) unSegment) / unSmoothness);
                        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                        fXStart = fXEnd;
                        fYStart = fYEnd;
                    }
                } else {
                    Log("Invalid short cubic Bezier curve in SVG file (too few parameters).");
                }
                bEndOfLine = false;
                break;

            case 'Q':
                cLastCommand = *(pszVar - 1);
                while (ReadQuadratic(&pszVar, &fXControl1, &fYControl1, &fXEnd, &fYEnd) == 4) {
                    fXCurveStart = fXStart;
                    fYCurveStart = fYStart;
                    fXCurveEnd = fXEnd;
                    fYCurveEnd = fYEnd;

                    for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                        // Piece-wise linear approximation

                        fXEnd = fXCurveEnd;
                        fYEnd = fYCurveEnd;
                        QuadraticBezier(fXCurveStart, fYCurveStart, &fXEnd, &fYEnd, fXControl1, fYControl1, ((float) unSegment) / unSmoothness);
                        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                        fXStart = fXEnd;
                        fYStart = fYEnd;
                    }
                }
                bEndOfLine = false;
                break;

            case 'q':
                cLastCommand = *(pszVar - 1);
                while (ReadQuadratic(&pszVar, &fXControl1, &fYControl1, &fXDelta, &fYDelta) == 6) {
                    fXControl1 = fXStart + fXControl1;
                    fYControl1 = fYStart + fYControl1;
                    fXCurveStart = fXStart;
                    fYCurveStart = fYStart;
                    fXCurveEnd = fXStart + fXDelta;
                    fYCurveEnd = fYStart + fYDelta;

                    for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                        // Piece-wise linear approximation

                        fXEnd = fXCurveEnd;
                        fYEnd = fYCurveEnd;
                        QuadraticBezier(fXCurveStart, fYCurveStart, &fXEnd, &fYEnd, fXControl1, fYControl1, ((float) unSegment) / unSmoothness);
                        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                        fXStart = fXEnd;
                        fYStart = fYEnd;
                    }
                }
                bEndOfLine = false;
                break;

            case 'T':
                cLastCommand = *(pszVar - 1);
                fXControl1 = (2 * fXEnd) - fXControl1;
                fYControl1 = (2 * fYEnd) - fYControl1;
                if (ReadShortQuadratic(pszVar, &fXEnd, &fYEnd) == 2) {
                    fXCurveStart = fXStart;
                    fYCurveStart = fYStart;
                    fXCurveEnd = fXEnd;
                    fYCurveEnd = fYEnd;

                    for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                        // Piece-wise linear approximation

                        fXEnd = fXCurveEnd;
                        fYEnd = fYCurveEnd;
                        QuadraticBezier(fXCurveStart, fYCurveStart, &fXEnd, &fYEnd, fXControl1, fYControl1, ((float) unSegment) / unSmoothness);
                        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                        fXStart = fXEnd;
                        fYStart = fYEnd;
                    }
                } else {
                    Log("Invalid short quadratic Bezier curve in SVG file (too few parameters).");
                }
                bEndOfLine = false;
                break;

            case 't':
                cLastCommand = *(pszVar - 1);
                fXControl1 = (2 * fXEnd) - fXControl1;
                fYControl1 = (2 * fYEnd) - fYControl1;
                if (ReadShortQuadratic(pszVar, &fXDelta, &fYDelta) == 2) {
                    fXCurveStart = fXStart;
                    fYCurveStart = fYStart;
                    fXCurveEnd = fXStart + fXDelta;
                    fYCurveEnd = fYStart + fYDelta;

                    for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                        // Piece-wise linear approximation

                        fXEnd = fXCurveEnd;
                        fYEnd = fYCurveEnd;
                        QuadraticBezier(fXCurveStart, fYCurveStart, &fXEnd, &fYEnd, fXControl1, fYControl1, ((float) unSegment) / unSmoothness);
                        WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                        fXStart = fXEnd;
                        fYStart = fYEnd;
                    }
                } else {
                    Log("Invalid short quadratic Bezier curve in SVG file (too few parameters).");
                }
                bEndOfLine = false;
                break;

            case 'A':
                cLastCommand = *(pszVar - 1);
                if (ReadEllipticArc(&pszVar, &fXRadius, &fYRadius, &fXRot, &bLargeArc, &bSweep, &fXEnd, &fYEnd) == 7) {
                    float            fXCentre, fYCentre, fStartAngle, fDeltaAngle;

                    fXRot = 2 * PI * fXRot / 360;
                    fXRot += fRotation;

                    // See http://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes

                    // Get angle in range 0 to 2PI - Why?

                    while (fXRot < 0) {
                        fXRot += 2 * PI;
                    }
                    
                    // Ensure radii are positive - F.6.6.1

                    if (fXRadius < 0) {
                        fXRadius = -fXRadius;
                    }
                    if (fYRadius < 0) {
                        fYRadius = -fYRadius;
                    }

                    if (!((fXStart == fXEnd) && (fYStart == fYEnd))) {
                        if ((fXRadius == 0) || (fYRadius == 0)) {
                            // Straight line!

                            WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);
                        } else {
                            GetEllipticArcSweep(fXRadius, fYRadius, fXRot, bLargeArc, bSweep, fXStart, fYStart, fXEnd, fYEnd, &fXCentre, &fYCentre, &fStartAngle, &fDeltaAngle);

                            for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                                // Piece-wise linear approximation

                                EllipticArc(fXRadius, fYRadius, fXRot, fXCentre, fYCentre, fStartAngle + fDeltaAngle * (float) unSegment / (float) unSmoothness, &fXEnd, &fYEnd);
                                WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                                fXStart = fXEnd;
                                fYStart = fYEnd;
                            }
                        }
                    } else {
                        Log("Invalid elliptic arc in SVG file (arc has zero length).");
                    }
                } else {
                    Log("Invalid elliptic arc in SVG file (too few parameters).");
                }
                bEndOfLine = false;
                break;

            case 'a':
                cLastCommand = *(pszVar - 1);
                if (ReadEllipticArc(&pszVar, &fXRadius, &fYRadius, &fXRot, &bLargeArc, &bSweep, &fXDelta, &fYDelta) == 7) {
                    float fXCentre, fYCentre, fStartAngle, fDeltaAngle;

                    fXEnd = fXStart + fXDelta;
                    fYEnd = fYStart + fYDelta;
                    fXRot = 2 * PI * fXRot / 360;
                    fXRot += fRotation;

                    // See http://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes

                    // Get angle in range 0 to 2PI - Why?

                    while (fXRot < 0) {
                        fXRot += 2 * PI;
                    }
                    
                    // Ensure radii are positive - F.6.6.1

                    if (fXRadius < 0) {
                        fXRadius = -fXRadius;
                    }
                    if (fYRadius < 0) {
                        fYRadius = -fYRadius;
                    }

                    if (!((fXStart == fXEnd) && (fYStart == fYEnd))) {
                        if ((fXRadius == 0) || (fYRadius == 0)) {
                            // Straight line!

                            WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);
                        } else {
                            GetEllipticArcSweep(fXRadius, fYRadius, fXRot, bLargeArc, bSweep, fXStart, fYStart, fXEnd, fYEnd, &fXCentre, &fYCentre, &fStartAngle, &fDeltaAngle);

                            for (unSegment = 1; unSegment <= unSmoothness; unSegment++) {
                                // Piece-wise linear approximation

                                EllipticArc(fXRadius, fYRadius, fXRot, fXCentre, fYCentre, fStartAngle + fDeltaAngle * (float) unSegment / (float) unSmoothness, &fXEnd, &fYEnd);
                                WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                                fXStart = fXEnd;
                                fYStart = fYEnd;
                            }
                        }
                    } else {
                        Log("Invalid elliptic arc in SVG file (arc has zero length).");
                    }
                } else {
                    Log("Invalid elliptic arc in SVG file (too few parameters).");
                }
                bEndOfLine = false;
                break;

            case 'Z':
            case 'z':
                fXStart = fXEnd;
                fYStart = fYEnd;
                fXEnd = fInitX;
                fYEnd = fInitY;
                bEndOfLine = true;
                cLastCommand = 'Z';
                break;
            }

            if (bEndOfLine) {
                bEndOfLine = false;

                WriteKicadLine(pfileKicad, cType,  fXStart, fYStart, fXEnd, fYEnd, fXTranslate, fYTranslate, fStrokeWidth, fScale);

                fXStart = fXEnd;
                fYStart = fYEnd;

            }
        } while ((pszVar != NULL) && (*pszVar != 0));
    }
}

void WriteKicadLine(FILE *pfileKicad, char cType, float fXStart, float fYStart, float fXEnd, float fYEnd, float fXTranslate, float fYTranslate, float fStrokeWidth, float fScale) {
    int nXStart, nYStart, nXEnd, nYEnd, nStrokeWidth;
    char szKicadLine[1000];

    // Create kicad line

    nXStart = Round((fXStart + fXTranslate) * fScale);
    nYStart = Round((fYStart + fYTranslate) * fScale);
    nXEnd = Round((fXEnd + fXTranslate) * fScale);
    nYEnd = Round((fYEnd + fYTranslate) * fScale);
    if (fStrokeWidth == 0)
    {
        nStrokeWidth = LINE_THICKNESS;
    }
    else
    {
        nStrokeWidth = Round(fStrokeWidth * fScale);
    }

    if (cType == 'l')
    {
        sprintf(szKicadLine, "P 2 0 1 0  %ld %ld %ld %ld N\r\n", nXStart, -nYStart, nXEnd, -nYEnd);
    }
    else
    {
        sprintf(szKicadLine, "DS %ld %ld %ld %ld %ld 21\r\n", nXStart, nYStart, nXEnd, nYEnd, nStrokeWidth);
    }
    fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
}

void WriteKicadRect(FILE *pfileKicad, char cType, float fXStart, float fYStart, float fXEnd, float fYEnd, float fXTranslate, float fYTranslate, float fRotation, float fStrokeWidth, float fScale)
{
    int            nX1, nY1, nX2, nY2, nX3, nY3, nX4, nY4, nStrokeWidth;
    char        szKicadLine[1000];
    float        fX1, fY1, fX2, fY2, fX3, fY3, fX4, fY4;
    float        fXOffset, fYOffset;

    // Translate and scale

    fXStart = (fXStart + fXTranslate) * fScale;
    fYStart = (fYStart + fYTranslate) * fScale;
    fXEnd = (fXEnd + fXTranslate) * fScale;
    fYEnd = (fYEnd + fYTranslate) * fScale;

    // Rotate

    fXOffset = fXStart;
    fYOffset = fYStart;
    fXStart -= fXOffset;
    fYStart -= fYOffset;
    fXEnd -= fXOffset;
    fYEnd -= fYOffset;

    fX1 = ((float) cos(fRotation) * fXStart - (float) sin(fRotation) * fYStart) + fXOffset;
    fY1 = ((float) sin(fRotation) * fXStart + (float) cos(fRotation) * fYStart) + fYOffset;
    fX2 = ((float) cos(fRotation) * fXEnd - (float) sin(fRotation) * fYStart + fXOffset);
    fY2 = ((float) sin(fRotation) * fXEnd + (float) cos(fRotation) * fYStart) + fYOffset;
    fX3 = ((float) cos(fRotation) * fXEnd - (float) sin(fRotation) * fYEnd + fXOffset);
    fY3 = ((float) sin(fRotation) * fXEnd + (float) cos(fRotation) * fYEnd + fYOffset);
    fX4 = ((float) cos(fRotation) * fXStart - (float) sin(fRotation) * fYEnd + fXOffset);
    fY4 = ((float) sin(fRotation) * fXStart + (float) cos(fRotation) * fYEnd + fYOffset);

    // Create kicad rect

    nX1 = Round(fX1);
    nY1 = Round(fY1);
    nX2 = Round(fX2);
    nY2 = Round(fY2);
    nX3 = Round(fX3);
    nY3 = Round(fY3);
    nX4 = Round(fX4);
    nY4 = Round(fY4);
    if (fStrokeWidth == 0)
    {
        nStrokeWidth = LINE_THICKNESS;
    }
    else
    {
        nStrokeWidth = Round(fStrokeWidth * fScale);
    }

    if (cType == 'l')
    {
        if (fRotation == 0)
        {
            sprintf(szKicadLine, "S %ld %ld %ld %ld 0 1 0 N\r\n", nX1, -nY1, nX3, -nY3);
            fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
        }
        else
        {
            sprintf(szKicadLine, "P 2 0 1 0  %ld %ld %ld %ld N\r\n", nX1, -nY1, nX2, -nY2);
            fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
            sprintf(szKicadLine, "P 2 0 1 0  %ld %ld %ld %ld N\r\n", nX2, -nY2, nX3, -nY3);
            fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
            sprintf(szKicadLine, "P 2 0 1 0  %ld %ld %ld %ld N\r\n", nX3, -nY3, nX4, -nY4);
            fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
            sprintf(szKicadLine, "P 2 0 1 0  %ld %ld %ld %ld N\r\n", nX4, -nY4, nX1, -nY1);
            fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
        }
    }
    else
    {
        sprintf(szKicadLine, "DS %ld %ld %ld %ld %ld 21\r\n", nX1, nY1, nX2, nY2, nStrokeWidth);
        fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
        sprintf(szKicadLine, "DS %ld %ld %ld %ld %ld 21\r\n", nX2, nY2, nX3, nY3, nStrokeWidth);
        fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
        sprintf(szKicadLine, "DS %ld %ld %ld %ld %ld 21\r\n", nX3, nY3, nX4, nY4, nStrokeWidth);
        fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
        sprintf(szKicadLine, "DS %ld %ld %ld %ld %ld 21\r\n", nX4, nY4, nX1, nY1, nStrokeWidth);
        fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
    }
}

void WriteKicadCircle(FILE *pfileKicad, char cType, float fXCentre, float fYCentre, float fXRadius, float fXTranslate, float fYTranslate, float fStrokeWidth, float fScale)
{
    int            nXCentre, nYCentre, nXEdge, nXRadius, nStrokeWidth;
    char        szKicadLine[1000];

    // Create kicad line

    nXCentre = Round((fXCentre + fXTranslate) * fScale);
    nYCentre = Round((fYCentre + fYTranslate) * fScale);
    nXRadius = Round(fXRadius);
    nXEdge = Round((fXCentre + fXTranslate + fXRadius) * fScale);
    if (fStrokeWidth == 0)
    {
        nStrokeWidth = LINE_THICKNESS;
    }
    else
    {
        nStrokeWidth = Round(fStrokeWidth * fScale);
    }

    if (cType == 'l')
    {
        sprintf(szKicadLine, "C %ld %ld %ld 0 1 0 N\r\n", nXCentre, -nYCentre, nXRadius << 1);
    }
    else
    {
        sprintf(szKicadLine, "DC %ld %ld %ld %ld %ld 21\r\n", nXCentre, nYCentre, nXEdge, nYCentre, nStrokeWidth);
    }
    fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
}

void WriteKicadText(FILE *pfileKicad, char cType, float fXStart, float fYStart, char *pszText, float fXTranslate, float fYTranslate, float fRotation, float fScale)
{
    int            nXStart, nYStart, nRotation;
    char        szKicadLine[1000];

    // Create kicad line

    nXStart = Round((fXStart + fXTranslate) * fScale);
    nYStart = Round((fYStart + fYTranslate) * fScale);
    nRotation = Round(3600 * fRotation / (2 * PI));

    if (cType == 'l')
    {
        sprintf(szKicadLine, "T %ld %ld %ld %ld 0 0 0 \"%s\" Normal 0 L C\r\n", nRotation, nXStart, -nYStart, TEXT_HEIGHT / 10, pszText);
    }
    else
    {
        sprintf(szKicadLine, "T2 %ld %ld %ld %ld %ld %ld N V 21 N \"%s\"\r\n", nXStart, nYStart, TEXT_WIDTH, TEXT_HEIGHT, nRotation, LINE_THICKNESS, pszText);
    }
    fwrite(szKicadLine, 1, strlen(szKicadLine), pfileKicad);
}

int ReadLinear(char *pszVar, float *pfX, float *pfY)
{
    int            nRet;

    // Get X and Y

    nRet = sscanf(pszVar, "%f,%f", pfX, pfY);
    if (nRet == 1)
    {
        // No comma?

        nRet = sscanf(pszVar, "%f%f", pfX, pfY);
    }

    return nRet;
}

int ReadLinearOffset(char *pszVar, float *pfOffset)
{
    int            nRet;

    // Get offset

    nRet = sscanf(pszVar, "%f", pfOffset);

    return nRet;
}

int ReadCubic(char **ppszVar, float *pfX1, float *pfY1, float *pfX2, float *pfY2, float *pfX, float *pfY)
{
    int            nRet, nRetPoint;

    nRet = 0;

    // Get dimensions

    nRetPoint = sscanf(*ppszVar, "%f,%f", pfX1, pfY1);
    if (nRetPoint == 1)
    {
        nRetPoint = sscanf(*ppszVar, "%f%f", pfX1, pfY1);
    }
    nRet += nRetPoint;

    if (nRetPoint == 2)
    {
        *ppszVar = GetNextPoint(*ppszVar);
        nRetPoint = sscanf(*ppszVar, "%f,%f", pfX2, pfY2);
        if (nRetPoint == 1)
        {
            nRetPoint = sscanf(*ppszVar, "%f%f", pfX2, pfY2);
        }
        nRet += nRetPoint;
    }

    if (nRetPoint == 2)
    {
        *ppszVar = GetNextPoint(*ppszVar);
        nRetPoint = sscanf(*ppszVar, "%f,%f", pfX, pfY);
        if (nRetPoint == 1)
        {
            nRetPoint = sscanf(*ppszVar, "%f%f", pfX, pfY);
        }
        nRet += nRetPoint;

    }

    if (nRetPoint == 2)
    {
        *ppszVar = GetNextPoint(*ppszVar);
    }

    return nRet;
}

int ReadShortCubic(char **ppszVar, float *pfX2, float *pfY2, float *pfX, float *pfY)
{
    int            nRet, nRetPoint;

    nRet = 0;

    // Get dimensions

    nRetPoint = sscanf(*ppszVar, "%f,%f", pfX2, pfY2);
    if (nRetPoint == 1)
    {
        nRetPoint = sscanf(*ppszVar, "%f%f", pfX2, pfY2);
    }
    nRet += nRetPoint;

    if (nRetPoint == 2)
    {
        *ppszVar = GetNextPoint(*ppszVar);
        nRetPoint = sscanf(*ppszVar, "%f,%f", pfX, pfY);
        if (nRetPoint == 1)
        {
            nRetPoint = sscanf(*ppszVar, "%f%f", pfX, pfY);
        }
        nRet += nRetPoint;
    }

    if (nRetPoint == 2)
    {
        *ppszVar = GetNextPoint(*ppszVar);
    }

    return nRet;
}

int ReadQuadratic(char **ppszVar, float *pfX1, float *pfY1, float *pfX, float *pfY)
{
    return ReadShortCubic(ppszVar, pfX1, pfY1, pfX, pfY);
}

int ReadShortQuadratic(char *pszVar, float *pfX, float *pfY)
{
    return ReadLinear(pszVar, pfX, pfY);
}

int ReadEllipticArc(char **ppszVar, float *pfXRadius, float *pfYRadius, float *pfXRot, boolean *pbLargeArc, boolean *pbSweep, float *pfXEnd, float *pfYEnd)
{
    int            nRet, nRetPoint;

    // Get X and Y

    nRet = 0;
    nRetPoint = sscanf(*ppszVar, "%f,%f", pfXRadius, pfYRadius);
    if (nRetPoint == 1)
    {
        // No comma?

        nRetPoint = sscanf(*ppszVar, "%f%f", pfXRadius, pfYRadius);
    }
    nRet += nRetPoint;

    if (nRetPoint == 2)
    {
        *ppszVar = GetNextPoint(*ppszVar);
        nRetPoint = sscanf(*ppszVar, "%f", pfXRot);
        nRet += nRetPoint;
    }

    if (nRetPoint == 1)
    {
        *ppszVar = GetNextValue(*ppszVar);
        nRetPoint = sscanf(*ppszVar, "%uc", pbLargeArc);
        nRet += nRetPoint;
    }

    if (nRetPoint == 1)
    {
        *ppszVar = GetNextValue(*ppszVar);
        nRetPoint = sscanf(*ppszVar, "%uc", pbSweep);
        nRet += nRetPoint;
    }

    if (nRetPoint == 1)
    {
        *ppszVar = GetNextValue(*ppszVar);
        nRetPoint = sscanf(*ppszVar, "%f,%f", pfXEnd, pfYEnd);
        if (nRetPoint == 1)
        {
            // No comma?

            nRetPoint = sscanf(*ppszVar, "%f%f", pfXEnd, pfYEnd);
        }
        nRet += nRetPoint;
    }

    return nRet;
}

char *GetNextPoint(char *pszVar)
{
    char            *pszNext;

    pszNext = GetNextValue(pszVar);
    if (*pszNext != 0)
    {
        pszNext = GetNextValue(pszNext);
    }

    return pszNext;
}

char *GetNextValue(char *pszVar)
{
    char            *pszNext;

    pszNext = pszVar;

    // Dump whitespace

    while ((*pszNext != 0) && (*pszNext == ' '))
    {
        *pszNext++;
    }

    do
    {
        *pszNext++;
    }
    while ((*pszNext != '-') && (*pszNext != ',') && (*pszNext != ' ') && (*pszNext != 0) && !isalpha(*pszNext));

    if ((*pszNext != 0) && (*pszNext == ','))
    {
        *pszNext++;
    }

    return pszNext;
}

void CubicBezier(float fXStart, float fYStart, float *pfXEnd, float *pfYEnd, float fX1, float fY1, float fX2, float fY2, float ft)
{
    // B(t) = (1-t)^3P0 + 3P1t(1-t)^2 + 3P2(1-t)t^2 + t^3P3

    float            fXEnd, fYEnd, f1_t, ft2, f1_t2;

    f1_t = 1 - ft;
    ft2 = ft * ft;
    f1_t2 = f1_t * f1_t;
    fXEnd = (float) ((fXStart * f1_t2 * f1_t) + (3 * fX1 * ft * f1_t2) + (3 * fX2 * f1_t * ft2) + (*pfXEnd * ft2 * ft));
    fYEnd = (float) ((fYStart * f1_t2 * f1_t) + (3 * fY1 * ft * f1_t2) + (3 * fY2 * f1_t * ft2) + (*pfYEnd * ft2 * ft));

    *pfXEnd = fXEnd;
    *pfYEnd = fYEnd;
}

void QuadraticBezier(float fXStart, float fYStart, float *pfXEnd, float *pfYEnd, float fX1, float fY1, float ft)
{
    // B(t) = (1-t)^2P0 + 2P1t(1-t) + t^2P2

    float            fXEnd, fYEnd, f1_t, ft2, f1_t2;

    f1_t = 1 - ft;
    ft2 = ft * ft;
    f1_t2 = f1_t * f1_t;
    fXEnd = (float) ((fXStart * f1_t2) + (2 * fX1 * ft * f1_t) + (*pfXEnd * ft2));
    fYEnd = (float) ((fYStart * f1_t2) + (2 * fY1 * ft * f1_t) + (*pfYEnd * ft2));

    *pfXEnd = fXEnd;
    *pfYEnd = fYEnd;
}

void GetEllipticArcSweep(float fXRadius, float fYRadius,
                        float fXRot, boolean bLargeArc, boolean bSweep,
                        float fXStart, float fYStart, float fXEnd, float fYEnd,
                        float *pfCX, float *pfCY,
                        float *pfStartAngle, float *pfDeltaAngle)
{
    float            fX1, fY1, fCX1, fCY1, fM;
    float            fRX2, fRY2;

    // Here be dragons
    // If you really want to know, see
    // http://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes

    // F.6.5.1

    fX1 = ((float) cos(fXRot) * ((fXStart - fXEnd) / 2)) + ((float) sin(fXRot) * ((fYStart - fYEnd) / 2));
    fY1 = ((float) -sin(fXRot) * ((fXStart - fXEnd) / 2)) + ((float) cos(fXRot) * ((fYStart - fYEnd) / 2));

    // Ensure radii arelarge enough - F.6.6.2 - F.6.6.3

    fM = ((fX1 * fX1) / (fXRadius * fXRadius)) + (fY1 * fY1) / (fYRadius * fYRadius);
    if (fM > 1.0f)
    {
        fM = (float) sqrt(fM);
        fXRadius *= fM;
        fXRadius *= fM;
    }

    // F.6.5.2

    fRX2  = fXRadius * fXRadius;
    fRY2  = fYRadius * fYRadius;
    fM = (fRX2 * fRY2 - fRX2 * fY1 * fY1 - fRY2 * fX1 * fX1) / (fRX2 * fY1 * fY1 + fRY2 * fX1 * fX1);

    // Rounding error protection

    if (fM < 0)
    {
        fM = 0;
    }

    fM = (float) sqrt(fM);
    if (bLargeArc == bSweep)
    {
        fM = -fM;
    }
    fCX1 = fM * fXRadius * fY1 / fYRadius;
    fCY1 = -fM * fYRadius * fX1 / fXRadius;

    // F.6.5.3

    *pfCX = ((float) cos(fXRot) * fCX1) - ((float) sin(fXRot) * fCY1) + ((fXStart + fXEnd) / 2);
    *pfCY = ((float) sin(fXRot) * fCX1) + ((float) cos(fXRot) * fCY1) + ((fYStart + fYEnd) / 2);

    // Get angles

    *pfStartAngle = GetVectorAngle(1, 0, (fX1 - fCX1) / fXRadius, (fY1 - fCY1) / fYRadius);
    *pfDeltaAngle = GetVectorAngle((fX1 - fCX1) / fXRadius, (fY1 - fCY1) / fYRadius, (-fX1 - fCX1) / fXRadius, (-fY1 - fCY1) / fYRadius);

    // Place angles in range

    while (*pfDeltaAngle > 2 * PI)
    {
        *pfDeltaAngle = *pfDeltaAngle - 2 * PI;
    }
    while (*pfDeltaAngle < 0)
    {
        *pfDeltaAngle = *pfDeltaAngle + 2 * PI;
    }

    if (bSweep)
    {
        if (*pfDeltaAngle < 0)
        {
            *pfDeltaAngle = *pfDeltaAngle + 2 * PI;
        }
    }
    else
    {
        if (*pfDeltaAngle > 0)
        {
            *pfDeltaAngle = *pfDeltaAngle - 2 * PI;
        }
    }
}

void EllipticArc(float fXRadius, float fYRadius, float fXRot,
                        float fXCentre, float fYCentre, float fTheta,
                        float *pfX, float *pfY)
{
    *pfX = ((float) (cos(fXRot) * cos(fTheta)) * fXRadius) - ((float) (sin(fXRot) * sin(fTheta)) * fYRadius) + fXCentre;
    *pfY = ((float) (sin(fXRot) * cos(fTheta)) * fXRadius) + ((float) (cos(fXRot) * sin(fTheta)) * fYRadius) + fYCentre;
}

float GetVectorAngle(float fUx, float fUy, float fVx, float fVy)
{
    float            fMagU, fMagV, fAngle, fCosAngle;

    // Gets the angle between two vectors

    fMagU = (float) sqrt(fUx * fUx + fUy * fUy);
    fMagV = (float) sqrt(fVx * fVx + fVy * fVy);
    fCosAngle = (fUx * fVx + fUy * fVy) / (fMagU * fMagV);

    // Rounding error protection

    if (fCosAngle > 1.0f)
    {
        fCosAngle = 1.0f;
    }
    if (fCosAngle < -1.0f)
    {
        fCosAngle = -1.0f;
    }

    fAngle = (float) acos(fCosAngle);
    if (fUx * fVy - fUy * fVx < 0)
    {
        fAngle = -fAngle;
    }
    
    return fAngle;
}

char *GetVar(char *szTag, char *szVarName)
{
    char            *pszVar;
    char            *pszEndVar;

    pszVar = strstr(szTag, szVarName);

    if (pszVar == NULL)
    {
        if (strcmp(szVarName, " stroke-width=") == 0)
        {
            // In a style element?

            pszVar = strstr(szTag, " style=");
            if (pszVar)
            {
                char            *pszStyleName;

                pszStyleName = malloc(strlen(szVarName) + 1);
                if (pszStyleName == NULL)
                {
                    Log("Out of memory.");
                    pszVar = NULL;
                }
                else
                {
                    strcpy(pszStyleName, szVarName);
                    *strrchr(pszStyleName, '=') = ':';
                    pszVar = strstr(szTag, pszStyleName);
                    if (pszVar != NULL)
                    {
                        pszVar += strlen(pszStyleName) + 1;
                        pszEndVar = pszVar;
                        while ((*pszEndVar != 0) && (*pszEndVar != ';') && (*pszEndVar != '\"'))
                        {
                            *pszEndVar++;
                        }
                        if ((*pszEndVar != ';') || (*pszEndVar != '\"'))
                        {
                            *pszEndVar = 0;
                        }
                        else
                        {
                            pszVar = NULL;
                        }
                    }
                    free(pszStyleName);
                }
            }
        }
    }
    else
    {
        while ((*pszVar != 0) && (*pszVar != '\"'))
        {
            *pszVar++;
        }
        if (*pszVar == '\"')
        {
            *pszVar++;
        }

        pszEndVar = strstr(pszVar, "\"");
        if (pszEndVar != NULL)
        {
            *pszEndVar = 0;
        }
    }

    return pszVar;
}

boolean IsTag(const char *pszTag, const char *pszTagName)
{
    boolean            bIsTag;
    char            *pszTerminator;

    bIsTag = (strncmp(pszTag, pszTagName, strlen(pszTagName)) == 0);
    if (bIsTag)
    {
        // Might be - take a closer look

        pszTerminator = strstr(pszTag, pszTagName) + strlen(pszTagName);
        bIsTag = (*pszTerminator == ' ') || (*pszTerminator == 0);
    }

    return bIsTag;
}

int GetNextTagLen(FILE *pfileSVG)
{
    return ReadNextTag(pfileSVG, NULL, 0);
}

int ReadNextTag(FILE *pfileSVG, char *szTag, int nLenTag)
{
    fpos_t            posFile;
    int                nChar, nLenReqd;
    char            *pszTag;

    if (szTag)
    {
        nLenReqd = fread(szTag, 1, nLenTag, pfileSVG);
        *(szTag + nLenTag - 1) = 0;
    }
    else
    {
        nLenReqd = 0;
        pszTag = szTag;

        do
        {
            nChar = fgetc(pfileSVG);
        }
        while ((nChar != '<') && (nChar != EOF));

        if (nChar != EOF)
        {
            // Dump whitespace

            do
            {
                nChar = fgetc(pfileSVG);
            }
            while (isspace(nChar) && (nChar != EOF));

            if ((nChar != EOF) && fgetpos(pfileSVG, &posFile) == 0)
            {
                // Read until the end of the tag

                while ((nChar != '>') && (nChar != EOF))
                {
                    if ((nChar != '>') && (nChar != EOF))
                    {
                        nLenReqd++;
                        if ((szTag != NULL) && (nLenReqd < nLenTag))
                        {
                            *pszTag = (char) nChar;
                            pszTag++;
                            *pszTag = 0;
                        }
                    }
                    nChar = fgetc(pfileSVG);
                }
            }

            if (nChar == EOF)
            {
                nLenReqd = -1;
            }
            else
            {
                if ((szTag == NULL) && (fsetpos(pfileSVG, &posFile) != 0))
                {
                    nLenReqd = -1;
                }
                else
                {
                    fseek(pfileSVG, -1, SEEK_CUR);
                    nLenReqd++;    // terminator
                }
            }
        }
    }

    return nLenReqd;
}

int ReadNextTagFromDefs(char **ppszDefs, char *szTag, int nLenTag)
{
    int                nChar, nLenReqd;
    char            *pszTag;
    char            *pszDefs;

    if (szTag)
    {
        strncpy(szTag, *ppszDefs, nLenTag - 1);
        *(szTag + nLenTag - 1) = 0;
        nLenReqd = strlen(szTag);
    }
    else
    {
        nLenReqd = 0;
        pszTag = szTag;

        do
        {
            nChar = **ppszDefs;
            (*ppszDefs)++;
        }
        while ((nChar != '<') && (nChar != 0));

        if (nChar != 0)
        {
            // Dump whitespace

            do
            {
                nChar = **ppszDefs;
                if (isspace(nChar))
                {
                    (*ppszDefs)++;
                }
            }
            while (isspace(nChar) && (nChar != 0));

            pszDefs = *ppszDefs;
            if (nChar != 0)
            {
                // Read until the end of the tag

                while ((nChar != '>') && (nChar != 0))
                {
                    if ((nChar != '>') && (nChar != 0))
                    {
                        nLenReqd++;
                        if ((szTag != NULL) && (nLenReqd < nLenTag))
                        {
                            *pszTag = (char) nChar;
                            pszTag++;
                            *pszTag = 0;
                        }
                    }
                    nChar = **ppszDefs;
                    (*ppszDefs)++;
                }
            }

            if (nChar == 0)
            {
                nLenReqd = -1;
            }
            else
            {
                if (szTag == NULL)
                {
                    *ppszDefs = pszDefs;
                }
                else
                {
                    nLenReqd++;    // terminator
                    (*ppszDefs)--;
                }
            }
        }
    }

    return nLenReqd;
}

int Round(float f)
{
    int            n;

    if (f >= 0)
    {
        n = (int) (f + 0.5);
    }
    else
    {
        n = (int) (f - 0.5);
    }

    return n;
}

char            *gszLog = NULL;

void Log(char *pszEntry)
{
    boolean            bAdd;

    // Don't add repeat entries

    bAdd = false;
    if (gszLog == NULL)
    {
        gszLog = malloc(strlen(pszEntry) + 3);
        if (gszLog)
        {
            *gszLog = 0;
            bAdd = true;
        }
    }
    else if (strstr(gszLog, pszEntry) == NULL)
    {
        int                nLen;

        nLen = strlen(gszLog);
        gszLog = realloc(gszLog, nLen + strlen(pszEntry) + 4);
        if (gszLog != NULL)
        {
            *(gszLog + nLen) = 0;
            bAdd = true;
        }
    }

    if (bAdd)
    {
        strcat(gszLog, pszEntry);
        strcat(gszLog, "\r\n");
    }
}

char *GetLog()
{
    return gszLog;
}

void DestroyLog()
{
    if (gszLog != NULL)
    {
        free(gszLog);
        gszLog = NULL;
    }
}
