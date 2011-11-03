/*
 * ProfileConsumerBase.h
 *
 *  Created on: Sep 21, 2011
 *      Author: poseidon
 */

#ifndef PROFILECONSUMERBASE_H_
#define PROFILECONSUMERBASE_H_

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
			m_xsource( std::unique_ptr<SourceTypeForThis>( xsource )),
			m_ysource( std::unique_ptr<SourceTypeForThis>( ysource )),
			m_plotName ( plotName)
	{

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

		m_profile = std::unique_ptr<Profile2d> (
				new Profile2d( m_plotName,
								this->GetPipelineSettings().GetRootFileFolder())
		);
		m_profile->AddModifiers( modlist );
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

		m_xsource->GetValue( event, metaData, this->GetPipelineSettings(), xval );
		m_ysource->GetValue( event, metaData, this->GetPipelineSettings(), yval );

		m_profile->AddPoint(	xval,
								yval,
								metaData.GetWeight() );

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

		m_xsource->GetValue( event, metaData, this->GetPipelineSettings(), xval );
		m_ysource->GetValue( event, metaData, this->GetPipelineSettings(), yval );

			m_profile->AddPoint(	xval,
									yval,
								metaData.GetWeight() );


		m_ysource->EndOnEvent( event, metaData, this->GetPipelineSettings() );
	}

	// store all histograms
	virtual void Finish()
	{
		m_profile->Store( this->GetPipelineSettings().GetRootOutFile());
	}


	void SetXSource ( SourceTypeForThis * xsource )
	{
		m_xsource = std::unique_ptr<SourceTypeForThis>( xsource );
	}

	void SetYSource ( SourceTypeForThis * ysource )
	{
		m_ysource = std::unique_ptr<SourceTypeForThis>( ysource );
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
	std::unique_ptr<SourceTypeForThis> m_xsource;
	std::unique_ptr<SourceTypeForThis> m_ysource;

	std::unique_ptr<Profile2d> m_profile;

	std::string m_plotName;
	bool m_runUnfiltered;
};

}

#endif /* PROFILECONSUMERBASE_H_ */
