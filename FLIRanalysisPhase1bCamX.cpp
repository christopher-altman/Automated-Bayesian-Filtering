
#include <fstream>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <vector>

using namespace std;

vector<bool> SignalTruth;
double prob[216000];

int main ( int argc, char** argv ) {
  
  double data[26]; vector<double> store[26]; vector<short> extrema;
  unsigned long i = 0; signed short j = 0;
  int RewFram = 100, ForFram = 320, FramBefNew = 320, SubThr = 20; //be sure to sync with Phase 1a! (100->103,160->159)
  float AbsAvg, AbsStdDev, NumStdDev = 3., FracYes = 0.07; //0.999437 for 14_16-46-51_B1
  double GlobPixMean[7] = {0.,0.,0.,0.,0.,0.,0.};
  
  ifstream ifp(argv[1]); long NumFrames = atol(argv[2]); double level = atof(argv[3]);
  for ( i = 0; i < NumFrames; ++i ) {
    ifp >> data[0] >> data[1] >> data[2] >> data[3] >> data[4] >> data[5] >> data[6] >> data[7] >> data[8] >> data[9] >> data[10] >> data[11] >> data[12] >> data[13]
        >> data[14] >> data[15] >> data[16] >> data[17] >> data[18] >> data[19] >> data[20] >> data[21] >> data[22] >> data[23] >> data[24] >> data[25];
    GlobPixMean[0] += data[1];
    GlobPixMean[1] += data[4];
    GlobPixMean[2] += data[7];
    GlobPixMean[3] -= data[10];
    GlobPixMean[4] -= data[13];
    GlobPixMean[5] -= data[16];
    GlobPixMean[6] += data[25];
    for ( j = 0; j < 26; ++j ) store[j].push_back(data[j]);
  }
  ifp.close();
  
  for ( j = 0; j < 7; ++j ) { GlobPixMean[j] /= double(NumFrames); }
  double OverallAverage= (GlobPixMean[0]+GlobPixMean[1]+GlobPixMean[2]+GlobPixMean[3]+GlobPixMean[4]+GlobPixMean[5]) / 6.;
  double OverallStdDev = pow(OverallAverage-GlobPixMean[0],2.)+
                         pow(OverallAverage-GlobPixMean[1],2.)+
                         pow(OverallAverage-GlobPixMean[2],2.)+
                         pow(OverallAverage-GlobPixMean[3],2.)+
                         pow(OverallAverage-GlobPixMean[4],2.)+
                         pow(OverallAverage-GlobPixMean[5],2.);
  OverallStdDev /= 5.; OverallStdDev = sqrt(OverallStdDev);
  fprintf(stderr,"Grey(8-bit) pixel diff mu and sigma of %.1f +/- %.2f (units of 0-255)\n",OverallAverage,OverallStdDev);
  int MinThr = std::max(int(floor(OverallAverage+NumStdDev*OverallStdDev+0.5)),34); //33 for over-fit to initial test
  fprintf(stderr,"So, setting a minimum threshold of %.1f SIGMA = %i (so-called SUB threshold of %d)\n",NumStdDev,MinThr,SubThr);
  int MaxThr = std::min(MinThr+SubThr,54); //76
  fprintf(stderr,"And setting a maximum threshold of %i\n",MaxThr);
  double GlobPixSigma = 0.0;
  for ( i = 0; i < NumFrames; ++i )
  GlobPixSigma += pow(GlobPixMean[6]-store[25][i],2.);
  GlobPixSigma /= (double(NumFrames)-1.);
  GlobPixSigma = sqrt(GlobPixSigma);
  fprintf(stderr,"The number of pixels above %d units is %.2f +/- %.2f\n",SubThr,GlobPixMean[6],GlobPixSigma);
  int MinPix = std::max(3,int(ceil(GlobPixMean[6]+1.)));
  int MaxPix = std::min(30,int(ceil(GlobPixMean[6]+2.*NumStdDev*GlobPixSigma+1.)));
  fprintf(stderr,"Minimum number of pixels allowed to be above the sub-threshold of %d is %d\n",SubThr,MinPix);
  fprintf(stderr,"Maximum number of pixels allowed to be above the sub-threshold of %d is %d\n",SubThr,MaxPix);
  
  for ( i = 1; i < (NumFrames-1); ++i ) {
    
    unsigned long frameNum = (unsigned long)store[0][i];
    unsigned short maxR = (unsigned short)store[1][i];
    unsigned int xR = (unsigned int)store[2][i];
    unsigned int yR = (unsigned int)store[3][i];
    unsigned short maxG = (unsigned short)store[4][i];
    unsigned int xG = (unsigned int)store[5][i];
    unsigned int yG = (unsigned int)store[6][i];
    unsigned short maxB = (unsigned short)store[7][i];
    unsigned int xB = (unsigned int)store[8][i];
    unsigned int yB = (unsigned int)store[9][i];
    unsigned short maxr = (unsigned short)(-store[10][i]);
    unsigned int xr = (unsigned int)store[11][i];
    unsigned int yr = (unsigned int)store[12][i];
    unsigned short maxg = (unsigned short)(-store[13][i]);
    unsigned int xg = (unsigned int)store[14][i];
    unsigned int yg = (unsigned int)store[15][i];
    unsigned short maxb = (unsigned short)(-store[16][i]);
    unsigned int xb = (unsigned int)store[17][i];
    unsigned int yb = (unsigned int)store[18][i];
    unsigned short NumPixAbvThrR = (unsigned short)store[19][i];
    unsigned short NumPixAbvThrG = (unsigned short)store[20][i];
    unsigned short NumPixAbvThrB = (unsigned short)store[21][i];
    unsigned short NumPixAbvThrr = (unsigned short)store[22][i];
    unsigned short NumPixAbvThrg = (unsigned short)store[23][i];
    unsigned short NumPixAbvThrb = (unsigned short)store[24][i];
    
    unsigned int NumPixAbvThrSum = NumPixAbvThrR + NumPixAbvThrG + NumPixAbvThrB + NumPixAbvThrr + NumPixAbvThrg + NumPixAbvThrb;
    unsigned int NumPixAbvSubThrSum = (unsigned int)store[25][i];
    
    extrema.clear();
    extrema.push_back(maxR);
    extrema.push_back(maxG);
    extrema.push_back(maxB);
    extrema.push_back(maxr);
    extrema.push_back(maxg);
    extrema.push_back(maxb);
    std::sort( extrema.begin(), extrema.end() );
    unsigned short AbsMax =abs(extrema.back());
    
    AbsAvg = ( maxR + maxG + maxB + maxr + maxg + maxb ) / 6.;
    AbsStdDev = ( maxR - AbsAvg ) * ( maxR - AbsAvg ) + ( maxG - AbsAvg ) * ( maxG - AbsAvg ) + ( maxB - AbsAvg ) * ( maxB - AbsAvg ) +
      ( maxr - AbsAvg ) * ( maxr - AbsAvg ) + ( maxg - AbsAvg ) * ( maxg - AbsAvg ) + ( maxb - AbsAvg ) * ( maxb - AbsAvg );
    AbsStdDev = sqrt ( AbsStdDev / 5. );
    // "classic" values for the next line: ampl .633, mu 1.97, sig 1.89, skew 2.5
    prob[i] = 1. - 0.673*exp(-0.5*(AbsStdDev-2.)*(AbsStdDev-2.)/(2.*2.))*(1.+erf(2.*(AbsStdDev-2.)/(2.*sqrt(2.))));
    
    if ( prob[i] > level && (AbsMax > MinThr && AbsMax < MaxThr) && NumPixAbvSubThrSum > MinPix && NumPixAbvSubThrSum < MaxPix )
      SignalTruth.push_back(true);
    else
      SignalTruth.push_back(false);
    
  }
  
  double previous[4]; unsigned short int iEvtN = 0; // the global event number
  unsigned short FrameNum[2] = { 0, 0 };
  
  for ( i = RewFram; i < (NumFrames-ForFram-2); ++i ) {
    
    bool GoldenEvent = false; unsigned int NumBools = 0;
    if ( SignalTruth[i-1] ) GoldenEvent = true;
    for ( j = -RewFram; j < ForFram; ++j ) {
      if ( SignalTruth[i+j] ) ++NumBools; //used with "FracYes" below
    }
    
    if ( GoldenEvent || float(NumBools) / float(ForFram+RewFram) > FracYes ) {
      unsigned short maxR = (unsigned short)store[1][i];
      unsigned short maxG = (unsigned short)store[4][i];
      unsigned short maxB = (unsigned short)store[7][i];
      unsigned short maxr = (unsigned short)(-store[10][i]);
      unsigned short maxg = (unsigned short)(-store[13][i]);
      unsigned short maxb = (unsigned short)(-store[16][i]);
      extrema.clear();
      extrema.push_back(maxR); extrema.push_back(maxG); extrema.push_back(maxB); extrema.push_back(maxr); extrema.push_back(maxg); extrema.push_back(maxb);
      std::sort(extrema.begin(),extrema.end()); unsigned short AbsMax = abs(extrema.back());
      extrema.clear();
      if ( AbsMax == maxR ) { extrema.push_back( maxR); extrema.push_back(store[2][i]); extrema.push_back(store[3][i]); }
      else if ( AbsMax == maxG ) { extrema.push_back( maxG); extrema.push_back(store[5][i]); extrema.push_back(store[6][i]); }
      else if ( AbsMax == maxB ) { extrema.push_back( maxB); extrema.push_back(store[8][i]); extrema.push_back(store[9][i]); }
      else if ( AbsMax == maxr ) { extrema.push_back(-maxr); extrema.push_back(store[11][i]); extrema.push_back(store[12][i]); }
      else if ( AbsMax == maxg ) { extrema.push_back(-maxg); extrema.push_back(store[14][i]); extrema.push_back(store[15][i]); }
      else { extrema.push_back(-maxb); extrema.push_back(store[17][i]); extrema.push_back(store[18][i]); }
      FrameNum[1] = store[0][i];
      if ( (FrameNum[1]-FrameNum[0]) > FramBefNew ) ++iEvtN;
      FrameNum[0] = FrameNum[1];
      if ( !GoldenEvent )
	cout << iEvtN << "\t\t" << store[0][i] << "\t" << short(previous[0]) << "  " << (unsigned short)previous[1] << " " << (unsigned short)previous[2] << "\t" << previous[3] << endl;
      else {
	cout << iEvtN << "\t\t" << store[0][i] << "\t" << extrema[0] << "  " << extrema[1] << " " << extrema[2] << "\t" << prob[i] << endl;
	previous[3] = prob[i];
	previous[0] = double(extrema[0]);
	previous[1] = double(extrema[1]);
	previous[2] = double(extrema[2]);
      }
      /*printf("%.0f\t\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t\t%.0f %.0f %.0f\t%.0f %.0f %.0f\t\t%.0f\t%.12f\n",store[0][i],
	     store[1][i],store[2][i],store[3][i],store[4][i],store[5][i],store[6][i],store[7][i],store[8][i],store[9][i],
	     store[10][i],store[11][i],store[12][i],store[13][i],store[14][i],store[15][i],store[16][i],store[17][i],store[18][i],
	     store[19][i],store[20][i],store[21][i],
	     store[22][i],store[23][i],store[24][i],store[25][i],prob[i]);*/
    }
    
  }
  
  return 0;
  
}
