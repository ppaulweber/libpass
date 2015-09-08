//  
//  Copyright (c) 2015 Philipp Paulweber
//  All rights reserved.
//  
//  Developed by: Philipp Paulweber
//                https://github.com/ppaulweber/libpass
//  
//  Permission is hereby granted, free of charge, to any person obtaining a 
//  copy of this software and associated documentation files (the "Software"), 
//  to deal with the Software without restriction, including without limitation 
//  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//  and/or sell copies of the Software, and to permit persons to whom the 
//  Software is furnished to do so, subject to the following conditions:
//  
//  * Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimers.
//  
//  * Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimers in the 
//    documentation and/or other materials provided with the distribution.
//  
//  * Neither the names of the copyright holders, nor the names of its 
//    contributors may be used to endorse or promote products derived from 
//    this Software without specific prior written permission.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
//  CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
//  WITH THE SOFTWARE.
//  

#ifndef _LIB_PASS_PASSINFO_H_
#define _LIB_PASS_PASSINFO_H_

#include "Type.h"

/**
   @brief    TODO
   
   TODO
*/

namespace libpass
{
	class PassInfo
	{
	private:
		const char* const name;
		const char* const description;
		const char* const arg_str;
		const char        arg_char;
	
		const PassId  id;
	
		PassConstructor constructor;
	
		uint64_t changes;

	public:
		PassInfo
		( const char* passName
		, const char* passDescription
		, const char* passArgStr
		, const char  passArgChar
		, const PassId passID
		, PassConstructor passConstructor
		)
		: name( passName )
		, description( passDescription )
		, arg_str( passArgStr )
		, arg_char( passArgChar )
		, id( passID )
		, constructor( passConstructor )
		{
		}
		
		const char* getPassName(void) const 
		{ 
			return name; 
		}
	
		const char* getPassDescription(void) const 
		{ 
			return description; 
		}
	
		const char* getPassArgString(void) const 
		{ 
			return arg_str; 
		}
	
		const char getPassArgChar(void) const 
		{ 
			return arg_char; 
		}
	
		const PassId getPassId(void) const
		{
			return id;
		}

		bool isPassID(const PassId passID) const
		{
			return passID == id;
		}
	
		void addChanges(uint64_t change)
		{
			changes += change;
		}
	
		uint64_t getChanges(void)
		{
			return changes;
		}
	
		Pass* constructPass(void) const
		{
			return constructor();
		}
	
		friend ostream& operator<<(ostream& os, PassInfo& pi)
		{
			return os; // << pi.getPassName() << ": ";
		}
	};
}

#endif /* _LIB_PASS_PASSINFO_H_ */


//  
//  Local variables:
//  mode: c++
//  indent-tabs-mode: t
//  c-basic-offset: 4
//  tab-width: 4
//  End:
//  vim:noexpandtab:sw=4:ts=4:
//  
