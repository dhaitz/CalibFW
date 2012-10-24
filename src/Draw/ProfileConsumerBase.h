/*
 * ProfileConsumerBase.h
 *
 *  Created on: Sep 21, 2011
 *      Author: poseidon
 */

#ifndef PROFILECONSUMERBASE_H_
#define PROFILECONSUMERBASE_H_

#include <boost/scoped_ptr.hpp>

#include "Pipeline/EventPipeline.h"
#include "Draw/Profile.h"
#include "Pipeline/SourceBase.h"

namespace CalibFW
{

template < class TEvent, class TMetaData, class TSettings >
class ProfileConsumerBase : public EventConsumerBase< TEvent, TMetaData, TSettings>
{
public:
	typedef SourceBase<TEvent, TMetaData, TSettings> SourceTypeForThis;
	typedef EventPipeline<TEvent, TMetaData, TSettings> PipelineTypeForThis;


	ProfileConsumerBase() : m_runUnfiltered( false )
	{

	}
	ProfileConsumerBase( SourceTypeForThis * xsource,
			SourceTypeForThis * ysource,
			std::string plotName ) :
			m_plotName ( plotName)
	{
        m_xsource.reset( xsource );
        m_ysource.reset( ysource );
	}

	virtual void Init(PipelineTypeForThis * pset)
	{
		EventConsumerBase< TEvent, TMetaData, TSettings>::Init( pset );

		// init the profile plot
		Profile2d::ModifierList modlist;

		if ( m_xsource->HasDefaultBinCount())
			modlist.push_back( new ModProfileXBinCount( m_xsource->GetDefaultBinCount()) );

		if ( m_xsource->HasDefaultBins())
			modlist.push_back( new ModProfileXBinRange( m_xsource->GetDefaultLowBin(),
													m_xsource->GetDefaultHighBin() ));

		m_profile.reset(
				new Profile2d( m_plotName,
								this->GetPipelineSettings().GetRootFileFolder())
		);
		m_profile->AddModifiers( modlist );
		m_profile->Init();
	}

	virtual void ProcessEvent(TEvent const& event, TMetaData const& metaData,
			FilterResult & result)
	{
		EventConsumerBase< TEvent, TMetaData, TSettings>::ProcessEvent( event, metaData, result);

		if ( !m_runUnfiltered)
			return;

		if ( ! result.HasPassedIfExcludingFilter( "incut" ) )
			return;


		double xval;
		double yval;

        bool has_xval;
        bool has_yval;

		has_xval = m_xsource->GetValue( event, metaData, this->GetPipelineSettings(), xval );
		has_yval = m_ysource->GetValue( event, metaData, this->GetPipelineSettings(), yval );

        if (has_xval && has_yval) {
		    m_profile->AddPoint(	xval,
								    yval,
								    metaData.GetWeight() );
        }
		m_ysource->EndOnEvent( event, metaData, this->GetPipelineSettings() );
	}

	virtual void ProcessFilteredEvent(TEvent const& event,
			TMetaData const& metaData)
	{
		EventConsumerBase< TEvent, TMetaData, TSettings>::ProcessFilteredEvent( event, metaData);

		if ( m_runUnfiltered)
			return;



		m_ysource->StartOnEvent( event, metaData, this->GetPipelineSettings() );

		/*while ( m_ysource->HasValue() )
		{*/

		double xval;
		double yval;

        bool has_xval;
        bool has_yval;


		has_xval = m_xsource->GetValue( event, metaData, this->GetPipelineSettings(), xval );
        has_yval = m_ysource->GetValue( event, metaData, this->GetPipelineSettings(), yval );

        if (has_xval && has_yval) {
		    m_profile->AddPoint(	xval,
								    yval,
							    metaData.GetWeight() );
        }


		m_ysource->EndOnEvent( event, metaData, this->GetPipelineSettings() );
	}

	// store all histograms
	virtual void Finish()
	{
		m_profile->Store( this->GetPipelineSettings().GetRootOutFile());
	}


	void SetXSource ( SourceTypeForThis * xsource )
	{
		m_xsource.reset( xsource );
	}

	void SetYSource ( SourceTypeForThis * ysource )
	{
		m_ysource.reset( ysource );
	}

	void SetPlotName ( std::string plotName )
	{
		m_plotName = plotName;
	}

	void SetRunUnfiltered( bool val)
	{
		m_runUnfiltered = val;
	}

private:
	boost::scoped_ptr<SourceTypeForThis> m_xsource;
	boost::scoped_ptr<SourceTypeForThis> m_ysource;

	boost::scoped_ptr<Profile2d> m_profile;

	std::string m_plotName;
	bool m_runUnfiltered;
};

}

#endif /* PROFILECONSUMERBASE_H_ */
