// (C) 2003 Wesley Steiner

#ifndef MODULE_C_UTILITY_NONCOPYABLE
#define MODULE_C_UTILITY_NONCOPYABLE

#ifdef __cplusplus

namespace lib {

class NonCopyable 
// A class derived from this class is made uncopyable at compile-time.
	{
	protected:
	NonCopyable() { }
	virtual ~NonCopyable() { }
	private:
	NonCopyable(const NonCopyable&);	
	NonCopyable& operator=(const NonCopyable&);	
	};

} // namespaces

#define DECLARE_NO_COPY virtual protected lib::NonCopyable

// legacy
#define utility_declare_no_copy virtual protected lib::NonCopyable
#define _WSM_DECLARE_NONCOPYABLE utility_declare_no_copy
#define DECLARE_NONCOPYABLE _WSM_DECLARE_NONCOPYABLE

#endif /* __cplusplus */

#endif // MODULE
