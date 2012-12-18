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

Hist1D::ModifierList Hist1D::GetResponseModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(0.0f, 2.0f));
	modList.push_back(new ModHistBinCount(200));

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

Hist1D::ModifierList Hist1D::GetAbsEtaModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(0.0f, +10.0f));
	modList.push_back(new ModHistBinCount(100));

	return modList;
}


Hist1D::ModifierList Hist1D::GetCountModifier(unsigned int max, int min)
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(float(min) - 0.5f, float(max) + 0.5f));
	modList.push_back(new ModHistBinCount(max - min + 1));

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

	modList.push_back(new ModHistBinRange(-3.141459f, +3.141459f));
	modList.push_back(new ModHistBinCount(100));
	return modList;
}

Hist1D::ModifierList Hist1D::GetFractionModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(-0.005f, +1.495f));
	modList.push_back(new ModHistBinCount(150));
	return modList;
}

Hist1D::ModifierList Hist1D::GetAreaModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 2.0f));
	modList.push_back(new ModHistBinCount(100));
	return modList;
}

Hist1D::ModifierList Hist1D::GetRhoModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 50.0f));
	modList.push_back(new ModHistBinCount(100));
	return modList;
}

Hist1D::ModifierList Hist1D::GetMETModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 2500.0f));
	modList.push_back(new ModHistBinCount(250));
	return modList;
}

Hist1D::ModifierList Hist1D::GetDeltaRModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 15.0f));
	modList.push_back(new ModHistBinCount(150));
	return modList;
}

Hist1D::ModifierList Hist1D::GetDeltaPhiModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 3.14159f));
	modList.push_back(new ModHistBinCount(100));
	return modList;
}

Hist1D::ModifierList Hist1D::GetDeltaEtaModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 20.0f));
	modList.push_back(new ModHistBinCount(200));
	return modList;
}


Hist1D::ModifierList Hist1D::GetMETFractionModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(.0f, 0.5f));
	modList.push_back(new ModHistBinCount(200));
	return modList;
}

Hist1D::ModifierList Hist1D::GetRunModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(190000, 205000));
	modList.push_back(new ModHistBinCount(10000));
	return modList;
}


}

