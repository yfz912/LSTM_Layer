#pragma once

#include "stdafx.h"
#include "MatrixD.h"
#include "LSTMLayerState.h"
#include "ActivationFunctions.h"
#include "NeuroNetworkLayer.h"
#include "LSTMNeuroNetworkError.h"
#include "LSTMBackPropogateResult.h"


class LSTMLayer : public NeuroNetworkLayer
{
protected:
	MatrixD Wc;
	MatrixD Wi;
	MatrixD Wo;
	MatrixD Wf;

	MatrixD Uc;
	MatrixD Ui;
	MatrixD Uo;
	MatrixD Uf;

	MatrixD ctmo;
	MatrixD htmo;

	int numInputs;
	int numOutputs;

public:

	LSTMLayer( const int &inputs , const int &outputs )
		: Wc( outputs , inputs ) , Uc( outputs , outputs ) ,
		Wi( outputs , inputs ) , Ui( outputs , outputs ) ,
		Wf( outputs , inputs ) , Uf( outputs , outputs ) ,
		Wo( outputs , inputs ) , Uo( outputs , outputs ) ,
		ctmo( outputs , 1 , 0.0 ) , htmo( outputs , 1 , 0.0 )
	{
		numInputs = inputs;
		numOutputs = outputs;

	}

	LSTMLayer(const LSTMLayer &copy) 
		:Wc(copy.Wc), Wi(copy.Wi), Wo(copy.Wo),
		Uc(copy.Uc), Ui(copy.Ui), Uo(copy.Uo), Uf(copy.Uo),
		ctmo(copy.ctmo), htmo(copy.htmo), 
		numInputs(copy.numInputs), numOutputs(copy.numOutputs)
	{

	}

	virtual NeuroNetworkLayerState ForwardPass( const MatrixD &data ) override
	{


		MatrixD ats = (Wc * data + Uc * htmo);
		MatrixD its = (Wi * data + Ui * htmo);
		MatrixD fts = (Wf * data + Uf * htmo);
		MatrixD ots = (Wo * data + Uo * htmo);

		MatrixD at = ats.ApliedFunction( TanhActivationFunction );
		MatrixD it = ats.ApliedFunction( SigmoidActivationFunction );
		MatrixD ft = ats.ApliedFunction( SigmoidActivationFunction );
		MatrixD ot = ats.ApliedFunction( SigmoidActivationFunction );

		MatrixD ct = it % at + ft % ctmo;
		MatrixD ht = ot % ct.ApliedFunction( TanhActivationFunction );


		LSTMLayerState state( ats , its , fts ,
			at , it , ft , ot ,
			data , ctmo , ct , ht , htmo
		);



		htmo = ht;
		ctmo = ct;
		return state;
	}




	virtual BackPropagateResult BackPropagation( const NeuroNetworkError &error , const NeuroNetworkLayerState &state ) override
	{
		const LSTMNeuroNetworkError &lstmError = static_cast<const LSTMNeuroNetworkError&>(error);
		const LSTMLayerState &lstmState = static_cast<const LSTMLayerState&>(state);
		MatrixD deltaOt = lstmError.outputError % lstmState.ct.ApliedFunction( TanhActivationFunction );
		MatrixD deltaCt = lstmError.outputError % deltaOt % (lstmState.ct.ApliedFunction( TanhActivationFunctionDiff )) + lstmError.cError;

		MatrixD deltaIt = deltaCt % lstmState.at;
		MatrixD deltaFt = deltaCt % lstmState.ctmo;
		MatrixD deltaAt = deltaCt % lstmState.it;
		MatrixD deltaCtmo = deltaCt % lstmState.ft;

		MatrixD deltaAts = deltaAt % lstmState.ats.ApliedFunction( TanhActivationFunctionDiff );
		MatrixD deltaIts = deltaAt % lstmState.it.ApliedFunction( SigmoidActivationFunctionDiff );
		MatrixD deltaFts = deltaAt % lstmState.ft.ApliedFunction( SigmoidActivationFunctionDiff );
		MatrixD deltaOts = deltaAt % lstmState.ot.ApliedFunction( SigmoidActivationFunctionDiff );

		MatrixD deltaXt = Wc * deltaAts + Wf * deltaFts + Wi * deltaIts + Wo * deltaOts;
		MatrixD deltaHtmo = Uc * deltaAts + Uf * deltaFts + Ui * deltaIts + Uo * deltaOts;

		MatrixD xtTrans = lstmState.xt.Transposed();

		MatrixD deltaWc = deltaAts * xtTrans;
		MatrixD deltaWi = deltaIts * xtTrans;
		MatrixD deltaWf = deltaFts * xtTrans;
		MatrixD deltaWo = deltaOts * xtTrans;

		MatrixD htmoTrans = lstmState.htmo.Transposed();

		MatrixD deltaUc = deltaAts * htmoTrans;
		MatrixD deltaUi = deltaIts * htmoTrans;
		MatrixD deltaUf = deltaFts * htmoTrans;
		MatrixD deltaUo = deltaOts * htmoTrans;


		LSTMBackPropogateResult res( deltaHtmo , deltaCt ,
			deltaWc , deltaWi , deltaWf , deltaWo ,
			deltaUc , deltaUi , deltaUf , deltaUo );
		return res;
	}



	virtual void ApplyBackPropagation( const BackPropagateResult &result ) override
	{
		const LSTMBackPropogateResult &lstmRes = static_cast<const LSTMBackPropogateResult &>(result);

		Wc += lstmRes.deltaWc;
		Wi += lstmRes.deltaWi;
		Wf += lstmRes.deltaWf;
		Wo += lstmRes.deltaWo;

		Uc += lstmRes.deltaUc;
		Ui += lstmRes.deltaUi;
		Uf += lstmRes.deltaUf;
		Uo += lstmRes.deltaUo;

	}

};