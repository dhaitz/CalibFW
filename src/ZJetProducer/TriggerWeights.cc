namespace Artus
{

double efficiencyMu8(double eta)
{
	//e(8,X)
	if (eta < -2.4) return 0.0;
	else if (eta < -2.1) return 0.901234;
	else if (eta < -1.6) return 0.946693;
	else if (eta < -1.2) return 0.963248;
	else if (eta < -0.9) return 0.969777;
	else if (eta < -0.6) return 0.969927;
	else if (eta < -0.3) return 0.980704;
	else if (eta < -0.2) return 0.938091;
	else if (eta < 0.0) return 0.978146;
	else if (eta < 0.2) return 0.979885;
	else if (eta < 0.3) return 0.946208;
	else if (eta < 0.6) return 0.977650;
	else if (eta < 0.9) return 0.970148;
	else if (eta < 1.2) return 0.967241;
	else if (eta < 1.6) return 0.961170;
	else if (eta < 2.1) return 0.951291;
	else if (eta < 2.4) return 0.899130;
	return 0.0;
}

double efficiencyMu17(double eta)
{
	//e(17,X)
	if (eta < -2.4) return 0.0;
	else if (eta < -2.1) return 0.971070;
	else if (eta < -1.6) return 0.991109;
	else if (eta < -1.2) return 0.995833;
	else if (eta < -0.9) return 0.997195;
	else if (eta < -0.6) return 0.998533;
	else if (eta < -0.3) return 0.999366;
	else if (eta < -0.2) return 0.998986;
	else if (eta < 0.0) return 0.999123;
	else if (eta < 0.2) return 0.999390;
	else if (eta < 0.3) return 0.999256;
	else if (eta < 0.6) return 0.999265;
	else if (eta < 0.9) return 0.998549;
	else if (eta < 1.2) return 0.996644;
	else if (eta < 1.6) return 0.995183;
	else if (eta < 2.1) return 0.990810;
	else if (eta < 2.4) return 0.972960;
	return 0.0;
}

//
/* %http://fwyzard.web.cern.ch/fwyzard/hlt/2012/summary
Path HLT_Mu17_Mu8:
 - first seen online on run 190456 (/cdaq/physics/Run2012/5e33/v4.4/HLT/V5)
 - last  seen online on run 209151 (/cdaq/special/25ns/v1.1/HLT/V2)
 - V16: (runs 190456 - 193621) seeded by: L1_DoubleMu_10_Open
 - V17: (runs 193834 - 196027) seeded by: L1_DoubleMu_10_Open
 - V18: (runs 196046 - 196531) seeded by: L1_DoubleMu_10_Open OR L1_DoubleMu_10_3p5
 - V19: (runs 198022 - 199608) seeded by: L1_DoubleMu_10_Open OR L1_DoubleMu_10_3p5
 - V21: (runs 199698 - 205238) seeded by: L1_DoubleMu_10_Open OR L1_DoubleMu_10_3p5
 - V22: (runs 205303 - 209151) seeded by: L1_DoubleMu_10_Open OR L1_DoubleMu_10_3p5
*/

double turnonMu17(double pt)
{

	const double mx = 0.996307875;
	const double p0 =  0.972684; //  +/-   0.00445738
	const double p1 =  0.00123763; //  +/-   0.000348737
	const double p2 = -2.19029e-05; //  +/-   8.8258e-06
	const double p3 =  1.31997e-07; //  +/-   7.24225e-08
	if (pt > 50)
		return 1.0;
	return (p0 + p1 * pt + p2 * pt * pt + p3 * pt * pt * pt) / mx;
}
/*
0-45
p0                        =     0.951495   +/-   0.00107701
p1                        =  0.000271895   +/-   2.88685e-05
45-50
p0                        =     0.992915   +/-   0.0106869
p1                        = -0.000642644
50-inf
p0                        =     0.966025   +/-   0.00263384
p1                        = -9.21566e-05   +/-   4.30672e-05
*/

}

