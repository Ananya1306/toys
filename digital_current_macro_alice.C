//#include "FieldSim.h"
#include "AnnularFieldSim.h"
R__LOAD_LIBRARY(.libs/libfieldsim)

void digital_current_macro_alice(int reduction=0, bool loadOutputFromFile=false, const char* fname="pre-hybrid_fixed_reduction_0.ttree.root"){

  //bonk.  making sure this updates to my other branch.
  printf("hello\n");
  if (loadOutputFromFile) printf("loading out1 vectors from %s\n",fname);

  TTime now, start;
  start=now=gSystem->Now();
  printf("the time is %lu\n",(unsigned long)now);

  /*
  //load the TPC space charge model
  const float tpc_rmin=83.5;
  const float tpc_rmax=254.5;
  float tpc_deltar=tpc_rmax-tpc_rmin;
  const float tpc_z=249.7;
  const float tpc_driftVolt=-99930; //V
  const float tpc_driftVel=2.58*1e6;//cm per s
  const float tpc_magField=0.5;//T
  const double epsilonnaught=8.854e-12;// units of C/(V*m)
  const double eps_in_cm=epsilonnaught/100; //units of C/(V*cm)
  const double tpc_chargescale=8.85e-14;//their hist. has charge in units of C/cm^3 /eps0.  This is eps0 in (V*cm)/C units so that I can multiple by the volume in cm^3 to get Q in C.
const char scmapfilename[]="InputSCDensityHistograms_8000events.root";
const char scmaphistname[]="inputSCDensity3D_8000_avg";
  */

    //load the sPHENIX space charge model
  const float tpc_rmin=20.0;
  const float tpc_rmax=78.0;
  float tpc_deltar=tpc_rmax-tpc_rmin;
  const float tpc_z=105.5;
  const float tpc_driftVolt=-400*105.5; //V
  const float tpc_driftVel=8.0*1e6;//cm per s
  const float tpc_magField=1.4;//T
  const double epsilonnaught=8.854e-12;// units of C/(V*m)
  const double eps_in_cm=epsilonnaught/100; //units of C/(V*cm)
  const double tpc_chargescale=1;//our hist. has charge in units of C/cm^3, so we do not need a multiplier.
  const char scmapfilename[]="G4Hits_new_1500.sum.hist.root";
  const char scmaphistname[]="sphenix_minbias_charge";


  //define a region of interest, in units of the intrinsic scale of the tpc histogram:
  //we will reduce these when we call the macro, but keep the full scale here so the calculations for our test grid are not changed.
  int nr=159;//159 nominal
  int nr_roi_min=0;
  int nr_roi=nr;
  int nr_roi_max=nr_roi_min+nr_roi;
  int nphi=360;//360 nominal
  int nphi_roi_min=0;
  int nphi_roi=nphi;
  int nphi_roi_max=nphi_roi_min+nphi_roi;
  int nz=62;//62 nominal
  int nz_roi_min=0;
  int nz_roi=nz;
  int nz_roi_max=nz_roi_min+nz_roi;

  float rmin_roi=tpc_rmin+tpc_deltar/(nr*1.0)*nr_roi_min;
  float rmax_roi=rmin_roi+tpc_deltar/nr*nr_roi;
  float phimin_roi=2*TMath::Pi()/(nphi*1.0)*nphi_roi_min;
 float phimax_roi=phimin_roi+2*TMath::Pi()/nphi*nphi_roi;
 float zmin_roi=tpc_z/(nz*1.0)*nz_roi_min;
  float zmax_roi=zmin_roi+tpc_z/nz*nz_roi;

  float rmin_roi_with_buffer=rmin_roi+tpc_deltar/(nr*1.0)*(0.5);
  float rmax_roi_with_buffer=rmax_roi-tpc_deltar/(nr*1.0)*(0.5);
  float phimin_roi_with_buffer=phimin_roi+2*TMath::Pi()/(nphi*1.0)*(0.5);
 float phimax_roi_with_buffer=phimax_roi-2*TMath::Pi()/(nphi*1.0)*(0.5);
 float zmin_roi_with_buffer=zmin_roi+tpc_z/(nz*1.0)*(0.5);
 float zmax_roi_with_buffer=zmax_roi-tpc_z/(nz*1.0)*(0.5);



 

 printf("r bounds are %f<%f<%f<r<%f<%f<%f\n",tpc_rmin,rmin_roi,rmin_roi_with_buffer,rmax_roi_with_buffer,rmax_roi,tpc_rmax);
 printf("phi bounds are %f<%f<%f<phi<%f<%f<%f\n",0.0,phimin_roi,phimin_roi_with_buffer,phimax_roi_with_buffer,phimax_roi,2*TMath::Pi());

 //now that we've set certain bounds, re-define the size of the simulation per our reduction factor:
 if (reduction==0){
   //do nothing
 }else if (reduction>0){
   //reduce all dimensions by the specified amount:
   nr=nr-reduction;
   nphi=nphi-reduction;
   nz=nz-reduction;
 } else if (reduction<0){
   //scale all dimensions by the specified amount in percent:
   nr=ceil(-nr*reduction*0.01);
   nphi=ceil(-nphi*reduction*0.01);
   nz=ceil(-nz*reduction*0.01);
 }
 //make sure we didn't break anything:
   if (nr<1)nr=1;
   if (nphi<1)nphi=1;
   if (nz<1)nz=1;
   if (nr_roi_max>nr)nr_roi_max=nr;
   if (nphi_roi_max>nphi)nphi_roi_max=nphi;
   if (nz_roi_max>nz)nz_roi_max=nz;


 
  //get the SC density histogram
  TFile *f=TFile::Open(scmapfilename);
  TH3F* tpc_average=(TH3F*)f->Get(scmaphistname);
  now=gSystem->Now();
  printf("loaded hist.  the dtime is %lu\n",(unsigned long)(now-start));
  start=now;
  AnnularFieldSim *tpc=
    new  AnnularFieldSim(tpc_rmin,tpc_rmax,tpc_z,
			 nr, nr_roi_min,nr_roi_max,1,2,
			 nphi,nphi_roi_min, nphi_roi_max,1,2,
			 nz, nz_roi_min, nz_roi_max,1,2,
			 tpc_driftVel, AnnularFieldSim::PhiSlice, AnnularFieldSim::NoSpacecharge);
  //  new AnnularFieldSim(tpc_rmin,tpc_rmax,tpc_z,9,120,9,tpc_driftVel);
   
    // dropping half-res for test: new AnnularFieldSim(tpc_rmin,tpc_rmax,tpc_z,53,18,31,tpc_driftVel);
    //full resolution is too big:  new AnnularFieldSim(tpc_rmin,tpc_rmax,tpc_z,159,360,62,tpc_driftVel);
  now=gSystem->Now();
  printf("created sim obj.  the dtime is %lu\n",(unsigned long)(now-start));
  start=now;
  tpc->setFlatFields(tpc_magField, 12345);// tpc_driftVolt/tpc_z);
  tpc->loadBfield("sPHENIX.2d.root","fieldmap");
  tpc->loadEfield("externalEfield.ttree.root","fTree");
  now=gSystem->Now();
  printf("set fields.  the dtime is %lu\n",(unsigned long)(now-start));
  start=now;
  //tpc->load_rossegger();
   now=gSystem->Now();
    printf("load rossegger greens functions.  the dtime is %lu\n",(unsigned long)(now-start));
  start=now;
  //tpc->load_spacecharge(tpc_average,0,tpc_chargescale); //(TH3F charge histogram, float z_shift in cm, float multiplier to local units)
  //computed the correction to get the same spacecharge as in the tpc histogram:
  //todo: make the analytic scale proportional to the tpc_chargescale.
  double tpc_analytic_scale=1.237320E-06/9.526278E-11;
  tpc->load_analytic_spacecharge(0);//tpc_analytic_scale);
  now=gSystem->Now();
  printf("loaded spacecharge.  the dtime is %lu\n",(unsigned long)(now-start));
  start=now;
  tpc->populate_lookup();
  now=gSystem->Now();
  printf("populated lookup.  the dtime is %lu\n",(unsigned long)(now-start));
  start=now;
  tpc->populate_fieldmap();
 now=gSystem->Now();
  printf("populated fieldmap.  the dtime is %lu\n",(unsigned long)(now-start));
  start=now;
  printf("consistency check:  integrate field along IR and OR, confirm V:\n");

  if (reduction==0){
    TH3F* hAnCharge=new TH3F("hAnCharge","hAnCharge;phi;r;z",nphi,0,6.281,nr,tpc_rmin,tpc_rmax,nz,0,tpc_z);
    hAnCharge->Fill(0.0,0.0,0.0,0.0); //prime it so it draws correctly

    int rh,ph,zh;
    for (int i=0;i<hAnCharge->GetNcells();i++){
      hAnCharge->GetBinXYZ(i,ph,rh,zh);
      hAnCharge->SetBinContent(i,tpc->q->Get(rh%nr,ph%nphi,zh%nz));
    }
    hAnCharge->Project3D("XZ")->Draw("colz");
    TCanvas *ct=new TCanvas();
    hAnCharge->Project3D("YZ")->Draw("colz");
    ct=new TCanvas();
    hAnCharge->Project3D("YX")->Draw("colz");
  }

  // return;
  //define a grid of test points:
  const int divisor=100;
  const int nparticles=divisor*divisor;
  TVector3 testparticle[nparticles];
  TVector3 outparticle[nparticles];
  TVector3 outparticle2[nparticles];
  TVector3 backparticle[nparticles];
  float outx[nparticles], outy[nparticles],outz[nparticles];
  for (int i=0;i<nparticles;i++){
    int rpos=i/divisor;
    int phipos=i%divisor;
    testparticle[i].SetXYZ((rmax_roi_with_buffer-rmin_roi_with_buffer)*(rpos/(divisor*1.0))+rmin_roi_with_buffer,0,zmin_roi);
    testparticle[i].RotateZ((phimax_roi_with_buffer-phimin_roi_with_buffer)*(phipos/(divisor*1.0))+phimin_roi_with_buffer);
  }


  //for this study, we load the 'out' particles from the ttree of the full-scale version, and propagate them backward:
  if (loadOutputFromFile){
    TFile *input=TFile::Open(fname);
    TTree *inTree=(TTree*)input->Get("pTree");
    TVector3 *outFromFile;
    TVector3 *origFromFile;
    inTree->SetBranchAddress("out1",&outFromFile);
    inTree->SetBranchAddress("orig",&origFromFile);
    for (int i=0;i<inTree->GetEntries();i++){
      inTree->GetEntry(i);
      outparticle[i]=(*outFromFile)*(1/(1.0e4)); //convert back to local units.
      testparticle[i]=(*origFromFile)*(1/(1.0e4)); //convert back to local units.
    }
    input->Close();
  }

  TFile *output=TFile::Open("last_macro_output.ttree.root","RECREATE");
  TVector3 orig,outa,out1,out2,back1,back2;
  int goodSteps[2];
  TTree pTree("pTree","Particle Tree");
  pTree.Branch("orig","TVector3",&orig);
  pTree.Branch("outa","TVector3",&outa);
  pTree.Branch("out1","TVector3",&out1);
    pTree.Branch("out1N",&goodSteps[0]);
  pTree.Branch("back1","TVector3",&back1);
  pTree.Branch("back1N",&goodSteps[1]);

  //save data about the Efield:
  TTree fTree("fTree","field Tree");
  TVector3 pos0,pos,Efield,Bfield,phihat;
  TVector3 zero(0,0,0);
  TVector3 Eint,EintA;
  bool inroi;
  float charge,eintp, ep;
  fTree.Branch("pos","TVector3",&pos);
  fTree.Branch("phihat","TVector3",&phihat);

  fTree.Branch("E","TVector3",&Efield);
  fTree.Branch("B","TVector3",&Bfield);
  fTree.Branch("Eint","TVector3",&Eint);
  fTree.Branch("EintA","TVector3",&EintA);
  fTree.Branch("q",&charge);
  fTree.Branch("Eintp",&eintp);
  fTree.Branch("Ep",&ep);
  fTree.Branch("roi",&inroi);
  float delr,delp;//=tpc->GetCellCenter(2,0,0)-tpc->GetCellCenter(1,0,0);
  //TVector3 delp;
  float delz=tpc->GetCellCenter(0,0,1).Z()-tpc->GetCellCenter(0,0,0).Z();
  
  bool inr,inp,inz;
  int rl,pl, zl;
  int fieldmap_output_extra_sampling=0;
  if (0)
  for (int ir=nr_roi_min;ir<nr_roi_max;ir++){
    rl=ir-nr_roi_min;
    inr=(rl>=0 && rl<nr_roi);
    for (int ip=nphi_roi_min;ip<nphi_roi_max;ip++){
      pl=ip-nphi_roi_min;
      inp=(pl>=0 && pl<nphi_roi);     
      for (int iz=0;iz<nz;iz++){
	zl=iz-nz_roi_min;
	inz=(zl>=0 && zl<nz_roi);
	pos0=tpc->GetCellCenter(ir,ip,iz);
	delr=(tpc->GetCellCenter(ir+1,ip,iz)).Perp()-pos0.Perp();
	delp=(tpc->GetCellCenter(ir,ip+1,iz)).Phi()-pos0.Phi();

	Efield=zero;
	Bfield=zero;
	inroi=inr && inp && inz;
	if (inroi){
	  Efield=tpc->Efield->Get(ir-nr_roi_min,ip-nphi_roi_min,iz-nz_roi_min);
	  Bfield=tpc->Bfield->Get(ir-nr_roi_min,ip-nphi_roi_min,iz-nz_roi_min);
	  //for (int rlocal=-10;rlocal<10;rlocal++){
	    //int plocal=0;
	  int rlocal=0;
	    for (int plocal=-fieldmap_output_extra_sampling;plocal<fieldmap_output_extra_sampling+1;plocal++){
	      pos=pos0;
	      pos.SetPerp(pos0.Perp()+delr/(1+2*fieldmap_output_extra_sampling)*rlocal);
	      pos.RotateZ(delp/(1+2*fieldmap_output_extra_sampling)*plocal);//+(rlocal)/20.1*delr+(plocal)/(20.1)*delp;
	      //printf("trying pos=(%f,%f,%f)= rphiz(%f,%f,%f)\n",pos.X(),pos.Y(),pos.Z(),pos.Perp(),pos.Phi(),pos.Z());
	      phihat=pos;// build our phi position by starting with the vector in the rz plane:
	      phihat.SetZ(0);//remove the z component so it points purely in r
	      phihat.SetMag(1.0);//scale it to 1.0;
	      phihat.RotateZ(TMath::Pi()/2);//rotate 90 degrees from the position vector so it now points purely in phi; 
	      Eint=(tpc->interpolatedFieldIntegral(pos.Z()-delz/(4.0),pos))*(4.0/delz);
	      EintA=(tpc->analyticFieldIntegral(pos.Z()-delz/(4.0),pos))*(4.0/delz);
	      charge=tpc->q->Get(ir,ip,iz);
	      eintp=Eint.Dot(phihat);
	      ep=Efield.Dot(phihat);
	      fTree.Fill();
	      }
	    //}
	}
	
      }
    }
  }
  fTree.Write();
  //return;
  
  int validToStep=-1;
  start=gSystem->Now();
  for (int i=0;i<nparticles;i++){
    if (!(i%((nparticles-1)/20))) {
       now=gSystem->Now();
       printf("(periodic progress...) test[%d]=(%f,%f,%f) to %f\t dtime=%lu\n",
	      i,testparticle[i].X(),testparticle[i].Y(),testparticle[i].Z(),zmax_roi,(unsigned long)(now-start));
      start=now;
    }



      
    orig=testparticle[i];
    if (!loadOutputFromFile)
      {
	outparticle[i]=tpc->swimToInSteps(zmax_roi,testparticle[i],60,true, &validToStep);
	outparticle2[i].SetXYZ(0,0,0);//=tpc->swimToInAnalyticSteps(zmax_roi,testparticle[i],60, &validToStep); //no arg for interpolation in the analytic swim, since we sample the exact position.
      }

    out1=outparticle[i];//not generating from the swim.=tpc->swimToInSteps(zmax_roi,testparticle[i],600,true);
    outa=outparticle2[i];//not generating from the swim.=tpc->swimToInSteps(zmax_roi,testparticle[i],600,true);
    outx[i]=outparticle[i].X();
    outy[i]=outparticle[i].Y();
    outz[i]=outparticle[i].Z();
    goodSteps[0]=validToStep;
    TVector3 delta=out1-testparticle[i];
    //printf("delta1=(%E,%E,%E)\n",delta.X(),delta.Y(),delta.Z());
    //printf("out[%d]=(%f,%f,%f)\n",i,outparticle[i].X(),outparticle[i].Y(),outparticle[i].Z());
    //drift back from the analytic position:
      back1=backparticle[i]=tpc->swimToInSteps(testparticle[i].Z(),out1,600,true,&validToStep);
    goodSteps[1]=validToStep;

    //for convenience of reading, set all of the pTree in microns, not cm:
    orig*=1e4;//10mm/cm*1000um/mm
    out1*=1e4;//10mm/cm*1000um/mm
    outa*=1e4;
    back1*=1e4;//10mm/cm*1000um/mm
    pTree.Fill();
  }
  pTree.Write();



   


  return;
  

  AnnularFieldSim *test;
  AnnularFieldSim *testgen;
  const TVector3 cyl(60,0,100);
  TVector3 ptemp(12.005,45.005,75.99);
  TVector3 ftemp,btemp;
  TTime t[10];
  Int_t isteps=22;
  int imin=8;//can't do interpolation without at least some divisions, otherwise we necessarily go out of bounds.
  int imax=imin+isteps;
  Int_t tCreate[isteps];
  Int_t tSetField[isteps];
  Int_t tLookup[isteps];
  Int_t tGenMap[isteps];
  Int_t tSwim1k[isteps];
  Int_t steps[isteps];
  float rdiff[isteps];
  float rphidiff[isteps];
  
  testgen=new AnnularFieldSim(cyl.Perp(),2*TMath::Pi(),cyl.Z(),isteps+imin-1,isteps+imin-1,isteps+imin-1,100.0/12e-6);
  testgen->setFlatFields(1.4,200);
  testgen->populate_lookup();//2-3
  testgen->q->Set(isteps+imin-2,0,0,1e-12);///that's 10^7 protons in that box.
  testgen->populate_fieldmap();
  ftemp=ptemp;
  for (int j=0;j<isteps+imin;j++){
    ftemp=testgen->swimTo(ptemp.Z()*(isteps+imin-1-j)/(isteps+imin),ftemp,true);
  }  
  for (int i=0;i<isteps;i++){
    printf("create %d\n",i);
    t[0]=gSystem->Now();
    test=new AnnularFieldSim(cyl.Perp(),2*TMath::Pi(),cyl.Z(),i+imin,i+imin,i+imin,100.0/12e-6);
    t[1]=gSystem->Now();
    printf("setFlat %d\n",i);
    test->setFlatFields(1.4,200);
    t[2]=gSystem->Now();
    printf("populate %d\n",i);
    test->populate_lookup();//2-3
    t[3]=gSystem->Now();
    printf("setQ %d\n",i);
    test->q->Set(i+imin-1,0,0,1e-12);///that's 10^7 protons in that box.
    t[4]=gSystem->Now();
    printf("make fieldmap %d\n",i);
   test->populate_fieldmap();
    t[5]=gSystem->Now();
    printf("swim %d\n",i);
    for (int n=0;n<1000;n++){
    btemp=ftemp;
    for (int j=0;j<i+imin;j++){
      btemp=test->swimTo(ptemp.Z()*((j+1)*1.0/(1.0*(i+imin))),btemp,true);
    }
    }
    t[6]=gSystem->Now();
    tCreate[i]=(long)(t[1]-t[0]);
    tSetField[i]=(long)(t[2]-t[1]);
    tLookup[i]=(long)(t[3]-t[2]);
    tGenMap[i]=(long)(t[5]-t[4]);
    tSwim1k[i]=(long)(t[6]-t[5]);
    steps[i]=i+imin;
    rdiff[i]=(btemp.Perp()-ptemp.Perp())*1e4;
    rphidiff[i]=(btemp.Perp()*btemp.Phi()-ptemp.Perp()*ptemp.Phi())*1e4;

  }

  int i=0;

  TGraph *rPhiDiff[3];
  TMultiGraph *rphi=new TMultiGraph();
  rphi->SetTitle("r and r*phi offset for varying reco grid sizes;r(um);rphi(um)");
  rPhiDiff[i]=new TGraph(isteps,rdiff,rphidiff);
  rPhiDiff[i]->SetTitle("rphi diff;size;t(ms)");
  rPhiDiff[i]->SetMarkerColor(kBlack);
  rPhiDiff[i]->SetMarkerStyle(kStar);
  rphi->Add(rPhiDiff[i]);
  i++;
  rPhiDiff[i]=new TGraph(1,rdiff,rphidiff);
  rPhiDiff[i]->SetTitle(Form("rphi diff %d^3 grid;size;t(ms)",steps[0]));
  rPhiDiff[i]->SetMarkerColor(kRed);
  rPhiDiff[i]->SetMarkerStyle(kStar);
  rphi->Add(rPhiDiff[i]);
  i++;
  rPhiDiff[i]=new TGraph(1,rdiff+isteps-1,rphidiff+isteps-1);
  rPhiDiff[i]->SetTitle(Form("rphi diff %d^3 grid;size;t(ms)",steps[isteps-1]));
  rPhiDiff[i]->SetMarkerColor(kGreen);
  rPhiDiff[i]->SetMarkerStyle(kStar);
  rphi->Add(rPhiDiff[i]);
  i++;
  rphi->Draw("AC*");
  
 




 i=0;
  
  TMultiGraph *multi=new TMultiGraph();
  multi->SetTitle("timing for various simulation steps;size;t(ms)");
  TGraph *graph[10];
  graph[i]=new TGraph(isteps,steps,tCreate);
  graph[i]->SetTitle("FieldSim::FieldSim;size;t(ms)");
  graph[i]->SetMarkerStyle(kStar);
  graph[i]->SetMarkerColor(i+1);
  multi->Add(graph[i]);
  i++;
  graph[i]=new TGraph(isteps,steps,tSetField);
  graph[i]->SetTitle("FieldSim::SetField;size;t(ms)");
  graph[i]->SetMarkerStyle(kStar);
  graph[i]->SetMarkerColor(i+1);
  multi->Add(graph[i]);
  i++;
  graph[i]=new TGraph(isteps,steps,tLookup);
  graph[i]->SetTitle("FieldSim::GenLookupTable;size;t(ms)");
  graph[i]->SetMarkerStyle(kStar);
  graph[i]->SetMarkerColor(i+1);
  multi->Add(graph[i]);
  i++;
   graph[i]=new TGraph(isteps,steps,tGenMap);
  graph[i]->SetTitle("FieldSim::GenFieldMap;size;t(ms)");
  graph[i]->SetMarkerStyle(kStar);
  graph[i]->SetMarkerColor(i+1);
  multi->Add(graph[i]);
  i++;
   graph[i]=new TGraph(isteps,steps,tSwim1k);
  graph[i]->SetTitle("FieldSim::Swim*1k;size;t(ms)");
  graph[i]->SetMarkerStyle(kStar);
  graph[i]->SetMarkerColor(i+1);
  multi->Add(graph[i]);
  i++;
  multi->Draw("AC*");  
  return;
}
