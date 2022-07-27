
#include "stdlib.h"
#include "stdio.h"
#include <vector>
#include <string>
#include "math.h"
#include <iostream>
#include <fstream>

#define LineSize 100
#define EneConv 0.049

using namespace std;

int main ( int argc, char** argv ) {
  
  FILE *filePointer;
  char date[14], time[19], index[7];
  unsigned long long int live, liveTime_ms;
  float area,peak, tempC;
  unsigned long int dead;
  double liveTime_s;
  
  filePointer = fopen(argv[1],"r");
  if ( !filePointer ) {
    fprintf(stderr,"File not found: recheck name and path.\n");
    return 1;
  }
  
  vector<string> dates, times;
  vector<float> energies;
  vector<signed long long int> life, death;
  unsigned long long i = 0, evtNum;
  
  bool FunkyHeader = false, MicroSDcardMode = false;
  char fileText[LineSize];
  fgets(fileText, LineSize, filePointer);
  if ( fileText[0] == '#' )
    FunkyHeader = true;
  if ( FunkyHeader ) {
    for ( int k = 0; k < 5; ++k )
      fgets(fileText, LineSize, filePointer);
    if ( fileText[0] == 'D' && fileText[11] == '1' )
      MicroSDcardMode =false;
    else
      MicroSDcardMode = true;
  }
  else
    rewind(filePointer);
  
  while ( !feof(filePointer) ) {
    if ( MicroSDcardMode ) {
      if ( fscanf(filePointer, "%llu %llu %f %f %lu %f", &evtNum, &live, &peak, &area, &dead, &tempC) != EOF ) {
	++i;
	death.push_back(dead);
	life.push_back(live);
	energies.push_back(area*EneConv);
      }
    }
    else {
      if ( fscanf(filePointer, "%s %s %s %llu %f %f %lu", date, time, index, &live, &area, &peak, &dead) != EOF ) { //if NaN's delete by hand
	++i;
	string str1(date);
	string str2(time);
	dates.push_back(str1);
	times.push_back(str2);
	death.push_back(dead);
	life.push_back(live);
	if ( FunkyHeader )
	  energies.push_back(peak*EneConv);
	else
	  energies.push_back(area*EneConv);
	//cerr << date << "\t" << time << "\t" << index << "\t" << live << "\t" << area << "\t" << peak << "\t" << dead << endl;
      }
    }
    //cerr << peak <<  "\t" << energies[i-1]/EneConv << endl;
    if ( (peak+0.01) < energies[i-1]/EneConv && (peak < 800 || area < 800) && (peak > 15 || area > 15) ) {
      fprintf(stderr,"Peak value in ADC counts is smaller than a (low-energy) scintillation pulse area in photo-electrons!\n");
      return 2;
    }
  }
  fclose(filePointer);
  
  liveTime_ms = life.back() - life[0];
  unsigned long j = 0;
  bool OldStyleDead = false; //true = the summing method
  if ( death[0] < death[1] && death[1] < death[2] && death[2] < death[3] &&
       death[0] < death[2] && death[0] < death[3] && death[1] < death[3] && 
       death[3] < death[4] && death[4] < death[5] && death[5] < death[6] )
    dead = death.back() - death[0];
  else {
    OldStyleDead = true;
    dead = 0;
    for ( j = 0; j < death.size(); ++j )
      dead += death[j];
  }
  
  float energy[2] = { 0., 0. }, eneAvg, eneMed, eneRMS, eneMin, eneMax, eneSig;
  for ( j = 0; j < energies.size(); ++j ) {
    energy[0] += energies[j];
    energy[1] += energies[j] * energies[j];
  }
  eneAvg = energy[0] / float(energies.size());
  eneRMS = sqrt ( energy[1] / float(energies.size()) );
  energy[0] = 0.0;
  for ( j = 0; j < energies.size(); ++j ) {
    energy[0] += ( eneAvg - energies[j] ) * ( eneAvg - energies[j] );
  }
  eneSig = sqrt ( energy[0] / (float(energies.size())-1.) );
  
  unsigned short int count = 100; //TO-DO: make argv[2] as option at least?
  if ( i <= count ) {
    fprintf(stderr, "File is too small for the given count variable!\n");
    return 3;
  }
  double deadTime_s = 0.00;
  vector<float> SumEnergy, MaxEnergy;
  vector<double> frequencies;
  for ( j = 0; j < (life.size()-count); ++j ) {
    liveTime_s = (double(life[j+count-1])-double(life[j]))/1e3;
    deadTime_s = 0.000;
    energy[1] = 0.;
    if ( !OldStyleDead ) deadTime_s = (double(death[j+count-1])-double(death[j]))/1e3;
    else {
      for ( int k = j; k < (j+count); ++k ) deadTime_s += double(death[k])/1e3;
    }
    liveTime_s -= deadTime_s;
    eneMax = -1.;
    frequencies.push_back(double(count)/liveTime_s);
    for ( int k = j; k < (j+count); ++k ) {
      energy[1] += energies[k];
      if ( energies[k] > eneMax )
	eneMax = energies[k];
    }
    MaxEnergy.push_back(eneMax);
    SumEnergy.push_back(energy[1]);
  }
  
  sort(energies.begin(), energies.end());
  eneMin = energies[0];
  eneMax = energies.back();
  eneMed = energies[energies.size()/2];
  
  liveTime_ms -= dead;
  liveTime_s = double(liveTime_ms)/1e3;
   cout << i << "\t" << liveTime_s << "\t" << double ( i ) / liveTime_s << endl;
  printf("%f\t%f\t%f\t%f\t%f\t%f\n",eneMin,eneMax,eneAvg,eneMed,eneRMS,eneSig);
  
  for ( j = 0; j < frequencies.size(); ++j ) {
    if ( MicroSDcardMode )
      cerr << life[j+count/2] << "\t\t" << frequencies[j] << "\t" << SumEnergy[j] << "\t" << MaxEnergy[j] << endl;
    else
      cerr << times[j+count/2] << "\t\t" << frequencies[j] << "\t" << SumEnergy[j] << "\t" << MaxEnergy[j] << endl;
    //cerr << dates[j] << "\t" << times[j] << "\t\t" << frequencies[j] << endl;
  }
  
  return 0;
  
}
