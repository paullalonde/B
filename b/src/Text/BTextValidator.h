// ==========================================================================================
//  
//  Copyright (C) 2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

#ifndef BTextValidator_H_
#define BTextValidator_H_

#pragma once

// library headers
#include <boost/signal.hpp>

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class   String;
class   TextSource;


class TextValidator
{
public:
    
    enum Action
    {
        //! Use the input text as-is.
        Accept,
        //! Replace the input text with the output text.
        Replace,
        //! Delete the selection in the source text.
        Delete,
    };
    
private:
    
    class Combiner
    {
    public:
    
        typedef TextValidator::Action   result_type;
        
        Combiner();
        
        void    SetText(String* candidate, String* replacement)
                {
                    mCandidate      = candidate;
                    mReplacement    = replacement;
                }
        
        template <typename InputIterator>
        result_type operator () (
            InputIterator   first, 
            InputIterator   last) const;
        
    private:
        
        // member variables
        String* mCandidate;
        String* mReplacement;
    };
    
public:

    class Exception : public std::exception
    {
    public:
        
        virtual             ~Exception() throw();
        virtual const char* what() const throw();
        virtual String      Format(const String& inFormatString) const;
    };

    typedef boost::signal5<Action, bool, const TextSource&, const String&, const CFRange&, String&, Combiner>   Signal;
    
    Action  Filter(
                const TextSource&   inSource,
                const String&       inCandidateText,
                const CFRange&      inSelection,
                String&             outReplacementText);
    void    Validate(
                const TextSource&   inSource);
    
    void    Connect(Signal::slot_function_type inFunction);
    void    Connect(int inOrder, Signal::slot_function_type inFunction);
    void    Clear();

private:
    
    // member variables
    Combiner    mCombiner;
    Signal      mSignal;
};


}   // namespace B


#endif  // BTextFilter_H_
