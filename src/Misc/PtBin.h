
#pragma once

namespace CalibFW
{

class PtBin
{
public:
    PtBin()
    {
        m_fLowestPt = m_fHighestPt = 0.0;

    }


    PtBin( double lowPt, double highPt)
    {
      assert( lowPt < highPt);

        m_fLowestPt = lowPt;
        m_fHighestPt = highPt;
    }

    double GetBinCenter()
    {
       return (GetMax() + GetMin() ) / 2.0f;
    }

    double GetMin()
    {
      return m_fLowestPt;
    }
    double GetMax()
    {
	return m_fHighestPt;
    }

    bool IsInBin( double val)
    {
        return ( val >= m_fLowestPt ) && ( val  <  m_fHighestPt);
    }

    // m_fLowestPt <= Z.Pt < m_fHighestPt

    std::string id() {
        std::string id("Pt");
        id+=(int)m_fLowestPt;
        id+="to";
        id+=(int)m_fHighestPt;
        return id;
    };
    std::string good_id() {
        std::string id("");
        id+=(int)m_fLowestPt;
        id+=" < p_{T}^{Z} < ";
        id+=(int)m_fHighestPt;
        return id;
    };

    double m_fLowestPt;
    double m_fHighestPt;
};

}
