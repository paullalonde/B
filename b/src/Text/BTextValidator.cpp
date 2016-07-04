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

// file header
#include "BTextValidator.h"

// B headers
#include "BString.h"
#include "BTextSource.h"


namespace B {

// ==========================================================================================
//  TextValidator

// ------------------------------------------------------------------------------------------
TextValidator::Action
TextValidator::Filter(
    const TextSource&   inSource,
    const String&       inCandidateText,
    const CFRange&      inSelection,
    String&             outReplacementText)
{
    String  candidate   = inCandidateText;
    String  replacement;
    
    mSignal.combiner().SetText(&candidate, &replacement);
    
    Action  action = mSignal(true, inSource, candidate, inSelection, replacement);
    
    if (action == Replace)
        outReplacementText = candidate;
    
    mSignal.combiner().SetText(NULL, NULL);
    
    return (action);
}

// ------------------------------------------------------------------------------------------
void
TextValidator::Validate(
    const TextSource&   inSource)
{
    String  candidate   = inSource.GetText();
    CFRange selection   = CFRangeMake(0, candidate.size());
    
    mSignal.combiner().SetText(&candidate, &candidate);
    
    Action  action = mSignal(false, inSource, candidate, selection, candidate);
    
    if (action != Accept)
        throw std::logic_error("text validator returned erroneous action from validate request.");
    
    mSignal.combiner().SetText(NULL, NULL);
}

// ------------------------------------------------------------------------------------------
void
TextValidator::Connect(Signal::slot_function_type inFunction)
{
    mSignal.connect(inFunction);
}

// ------------------------------------------------------------------------------------------
void
TextValidator::Connect(int inOrder, Signal::slot_function_type inFunction)
{
    mSignal.connect(inOrder, inFunction);
}

// ------------------------------------------------------------------------------------------
void
TextValidator::Clear()
{
    mSignal.disconnect_all_slots();
}


// ==========================================================================================
//  TextValidator::Exception

// ------------------------------------------------------------------------------------------
TextValidator::Exception::~Exception() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
TextValidator::Exception::what() const throw()
{
    return "B::TextValidator::Exception";
}

// ------------------------------------------------------------------------------------------
String
TextValidator::Exception::Format(const String& inFormatString) const
{
    return (inFormatString);
}


// ==========================================================================================
//  TextValidator::Combiner

// ------------------------------------------------------------------------------------------
TextValidator::Combiner::Combiner()
    : mCandidate(NULL), mReplacement(NULL)
{
}

// ------------------------------------------------------------------------------------------
template <typename InputIterator> TextValidator::Combiner::result_type
TextValidator::Combiner::operator () (
    InputIterator   first, 
    InputIterator   last) const
{
    result_type action      = Accept;
    bool        replacing   = false;
    bool        deleting    = false;
    
    while (first != last)
    {
        action  = *first++;
        
        if (action == Delete)
        {
            mCandidate->clear();
            deleting = true;
        }
        else if (action == Replace)
        {
            *mCandidate = *mReplacement;
            replacing = true;
        }
    }
    
    if (replacing)
        return Replace;
    else if (deleting)
        return Delete;
    else
        return Accept;
    
/*
    bool    replacing   = false;
    String  candidate   = ioText;
    
    for ( ; inBegin != inEnd; ++inBegin)
    {
        String  replacement = candidate;
        
        result = (*inBegin)(inSelection, replacement);
        
        if (result == Reject)
        {
            break;
        }
        else if (result == Delete)
        {
            candidate.clear();
        }
        else if (result == Replace)
        {
            candidate = replacement;
            replacing = true;
        }
    }
    
    if (replacing && (result == Accept))
    {
        ioText  = candidate;
        result  = Replace;
    }
*/
}

}   // namespace B
