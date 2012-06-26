#include "Draw/DrawBase.h"

namespace CalibFW
{

Hist1D::ModifierList Hist1D::GetNoModifier()
{
	Hist1D::ModifierList modList;
	return modList;
}

Hist1D::ModifierList Hist1D::GetPtModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(0.0f, 1000.0f));
	modList.push_back(new ModHistBinCount(500));

	return modList;
}

Hist1D::ModifierList Hist1D::GetJetMatchingModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(-2.5f, 100.5f));
	modList.push_back(new ModHistBinCount( 103 ));

	return modList;
}

Hist1D::ModifierList Hist1D::GetResponseModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(0.0f, 2.0f));
	modList.push_back(new ModHistBinCount(200));

	return modList;
}

Hist1D::ModifierList Hist1D::GetConstituentsModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(-0.5f, +150.5f));
	modList.push_back(new ModHistBinCount(151));

	return modList;
}

Hist1D::ModifierList Hist1D::GetPtModifier(double max)
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(0.0f, max));
	modList.push_back(new ModHistBinCount(500));

	return modList;
}

Hist1D::ModifierList Hist1D::GetEtaModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(-10.f, +10.0f));
	modList.push_back(new ModHistBinCount(200));

	return modList;
}


Hist1D::ModifierList Hist1D::GetCountModifier( unsigned int max)
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(-0.5f,  float(max) +0.5f));
	modList.push_back(new ModHistBinCount(max + 1));

	return modList;
}

Hist1D::ModifierList Hist1D::GetNRVModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(-0.5f, +50.5f));
	modList.push_back(new ModHistBinCount(51));

	return modList;
}

Hist1D::ModifierList Hist1D::GetMassModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(0.f, 500.0f));
	modList.push_back(new ModHistBinCount(500));
	return modList;
}

Hist1D::ModifierList Hist1D::GetPhiModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(-5.f, +5.0f));
	modList.push_back(new ModHistBinCount(100));
	return modList;
}

Hist1D::ModifierList Hist1D::GetFractionModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 1.2f));
	modList.push_back(new ModHistBinCount(120));
	return modList;
}

Hist1D::ModifierList Hist1D::GetAreaModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 2.0f));
	modList.push_back(new ModHistBinCount(100));
	return modList;
}

Hist1D::ModifierList Hist1D::GetRunModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(160000, 200000));
	modList.push_back(new ModHistBinCount(40000));
	return modList;
}


}

