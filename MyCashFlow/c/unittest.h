// (C) 2004 Wesley Steiner

#ifndef MODULE_UNITTEST
#define MODULE_UNITTEST

#ifdef TEST
#define test_protected public
#define test_private public
#define test_virtual virtual
#else
#define test_protected protected
#define test_private private
#define test_virtual
#endif

namespace fake 
	{
	class MethodInfo
		{
		public:
		MethodInfo() : my_called_count(0) { }
		void Enter() { ++my_called_count; }
		bool WasCalled() const { return CalledCount() > 0; }
		int CalledCount() const { return my_called_count; }
		private:
		int my_called_count;
		};
	}

#endif
