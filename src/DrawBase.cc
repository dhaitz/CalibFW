#include "DrawBase.h"

namespace CalibFW
{

Hist1D::ModifierList Hist1D::GetPtModifier()
{
	Hist1D::ModifierList modList;

	modList.push_back(new ModHistBinRange(0.0f, 1000.0f));
	modList.push_back(new ModHistBinCount(500));

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
	modList.push_back(new ModHistBinCount(240));
	return modList;
}

}

