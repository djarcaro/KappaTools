#include "StandardP4Plots.h"

KappaTools::TriggerP4Plots::TriggerP4Plots(TDirectory * tmpFile, TString tmpDirectory) : StandardP4Plots::StandardP4Plots(tmpFile, tmpDirectory)
{
	double pt_binning[11] = { 0,5,9,12,15,20,30,40,60,100 };
	//double pt_binning[16] = { 0,5,6,7,8,9,10,11,13,15,20,25,40,60,100 };
	//double eta_binning[17] = { -2.4, -2.1, -1.8, -1.5, -1.2, -0.9, -0.6, -0.3, 0, 0.3, 0.6, 0.9, 1.2, 1.5, 1.8, 2.1, 2.4};
	double eta_binning[21] = { -2.4, -2.1, -1.85, -1.6, -1.4, -1.2, -1.05, -0.9, -0.6, -0.3, 0, 0.3, 0.6, 0.9, 1.05, 1.2, 1.4, 1.6, 1.85, 2.1, 2.4};
	double eta_regions_binning[8] = { -2.4, -2.1, -1.2, -0.9, 0.9, 1.2, 2.1, 2.4};

	pt_binned		= new TH1D("pt_binned","p_{\\mathrm{T}}", 9, pt_binning);
	
	eta_binned		= new TH1D("eta_binned","#eta", 20, eta_binning);
	eta_regions		= new TH1D("eta_regions","#eta", 7, eta_regions_binning);
	
	pt_eta = new TH2D("pt_eta", "#pt vs. #eta", 7, eta_regions_binning, 9, pt_binning);
}

void KappaTools::TriggerP4Plots::process(RMDataLV p4, double weight)
{
	process((RMLV) p4, weight);
}
void KappaTools::TriggerP4Plots::process(RMLV p4, double weight)
{
	pt_binned->Fill(p4.pt(), weight);
	eta_binned->Fill(p4.eta(), weight);
	eta_regions->Fill(p4.eta(), weight);
	pt_eta->Fill(p4.eta(), p4.pt(), weight);
	StandardP4Plots::process(p4, weight);
}

void KappaTools::TriggerP4Plots::final()
{
	StandardP4Plots::final();
}

KappaTools::StandardP4Plots::StandardP4Plots(TDirectory * tmpFile, TString tmpDirectory)
{
	getDirectory(tmpFile, tmpDirectory);

	pt 						= new TH1D("pt","p_{\\mathrm{T}}", 75, 0., 75.);
	pt_low 				= new TH1D("pt_low","p_{\\mathrm{T}}", 50, 0., 25.);

	pt_full 			= new TH1D("pt_full","p_{\\mathrm{T}}", 100, 0., 1000.);
	eta						= new TH1D("eta","#eta", 50, -5., 5.);

	eta_zoom			= new TH1D("eta_zoom","#eta", 50, -2.5, 2.5);
	phi						= new TH1D("phi","#phi", 50, -3.5, 3.5);
	mass					= new TH1D("mass","m", 75, 0., 250.);
	mass_low			= new TH1D("mass_low","m", 75, 0., 75.);
	mass_zwindow	= new TH1D("mass_zwindow","m", 70, 50., 130.);
	eta_zoom_phi	= new TH2D("eta_zoom_phi", "#eta vs. #phi", 50, -2.5, 2.5, 50, -3.5, 3.5);
}

void KappaTools::StandardP4Plots::process(RMLV p4, double weight)
{
	pt->Fill(p4.pt(), weight);
	pt_low->Fill(p4.pt(), weight);
	pt_full->Fill(p4.pt(), weight);
	eta->Fill(p4.eta(), weight);
	eta_zoom->Fill(p4.eta(), weight);
	phi->Fill(p4.phi(), weight);
	mass->Fill(p4.mass(), weight);
	mass_low->Fill(p4.mass(), weight);
	mass_zwindow->Fill(p4.mass(), weight);
	eta_zoom_phi->Fill(p4.eta(), p4.phi(), weight);
}

void KappaTools::StandardP4Plots::final()
{
}


template <typename PlottingClass, typename ObjectType1, typename ObjectType2>
KappaTools::PlotsByPt<PlottingClass, ObjectType1, ObjectType2>::PlotsByPt(std::vector<double> binning_, TDirectory * tmpFile, TString tmpDirectory) : binning(binning_), mode(FILL_EXCL)
{
	TDirectory * curDirectory = getDirectory(tmpFile, tmpDirectory);

	all = new PlottingClass(curDirectory, "all");
	underflow = new PlottingClass(curDirectory, "underflow");
	overflow = new PlottingClass(curDirectory, "overflow");

	for (unsigned int idx = 0; idx < binning.size(); idx++)
	{
		double pt_ = binning[idx];
		TString bname;
		if (pt_<100.)
			if (pt_<10.)
				bname = "pt_00";
			else
				bname = "pt_0";
		else
			bname = "pt_";
		bname+=pt_;
		plotsByBin.push_back(PlottingClass(curDirectory, bname.ReplaceAll(" ","")));
	}
}

