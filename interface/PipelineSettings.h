#pragma once

namespace CalibFW {

#define IMPL_SETTING(TYPE, SNAME) \
private: \
TYPE m_##SNAME;                                                                                                                        \
public: \
std::string Key##SNAME () { return "##SNAME"; }                                                        \
std::string FullKey##SNAME () { return GetSettingsRoot() + "." + #SNAME; }                                                     \
VarCache<TYPE> Cache##SNAME; \
TYPE Get##SNAME ( ) { if (Cache##SNAME.IsCached()) { return Cache##SNAME.GetValue(); }         \
       TYPE  val = GetPropTree()->get< TYPE >( FullKey##SNAME ());     \
       Cache##SNAME.SetCache( val ); \
       return val;}            \
void Set##SNAME ( TYPE val) { GetPropTree()->put( FullKey##SNAME (), val);     \
                                                               Cache##SNAME.SetCache( val );}  \


#define IMPL_SETTING_DEFAULT(TYPE, SNAME, DEFAULT_VAL) \
private: \
TYPE m_##SNAME;                                                                                                                        \
public: \
std::string Key##SNAME () { return "##SNAME"; }                                                        \
std::string FullKey##SNAME () { return GetSettingsRoot() + "." + #SNAME; }                                                     \
VarCache<TYPE> Cache##SNAME; \
TYPE Get##SNAME ( ) { if (Cache##SNAME.IsCached()) { return Cache##SNAME.GetValue(); }         \
       TYPE  val = GetPropTree()->get< TYPE >( FullKey##SNAME (), DEFAULT_VAL );       \
       Cache##SNAME.SetCache( val ); \
       return val;}            \
void Set##SNAME ( TYPE val) { GetPropTree()->put( FullKey##SNAME (), val);     \
                                                               Cache##SNAME.SetCache( val );}  \

#define RETURN_CACHED(CACHE_MEMBER,VALUEPATH) \
{ if (! CACHE_MEMBER.IsCached() ) \
               CACHE_MEMBER.SetCache( VALUEPATH ); \
return CACHE_MEMBER.GetValue(); }\

template<class TData>
class VarCache {
public:
	VarCache() :
		m_isCached(false) {

	}

	void SetCache(TData t) {
		m_val = t;
		m_isCached = true;
	}

	TData GetValue() {
		if	(!m_isCached)
			CALIB_LOG_FATAL("not Cached variable used")

			return m_val;
	}

	bool IsCached()
	{
		return m_isCached;
	}

	bool m_isCached;
	TData m_val;
};

class PropertyTreeSupport
{
public:
	static stringvector GetAsStringList(boost::property_tree::ptree * propTree,
			std::string path)
	{
		stringvector fvec;
		try
		{
			BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
					propTree->get_child(path))
					{	fvec.push_back(v.second.data());
					}
		}
		catch ( boost::property_tree::ptree_bad_path & e)
		{
			// no problem, node optional
		}
		return fvec;
	}
};

}

