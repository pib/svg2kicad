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

 #ifdef __cplusplus
 extern "C" {
 #endif 

typedef unsigned char boolean;
#define true 1
#define false 0

#ifndef WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif 

#define PI				3.14159f
#define MAX_TEXT		1024
#define TEXT_WIDTH		500
#define TEXT_HEIGHT		600
#define LINE_THICKNESS	100

// Command line only functions

#ifndef MFC
int main(int argc, char *argv[ ]);
void PrintUsage(void);
#endif // _MFC

// SVG to Kicad functions

int SVG2Kicad(const char *pcSVGPathname, const char *pcKicadPathname, const char* pszName, char cType, float fScale, char m_cScaleType, unsigned int unSmoothness);
void GetTransform(char *pszTag, float *pfXTranslate, float *pfYTranslate, float *pfRotation);
boolean IsTag(const char *pszTag, const char *pszTagName);
int GetNextTagLen(FILE *pfileSVG);
int ReadNextTag(FILE *pfileSVG, char *szTag, int nLenTag);
int ReadNextTagFromDefs(char **ppszDefs, char *szTag, int nLenTag);
char *GetVar(char *szTag, char *szVarName);
void GetSize(char *szTag, float *pfWidth, float *pfHeight);
void ProcessTag(FILE *pfileSVG, char **ppszDefs, FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale, unsigned int unSmoothness, char *szDefs);
void ProcessText(FILE *pfileSVG, char **ppszDefs, FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale);
void ProcessPath(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale, unsigned int unSmoothness);
void ProcessCircle(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fScale);
void ProcessEllipse(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale, unsigned int unSmoothness);
void ProcessRect(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale);
void ProcessLine(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale);
void ProcessPoly(FILE *pfileKicad, char cType, char *szTag, float fXTranslate, float fYTranslate, float fRotation, float fScale, boolean bClose);
void WriteKicadLine(FILE *pfileKicad, char cType, float fXStart, float fYStart, float fXEnd, float fYEnd, float fXTranslate, float fYTranslate, float fStrokeWidth, float fScale);
void WriteKicadCircle(FILE *pfileKicad, char cType, float fXCentre, float fYCentre, float fXRadius, float fXTranslate, float fYTranslate, float fStrokeWidth, float fScale);
void WriteKicadRect(FILE *pfileKicad, char cType, float fXStart, float fYStart, float fXEnd, float fYEnd, float fXTranslate, float fYTranslate, float fRotation, float fStrokeWidth, float fScale);
void WriteKicadText(FILE *pfileKicad, char cType, float fXStart, float fYStart, char *pszText, float fXTranslate, float fYTranslate, float fRotation, float fScale);
int ReadLinear(char *pszVar, float *pfX, float *pfY);
int ReadLinearOffset(char *pszVar, float *pfOffset);
int ReadCubic(char **ppszVar, float *pfX1, float *pfY1, float *pfX2, float *pfY2, float *pfX, float *pfY);
int ReadShortCubic(char **ppszVar, float *pfX2, float *pfY2, float *pfX, float *pfY);
int ReadQuadratic(char **ppszVar, float *pfX1, float *pfY1, float *pfX, float *pfY);
int ReadShortQuadratic(char *pszVar, float *pfX, float *pfY);
int ReadEllipticArc(char **ppszVar, float *pfRadiusX, float *pfRadiusY, float *pfXRot, boolean *pbLargeArc, boolean *pbSweep, float *pfXEnd, float *pfYEnd);
char *GetNextPoint(char *pszVar);
char *GetNextValue(char *pszVar);
void CubicBezier(float fXStart, float fYStart, float *pfXEnd, float *pfYEnd, float fX1, float fY1, float fX2, float fY2, float ft);
void QuadraticBezier(float fXStart, float fYStart, float *pfXEnd, float *pfYEnd, float fX1, float fY1, float ft);
void GetEllipticArcSweep(float fRadiusX, float fRadiusY,
						float fXRot, boolean bLargeArc, boolean bSweep,
						float fXStart, float fYStart, float fXEnd, float fYEnd,
						float *pfCX, float *pfCY,
						float *pfStartAngle, float *pfDeltaAngle);
void EllipticArc(float fRadiusX, float fRadiusY, float fXRot,
						float fXCentre, float fYCentre, float fTheta,
						float *pfX, float *pfY);
float GetVectorAngle(float fUx, float fUy, float fVx, float fVy);
int Round(float f);
void Log(char *pszEntry);
char *GetLog();
void DestroyLog();

 #ifdef __cplusplus
 }
 #endif