template <typename PlottingClass, typename ObjectType1, typename ObjectType2>
void KappaTools::PlotsByPt<PlottingClass, ObjectType1, ObjectType2>::process(ObjectType1 * obj, KDataVertex * pv, double weight, ObjectType2 * obj2)
{
	if (!obj || !obj2)
		return;

	all->process(obj);

	if (binning.size()==0)
		return;

	double pt = obj2->p4.pt();

	if (pt < binning[0])
		underflow->process(obj);
	if (pt > binning[binning.size()-1])
		underflow->process(obj);

	for (unsigned int idx = 0; idx < binning.size(); idx++)
	{
		if (mode & FILL_ABS)
			pt = std::abs(pt);

		bool process = false;
		if (mode & FILL_EXCL)
			if (idx < binning.size()-1 && pt >= binning[idx] && pt < binning[idx+1])
				process = true;

		if (mode & FILL_INCL_UP)
			if (pt >= binning[idx])
				process = true;

		if (mode & FILL_INCL_DOWN)
			if (pt <= binning[idx])
				process = true;

		if (process)
			plotsByBin[idx].process(obj);
	}
}

template <typename PlottingClass, typename ObjectType1, typename ObjectType2>
void KappaTools::PlotsByPt<PlottingClass, ObjectType1, ObjectType2>::final()
{
}

template <typename PlottingClass, typename ObjectType1, typename ObjectType2>
void KappaTools::PlotsByPt<PlottingClass, ObjectType1, ObjectType2>::setMode(char mode_)
{
	mode = mode_;
}


template <typename PlottingClass, typename ObjectType1, typename ObjectType2>
KappaTools::PlotsByEta<PlottingClass, ObjectType1, ObjectType2>::PlotsByEta(std::vector<double> binning_, TDirectory * tmpFile, TString tmpDirectory) : binning(binning_), mode(FILL_EXCL | FILL_ABS)
{
	TDirectory * curDirectory = getDirectory(tmpFile, tmpDirectory);

	all = new PlottingClass(curDirectory, "all");
	underflow = new PlottingClass(curDirectory, "underflow");
	overflow = new PlottingClass(curDirectory, "overflow");

	for (unsigned int idx = 0; idx < binning.size(); idx++)
	{
		double eta_ = binning[idx];
		TString bname = "eta_";
		bname+=eta_;
		plotsByBin.push_back(PlottingClass(curDirectory, bname.ReplaceAll(" ","")));
	}
}

template <typename PlottingClass, typename ObjectType1, typename ObjectType2>
void KappaTools::PlotsByEta<PlottingClass, ObjectType1, ObjectType2>::process(ObjectType1 * obj, KDataVertex * pv, double weight, ObjectType2 * obj2)
{
	if (!obj || !obj2)
		return;

	all->process(obj);

	if (binning.size()==0)
		return;

	double eta = obj2->p4.eta();

	if (eta < binning[0])
		underflow->process(obj);
	if (eta > binning[binning.size()-1])
		underflow->process(obj);

	for (unsigned int idx = 0; idx < binning.size(); idx++)
	{
		if (mode & FILL_ABS)
			eta = std::abs(eta);

		bool process = false;
		if (mode & FILL_EXCL)
			if (idx < binning.size()-1 && eta >= binning[idx] && eta < binning[idx+1])
				process = true;

		if (mode & FILL_INCL_UP)
			if (eta >= binning[idx])
				process = true;

		if (mode & FILL_INCL_DOWN)
			if (eta <= binning[idx])
				process = true;

		if (process)
			plotsByBin[idx].process(obj);
	}
}

template <typename PlottingClass, typename ObjectType1, typename ObjectType2>
void KappaTools::PlotsByEta<PlottingClass, ObjectType1, ObjectType2>::final()
{
}

template <typename PlottingClass, typename ObjectType1, typename ObjectType2>
void KappaTools::PlotsByEta<PlottingClass, ObjectType1, ObjectType2>::setMode(char mode_)
{
	mode = mode_;
}

template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataJet, KDataMET>, KappaTools::ZmumuObjects<KDataJet, KDataMET>, KappaTools::ZmumuObjects<KDataJet, KDataMET> >;
template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataPFJet, KDataMET>, KappaTools::ZmumuObjects<KDataPFJet, KDataMET>, KappaTools::ZmumuObjects<KDataPFJet, KDataMET> >;
template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataPFJet, KDataPFMET>, KappaTools::ZmumuObjects<KDataPFJet, KDataPFMET>, KappaTools::ZmumuObjects<KDataPFJet, KDataPFMET> >;

template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataJet, KDataMET>, KappaTools::ZmumuObjects<KDataJet, KDataMET>, KDataJet>;
template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataPFJet, KDataMET>, KappaTools::ZmumuObjects<KDataPFJet, KDataMET>, KDataPFJet>;
template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataPFJet, KDataPFMET>, KappaTools::ZmumuObjects<KDataPFJet, KDataPFMET>, KDataPFJet>;

template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataJet, KDataMET>, KappaTools::ZmumuObjects<KDataJet, KDataMET>, KParton>;
template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataPFJet, KDataMET>, KappaTools::ZmumuObjects<KDataPFJet, KDataMET>, KParton>;
template class KappaTools::PlotsByPt<KappaTools::ZmumuPlots<KDataPFJet, KDataPFMET>, KappaTools::ZmumuObjects<KDataPFJet, KDataPFMET>, KParton>;

template class KappaTools::PlotsByPt<KappaTools::MuonPlotsByType<KappaTools::StandardMuonPlots>, KDataMuon, KDataMuon >;
template class KappaTools::PlotsByEta<KappaTools::MuonPlotsByType<KappaTools::StandardMuonPlots>, KDataMuon, KDataMuon >;

