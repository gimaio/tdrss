#ifndef DO_TABLE_H_INCLUDED
#define DO_TABLE_H_INCLUDED

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   DO_Table.h
 * Author: gmaio
 *
 * Created on 3 marzo 2018, 10.12
 */

#include <sstream>

class DO_Table {

   private:

       /* 4800-Bit Block Format */
       int syncPattern = 0x627627;  // NASCOM Sync Pattern.
       int sourceCode = 0xE2;       // Source Code for VEGA vectors to FDF.
       int destinationCode = 0x10;  // Destination Code for VEGA vectors to FDF.
       int sequenceNumber = 0x0E;   // Sequence Number / Format Code for VEGA vectors to FDF.
       int spacecraftID = 0x0101;   // Spacecraft ID / Stream ID.
       int messageType = 0xE2;      // messageType
       int messageBlockCount = 0x01;// messageBlockCount
       int dataLength = 0x0284;     // dataLength
       char* timeUTC;               // 6 Bytes

       // CNES Example> DS N 608 1 12:18:46.467 -5577.533 00  3.842444000E+06 -5.057633500E+06  5.782768750E+05  3.311701491E-02 -4.361366481E-02  4.503619391E-03
       char* blockHeader;   // 2 ASCII character - DS = Designation
       char* typeOperation; // 4 ASCII character - V = Flight E = Trial R = Replay + Flight N° (3 digits)
       char* motQualite;    // 1 ASCII character - 0 = no data - 1 = correct message sent
       char* tempsTU;       // 9 ASCII character - HHMMSSmsmsms (9 digits)
       char* source;        // 2 ASCII character - B1=RADAR Bretagne 1 - B2=RADAR Bretagne 2 - A2=RADAR Adour 2 - CN= Centrale inertielle nominale - CS= Centrale inertielle secours
       float xCoord;           // floating IEEE 32 bits	- Geocentric coordinates
       float yCoord;           // floating IEEE 32 bits	- Geocentric coordinates
       float zCoord;           // floating IEEE 32 bits	- Geocentric coordinates
       float xPCoord;           // floating IEEE 32 bits - Geocentric coordinates
       float yPCoord;           // floating IEEE 32 bits - Geocentric coordinates
       float zPCoord;           // floating IEEE 32 bits - Geocentric coordinates
       std::string separator;      // separator between two parameters

       int spare = 0x00;     // spare (1 Byte)
       int polynomialRemainder = 0x000000; // CRC22 check

   public:
       DO_Table(){
           set_blockHeader("");
           set_typeOperation("");
           set_motQualite("");
           set_tempsTU("");
           set_source("");
           set_xCoord(0);
           set_yCoord(0);
           set_zCoord(0);
           set_xPCoord(0);
           set_yPCoord(0);
           set_zPCoord(0);
           set_separator("");
       }

       std::string to_string(){
           std::string DOstring;
           DOstring+=syncPattern;
           DOstring+=get_separator();
           DOstring+=get_blockHeader();
           DOstring+=get_separator();
           DOstring+=get_typeOperation();
           DOstring+=get_separator();
           DOstring+=get_motQualite();
           DOstring+=get_separator();
           DOstring+=get_tempsTU();
           DOstring+=get_separator();
           DOstring+=get_source();
           DOstring+=get_separator();
           DOstring+=floatToString(get_xCoord());
           DOstring+=get_separator();
           DOstring+=floatToString(get_yCoord());
           DOstring+=get_separator();
           DOstring+=floatToString(get_zCoord());
           DOstring+=get_separator();
           DOstring+=floatToString(get_xPCoord());
           DOstring+=get_separator();
           DOstring+=floatToString(get_yPCoord());
           DOstring+=get_separator();
           DOstring+=floatToString(get_zPCoord());
           DOstring+=get_separator();

           return DOstring;
       }

       float get_syncPattern(){
           return syncPattern;
       }

       void set_syncPattern(float pSyncPattern){
           syncPattern = pSyncPattern;
       }


       std::string get_separator(){
           return separator;
       }

       void set_separator(char *pSeparator){
           separator = pSeparator;
       }

       char* get_blockHeader(){
           return blockHeader;
       }

       void set_blockHeader(char *pBlockHeader){
           blockHeader = pBlockHeader;
       }

       char* get_typeOperation(){
           return typeOperation;
       }

       void set_typeOperation(char *pTypeOperation){
           typeOperation = pTypeOperation;
       }

       char* get_motQualite(){
           return motQualite;
       }

       void set_motQualite(char *pMotQualite){
           motQualite = pMotQualite;
       }

       char* get_tempsTU(){
           return tempsTU;
       }

       void set_tempsTU(char *pTempsTU){
           tempsTU = pTempsTU;
       }

       char* get_source(){
           return source;
       }

       void set_source(char *pSource){
           source = pSource;
       }

       float get_xCoord(){
           return xCoord;
       }

       void set_xCoord(float pXCoord){
           xCoord = pXCoord;
       }

       float get_yCoord(){
           return yCoord;
       }

       void set_yCoord(float pYCoord){
           yCoord = pYCoord;
       }

       float get_zCoord(){
           return zCoord;
       }

       void set_zCoord(float pZCoord){
           zCoord = pZCoord;
       }

       float get_xPCoord(){
           return xPCoord;
       }

       void set_xPCoord(float pXPCoord){
           xPCoord = pXPCoord;
       }

       float get_yPCoord(){
           return yPCoord;
       }

       void set_yPCoord(float pYPCoord){
           yPCoord = pYPCoord;
       }

       float get_zPCoord(){
           return zPCoord;
       }

       void set_zPCoord(float pZPCoord){
           zPCoord = pZPCoord;
       }

       std::string floatToString(float floatNumber){
           std::ostringstream aux;
           aux << floatNumber;
           return aux.str();
       }
};

#endif // DO_TABLE_H_INCLUDED
