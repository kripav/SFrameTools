#ifndef MCDataScaleFactors_H
#define MCDataScaleFactors_H

#include "TF1.h"

#include "include/Utils.h"
#include "include/EventCalc.h"

/**
 *  @short module to apply data-MC lepton scale factors for trigger and ID
 *
 *
 */
class LeptonScaleFactors {
public:
    /**
     * constructor
     *
     * first argument: list of corrections to be applied together with weight factors for luminosity, example: "MuonRunA 1.5 MuonRunB 2.6 MuonRunC 7.8"
     *
     * second argument: systematic shift
     * @see E_SystShift
     */
    LeptonScaleFactors(std::vector<std::string> correctionlist, E_SystShift syst_shift=e_Default);
    ///Default destructor
    ~LeptonScaleFactors() {};

    ///browse configuration and fill all requested correction factors
    void FillWeights();

    ///return the total weight (muon*electron)
    ///derived from the weighted correction factor for muons (ID, trigger and isolation)
    ///and all electron weights
    double GetWeight();

    ///return the weighted correction factor for muon ID
    double GetMuonIDWeight();

    ///return the weighted correction factor for muon trigger
    double GetMuonTrigWeight();

    ///return the weighted correction factor for muon isolation
    double GetMuonIsoWeight();

    ///return the weighted correction factor for muons (ID, trigger and isolation)
    double GetMuonWeight();

    ///return the weighted correction factor for electron trigger
    double GetElectronTrigWeight();

    ///return the weighted correction factor for electrons (right now: only trigger)
    double GetElectronWeight();

    ///check if the scale factors are up-to-date, fill them only once per run
    ///implemented for run-dependent scale factors
    bool IsUpToDate();
    
    /// return the bin number of the muon eta bin
    int GetMuonEtaBin(double eta);

private:
    E_SystShift m_syst_shift;
    std::vector<std::pair<std::string, double> > m_correctionlist;
    bool m_apply;                   // should any scale factors be applied?
    int m_current_run;              // run for which the scale factors are valid
    std::vector<double> m_mu_id;    // allow for different bins, right now: 3 eta bins
    std::vector<double> m_mu_trig;  // allow for different bins, right now: 3 eta bins
    std::vector<double> m_mu_iso;   // allow for different bins, right now: 3 eta bins
    std::vector<double> m_ele_trig; // two-parameter function of relative isolation times additional weight

};


/**
 *  @short modules to apply data-MC btagging corrections
 *
 *
 */


class BtagFunction {
public:
    BtagFunction(E_BtagType btagtype) {
        m_btagtype = btagtype;
    }

    virtual ~BtagFunction() {
    };

    virtual float value(const float &x) const = 0;
    virtual float value_plus(const float &x) const = 0;
    virtual float value_minus(const float &x) const = 0;

protected:
    E_BtagType m_btagtype;
};


class BtagScale: public BtagFunction {
public:

    BtagScale(E_BtagType);

    virtual float value(const float &jet_pt) const;
    virtual float value_plus(const float &jet_pt) const {
        return value(jet_pt) + error(jet_pt);
    }

    virtual float value_minus(const float &jet_pt) const {
        const float value_ = value(jet_pt) - error(jet_pt);
        return value_ > 0 ? value_ : 0;
    }

protected:

    virtual float error(const float &jet_pt) const;

private:

    TF1 * _scale;
    std::vector<float> _errors;
    std::vector<float> _bins;
    const unsigned int find_bin(const float &jet_pt) const;
};


class CtagScale: public BtagScale {
public:

    CtagScale(E_BtagType btagtype) : BtagScale(btagtype) {}

protected:

    virtual float error(const float &jet_pt) const;

};


class LtagScale: public BtagFunction {
public:

    LtagScale(E_BtagType btagtype);

    virtual float value(const float &jet_pt) const;
    virtual float value_plus(const float &jet_pt) const;
    virtual float value_minus(const float &jet_pt) const;

private:

    TF1 * _scale;
    TF1 * _scale_plus;
    TF1 * _scale_minus;

};


class BtagEfficiency: public BtagFunction {
public:

    BtagEfficiency(E_BtagType, E_LeptonSelection);

    virtual float value(const float &jet_pt) const;
    virtual float value_plus(const float &jet_pt) const {
        return value(jet_pt);
    }

    virtual float value_minus(const float &jet_pt) const {
        return value(jet_pt);
    }

protected:

    const unsigned int find_bin(const float &jet_pt) const;
    std::vector<float> _values;
    std::vector<float> _bins;

};


class CtagEfficiency: public BtagEfficiency {
public:

    CtagEfficiency(E_BtagType, E_LeptonSelection);

};


class LtagEfficiency: public BtagEfficiency {
public:

    LtagEfficiency(E_BtagType, E_LeptonSelection);

};


/**
 *  @short module to apply data-MC scale factors for b tagging
 *
 *
 */
class BTaggingScaleFactors {
public:
    /**
     * constructor
     *
     * second argument: systematic shift
     * @see E_SystShift
     */
    BTaggingScaleFactors(
        E_BtagType, E_LeptonSelection, E_SystShift sys_bjets=e_Default, E_SystShift sys_ljets=e_Default
    );
    ///Default destructor
    ~BTaggingScaleFactors() {};

    ///return the weighted correction factor
    double GetWeight();

private:

    E_SystShift m_sys_bjets;
    E_SystShift m_sys_ljets;
    E_BtagType m_btagtype;
    E_LeptonSelection m_lepton_selection;

    float scale(const bool &is_tagged,
                const float &jet_pt,
                const BtagFunction* sf,
                const BtagFunction* eff,
                const E_SystShift &sytematic);

    float scale_data(const bool &is_tagged,
                     const float &jet_pt,
                     const BtagFunction* sf,
                     const BtagFunction* eff,
                     const E_SystShift &sytematic);

    BtagFunction* _scale_btag;
    BtagFunction* _eff_btag;

    BtagFunction* _scale_ctag;
    BtagFunction* _eff_ctag;

    BtagFunction* _scale_light;
    BtagFunction* _eff_light;
};

#endif
