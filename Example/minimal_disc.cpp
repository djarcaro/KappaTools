/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Kappa/DataFormats/interface/KDebug.h"
#include "../Toolbox/libKToolbox.h"
#include "../RootTools/libKRootTools.h"
#include "../RootTools/FileInterface2Adv.h"
#include "../PlotTools/PlotInterface.h"
#include "../PlotTools/Plot_DataPFJet.h"

using namespace std;

int main(int argc, char **argv)
{
	vector<string> files;
	for (int i = 1; i < argc; ++i)
		files.push_back(argv[i]);
	cout << "Loading files: " << files << endl;

	FileInterface2Adv fi(files);

	std::vector<std::string> names = fi.GetNames<KDataPFJets>();
	cout << "Available PF Jets: " << names << endl;
	cout << "Available PF Taus: " << fi.GetNames<KDataPFTaus>() << endl;

	// Retrieve first PFJet collection and per event / per lumi metadata
	KDataPFJets *jets = fi.Get<KDataPFJets>(names[0]);
	KDataPFTaus *taus = fi.Get<KDataPFTaus>("hpsPFTaus");
	KEventMetadata *meta_event = fi.Get<KEventMetadata>();
	//KLumiMetadata *meta_lumi = fi.GetMeta<KLumiMetadata>("KLumiMetadata");
	KTauDiscriminatorMetadata *meta_taudisc = fi.GetMeta<KTauDiscriminatorMetadata>("hpsPFTaus");

	// Interface for output / booking histograms
	PlotInterface pi("output.root", PlotInterface::reset);
	Plot_DataPFJet plot_jet1(pi.dir("jet1"), "leading jet");
	TH1D *pt_spec_inc = pi.book<TH1D>("pt_spec", "dN/dp_{T} - inclusive",
		Variable(LogBinning(30, 10, 3000), "p_{T} [GeV]"));

	long long nEvents = fi.eventdata.GetEntries();
	cout << "Processing " << nEvents << " events" << endl;
	ProgressMonitor pm(nEvents);
	fi.SpeedupTree();
	for (long long iEvent = 0; iEvent < nEvents; ++iEvent)
	{
		if (!pm.Update()) break;
		fi.eventdata.GetEntry(iEvent);

		static LSWatcher lsWatcher;
		if (lsWatcher.Changed(meta_event))
		{
//			cout << "Reading new lumi metadata!" << endl;
			fi.GetMetaEntry();
//			cout << "Lumi metadata: " << *meta_lumi << endl;
//			cout << "Tau metadata: " << *meta_taudisc << endl;
		}

		// Example how to read tau discriminator information
		for (size_t i = 0; i < taus->size(); ++i)
		{
			const KDataPFTau &tau = taus->at(i);
			std::cout << tau << std::endl;
			for (size_t j = 0; j < meta_taudisc->discriminatorNames.size(); ++j)
			{
				std::string dname = meta_taudisc->discriminatorNames[j];
				std::cout << "\t" << dname << ": " << ((tau.hasID(dname, meta_taudisc) == true) ? "accepted" : "not accepted") << std::endl;
			}
		}

		if (jets->size() > 0)
			plot_jet1.Fill(jets->at(0), 1);
		for (size_t i = 0; i < jets->size(); ++i)
			pt_spec_inc->Fill(jets->at(i).p4.pt());
	}
	return 0;
}