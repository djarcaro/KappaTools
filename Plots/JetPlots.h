#ifndef KAPPA_JETPLOTS_H
#define KAPPA_JETPLOTS_H

#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include "TDirectory.h"
#include "BasePlot.h"
#include "DataFormats/interface/KBasic.h"
#include "DataFormats/interface/KJetMET.h"

namespace KappaTools
{
	template <typename JetType>
	class StandardJetPlots : public BasePlot
	{
		private:
			TH1D * jet_pt, * jet_pt_low, * jet_pt_full;
			TH1D * jet_eta, * jet_eta_zoom, * jet_phi;

			TH1D * n90;
			TH1D * nConstituents;
			TH1D * emFraction, * hadFraction;

			// pfjet-specific:
			TH1D * neutralEmFraction, * chargedEmFraction;
			TH1D * neutralHadFraction, * chargedHadFraction;

			// calojet-specific:

			void initSpecific();
			void processSpecific(JetType * jet, KDataVertex * pv, double weight);
		public:
			StandardJetPlots(TDirectory * tmpFile, TString tmpDirectory="");

			// TDirectory
			void process(JetType * jet, KDataVertex * pv = 0, double weight = 1.);
			void final();
	};
	/*
	class StandardPFJetsPlots : public StandardJetPlots<typename JetType>
	{
		private:

		public:
			StandardPFJetsPlots(TDirectory * tmpFile, TString tmpDirectory="");
			void process(MTAPFJet * jet, MTAVertex * pv = 0);
			void final();
	};
	*/
}
#endif

